/**
 * @file        nomain.c
 * @version     0.1.0
 * @brief       Test unitaire d'exécution des tests sans main défini.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/execution/nomain.c -o build/objs/tests/units/core/execution/nomain.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/core/execution/nomain.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/execution/nomain
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

// Test simple, affiche un message sur stdout si le test est exécuté.
SCCROLL_TEST(test_print, .std[STDOUT_FILENO].content="Test executed.") { puts("Test executed."); }
