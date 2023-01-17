/**
 * @file        errors.c
 * @version     0.1.0
 * @brief       Test unitaire des gestions d'erreurs de la librairie.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/errors.c -o build/objs/tests/units/core/errors.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/core/errors.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c test/units/core/errors.c) \
 *     -o build/bin/tests/core/errors
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

// Fonction de test réussit quelles que soient les conditions.
void test_success(void) {};

// Effectue un test unitaire de ftest dans un fork (la fonction
// originale), et vérifie qu'une erreur est bien levée par
// sccroll_run.
static bool assertMock(SccrollMockFlags mock, int delay)
{
    SccrollMockTrigger trigger = {
        .mock = mock,
        .delay = delay,
        .opts = SCCMABORT,
    };
    const SccrollEffects test = {
        .wrapper = test_success,
        .name = sccroll_mockName(mock),
        .flags = sccroll_hasFlags(mock, SCCEFORK) ? 0 : NOFORK,
    };

    pid_t pid = __real_fork();
    if (pid < 0)
        err(EXIT_FAILURE, "__real_fork failed for %s", test.name);
    else if (pid == 0) {
        sccroll_mockTrigger(&trigger);
        sccroll_register(&test);
        sccroll_run();
        exit(0);
    }

    int status, code, signal;
    wait(&status);
    code = WEXITSTATUS(status);

    // On vérifie qu'il n'y a pas d'erreur si aucun simulacre ne
    // devrait entrer en erreur.
    assert(!(!mock && code));

    // On vérifie que les simulacres n'ont pas envoyé SIGABRT; ce
    // serait le signe que l'erreur du simulacre appelée à delay n'a
    // pas été prise en compte.
    signal = WTERMSIG(status);
    if (signal == SIGABRT) {
        fprintf(stderr,
                "assertMock: %s mock error in call #%i not handled\n",
                sccroll_mockName(mock), delay);
        raise(SIGABRT);
    }

    // Si une erreur est levée (code ou signal), on peut supposer
    // qu'il reste encore des appels à vérifier.
    return mock && (code || signal);
}

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

int main(void)
{
    SccrollMockFlags mock;
    int delay;
    for (mock = SCCENONE, delay = 0; mock < SCCEMAX; ++mock, delay = 0)
        while (assertMock(mock, delay++));
    return EXIT_SUCCESS;
}
