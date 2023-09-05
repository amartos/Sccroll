/**
 * @file        helpers.h
 * @version     0.1.0
 * @brief       Helper macros and functions.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Helpers Helper macros and functions
 * @{
 */

#ifndef SCCROLL_HELPERS_H_
#define SCCROLL_HELPERS_H_

#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <err.h>
#include <unistd.h>
#include <sys/wait.h>

// clang-format off

/******************************************************************************
 * @name Macros building
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def SCCCOMMA
 * @since 0.1.0
 * @brief Generates a comma.
 *
 * This macro is used as argument of other macros, when a comma is
 * needed in the result but must be ignored by the parent macro.
 */
#define SCCCOMMA ,

// clang-format off

/******************************************************************************
 * @}
 * @name Messages handling
 * @{
 ******************************************************************************/
// clang-format on

#ifdef DEBUG
/**
 * @def sccroll_dbgline
 * @since 0.1.0
 * @brief Print a simple debug message on stderr indicating the
 * source, function and line number.
 *
 * This macro is used to ensure that its call point has been reached.
 */
#define sccroll_dbgline()                                               \
    fprintf(stderr, "[ DEBUG ] %s::%s l.%i\n", __FILE__, __FUNCTION__, __LINE__)
#endif

/**
 * @def sccroll_variadic
 * @since 0.1.0
 * @brief Facilitate variadic functions usage.
 * @alert The macro assumes that the variadic arguments of the caller
 * functions is located after @p arg.
 * @param arg The last named argument of the caller function.
 * @param list The @c va_list variable name.
 * @param ... The actions to perform on @p list.
 */
#define sccroll_variadic(arg, list, ...)        \
    {                                           \
        va_list list;                           \
        va_start(list, arg);                    \
        __VA_ARGS__;                            \
        va_end(list);                           \
    }

// clang-format off

/******************************************************************************
 * @}
 * @name Flags handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Indicate if the given flags intersect with the given
 * values.
 * @param flags The or'ed flags.
 * @param values The or'ed values.
 * @return @p values if @p flags contains all @p values bits, @c 0
 * otherwise.
 */
unsigned sccroll_hasFlags(unsigned flags, unsigned values);

// clang-format off

/******************************************************************************
 * @}
 * @name Callbacks handling.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef SccrollFunc
 * @since 0.1.0
 * @brief Wrapper functions prototype.
 */
typedef void (*SccrollFunc)(void);

/**
 * @since 0.1.0
 * @brief Execute a callback function within a fork.
 * @param desc Description of the callback function (used in case of
 * errors).
 * @param callback The callback wrapper function.
 * @return The wait() status. If the callback does not exits,
 * sccroll_simplefork() exits itself with status #EXIT_SUCCESS.
 */
int sccroll_simplefork(const char* restrict desc, SccrollFunc callback) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 * @name Alias definitions
 * @{
 * @note The aliased functions attributes are all copied by these
 * macros.
 * @param storage A storage type specifier (@c extern, @c static, @c
 * register, @c auto, or nothing).
 * @param name Aliased function name.
 * @param aliasname Alias name.
 * @param ... Additional attributes for the alias.
 ******************************************************************************/
// clang-format on

/**
 * @def attr_rename
 * @since 0.1.0
 * @brief Generate a function alias.
 * @attention The alias is a mere rename of the aliased function,
 * which is not the exact definition of an alias used in the GNU C
 * library. In the C library, aliases are defined using the
 * @code alias(name) @endcode attribute, which is not the case of this
 * macro.
 */
#define attr_rename(storage, name, aliasname, ...)     \
    storage __typeof__(name) aliasname                 \
    __attribute__((copy(name), ##__VA_ARGS__))

/**
 * @def attr_alias
 * @since 0.1.0
 * @brief Generate an alias the same way aliases are defined in the
 * GNU C library.
 */
#define attr_alias(storage, name, aliasname, ...)      \
    attr_rename(storage, name, aliasname, alias(#name))

/**
 * @def strong_alias
 * @since 0.1.0
 * @brief Generate a strong alias.
 */
#define strong_alias(storage, name, aliasname)  \
    attr_alias(storage, name, aliasname)

/**
 * @def weak_alias
 * @since 0.1.0
 * @brief Generate a weak alias.
 */
#define weak_alias(storage, name, aliasname)    \
    attr_alias(storage, name, aliasname, weak)

/**
 * @def extern_alias
 * @since 0.1.0
 * @brief Generate an alias of an external library function.
 * @param aliasmacro The alias macro of the present module to use to
 * generate the alias. Its parameters are the same as attr_rename().
 */
#define extern_alias(aliasmacro, name, aliasname)   \
    aliasmacro(extern, name, aliasname)

// clang-format off

/******************************************************************************
 * @}
 * @name Error codes translations
 *
 * These functions are integrated in the GNU C library, at least in
 * recent versions. However, the variety of versions in the wild would
 * provoke errors.
 *
 * While support of these is not widespread, they are redefined. If
 * the C library defines them, the functions below are ignored.
 *
 * @todo Delete these functions when their support is sufficient enough.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Give the errno code corresponding name.
 * @param errnum The errno value.
 * @return The name of the given errno code, or @c NULL if no name
 * corresponds to @p errnum.
 */
const char* strerrorname_np(int errnum) __attribute__((weak));

/**
 * @since 0.1.0
 * @brief Give the short signal name (the "XXX" of "SIGXXX").
 * @param sig The signal code.
 * @return The short name of the signal, or @c NULL if @p sig does not
 * corresponds to any.
 */
const char* sigabbrev_np(int sig) __attribute__((weak));

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_HELPERS_H_
/** @} @} */
