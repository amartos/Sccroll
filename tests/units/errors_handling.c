/**
 * @file        errors_handling.c
 * @version     0.1.0
 * @brief       Test unitaire des gestions d'erreurs.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/errors_handling.c -o build/objs/tests/units/errors_handling.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/errors_handling.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c test/units/errors_handling.c) \
 *     -o build/bin/tests/errors_handling
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

// Le test courant utilise fork, on s'assure donc d'utiliser
// l'originale et non le mock prédéfini.
extern __typeof__(fork) __real_fork;

// On s'assure d'utiliser la version originale de abort.
extern __typeof__((abort)) __real_abort;

// Le mock testé courant.
static SccrollMockFlags errnum = SCCENONE;

// Variable permettant de repousser la levée d'erreur de delay
// appels. Ceci permet de tester les appels et leurs erreurs qui sont
// situés plus loin dans la séquence d'exécution (puisqu'une erreur
// plus tôt stoppera le programme).
static int delay = 0;

// Variable indiquant si le simulacre a été appelé et doit entrer en
// erreur.
static bool called = false;

// Redéfinition prévue par l'API.
bool sccroll_mockTrigger(SccrollMockFlags mock)
{
    if (sccroll_hasFlags(errnum, mock))
        called = delay < 1 ? true : (--delay, false);
    return called;
}

// Fonction de test réussit quelles que soient les conditions.
void test_success(void) {};

// Effectue un test unitaire de ftest dans un fork (la fonction
// originale), et vérifie qu'une erreur est bien levée par
// sccroll_run.
static bool assertMock(void)
{
    called = false;
    const SccrollEffects test = {
        .wrapper = test_success,
        .name = sccroll_mockName(errnum),
        .flags = sccroll_hasFlags(errnum, SCCEFORK) ? 0 : NOFORK,
    };

    pid_t pid = __real_fork();
    if (pid < 0)
        err(EXIT_FAILURE, "__real_fork failed for %s", test.name);
    else if (pid == 0) {
        sccroll_register(&test);
        sccroll_run();
        // On envoie un signal indiquant que le simulacre testé n'est
        // pas appelé dans la librairie avec le delai donné.
        if (!called) __real_abort();
    }

    int status = 0;
    wait(&status);
    called = WTERMSIG(status) != SIGABRT;
    if (called && !WEXITSTATUS(status)) {
        fprintf(stderr, "%s error not handled (delay: %i)\n", test.name, delay);
        raise(SIGABRT);
    }
    return called;
}

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

int main(void)
{
    assertMock();
    for (errnum = 2, delay = 0; errnum < SCCEMAX; delay = 0, errnum <<= 1)
        while (assertMock()) ++delay;
    return EXIT_SUCCESS;
}
