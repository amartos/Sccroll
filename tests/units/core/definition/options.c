/**
 * @file        options.c
 * @version     0.1.0
 * @brief       Core module unit tests for options.
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

// Strings compared during the test.
#define strpstdout "This text is surrounded with spaces."
#define strpstderr "The left side is trimmed.        But not the center."
#define nostrpstdout " \n\n\t \t  " strpstdout "      \n\n\t"
#define nostrpstdoutnewline nostrpstdout "\n"
#define nostrpstderr "  " strpstderr
#define errstr "difference observed.\nother line\nanother line"

static int count = 0;

// fake unit test printing on stdout/stderr
void test_dummy(void)
{
    ++count;
    puts(nostrpstdout);
    fprintf(stderr, "%s", nostrpstderr);
}

// Expected effects for the fake unit test
static SccrollEffects test = {
    .wrapper = test_dummy,
    .std = {
        [STDOUT_FILENO].content.blob = strpstdout,
        [STDERR_FILENO].content.blob = strpstderr,
    },
};

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

// Individual options

void test_nostrp(void)
{
    test.name = "test_nostrp";
    sccroll_register(&test);

    SccrollEffects test_opt = test;
    test_opt.flags = NOSTRP;
    test_opt.std[STDOUT_FILENO].content.blob = nostrpstdoutnewline;
    test_opt.std[STDERR_FILENO].content.blob = nostrpstderr;
    sccroll_register(&test_opt);
    assert(!sccroll_run());
}

void test_nofork(void)
{
    test.name = "test_nofork";
    sccroll_register(&test);

    SccrollEffects test_opt = test;
    test_opt.flags = NOFORK;
    // NOFORK should not stop the tests in case of one test failure.
    test_opt.std[STDOUT_FILENO].content.blob = errstr;
    sccroll_register(&test_opt);
    assert(sccroll_run() == 1);
    assert(count == 1);
}

void test_nodiff(void)
{
    SccrollEffects test_opt = test;
    test_opt.name = "test_nodiff";
    test_opt.std[STDOUT_FILENO].content.blob = errstr;
    test_opt.std[STDERR_FILENO].content.blob = strpstdout;
    sccroll_register(&test_opt);
    test_opt.flags = NODIFF;
    sccroll_register(&test_opt);
    assert(sccroll_run() == 2);
}

// Multiple options

void test_integration(void)
{
    SccrollEffects test_opt = test;
    test_opt.name = "test_all";
    test_opt.flags = ~0;

    test_opt.std[STDOUT_FILENO].content.blob = nostrpstdoutnewline;
    test_opt.std[STDERR_FILENO].content.blob = errstr;

    sccroll_register(&test_opt);
    assert(sccroll_run() == 1);
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{

    test_nostrp();
    test_nofork();
    test_nodiff();
    test_integration();

    return EXIT_SUCCESS;
}
