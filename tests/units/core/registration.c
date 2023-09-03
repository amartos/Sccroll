/**
 * @file        registration.c
 * @version     0.1.0
 * @brief       Core module unit tests for tests registration.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 */

#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparations
 ******************************************************************************/
// clang-format on

enum {
    MAXT = 10, // max number of tests to register
};

// auto-registration
SCCROLL_TEST(test_success) {}

SCCROLL_TEST(test_fail_auto) { assert(false); }

// register through sccroll_register
void test_fail_manual(void) { assert(false); }

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    // Register manually.
    SccrollEffects test = {
        .wrapper = test_fail_auto,
        .name = "test_fail_auto manually registered"
    };
    sccroll_register(&test);

    test.wrapper = test_fail_manual;
    const char* prefix = "cutom test name:";
    char name[BUFSIZ] = { 0 };
    // register multiple times the same test, with different names.
    for (int i = 0; i < MAXT; ++i) {
        sprintf(name, "%s %i", prefix, i);
        test.name = strdup(name);
        sccroll_register(&test);
        memset(name, 0, strlen(name));
    }

    assert(sccroll_run() == MAXT+2);
    return EXIT_SUCCESS;
}
