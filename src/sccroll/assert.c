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

__attribute__((format(printf,2,3)))
void sccroll_assert(int expr, const char* restrict fmt, ...)
{
    if (!expr) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
    }
}

/** @} @} */
