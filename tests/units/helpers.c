/**
 * @file        helpers.c
 * @version     0.1.0
 * @brief       Helper unit tests.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 */

// On s'assure d'utiliser celui de la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

static const int error = 123;

static void sccroll_success(void) { }
static void sccroll_test_fatal(void) { abort(); }
static void sccroll_error(void) { exit(error); }

// clang-format off

/******************************************************************************
 * Execution
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
    status = sccroll_simplefork("fatal", sccroll_test_fatal);
    assert(WTERMSIG(status) == SIGABRT);
    assert(WEXITSTATUS(status) == 0);
    status = sccroll_simplefork("error", sccroll_error);
    assert(WTERMSIG(status) == 0);
    assert(WEXITSTATUS(status) == error);

    // The coverage calculations are biased for this module due to
    // this function, which is not called oftent. This line is used to
    // compensate.
    for (int i = 1; sigabbrev_np(i); ++i);
}
