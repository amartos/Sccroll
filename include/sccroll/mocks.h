/**
 * @file        mocks.h
 * @version     0.1.0
 * @brief       Mocks definition.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * @todo Rename the structures and functions.
 *
 * @addtogroup API
 * @{
 * @addtogroup MocksAPI Mocks definitions and usage.
 *
 * Mocks do not have the same definition in C than in OO languages.
 *
 * Here, a mock is a function (not an object) that replaces an already
 * defined function. This is done to allow fine control of the mocked
 * function behavior in tests that depend on it.
 *
 * This module defines some mocks for common functions in the C
 * library, either for usage, or because their behavior interferes
 * with the Sccroll library execution (notably abort(), which prevents
 * coverage dumps if not mocked).
 * @{
 */

#ifndef SCCROLL_MOCKS_H_
#define SCCROLL_MOCKS_H_

#include "sccroll/helpers.h"
#include "sccroll/assert.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <search.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name Mocks definition
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_unused
 * @since 0.1.0
 * @brief Indicate an unused function argument.
 * @attention This macro should be called as early as possible in the
 * mock function.
 * @param var An unused function argument.
 */
#define sccroll_unused(var) (void) var

/**
 * @def SCCROLL_MOCK
 * @since 0.1.0
 * @brief Define a mock function.
 *
 * Two definitions must be done to generate a mock.
 *
 * The first one is using this macro. It defines the mock function and
 * its behavior. The arguments passed to the macro are necessary for
 * the proper new mock execution and triggers.
 *
 * The second one is a function-to-mock replacement macro to be
 * defined in a header included in any source code to be tested. This
 * macro be named as the original function and call instead the
 * corresponding mock defined with SCCROLL_MOCK().
 *
 * The syntax would thus be:
 * @code #define fonction sccroll_mockfonction @endcode
 *
 * Defining mocks this way thus ensures that the mocks do not leak to
 * other parts of the code they are meant to be used in (as, for
 * examples, in linked libraries). Moreover, surrounding the
 * redefinition with @c #ifdef would allow to fine-control the mock
 * definition in each module, or even in only parts of a source file.
 *
 * @todo Improve mocks definitions, in particular the original
 * arguments handling.
 * @todo Add examples in documentation.
 *
 * @param expr A boolean expression which, when @c true, makes the
 * mock return @p errval instead of the original function return
 * value.
 * @param errval A value to return if @p expr is @c true.
 * @param retval The original function return type.
 * @param name The original function name.
 * @param protoargs All the original function prototype arguments
 * separated by #SCCCOMMA. The syntax would thus be
 * @code
 * protoargType protoargName SCCCOMMA protoargType protoargName SCCCOMMA ...
 * @endcode
 * @param ... The original function parameters names without the types
 * (@c protoargName). The @c void corresponding value is empty.
 * @return @p errval if @p expr is @c true, otherwise the value
 * returned by the original function.
 */
