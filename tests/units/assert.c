/**
 * @file        assert.c
 * @version     0.1.0
 * @brief       Tests unitaires des assertions.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/assert.c -o build/objs/tests/units/assert.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/assert.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/assert
 * @endcode
 */

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparation des tests.
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

// Compare deux entiers et renvoie:
// 0  si a == b
// 1  si a > b
// -1 si a < b
int intcmp(int a, int b) { return a == b ? 0 : a < b ? -1 : 1; };

// La macro assert de la librairie ne devrait pas être sensible à la
// définition de cette macro, au contraire de celle de la librairie C.
#define NDEBUG

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

// 1 test /2 en échec.
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

// 100% de tests réussis.

SCCROLL_TEST(test_assert_array)
{
    assertEqual(testia, testia, memcmp, sizeof(testia));
    assertNotEqual(testia, testic, memcmp, sizeof(testia));
    assertNotEqual(testia, testib, memcmp, sizeof(testia));
    assertGreater(testia, testic, memcmp, sizeof(testia));

    // Ici memcmp compare des entiers signés, mais utilise une
    // comparaison de char non signés. Du fait du bit de signe, les
    // nombres négatifs sont mécaniquement plus élevés pour la
    // fonction memcmp
    assertSmaller(testia, testib, memcmp, sizeof(testia));
}

SCCROLL_TEST(test_assert_str)
{
    assertEqual("foo", "foo", strcmp);
    assertNotEqual("foo", "bar", strcmp);
}

// Teste à la fois sccroll_fatal et sccroll_vfatal
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

// clang-format off
/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    assert(sccroll_run() == FAILED);
    return EXIT_SUCCESS;
}
