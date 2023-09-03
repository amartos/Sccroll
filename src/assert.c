/**
 * @file        assert.c
 * @version     0.1.0
 * @brief       Assertions module source file.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * @addtogroup Internals
 * @{
 * @addtogroup AssertInternals Assertions implementation
 * @{
 */

#include "sccroll/assert.h"

// clang-format off

/******************************************************************************
 * Implementation
 ******************************************************************************/
// clang-format on

void sccroll_vfatal(int sigint, const char* restrict fmt, va_list args)
{
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    // The final exit, although not used, is here to please the
    // compilers and avoid complains about a noreturn function that
    // would return (even if not).
    __gcov_dump(), raise(sigint), raise(SIGABRT), exit(1);
}

void sccroll_fatal(int sigint, const char* restrict fmt, ...)
{
    // ibid for the exit.
    sccroll_variadic(fmt, list, sccroll_vfatal(sigint, fmt, list), exit(1));
}

void sccroll_assert(int expr, const char* restrict fmt, ...)
{
    if (!expr) sccroll_variadic(fmt, list, sccroll_vfatal(SIGABRT, fmt, list));
}

/** @} @} */
