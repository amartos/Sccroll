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
    DOFORK = 1,
    CALLOC = 2,
    PIPE = 4,
    FORK = 8,
    DUP2 = 16,
    CLOSE = 32,
    READ = 64,
    WRITE = 128,
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

void ftest(void) { assert(!trigger(FORK)); }

static SccrollEffects test = {
    .wrapper = ftest,
    .name = "test_success",
    .flags = NOSTRP,
};

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
    // On indique un fork réussi, mais on ne fork pas vraiment pour ne
    // pas fausser les résultats attendus.
    return trigger(FORK) ? -1 : 0;
}

SCCROLL_MOCK(int, dup2, int oldfd, int newfd)
{
    unused(oldfd);
    unused(newfd);

    // On ne dup2 pas vraiment pour ne pas masquer les sorties.
    return trigger(DUP2) ? -1 : 0;
}

SCCROLL_MOCK(int, close, int fd)
{
    return trigger(CLOSE) ? -1 : __real_close(fd);
}

SCCROLL_MOCK(ssize_t, read, int fd, void* buf, size_t count)
{
    return trigger(READ) ? -1 : __real_read(fd, buf, count);
}

SCCROLL_MOCK(ssize_t, write, int fd, const void* buf, size_t count)
{
    return trigger(WRITE) ? -1 : __real_write(fd, buf, count);
}

static void _assertMock(int mocktrigger, const char* restrict name)
{
    pid_t pid = __real_fork();
    if (pid < 0)
        err(EXIT_FAILURE, "__real_fork failed for %s.", name);
    else if (pid == 0) {
        errnum = mocktrigger;
        test.flags |= !trigger(DOFORK) ? NOFORK : test.flags;
        sccroll_register(&test);
        sccroll_run();
        exit(EXIT_SUCCESS);
    }

    int status = 0;
    wait(&status);
    fprintf(stderr, ">>>>>> %s (delay: %i): ", name, delay);
    assert((bool) mocktrigger == (bool) WEXITSTATUS(status));
    fprintf(stderr, "ok\n");
}

#define assertMock(errval) _assertMock(errval, #errval)

int main(void)
{
    delay = 0;
    assertMock(NOERR);
    assertMock(CALLOC);
    assertMock(PIPE);
    assertMock(FORK | DOFORK);
    assertMock(DUP2);
    assertMock(CLOSE | DOFORK);
    assertMock(CLOSE);
    assertMock(READ);
    assertMock(WRITE | DOFORK);

    delay = 1;
    assertMock(CALLOC);
    assertMock(CLOSE);

    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
