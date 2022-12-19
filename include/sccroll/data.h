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

#include <stdlib.h>

// clang-format off

/******************************************************************************
 * @name Générateurs de données aléatoires
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_monkey
 * @since 0.1.0
 * @brief Rempli un espace mémoire de données aléatoires.
 * @param blob Un espace mémoire à remplir.
 * @param size Le nombre d'octets à remplir.
 */
#define sccroll_monkey arc4random_buf

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CREATE_H_
/** @} @} */
