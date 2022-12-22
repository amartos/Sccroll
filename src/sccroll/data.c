/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Ficher source de la génération de données pour
 *              tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 * @addtogroup Internals
 * @{
 * @addtogroup DataInternals Générateurs de données pour les tests unitaires
 * @{
 */

#include "sccroll/data.h"

void* sccroll_rndalloc(size_t nmemb, size_t size)
{
    size_t real_size = nmemb*size;
    void* randomized = malloc(real_size);
    if (randomized) sccroll_monkey(randomized, real_size);
    return randomized;
}

/** @} @} */
