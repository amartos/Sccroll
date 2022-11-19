/**
 * @file        sccroll_tests.c
 * @version     0.1.0
 * @brief       Fichier source des tests unitaires de la librairie Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 *     sccroll.c sccroll_tests.c      \
 *     -o sccroll_tests
 * @endcode
 *
 * @addtogroup Sccroll
 *
 * @{
 */

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * @addtogroup UnitTests
 *
 * @{
 ******************************************************************************/
// clang-format on

// clang-format off

/******************************************************************************
 * @addtogroup TestPrep
 *
 * @{
 ******************************************************************************/
// clang-format on

static int init   = 0;
static int clean  = 0;
static int before = 0;
static int after  = 0;

void sccroll_init(void)
{
    assert(init + clean + before + after == 0);
    ++init;
}

void sccroll_clean(void)
{
    assert(init == 1 && clean == 0 && before > 0 && before == after);
    fprintf(stderr, "%s called. All prep functions are OK.\n", __func__);
    fprintf(stderr, "This line must be outputed LAST.\n");
}

void sccroll_before(void)
{
    assert(before == after);
    ++before;
}

void sccroll_after(void)
{
    assert(before > 0 && after == before-1);
    ++after;
}

// clang-format off

/******************************************************************************
 * @} (TestPrep)
 *
 * @addtogroup Definition
 *
 * @{
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(test_exe) { puts("success on stdout"); }
// uncomment to fail "invisible output" tests with the makefile.
// SCCROLL_TEST(test_error_output) { puts("invisible: failed on stdout"); }
SCCROLL_TEST(test_output) { fprintf(stderr, "success on stderr: invisible"); }

// All tests below will fail.
SCCROLL_TEST(test_fail) { assert(false); }
void test_not_registered_with_macro(void) { assert(false); }
void other_test_not_registered_with_macro(void) { assert(false); }
static int will_fail = 3; // do change when changing the list of failing tests

int main(int argc, char* argv[])
{
    sccroll_register(test_not_registered_with_macro, "A custom test name.");
    sccroll_register(other_test_not_registered_with_macro, "Another test.");

    assert(sccroll_run() == will_fail);
    // uncomment to fail "last line" tests with the makefile.
    // puts("error");
    return EXIT_SUCCESS;
}

/******************************************************************************
 * @}
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
