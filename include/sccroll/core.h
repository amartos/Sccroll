/**
 * @file        scroll/core.h
 * @version     0.1.0
 * @brief       Core header file of the Sccroll library.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 *
 * Units Tests using this library are mainly built using the
 * SCCROLL_TEST() macro. The units testing source files can be used in
 * two ways:
 *
 * - no main() function is defined, and tests are defined using only
 *   the SCCROLL_TEST() macro
 * - a main() function is defined, and tests are registered through
 *   the use of the SCCROLL_TEST() macro and/or sccroll_register();
 *   in this case the sccroll_run() function **must** be called to run
 *   the tests
 *
 * A units testing source file containing only definitions using
 * SCCROLL_TEST() is thus perfectly valid.
 *
 * @addtogroup API Sccroll API
 * @{
 * @addtogroup CoreAPI Tests creation, execution and reports
 * @{
 */

#ifndef SCCROLL_CORE_H_
#define SCCROLL_CORE_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "sccroll/helpers.h"
#include "sccroll/data.h"
#include "sccroll/lists.h"

#ifdef _SCCUNITTESTS
// Allows easier errors handling tests of the library.
// TODO: remove this dependency, implying an architecture redesign.
#include "sccroll/mocks.h"
#endif

#include <argz.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name Hooks
 *
 * These hooks are executed at precise moment predetermined around
 * each test.
 *
 * The prototypes of this section are only an interface, their
 * definition is left up to the user. Without such definition, they
 * have no effect.
 *
 * @internal
 * @note All are defined as weak aliases.
 * @endinternal
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief First function executed by sccroll_run(). It is executed
 * only once.
 */
void sccroll_init(void);

/**
 * @since 0.1.0
 * @brief Last function executed by sccroll_run(). It is executed only
 * once.
 */
void sccroll_clean(void);

/**
 * @since 0.1.0
 * @brief Function executed before each test. It is executed for each
 * registered test.
 */
void sccroll_before(void);

/**
 * @since 0.1.0
 * @brief Function executed after each test. It is executed for each
 * registered test.
 */
