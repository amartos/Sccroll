/**
 * @file        sccroll_errors_handling_tests.c
 * @version     0.1.0
 * @brief       Test unitaire des gestions d'erreurs.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/sccroll_errors_handling_tests.c -L build/libs -l scroll \
 * -o build/bin/sccroll_basics_tests
 * @encode
 *
 * @addtogroup Sccroll
 * @{
 * @addtogroup UnitTests
 * @{
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

static int errnum = 0;
static int delay = 0;

enum {
    NOERR = 0,
    NOFORK = 1,
    CALLOC = 2,
    PIPE = 4,
    FORK = 8,
    DUP2 = 16,
    CLOSE = 32,
    READ = 64,
};

static bool trigger(int value)
{
    if (delay > 0 && (errnum & value))
    {
        --delay;
        return NOERR;
    }
    return errnum & value;
}

SCCROLL_MOCK(void*, calloc, size_t nmemb, size_t size)
{
    return trigger(CALLOC) ? NULL : __real_calloc(nmemb, size);
}

SCCROLL_MOCK(int, pipe, int pipefd[2])
{
    return trigger(PIPE) ? -1 : __real_pipe(pipefd);
}

SCCROLL_MOCK(pid_t, fork, void)
{
    return trigger(NOFORK) ? 0 : trigger(FORK) ? -1 : __real_fork();
}

SCCROLL_MOCK(int, dup2, int oldfd, int newfd)
{
    return trigger(DUP2) ? -1 : __real_dup2(oldfd, newfd);
}

SCCROLL_MOCK(int, close, int fd)
{
    return trigger(CLOSE) ? -1 : __real_close(fd);
}

SCCROLL_MOCK(ssize_t, read, int fd, void* buf, size_t count)
{
    return trigger(READ) ? -1 : __real_read(fd, buf, count);
}

void test_fail_unless_nofork(void) { if (!trigger(NOFORK)) assert(false); }

static void _assertMock(int mocktrigger, const char* restrict fmt, ...)
{
    char name[BUFSIZ] = { 0 };
    va_list args;
    va_start(args, fmt);
    vsprintf(name, fmt, args);
    va_end(args);

    pid_t pid = __real_fork();
    if (pid < 0)
        err(EXIT_FAILURE, "__real_fork failed for %s.", name);
    else if (pid == 0) {
        errnum = mocktrigger;
        sccroll_register(test_fail_unless_nofork, name);
        sccroll_run();
        exit(EXIT_SUCCESS);
    }

    int status = 0;
    wait(&status);
    fprintf(stderr, ">>>>>> %s: %i == %i\n", name, mocktrigger, WEXITSTATUS(status));
    assert((bool) mocktrigger == (bool) WEXITSTATUS(status));
}

#define assertMock(errval) _assertMock(errval, "%s (delay: %i)", #errval, delay)

int main(void)
{
    delay = 0;
    assertMock(NOERR);
    assertMock(CALLOC);
    assertMock(PIPE);
    assertMock(FORK);
    assertMock(DUP2 | NOFORK);
    assertMock(CLOSE | NOFORK);
    assertMock(CLOSE);
    assertMock(READ);

    delay = 1;
    assertMock(CALLOC);
    assertMock(CLOSE);

    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
