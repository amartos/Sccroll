/**
 * @file        errors.c
 * @version     0.1.0
 * @brief       Test unitaire des gestions d'erreurs de la librairie.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
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

// Fonction de test réussit quelles que soient les conditions.
void test_success(void) {};

// Fonction exécutant sccroll_run avec un test factice, mais avec un
// déclenchement de simulacre à un délai donné.
static void run_test(void)
{
    SccrollEffects test = {
        .wrapper = test_success,
        .name    = "testing errors",
    };
    sccroll_register(&test);
    test.flags |= NOFORK;
    sccroll_register(&test);
    sccroll_run();
}

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

int main(void)
{
    sccroll_mockPredefined(run_test);
    return EXIT_SUCCESS;
}
