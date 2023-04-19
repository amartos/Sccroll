/**
 * @file        helpers.c
 * @version     0.1.0
 * @brief       Test unitaire des aides à la programmation.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/helpers.c -o build/objs/tests/units/helpers.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/helpers.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c)              \
 *     -o build/bin/tests/helpers
 * @endcode
 */

// On s'assure d'utiliser celui de la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparation des tests
 ******************************************************************************/
// clang-format on

static const int error = 123;

static void sccroll_success(void) { }
static void sccroll_fatal(void) { abort(); }
static void sccroll_error(void) { exit(error); }

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

int main(void)
{
    int status;
    unsigned flags = 2|8|32;
    unsigned values = 0;
    assert(!sccroll_hasFlags(flags, values));
    values = 1|4|16|64;
    assert(!sccroll_hasFlags(flags, values));
    values = 2;
    assert(sccroll_hasFlags(flags, values) == values);
    values = 8;
    assert(sccroll_hasFlags(flags, values) == values);
    values = 32;
    assert(sccroll_hasFlags(flags, values) == values);
    values = flags;
    assert(sccroll_hasFlags(flags, values) == values);

    status = sccroll_simplefork("success", sccroll_success);
    assert(WTERMSIG(status) == 0);
    assert(WEXITSTATUS(status) == EXIT_SUCCESS);
    status = sccroll_simplefork("fatal", sccroll_fatal);
    assert(WTERMSIG(status) == SIGABRT);
    assert(WEXITSTATUS(status) == 0);
    status = sccroll_simplefork("error", sccroll_error);
    assert(WTERMSIG(status) == 0);
    assert(WEXITSTATUS(status) == error);
}
