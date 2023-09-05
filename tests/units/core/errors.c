/**
 * @file        errors.c
 * @version     0.1.0
 * @brief       Core module errors unit tests source code.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

void test_success(void) { puts("foobar"); };

// Execute sccroll_run with a fake successful test, but triggers
// mocks.
static void run_test(void)
{
    SccrollEffects test = {
        .wrapper = test_success,
        .name    = "testing errors",
        // For the FILE related functions.
        .std = {
            [STDOUT_FILENO] = {.path = "tests/assets/blobs/textfile",},
        }
    };
    sccroll_register(&test);
    test.flags |= NOFORK;
    sccroll_register(&test);
    sccroll_run();
}

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

int main(void)
{
    sccroll_mockPredefined(run_test);
    return EXIT_SUCCESS;
}
