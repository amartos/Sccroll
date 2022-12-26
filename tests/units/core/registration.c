/**
 * @file        registration.c
 * @version     0.1.0
 * @brief       Tests unitaires des enregistrements de tests.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -registration=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/registration.c \
 *     -o build/objs/tests/units/core/registration.o
 * gcc -L build/libs -lsccroll \
 *     build/objs/tests/units/core/registration.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/registration
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparatifs des tests.
 ******************************************************************************/
// clang-format on

// Constantes des tests unitaires.
enum {
    MAXT = 10,
};

SCCROLL_TEST(test_success) {}

SCCROLL_TEST(test_fail_auto) { assert(false); }

void test_fail_manual(void) { assert(false); }

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

int main(void)
{
    // Enregistrement manuel.
    SccrollEffects test = {
        .wrapper = test_fail_auto,
        .name = "test_fail_auto manually registered"
    };
    sccroll_register(&test);

    test.wrapper = test_fail_manual;
    const char* prefix = "cutom test name:";
    char name[BUFSIZ] = { 0 };
    for (int i = 0; i < MAXT; ++i) {
        sprintf(name, "%s %i", prefix, i);
        test.name = strdup(name);
        sccroll_register(&test);
        memset(name, 0, strlen(name));
    }

    assert(sccroll_run() == MAXT+2);
    return EXIT_SUCCESS;
}
