/**
 * @file        helpers.c
 * @version     0.1.0
 * @brief       Fichier source pour les aides à la programmation.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Helpers Aides à la programmation.
 * @{
 */

#include "sccroll/helpers.h"

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

unsigned sccroll_hasFlags(unsigned flags, unsigned values)
{
    return (flags & values);
}

/** @} @} */
