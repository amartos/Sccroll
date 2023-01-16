/**
 * @file        data.h
 * @version     0.1.0
 * @brief       Ficher en-tête pour la génération de données pour
 *              tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 * @addtogroup API
 * @{
 * @addtogroup DataAPI Générateurs de données pour les tests unitaires
 * @{
 */

#ifndef SCCROLL_DATA_H_
#define SCCROLL_DATA_H_

#include "sccroll/helpers.h"

#include <stdlib.h>
#include <time.h>

// clang-format off

/******************************************************************************
 * @name Générateurs de données aléatoires
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Rempli un espace mémoire de données aléatoires.
 * @param blob Un espace mémoire à remplir.
 * @param size Le nombre d'octets à remplir.
 */
void sccroll_monkey(void* blob, size_t size) __attribute__((leaf, nothrow, nonnull (1)));

/**
 * @since 0.1.0
 * @brief Alloue une zone mémoire remplie de données aléatoires.
 * @attention Utilise malloc.
 * @attention Cette fonction **ne vérifie pas** que le dernier octet
 * d'une chaîne de caractères est bien nul.
 * @param nmemb Le nombre d'éléments de la zone de mémoire.
 * @param size Le nombre d'octets d'un élément.
 * @return Une zone mémoire de @p nmemb éléments de @p size octets
 * remplis de données aléatoires, ou NULL en cas d'erreur.
 */
void* sccroll_rndalloc(size_t nmemb, size_t size);

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CREATE_H_
/** @} @} */
