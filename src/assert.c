/**
 * @file        assert.c
 * @version     0.1.0
 * @brief       Fichier source des assertions.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup Internals
 * @{
 * @addtogroup AssertInternals Assertions
 * @{
 */

#include "sccroll/assert.h"

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

void sccroll_vfatal(int sigint, const char* restrict fmt, va_list args)
{
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    // L'exit final est là pour contenter le compilateur, mais ne sera
    // jamais appelé.
    __gcov_dump(), raise(sigint), raise(SIGABRT), exit(1);
}

void sccroll_fatal(int sigint, const char* restrict fmt, ...)
{
    // L'exit final est là pour contenter le compilateur, mais ne sera
    // jamais appelé.
    sccroll_variadic(fmt, list, sccroll_vfatal(sigint, fmt, list), exit(1));
}

void sccroll_assert(int expr, const char* restrict fmt, ...)
{
    if (!expr) sccroll_variadic(fmt, list, sccroll_vfatal(SIGABRT, fmt, list));
}

/** @} @} */
