/**
 * @file        preparation.c
 * @version     0.1.0
 * @brief       Core module unit tests of tests peparation.
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

// Constants
enum {
    MAX_PREP = 100, // Max number of iterations
};

// Count the number of calls for each function.
static int init   = 0;
static int clean  = 0;
static int before = 0;
static int after  = 0;

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

void sccroll_init(void) {  ++init; }
void sccroll_clean(void) { ++clean; }
void sccroll_before(void) { ++before; }
void sccroll_after(void) { ++after; }

void test_prepfuncs(void)
{
    // init and clean should be called only once, at the start and end
    // of all the tests.
    assert(init == 1 && clean == init-1);

    // before and after are called at each test.
    assert(before > 0 && after == before-1);
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    SccrollEffects test = {
        .name = "test_prepfuncs",
        .wrapper = test_prepfuncs,
    };

    for (int i = 0; i < MAX_PREP; ++i) sccroll_register(&test);
    assert(!sccroll_run());
    assert(init == clean && init == 1);
    assert(before == after && before == MAX_PREP);
    return EXIT_SUCCESS;
}
