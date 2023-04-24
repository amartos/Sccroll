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

void sccroll_vfatal(FILE* stream, const char* restrict fmt, va_list args)
{
    vfprintf(stream, fmt, args);
    fprintf(stderr, "\n");
    abort();
}

void sccroll_fatal(const char* restrict fmt, ...)
{
    // L'exit final est là pour contenter le compilateur, mais ne sera
    // jamais appelé.
    sccroll_variadic(fmt, list, sccroll_vfatal(stderr, fmt, list), exit(1));
}

void sccroll_assert(int expr, const char* restrict fmt, ...)
{
    if (!expr) sccroll_variadic(fmt, list, sccroll_vfatal(stderr, fmt, list));
}

/** @} @} */
