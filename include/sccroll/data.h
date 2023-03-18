/**
 * @file        data.h
 * @version     0.1.0
 * @brief       Ficher en-tête pour la génération de données pour
 *              tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
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

#include <err.h>
#include <stdlib.h>
#include <string.h>
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
 * @attention Si @p size vaut 0, la fonction renvoie un pointeur
 * unique qui peut être passé à @c free (comportement identique à
 * @c malloc).
 * @param nmemb Le nombre d'éléments de la zone de mémoire.
 * @param size Le nombre d'octets d'un élément.
 * @return Une zone mémoire de @p nmemb éléments de @p size octets
 * remplis de données aléatoires, ou NULL en cas d'erreur.
 */
void* sccroll_rndalloc(size_t nmemb, size_t size);

// clang-format off

/******************************************************************************
 * @}
 * @name Copies de données.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Copie un block de donnée.
 * @param blob Le block à copier.
 * @param size Le nombre d'octets de blob à copier.
 * @return Un pointeur vers une copie des @p size premiers octets de
 * @p blob, ou un pointeur vers un emplacement mémoire de @p size
 * octets initialisés à 0 si @p blob est NULL, ou NULL si @p size vaut
 * 0.
 */
void* blobdup(const void* restrict blob, size_t size);

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CREATE_H_
/** @} @} */
