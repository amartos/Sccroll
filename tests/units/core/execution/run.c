/**
 * @file        run.c
 * @version     0.1.0
 * @brief       Core module unit tests for tests execution and reports.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 */

#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

enum {
    MAXF = 10, // max number of failed tests
    MAXS = 42, // max number of successful tests
};

// should not be modified by the runs.
// TODO: check if this is really necessary, as the tests are forked.
int zero = 0;

// simple test printing on stderr. Its line number is stored to help
// the test.
const int test_line = __LINE__ + 1;
void test_print(void) { (zero = 42, assert(false && "Test executed.")); }

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    // Set the expected error message. The string is formatted to
    // avoid to recalculate the line at each change in the file.
    const char* fmt = "run: %s:%i: test_print: Assertion `false && \"Test executed.\"' failed.";
    char errmsg[SCCMAX] = { 0 };

    sprintf(errmsg, fmt, __FILE__, test_line);
    SccrollEffects testf = { .wrapper = test_print, .name = "test_print_fail" };
    SccrollEffects tests = {
        .wrapper = test_print,
        .name = "test_print_success",
        .std[STDERR_FILENO].content.blob = errmsg,
        .code = { .type = SCCSIGNAL, .value = SIGABRT },
    };
    for (int i = 0; i < MAXF; ++i) sccroll_register(&testf);
    for (int i = 0; i < MAXS; ++i) sccroll_register(&tests);
    assert(sccroll_run() == MAXF);
    // Check that there are no more tests to run.
    assert(sccroll_run() == 0);
    // Check runs side-effects on this variable.
    assert(!zero);
    return EXIT_SUCCESS;
}
