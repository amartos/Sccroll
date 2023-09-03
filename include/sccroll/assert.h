/**
 * @file        assert.h
 * @version     0.1.0
 * @brief       Assertions functions and macros.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * @addtogroup API
 * @{
 * @addtogroup AssertAPI Assertions functions and macros
 *
 * The library defines its own assert() macros and overrides the
 * C library standard assert() macro in two cases:
 *
 * - @c assert.h is not included
 * - @c assert.h is included but @c NDEBUG is defined.
 *
 * This ensures that all the units tests assertions are executed in
 * all cases; this also renders the units tests insensible to the
 * definition of @c NDEBUG.
 *
 * assert(), while mimicking its C library sibling, prints a different
 * error message.
 *
 * The assertions of this module also ensure that all the coverage
 * data produced by for gcov are dumped before the abort() call. The
 * sccroll_abort() function is specifically designed for this. In the
 * case that the assert() macro used is the one of the C library, a
 * mock of abort() must be defined (see the mocks module for one).
 * @{
 */

#ifndef SCCROLL_ASSERT_H_
#define SCCROLL_ASSERT_H_

#include "sccroll/helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// clang-format off

/******************************************************************************
 * @name Coverage data dump
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Gcov dumping function.
 */
extern void __gcov_dump(void);

// clang-format off

/******************************************************************************
 * @}
 * @name Assertion messages formatting
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def SCCASSERTFMT
 * @since 0.1.0
 * @brief Assertions message format.
 * @see sccroll_assert()
 * @param s The file name.
 * @param i The line number.
 * @param s The function name.
 * @param s The tested expression.
 */
#define SCCASSERTFMT "%s (l. %i): Assertion `%s' failed."

// clang-format off

/******************************************************************************
 * @}
 * @name Errors handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Print a message on stderr and raise an error signal.
 * @param sigint The signal to raise (@c SIGABRT if the given signal
 * is ignored).
 * @param fmt The format string of the message.
 * @param args The format string arguments.
 * @throw @p sigint, or #SIGABRT if the @p sigint signal is ignored
 */
void sccroll_vfatal(int sigint, const char* restrict fmt, va_list args)
    __attribute__((noreturn,format(printf,2,0)));

/**
 * @since 0.1.0
 * @brief Print a message on stderr and raise an error signal.
 * @param sigint The signal to raise (@c SIGABRT if the given signal
 * is ignored).
 * @param fmt The format string of the message.
 * @param ... The format string arguments.
 * @throw @p sigint, or #SIGABRT if the @p sigint signal is ignored
 */
void sccroll_fatal(int sigint, const char* restrict fmt, ...)
    __attribute__((noreturn, format(printf,2,3)));

/**
 * @name Try-Catch-Finally
 *
 * These macro allow to catch errors. They may be nested, and multiple
 * catch used at the same level if the identifier is unique enough
 * (they are labels). Brackets around the try-catch codes are
 * optional for one-liners.
 *
 * For every catch(), a throw() is necessary. try() can be used alone,
 * even if useless in that case.
 *
 * @alert These macro **do not use setjmp() or longmp()**.
 * @todo add examples.
 * @{
 *
 * @param id The try() unique level identifier passed to throw(),
 * catch() and finally().
 * @param error A throw() and catch() unique error identifier.
 */

/**
 * @def try
 * @since 0.1.0
 * @brief Start a try-catch block.
 * @param id A unique identifier and local to the function.
 * @note This macro does nothing by itself.
 */
#define try(id)

/**
 * @def throw
 * @since 0.1.0
 * @brief Raise an error.
 */
#define throw(id,error) goto id ## error;

/**
 * @since 0.1.0
 * @brief Catch an error and execute a block of code.
 */
#define catch(id,error) goto id ## finally; id ## error:

/**
 * @since 0.1.0
 * @brief Execute a block of code after any try() or catch().
 */
#define finally(id)     id ## finally:
/** @} */

// clang-format off

/******************************************************************************
 * @name Assertions with custom error messages
 *
 * These macros and funtions raise an assertion error but do generate
 * any message by themselves.
 *
 * @{
 * @param expr Boolean expression that raise an assertion error if @c false.
 * @param fmt The message format string.
 * @param ... The format string parameters.
 * @throw #SIGABRT if @p expr is @c false.
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 */
void sccroll_assert(int expr, const char* restrict fmt, ...)
    __attribute__((nonnull (2), format(printf,2,3)));

