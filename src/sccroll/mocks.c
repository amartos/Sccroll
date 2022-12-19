/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Fichier source des simulacres pré-fournis.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -fpic -shared $(scripts/mocks.awk src/mocks.c) \
 *     src/sccroll.c src/mocks.c \
 *     -o build/libs/libsccroll.so
 * @endcode
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Mocks Simulacres pré-fournis.
 * @{
 */

#include "sccroll/mocks.h"

// clang-format off

/******************************************************************************
 * Documentation
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fonction renvoyant toujours #SCCENONE.
 * @note est utilisée comme alias faible de sccroll_mockTrigger().
 * @return #SCCENONE.
 */
static unsigned sccroll_enone(void);

/**
 * @since 0.1.0
 * @brief Fonction sauvegardant les données utilisées par gcov.
 */
extern void __gcov_dump(void);

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

weak_alias(sccroll_enone, sccroll_mockTrigger);
static unsigned sccroll_enone(void) { return SCCENONE; }

SCCROLL_MOCK(void, abort, void)
{
    // On enregistre les fonctions avec atexit afin de permettre une
    // couverture de code complète: puisque les fonctions sont
    // appelées après exit, toutes les lignes de la fonction sont
    // utilisées. L'ordre est important, car les fonctions sont
    // appelée en ordre inverse de leur inscription.
    //
    // La fonction doit quitter. Mais une erreur possible pour elle
    // est de quitter de la mauvaise manière: au lieu de s'arrêter
    // avec un signal SIGABRT et un status EXIT_SUCCESS, la fonction
    // s'arrête avec exit et un status d'erreur.
    atexit(sccroll_hasFlags(sccroll_mockTrigger(), SCCEABRT) ? NULL : __real_abort);
    atexit(__gcov_dump);
    exit(SIGABRT);
}

/** @} @} */
