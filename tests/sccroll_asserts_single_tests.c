/**
 * @file        sccroll_asserts_single_tests.c
 * @version     0.1.0
 * @brief       Tests unitaires des assertions simples.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/sccroll_asserts_single_tests.c -L build/libs -l scroll \
 * -o build/bin/sccroll_basics_tests
 * @encode
 *
 * @addtogroup Sccroll
 * @{
 * @addtogroup UnitTests
 * @{
 */

#include "sccroll.h"

SCCROLL_TEST(test_assertTrue_simple)
{
    assertTrue(true);
    assertTrue(!false);
    assertTrue(!NULL);
    assertTrue(!0);
    assertTrue(-12356);
    assertTrue(23698);
}

SCCROLL_TEST(test_assertTrue_operation)
{
    assertTrue(true == true);
    assertTrue((!false) == true);
    assertTrue(325 > 123);
    assertTrue(!strcmp("test", "test"));
    assertTrue(strcmp("foo", "bar"));
}

SCCROLL_TEST(test_assertFalse_simple)
{
    assertFalse(!true);
    assertFalse(false);
    assertFalse(NULL);
    assertFalse(0);
    assertFalse(!-12356);
    assertFalse(!23698);
}

SCCROLL_TEST(test_assertFalse_operation)
{
    assertFalse(true != true);
    assertFalse((!false) != true);
    assertFalse(325 < 123);
    assertFalse(strcmp("test", "test"));
    assertFalse(!strcmp("foo", "bar"));
}

SCCROLL_TEST(test_assertTrue_fail_simple) { assertTrue(false); }
SCCROLL_TEST(test_assertTrue_fail_operation) { assertTrue(true == false); }
SCCROLL_TEST(test_assertFalse_fail_simple) { assertFalse(true); }
SCCROLL_TEST(test_assertFalse_fail_operation) { assertFalse(false == false); }
SCCROLL_TEST(test_assertMsg_fail) { assertMsg(123 > 456, "%s %i %i", "foo", 1, -36); }

enum {
    FAILERS = 5,
};

int main(void)
{
    assert(sccroll_run() == FAILERS);
    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/

