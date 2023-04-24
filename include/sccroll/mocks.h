/**
 * @file        mocks.h
 * @version     0.1.0
 * @brief       Simulacres pré-fournis.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -fpic -shared $(scripts/mocks.awk src/mocks.c) \
 *     src/sccroll.c src/mocks.c \
 *     -o build/libs/libsccroll.so
 * @endcode
 * @todo Renommer structures et fonctions; les noms sont parfois
 * redondants et ne sont pas nécessairement bien choisi.
 *
 * @addtogroup API
 * @{
 * @addtogroup MocksAPI Simulacres
 *
 * En C, la définition de simulacres (*mocks* en anglais) n'a pas la
 * même signification que dans les langages orientés objets. Ici, un
 * simulacre sera principalement une fonction (et non plus un objet)
 * altérée de manière à pouvoir contrôler finement son comportement
 * lors de l'exécution du programme.
 *
 * Ce module redéfinit également quelques fonctions de la librairie C
 * car elles interfèrent avec le fonctionnement de Sccroll ; c'est
 * notamment le cas de abort() qui normalement bloque la récupération
 * de données de `gcov`, mais qui, ici, effectue un *dump* des données
 * avant de lever le signal.
 * @{
 */

#ifndef SCCROLL_MOCKS_H_
#define SCCROLL_MOCKS_H_

#include "sccroll/helpers.h"
#include "sccroll/assert.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name Création de simulacres.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_unused
 * @since 0.1.0
 * @brief Indique à la fonction que le paramètre n'est pas utilisé.
 *
 * Cette macro est utile pour les mocks pour éviter les erreurs de
 * compilation si un des paramètres n'est pas utilisé.
 * @param var Une variable non utilisée dans la fonction.
 */
#define sccroll_unused(var) (void) var

/**
 * @def SCCROLL_MOCK
 * @since 0.1.0
 * @brief Génère un simulacre d'une fonction.
 *
 * Comme pour SCCROLL_TEST(), cette macro s'utilise de manière analogue
 * à la définition d'une fonction. Le prototype est passé à la macro
 * et le code du simulacre et inclus dans des crochets suivant
 * directement la macro. La fonction originelle est toujours
 * disponible *via* l'appel de @c __real_name .
 *
 * La syntaxe d'une définition de simulacre est particulère pour les
 * paramètres dans le sens où un paramètre doit contenir à la fois le
 * type et le nom de la variable (ce dernier peut être différent de
 * celui de la fonction originelle) non séparés par une virgule.
 *
 * Les attributs de la fonction originelle sont copiés
 * automatiquement.
 *
 * @attention Cette macro est conçue pour une compilation avec GCC.
 * @attention L'utilisation de cette macro nécessite de passer le
 * paramètre `--wrap name` au linker @c ld .
 * L'option @c -Wl de GCC est utile en ce sens. De plus, le script
 * @c mocks.awk de la librairie facilite la compilation de sources
 * avec cette macro.
 * @param retval Le type des données renvoyées par la fonction
 * originelle.
 * @param name Le nom de la fonction originelle.
 * @param ... Les paramètres de la fonction originelle ou @c void si
 * le simulacre ne prend aucun paramètre.
 */