void sccroll_after(void);

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Unit tests creation.
 *
 * The structure SccrollEffects is the main struct of the library. It
 * is used to describe the test and its expected effects.
 *
 * The definition of SccrollEffects is facilitated by the
 * SCCROLL_TEST() macro.
 *
 * @{
 *****************************************************************************/
// clang-format on

/**
 * @enum SccrollIndexes
 * @since 0.1.0
 * @brief SccrollEffects tables index.
 */
typedef enum SccrollIndexes {
    SCCMAXSTD = STDERR_FILENO + 1, /**< SccrollEffects::std max index. */
    SCCMAX    = BUFSIZ,            /**< SccrollEffects::files max index. */
} SccrollIndexes;

/**
 * @enum SccrollFlags
 * @since 0.1.0
 * @brief Tests run options flags.
 * @attention The default for each test is "not any options".
 */
typedef enum SccrollFlags {
    NOSTRP = 1, /**< Do not strip left and right standard outputs. */
    NOFORK = 2, /**< Do not fork before executing the test. */
    NODIFF = 4, /**< Do no print diffs of expected/obtained. */
} SccrollFlags;

/**
 * @struct SccrollFile
 * @since 0.1.0
 * @brief Structure storing a file path and its content.
 */
typedef struct SccrollFile {
    const char* path; /**< The file path. */
    Data content;     /**< The file content. */
} SccrollFile;

/**
 * @enum SccrollCodeType
 * @since 0.1.0
 * @brief Error code types handled by SccrollEffects.
 */
typedef enum SccrollCodeType {
    SCCSIGNAL, /**< Signals. */
    SCCSTATUS, /**< Status codes. */
    SCCERRNUM, /**< Errno. */
} SccrollCodeType;

/**
 * @struct SccrollCode
 * @since 0.1.0
 * @brief Structure storing the error code type and value.
 */
typedef struct SccrollCode {
    SccrollCodeType type; /**< Type du code d'erreur. */
    int value;            /**< Valeur attendue. */
} SccrollCode;

/**
 * @struct SccrollEffects
 * @since 0.1.0
 * @brief Store units tests data.
 *
 * This structure is used to describe the expected side effects of a
 * unit test, including options for the test.
 *
 * This is a versatile structure, allowing to test one or many
 * functions in a single SccrollEffects::wrapper test function.
 *
 * ## Test registration and reports
 *
 * The function pointer is stored in SccrollEffects::wrapper. When
 * executed, if there is a discrepancy between observed effects and
 * expected, an error is raised. The test name SccrollEffects::name
 * and error description is printed on stderr.
 *
 * ## Standard IO
 *
 * Any string handled to the SccrollEffects::std structure is
 * copied and freed automatically. The original string is not freed by
 * the library, it is thus up to the user to handle it.
 *
 * The default library behavior is to strip whitespace left and right
 * of the SccrollEffects::std::content strings. To inhibit this
 * behavior, pass the #NOSTRP option to the corresponding test.
 *
 * A @c NULL value for both SccrollEffects::std::path and
 * SccrollEffects::std::content is considered a passing empty
 * strings.
 *
 * If a file path is given to SccrollEffects::std::path, the firsts
 * #SCCMAX-1 characters of the file are used as
 * SccrollEffects::std::content. If both a file path string and
 * content string are given to the structure,
 * SccrollEffects::std::content is overwritten for the path content.
 *
 * Any string stored in SccrollEffects::std::content at indexes
 * #STDOUT_FILENO and #STDERR_FILENO is compared to the corresponding
 * standard outputs of the test.
 *
 * The standard input for the test is simulated using
 * SccrollEffects::std at index #STDIN_FILENO.
 *
 * ## Error codes
 *
 * The structure can store only one type of error code for a given
 * test (see SccrollCodeTypes for the list). Errno won't be compared
 * if the test function exits by itself, thus be careful when
 * designing the test in this situation.
 *
 * ## Files side effects
 *
 * The SccrollEffects::files structure is used to indicate expected
 * files modifications. This array is expected to have a @c NULL
 * sentinel entry, after which any other entry is ignored.
 *
 * The file content is read from SccrollEffects::files::path as a
 * string, and compared as such. It thus stops at any null characters
 * the file contains. To indicate that the content is a bytes blob,
 * and should be compared as such (thus, not considering null
 * characters), indicate a content size in
 * SccrollEffects::files::size.
 *
 * ## Tests options
 *
 * All the available options for the tests are described in the
 * SccrollFlags enum. Multiple options can be or'ed set in
 * SccrollEffects::flags.
 *
 */
typedef struct SccrollEffects {
    SccrollFile files[SCCMAX];  /**< Files contents expected side effects. */
    SccrollFile std[SCCMAXSTD]; /**< Test expected standard IO. */
    SccrollCode code;     /**< Test expected error, signal or status codes. */
    SccrollFlags flags;   /**< Options flags for the test. */
    SccrollFunc wrapper;  /**< The test function wrapper pointer. */
    const char* name;     /**< The test name. */
} SccrollEffects;

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Tests registration
 *
 * The tests are registered two ways, which are **not** exclusive:
 * - passing a SccrollEffects struct to the sccroll_register() function
 * - direct definition of a test using the SCCROLL_TEST() macro
 *
 * @note Any SccrollWrapper registered multiple times, whatever
 * the registration method or parent SccrollEffects used, will be
 * executed as many times.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Registers a test.
 *
 * This function is only needed is a test must be registered
 * manually. In that case, the main() function would be redefined, and
 * thus the sccroll_run() function would also be needed.
 *
 * @param expected The test function and expected tests effects.
 */
void sccroll_register(const SccrollEffects* restrict expected)
    __attribute__((nonnull));

/**
 * @def SCCROLL_TEST
 * @since 0.1.0
 * @brief Define a unit test to run.
 *
 * This macro is the core of the library. It is used to define a unit
 * test and its expected effects, without needing any additional
 * function calls for registration and execution. Any test defined
 * with this macro is automatically registered and run. A units tests
 * source file containing only this macro (one for each test) is
 * perfectly valid --- neither sccroll_register() nor main()
 * calls are needed.
 *
 * The macro is used the same way any function would be defined.
 *
 * Its first argument is the SccrollWrapper function definition, which
 * is also used as the test name. It thus follows the syntax of
 * functions naming.
 *
 * The remaining arguments, if any, are used for the SccrollEffects
 * definition (except for the SccrollEffects::name and
 * SccrollEffects::wrapper values, which would be ignored). The syntax
 * is exactly the same as for any struct initialisation.
 *
 * Here are some basic test definition examples using this macro:
 *
 * @example SCCROLL_TEST.c
 *
 * @param testname The test wrapper name also used as the test name.
 * @param ... The remaining SccrollEffects data. If none is given, all
 * values are initialized at @c 0.
 */
#define SCCROLL_TEST(testname, ...)                                            \
    static void testname(void);                                                \
    __attribute__((constructor)) static void sccroll_register_##testname(void) \
    {                                                                          \
        const SccrollEffects expected = {                                      \
            .wrapper = testname,                                               \
            .name    = #testname,                                              \
            ##__VA_ARGS__                                                      \
        };                                                                     \
        sccroll_register(&expected);                                           \
    }                                                                          \
    static void testname(void)

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Tests execution
 *
 * Any units testing file using only SCCROLL_TEST() do not need any
 * "run the tests" call. The executable compiled from the units
 * testing file is self-sufficient.
 *
 * In the case there is a need to control the registration or
 * execution, or a main() function is defined in any file linked, the
 * library main function would thus be ignored. The following
 * functions are then needed to properly run the tests.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Run the registered units tests.
 * @attention This function is used in a redefined main() to launch
 * the tests execution and reports. It is not needed if the library
 * main() is used.
 * @return The total number of failed tests.
 */
int sccroll_run(void);

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CORE_H_
/** @} @} */
