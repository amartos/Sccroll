/**
 * @file        assert.c
 * @version     0.1.0
 * @brief       Assertions unit tests.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

static const char* a = "foo";
static const char* b = "foo";
static const char* c = "bar";

static const int testia[10] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

static const int testib[10] = {
    -9, -8, -7, -6, -5, -4, -3, -2, -1, 0
};

static const int testic[10] = { 0 };

// Compare two integers
int intcmp(int a, int b) { return a == b ? 0 : a < b ? -1 : 1; };

// Defined here to test the insesibility of the library macro.
#define NDEBUG

// clang-format off

/******************************************************************************
 * Unit tests
 ******************************************************************************/
// clang-format on

// 1/2 tests should fail.
enum {
    FAILED = 12,
};

SCCROLL_TEST(test_sccroll_assert_success) { sccroll_assert(true, "invisible line"); }
SCCROLL_TEST(test_sccroll_assert_fail) { sccroll_assert(false, "this test must fail successfully"); }
SCCROLL_TEST(test_libassert_success) { assert(true); }
SCCROLL_TEST(test_libassert_fail) { assert(false); }
SCCROLL_TEST(test_assertFalse_success) { assertFalse(false); }
SCCROLL_TEST(test_assertFalse_fail) { assertFalse(true); }
SCCROLL_TEST(test_assertEql_success) { assertEql(a, b); }
SCCROLL_TEST(test_assertEql_fail) { assertEql(a, c); }
SCCROLL_TEST(test_assertNotEql_success) { assertNotEql(a, c); }
SCCROLL_TEST(test_assertNotEql_fail) { assertNotEql(a, b); }
SCCROLL_TEST(test_assertCmp_success) { assertCmp(testia[0], >=, testib[0], intcmp); }
SCCROLL_TEST(test_assertCmp_fail) { assertCmp(testia[0], ==, testib[0], intcmp); }
SCCROLL_TEST(test_assertEqual_success) { assertEqual(testia[0], testib[9], intcmp); }
SCCROLL_TEST(test_assertEqual_fail) { assertEqual(testia[0], testib[0], intcmp); }
SCCROLL_TEST(test_assertNotEqual_success) { assertNotEqual(testia[3], testib[2], intcmp); }
SCCROLL_TEST(test_assertNotEqual_fail) { assertNotEqual(testia[0], testib[9], intcmp); }
SCCROLL_TEST(test_assertGreater_success) { assertGreater(testia[0], testib[0], intcmp); }
SCCROLL_TEST(test_assertGreater_fail) { assertGreater(testia[0], testia[1], intcmp); }
SCCROLL_TEST(test_assertSmaller_success) { assertSmaller(testib[3], testia[2], intcmp); }
SCCROLL_TEST(test_assertSmaller_fail) { assertSmaller(testia[0], testib[9], intcmp); }
SCCROLL_TEST(test_assertGreaterOrEqual_fail) { assertGreaterOrEqual(testia[0], testia[1], intcmp); }
SCCROLL_TEST(test_assertSmallerOrEqual_fail) { assertSmallerOrEqual(testia[0], testib[8], intcmp); }
SCCROLL_TEST(test_assertGreaterOrEqual_success) {
    assertGreaterOrEqual(testia[0], testib[0], intcmp);
    assertGreaterOrEqual(testia[0], testib[9], intcmp);
}

SCCROLL_TEST(test_assertSmallerOrEqual_success) {
    assertSmallerOrEqual(testib[3], testia[2], intcmp);
    assertSmallerOrEqual(testia[0], testic[0], intcmp);
}

// After this point, no test should fail

SCCROLL_TEST(test_assert_array)
{
    assertEqual(testia, testia, memcmp, sizeof(testia));
    assertNotEqual(testia, testic, memcmp, sizeof(testia));
    assertNotEqual(testia, testib, memcmp, sizeof(testia));
    assertGreater(testia, testic, memcmp, sizeof(testia));

    // memcmp compares is used to compare signed integers, thus a
    // negative number will be higher in this case.
    assertSmaller(testia, testib, memcmp, sizeof(testia));
}

SCCROLL_TEST(test_assert_str)
{
    assertEqual("foo", "foo", strcmp);
    assertNotEqual("foo", "bar", strcmp);
}

// Test both sccroll_fatal and sccroll_vfatal
SCCROLL_TEST(
    test_fatal,
    .code = {.type = SCCSIGNAL, .value = SIGABRT},
    .std  = {
        [STDERR_FILENO] = {.content.blob = "successfully crashed"}
    },
)
{ sccroll_fatal(SIGABRT, "successfully %s", "crashed"); }

SCCROLL_TEST(
    test_fatal_other_code,
    .code = {.type = SCCSIGNAL, .value = SIGTERM},
    .std  = {
        [STDERR_FILENO] = {.content.blob = "successfully terminated"}
    },
)
{ sccroll_fatal(SIGTERM, "successfully %s", "terminated"); }

SCCROLL_TEST(
    test_fatal_nomsg,
    .code = {.type = SCCSIGNAL, .value = SIGABRT},
)
{ sccroll_fatal(SIGABRT, NULL); }

SCCROLL_TEST(
    try_only,
    .std  = {
        [STDOUT_FILENO] = {.content.blob = "try executed\nout of try\n"}
    }
)
{
    try(test) { puts("try executed"); }
    puts("out of try");
}

SCCROLL_TEST(
    try_catch_throw_finally,
    .std  = {
        [STDOUT_FILENO] = {
            .content.blob =
            "try executed\n"
            "catch executed\n"
            "second error catched\n"
            "finally executed\n"
        }
    }
)
{
    try(test) {
        puts("try executed");
        throw(test, 18);
        assertMsg(false, "thow did not break flow !");
    }
    catch(test, 36) { puts("second error catched"); }
    catch(test, 18) {
        puts("catch executed");
        throw(test, 36);
    }
    finally(test) { puts("finally executed"); }
}

SCCROLL_TEST(
    try_catch_test_nested,
    .std  = {
        [STDOUT_FILENO] = {
            .content.blob =
            "parent try\n"
            "first child try\n"
            "second child try\n"
            "second child catch\n"
            "second child finally\n"
            "first child catch\n"
            "parent catch\n"
            "parent finally\n"
            "OK\n"
        }
    }
)
{
    try(parent) {
        puts("parent try");
        try(first) {
            puts("first child try");
            try(second) {
                puts("second child try");
                throw(second, 42);
            }
            catch(second, 42) { puts("second child catch"); }
            finally(second) {
                puts("second child finally");
                throw(first, 42);
            }
        }
        catch(first, 42) {
            puts("first child catch");
            throw(parent, 42);
        }
        finally(first)
            assertMsg(false, "this part should not be executed");
    }
    catch(parent, 42) { puts("parent catch"); }
    finally(parent) { puts("parent finally"); }

    puts("OK");
}

// clang-format off
/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    assert(sccroll_run() == FAILED);
    return EXIT_SUCCESS;
}