#define SCCROLL_MOCK(retval, name, ...)         \
    attr_rename(extern, name, __real_##name);   \
    attr_rename(extern, name, __wrap_##name);   \
    retval __wrap_##name(__VA_ARGS__)

// clang-format off

/******************************************************************************
 * @}
 * @name Simulacres prédéfinis.
 *
 * Divers simulacres sont prédéfinis par le module. Il est possible de
 * les déclencher (un par un) avec un appel à sccroll_mockTrigger(),
 * qui peut également délayer l'erreur d'un certain nombre d'appels.
 *
 * Les simulacres dont l'erreur n'est pas prise en charge lèvent une
 * erreur d'assertion à leur prochain appel. Le simulacre est
 * désactivé avant de lever cette erreur ; de même, __gcov_dump() est
 * appelé avant de quitter.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Drapeaux pour sccroll_mockTrigger() afin d'indiquer quel
 * simulacre pré-fourni doit être en erreur.
 * @attention Les drapeaux **ne peuvent pas** être combinés pour
 * déclencher plusieurs erreurs simultanément.
 */
typedef enum SccrollMockFlags {
    SCCENONE = 0, /**< Drapeau ne provoquant pas d'erreurs. */
    SCCEMALLOC,   /**< Drapeau de malloc(). */
    SCCECALLOC,   /**< Drapeau de calloc(). */
    SCCEPIPE,     /**< Drapeau de pipe(). */
    SCCEFORK,     /**< Drapeau de fork(). */
    SCCEDUP2,     /**< Drapeau de dup2(). */
    SCCECLOSE,    /**< Drapeau de close(). */
    SCCEREAD,     /**< Drapeau de read(). */
    SCCEWRITE,    /**< Drapeau de write(). */
    SCCEMAX,      /**< Valeur maximale des mocks individuels. */
} SccrollMockFlags;

/**
 * @since 0.1.0
 * @brief Fonction utilisée pour provoquer une erreur dans le
 * simulacre fourni par la bibliothèque et correspondant à la valeur
 * de @p mock.
 * @param mock Le simulacre à déclencher.
 * @param delay Le délai avant déclenchement du simulacre, en nombre
 * d'appels ; 0 indique un déclenchement immédiat.
 */
void sccroll_mockTrigger(SccrollMockFlags mock, unsigned delay);

/**
 * @since 0.1.0
 * @brief Donne le code SccrollMockFlags du simulacre prévu au
 * déclenchement.
 * @return Le code SccrollMockFlags du simulacre prévu au
 * déclenchement.
 */
SccrollMockFlags sccroll_mockGetTrigger(void);

/**
 * @since 0.1.0
 * @brief Donne le délai restant avant déclenchement du simulacre.
 * @return Le nombre d'appels restants du simulacre avant le
 * déclemenchement d'une erreur.
 */
unsigned sccroll_mockGetDelay(void);

/**
 * @since 0.1.0
 * @brief Donne le nombre d'appels effectués depuis le déclenchement
 * de l'erreur du simulacre.
 * @return Le nombre d'appels à partir du déclemenchement du simulacre
 * (le déclenchement compte pour le premier appel).
 */
unsigned sccroll_mockGetCalls(void);

/**
 * @since 0.1.0
 * @brief Désactive le simulacre courant.
 */
void sccroll_mockFlush(void);

/**
 * @since 0.1.0
 * @brief Invoque sccroll_mockFlush(), affiche un message sur stderr,
 * sauvegarde les données pour gcov et lève SIGABRT.
 * @param fmt La chaîne de formatage du message
 * @param ... Les arguments de la chaîne de formatage.
 */
void sccroll_mockFatal(const char* restrict fmt, ...)
    __attribute__((noreturn,format(printf,1,2)));

/**
 * @since 0.1.0
 * @brief Effectue un test d'erreur des simulacres prédéfinis sur une
 * fonction donnée.
 *
 * La fonction inscrit le déclenchement d'un simulacre, et exécute
 * @p wrapper dans un fork() (insensible au simulacre correspondant).
 *
 * La fonction propage ensuite tout signal levé par @p wrapper, mais
 * pas les code de status d'erreur pour tout simulacre à déclencher ;
 * si aucun simulacre n'est à déclencher (#SCCENONE) mais qu'un status
 * autre que nul est renvoyé par @p wrapper, elle le propagera
 * également. Les signaux ont la priorité sur les codes de status.
 *
 * Tous les délais possibles sont testés par la fonction. Le premier
 * délai ne renvoyant aucune erreur est considéré comme indiquant que
 * plus aucun simulacre ne sera déclenché par la suite.
 *
 * @param wrapper Le wrapper de la fonction à tester.
 */
void sccroll_mockPredefined(SccrollFunc wrapper) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Donne le nom de la fonction originale correspondant au
 * simulacre identifié par @p mock.
 * @return Le nom de la fonction originale correspondant à @p mock. La
 * chaîne renvoyée n'est pas allouée avec malloc, elle ne doit pas
 * être libérée.
 */
const char* sccroll_mockName(SccrollMockFlags mock) __attribute__((returns_nonnull));

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_MOCKS_H_
/** @} @} */
