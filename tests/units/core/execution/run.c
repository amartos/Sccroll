/**
 * @file        run.c
 * @version     0.1.0
 * @brief       Test unitaire d'exécution de sccroll_run().
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/execution/run.c -o build/objs/tests/units/core/execution/run.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/core/execution/run.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/execution/run
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

// Constantes numériques du test.
enum {
    MAXF = 10,
    MAXS = 42,
};

// Test simple, affiche un message sur stdout si le test est exécuté.
void test_print(void) { assert(false && "Test executed."); }

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    SccrollEffects testf = { .wrapper = test_print, .name = "test_print_fail" };
    SccrollEffects tests = {
        .wrapper = test_print,
        .name = "test_print_success",
        .std[STDERR_FILENO].content =
        "run: tests/units/core/execution/run.c:39: test_print: Assertion `false && \"Test executed.\"' failed.",
        .codes = { [SCCSIGNAL] = SIGABRT, },
    };
    for (int i = 0; i < MAXF; ++i) sccroll_register(&testf);
    for (int i = 0; i < MAXS; ++i) sccroll_register(&tests);
    assert(sccroll_run() == MAXF);
    return EXIT_SUCCESS;
}