/**
 * @def assertMsg
 * @since 0.1.0
 * @brief Macro alias of sccroll_assert().
 */
#define assertMsg sccroll_assert

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Assertions
 *
 * These assertions raise an assertion error if the given expression
 * is @c false and print an error message.
 *
 * @{
 * @throw #SIGABRT if @p expr is @c false.
 ******************************************************************************/
// clang-format on

#if !defined(_ASSERT_H) || (defined(_ASSERT_H) && defined(NDEBUG))
#undef assert

    /**
     * @def assert
     * @since 0.1.0
     * @brief Assertion macro insensible to #NDEBUG
     *
     * This macro is not loaded if the @c assert.h header is included
     * and #NDEBUG is not defined.
     * @param expr Boolean expression that raise an assertion error if @c false.
     */
    #define assert(expr)             \
        sccroll_assert((bool)(expr), \
            SCCASSERTFMT, __FILE__, __LINE__, #expr)
#endif // _ASSERT_H

/**
 * @def assertTrue
 * @since 0.1.0
 * @brief Alias of assert().
 * @param expr Boolean expression that raise an assertion error if @c false.
 */
#define assertTrue assert

/**
 * @def assertFalse
 * @since 0.1.0
 * @brief Assert that the expression is @c false.
 * @param expr Boolean expression that raise an assertion error if @c true.
 */
#define assertFalse(expr) assertTrue(!(expr))

/**
 * @def assertNot
 * @since 0.1.0
 * @brief Alias of assertFalse().
 * @param expr Boolean expression that raise an assertion error if @c true.
 */
#define assertNot assertFalse

/**
 * @def assertNull
 * @since 0.1.0
 * @brief Alias of assertFalse().
 * @param expr Boolean expression that raise an assertion error if @c true.
 */
#define assertNull assertFalse

/**
 * @def assertEql
 * @since 0.1.0
 * @brief Assert that @code a == b @endcode
 * @param a,b The variables or values to compare.
 */
#define assertEql(a, b) assert(a == b)

/**
 * @def assertNotEql
 * @since 0.1.0
 * @brief Assert that @code a != b @endcode
 * @param a,b The variables or values to compare.
 */
#define assertNotEql(a, b) assert(a != b)
/** @} */

// clang-format off

/******************************************************************************
 * @}
 * @name Assertions on data comparison
 *
 * @brief These macros and fonctions raise an assertion error if the
 * comparison of the data pointed by the given pointers is false.
 *
 * @alert The compared values are expected to be the data pointed by
 * the given pointer, not the pointers themselves.
 *
 * @{
 * @param a,b Pointers to data to compare.
 * @param sign Either @c <, @c > or @c ==.
 * @param cmp Comparison function taking @p a and @p b as parameters
 * and returning an integer, similar to the qsort() compare functions.
 * @param ... Additional optional arguments for the @p cmp function.
 * @throw SIGABRT si la comparaison attendue est fausse
 ******************************************************************************/
// clang-format on

/**
 * @def assertCmp
 * @since 0.1.0
 * @brief Assert the @code a sign b @endcode comparison.
 */
#define assertCmp(a, sign, b, cmp, ...) assert(cmp(a, b, ##__VA_ARGS__) sign 0)

/**
 * @def assertEqual
 * @since 0.1.0
 * @brief Assert the @code a == b @endcode comparison.
 */
#define assertEqual(a, b, cmp, ...) assertCmp(a, ==, b, cmp, ##__VA_ARGS__)

/**
 * @def assertNotEqual
 * @since 0.1.0
 * @brief Assert the @code a != b @endcode comparison.
 */
#define assertNotEqual(a, b, cmp, ...) assertCmp(a, !=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmaller
 * @since 0.1.0
 * @brief Assert the @code a < b @endcode comparison.
 */
#define assertSmaller(a, b, cmp, ...) assertCmp(a, <, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreater
 * @since 0.1.0
 * @brief Assert the @code a > b @endcode comparison.
 */
#define assertGreater(a, b, cmp, ...) assertCmp(a, >, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmallerOrEqual
 * @since 0.1.0
 * @brief Assert the @code a <= b @endcode comparison.
 */
#define assertSmallerOrEqual(a, b, cmp, ...) assertCmp(a, <=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreaterOrEqual
 * @since 0.1.0
 * @brief Assert the @code a >= b @endcode comparison.
 */
#define assertGreaterOrEqual(a, b, cmp, ...) assertCmp(a, >=, b, cmp, ##__VA_ARGS__)

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_ASSERT_H_
/** @} @} */