#define SCCROLL_MOCK(expr, errval, retval, name, protoargs, ...)        \
    __typeof__(name) (*lib##name) = NULL;                               \
    retval sccroll_mock##name(protoargs)                                \
    {                                                                   \
        if (!lib##name) {                                               \
            lib##name = dlsym(RTLD_NEXT, #name);                        \
            if (!lib##name) err(EXIT_FAILURE, "%s", dlerror());         \
        }                                                               \
        return (expr) ? errval : lib##name(__VA_ARGS__);                \
    }

// clang-format off

/******************************************************************************
 * @}
 * @name Predefined mocks
 *
 * This module predefines some mocks of common C library
 * functions. The trigger can be done by calling sccroll_mockTrigger(),
 * which can also delay the trigger by a given number of mock calls.
 *
 * If a predefined mock raises an error, but this error is not handled
 * by exiting the program, a fatal error is automatically raised.
 *
 * @todo split module in two: allow mocks definition without
 * predefined mocks.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Codes used by sccroll_mockTrigger() to indicate the
 * predefined mock to trigger.
 * @alert Some triggers may trigger other mocks too, where indicated.
 * @todo rename this enum, "flags" is misleading.
 */
typedef enum SccrollMockFlags {
    SCCENONE = 0, /**< No triggger. */
    SCCEMALLOC,   /**< Triggers malloc(). */
    SCCECALLOC,   /**< Triggers calloc(). */
    SCCEPIPE,     /**< Triggers pipe(). */
    SCCEFORK,     /**< Triggers fork(). */
    SCCEDUP2,     /**< Triggers dup2(). */
    SCCECLOSE,    /**< Triggers close(). */
    SCCEREAD,     /**< Triggers read(). */
    SCCEWRITE,    /**< Triggers write(). */
    SCCEFERROR,   /**< Triggers ferror(). */
    SCCEFOPEN,    /**< Triggers fopen() and ferror(). */
    SCCEFSEEK,    /**< Triggers fseek() and ferror(). */
    SCCEFTELL,    /**< Triggers ftell() and ferror(). */
    SCCEFREAD,    /**< Triggers fread() and ferror(). */
    SCCEFWRITE,   /**< Triggers fwrite() and ferror(). */
    SCCEFSCANF,   /**< Triggers fscanf() and ferror(). */
    SCCEFILENO,   /**< Triggers fileno(). */
    SCCEHCREATE,  /**< Triggers hcreate(). */
    SCCEHSEARCH,  /**< Triggers hsearch(). */
    SCCEMAX,      /**< Max SccrollMockFlags value. */
} SccrollMockFlags;

/**
 * @since 0.1.0
 * @brief Trigger a predefined mock.
 * @attention In the case that the tests use the #NOFORK option, or
 * that the trigger is set outside of a fork, the sccroll_mockFlush()
 * function should be used after the trigger tests to ensure no
 * additional side effects. This is done automatically if the
 * predefined mocks raise an error by themselves.
 * @param mock The code of the predefined mock to trigger.
 * @param delay The number of calls to skip before the trigger (@c 0
 * meaning "trigger at the first mock call").
 */
void sccroll_mockTrigger(SccrollMockFlags mock, unsigned delay);

/**
 * @since 0.1.0
 * @brief Give the predefined mock code scheduled to trigger.
 * @return The scheduled predefined mock SccrollMockFlags code.
 */
SccrollMockFlags sccroll_mockGetTrigger(void);

/**
 * @since 0.1.0
 * @brief Give the remaining number of calls that will be skipped
 * before a trigger.
 * @return The number of calls that will be skipped before the mock
 * trigger. A negative number indicates the number of calls done
 * *after* the trigger.
 */
int sccroll_mockGetCalls(void);

/**
 * @since 0.1.0
 * @brief Disable the predefined mock trigger.
 * @attention The predefined mock has to be registered again to
 * restore the trigger.
 */
void sccroll_mockFlush(void);

/**
 * @since 0.1.0
 * @brief Test errors handling of all predefined mocks.
 *
 * This function schedules a predefine mock and then calls the @p
 * wrapper function. If the wrapper does not handle the error value
 * returned, a fatal error is raised. Each predefined mock is tested,
 * one by one.
 *
 * The @p wrapper calls are all done in a fork. The
 * sccroll_mockPredefined() propagates any signal raised in the
 * wrapper, which end the test run. Non-null exit status codes are
 * also propagated but only in the case of no mock trigger
 * (#SCCENONE). Signals have priority over exit status codes.
 *
 * Mocks trigger delays are tested by this function. The first call
 * that makes the wrapper exit without errors is considered as the
 * latest call of the mock in the wrapper.
 *
 * @param wrapper Le wrapper de la fonction à tester.
 */
void sccroll_mockPredefined(SccrollFunc wrapper) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Give the original function name of a given mock code.
 * @param mock The mock code.
 * @return The name of the original function corresponding to @p mock,
 * or "none" if no one corresponds. The returned string is **not**
 * malloc'ed, and thus must not be freed.
 */
const char* sccroll_mockName(SccrollMockFlags mock) __attribute__((returns_nonnull));

// clang-format off

/******************************************************************************
 * @}
 * @name Predefined mocks prototypes and macros definition.
 *
 * The macros and functions of this section are not supposed to be
 * used directly, and serves only as definitions to include in the
 * tested source code.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_mockPrototype
 * @since 0.1.0
 * @brief Define the mocks prototypes.
 * @param name The original function name.
 */
#define sccroll_mockPrototype(name)           \
    extern __typeof__(name) (*lib##name);     \
    __typeof__(name) sccroll_mock##name

/**
 * @since 0.1.0
 * @brief Stores information on the last mock call.
 * @param source  @c __FILE__.
 * @param funcname @c __FUNCTION__.
 * @param line @c __LINE__.
 * @param mock The last predefined mock code called.
 */
void sccroll_mockTrace(const char* source, const char* funcname, int line, SccrollMockFlags mock);

/**
 * @def sccroll_mockCall
 * @since 0.1.0
 * @brief Macro facilitating mocks definitions with tracing.
 * @param name The original function name.
 * @param flag The predefined mock flag corresponding to @p name.
 * @return sccroll_mockname() return value.
 */
#define sccroll_mockCall(name, flag, ...)                               \
    (                                                                   \
        sccroll_mockTrace(__FILE__, __FUNCTION__, __LINE__, flag),      \
        sccroll_mock##name(__VA_ARGS__)                                 \
    )

/**
 * @name Predefined mocks prototypes definition.
 * @{
 */
sccroll_mockPrototype(malloc);
sccroll_mockPrototype(calloc);
sccroll_mockPrototype(pipe);
sccroll_mockPrototype(fork);
sccroll_mockPrototype(dup2);
sccroll_mockPrototype(close);
sccroll_mockPrototype(read);
sccroll_mockPrototype(write);
sccroll_mockPrototype(ferror);
sccroll_mockPrototype(fopen);
sccroll_mockPrototype(fseek);
sccroll_mockPrototype(ftell);
sccroll_mockPrototype(fread);
sccroll_mockPrototype(fwrite);
sccroll_mockPrototype(fscanf);
sccroll_mockPrototype(fileno);
sccroll_mockPrototype(hcreate);
sccroll_mockPrototype(hsearch);
/** @} */

/**
 * @name Predefined mocks functions overrides.
 * @{
 */
#define malloc(...) sccroll_mockCall(malloc, SCCEMALLOC, __VA_ARGS__)
#define calloc(...) sccroll_mockCall(calloc, SCCECALLOC, __VA_ARGS__)
#define pipe(...)   sccroll_mockCall(pipe, SCCEPIPE, __VA_ARGS__)
#define fork(...)   sccroll_mockCall(fork, SCCEFORK, __VA_ARGS__)
#define dup2(...)   sccroll_mockCall(dup2, SCCEDUP2, __VA_ARGS__)
#define close(...)  sccroll_mockCall(close, SCCECLOSE, __VA_ARGS__)
#define read(...)   sccroll_mockCall(read, SCCEREAD, __VA_ARGS__)
#define write(...)  sccroll_mockCall(write, SCCEWRITE, __VA_ARGS__)
#define ferror(...) sccroll_mockCall(ferror, SCCEFERROR, __VA_ARGS__)
#define fopen(...)  sccroll_mockCall(fopen, SCCEFOPEN, __VA_ARGS__)
#define fseek(...) sccroll_mockCall(fseek, SCCEFSEEK, __VA_ARGS__)
#define ftell(...)  sccroll_mockCall(ftell, SCCEFTELL, __VA_ARGS__)
#define fread(...)  sccroll_mockCall(fread, SCCEFREAD, __VA_ARGS__)
#define fwrite(...) sccroll_mockCall(fwrite, SCCEFWRITE, __VA_ARGS__)
#define fscanf(...) sccroll_mockCall(fscanf, SCCEFSCANF, __VA_ARGS__)
#define fileno(...) sccroll_mockCall(fileno, SCCEFILENO, __VA_ARGS__)
#define hcreate(...) sccroll_mockCall(hcreate, SCCEHCREATE, __VA_ARGS__)
#define hsearch(...) sccroll_mockCall(hsearch, SCCEHSEARCH, __VA_ARGS__)
/** @} */

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_MOCKS_H_
/** @} @} */
