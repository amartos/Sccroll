/**
 * @file        sccroll_errors_handling_tests.c
 * @version     0.1.0
 * @brief       Test unitaire des gestions d'erreurs.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -L build/libs -l scroll \
 *     -Wl,--wrap,abort,--wrap,calloc,--wrap,pipe,\
 *     --wrap,fork,--wrap,dup2,--wrap,close,--wrap,read,--wrap,write \
 *     tests/sccroll_errors_handling_tests.c -o build/bin/sccroll_errors_handling_tests
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparation des tests.
 ******************************************************************************/
// clang-format on

// Variable contenant le code d'erreur testée courant.
static int errnum = 0;

// Variable permettant de repousser la levée d'erreur de delay
// appels. Ceci permet de tester les appels et leurs erreurs qui sont
// situés plus loin dans la séquence d'exécution (puisqu'une erreur
// plus tôt stoppera le programme).
static int delay = 0;

// Drapeaux indiquant quelle erreur est testée.
enum {
    NOERR = 0,   // Aucune erreur levée
    DOFORK = 1,  // fork (nofork par défaut)
    CALLOC = 2,  // Test de calloc()
    PIPE = 4,    // Test de pipe()
    FORK = 8,    // Test de fork()
    DUP2 = 16,   // Test de dup2()
    CLOSE = 32,  // Test de close()
    READ = 64,   // Test de read()
    WRITE = 128, // Test de write()
};

// Détermine si la valeur donnée correspond au code d'erreur à lever.
static bool trigger(int value)
{
    if (delay > 0 && (errnum & value))
    {
        --delay;
        return NOERR;
    }
    return errnum & value;
}

// Lève une erreur d'assertion uniquement si la fonction testée est
// fork.
void ftest(void) { assert(!trigger(FORK)); }

// Template de test neutre.
// NOSTRP est utilisé ici afin de pouvoir vérifier les messages non
// modifiés levés par les erreurs rencontrées.
static SccrollEffects test = {
    .wrapper = ftest,
    .name = "test_success",
    .flags = NOSTRP,
};

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

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
    return trigger(FORK) ? -1 : __real_fork();
}

SCCROLL_MOCK(int, dup2, int oldfd, int newfd)
{
    sccroll_unused(oldfd);
    sccroll_unused(newfd);

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

// Effectue un test unitaire de ftest dans un fork (la fonction
// originale), et vérifie qu'une erreur est bien levée par
// sccroll_run.
// mocktrigger: Le code d'erreur correspondant à la gestion d'erreur
// testée.
// name: Le nom du test.
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
    assert((bool) (mocktrigger & ~DOFORK) == (bool) WEXITSTATUS(status));
    fprintf(stderr, "ok\n");
}

// Macro facilitant l'usage de _assertMock.
#define assertMock(errval) _assertMock(errval, #errval)

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    delay = 0;
    assertMock(NOERR | DOFORK);
    assertMock(NOERR);
    assertMock(CALLOC);
    assertMock(PIPE);
    assertMock(FORK | DOFORK);
    assertMock(DUP2);
    assertMock(CLOSE | DOFORK);
    assertMock(CLOSE);
    assertMock(READ);
    assertMock(WRITE);

    delay = 1;
    assertMock(CALLOC);
    assertMock(CLOSE);

    return EXIT_SUCCESS;
}
