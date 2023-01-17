/**
 * @file        mocks.h
 * @version     0.1.0
 * @brief       Simulacres pré-fournis.
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
 * @addtogroup API
 * @{
 * @addtogroup MocksAPI Simulacres
 *
 * En C, la définition de simulacres (*mocks* en anglais) n'a pas la
 * même signification que dans les langages orientés objets. Ici, un
 * simulacre sera principalement une fonction (et non plus un objet)
 * altérée de manière à pouvoir contrôler finement son comportement
 * lors de l'exécution du programme.
 * @{
 */

#ifndef SCCROLL_MOCKS_H_
#define SCCROLL_MOCKS_H_

#include "sccroll/helpers.h"

#include <stdbool.h>
#include <stdlib.h>
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
 * Il est possible de fournir des attributs à la fonction; ils doivent
 * toutefois être placés avant la macro pour être pris en compte.
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
    extern __typeof__(name) __real_##name;      \
    extern __typeof__(name) __wrap_##name;      \
    retval __wrap_##name(__VA_ARGS__)

// clang-format off

/******************************************************************************
 * @}
 * @name Simulacres prédéfinis.
 *
 * Le comportement des simulacres fournis est déterminé par le retour
 * de la fonction sccroll_mockTrigger(). Si celui-ci correspond au
 * code SccrollMockFlags du simulacre, l'erreur est déclenchée; sinon
 * la fonction agit comme l'originale.
 *
 * Certains simulacres fournissent également d'autres options et
 * capacités décrites dans leur documentation.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Drapeaux utilisables par sccroll_mockTrigger() pour indiquer
 * quel simulacre pré-fourni doit être en erreur.
 * @attention Les drapeaux **ne peuvent pas** être combinés pour
 * déclencher plusieurs erreurs simultanément.
 */
typedef enum SccrollMockFlags {
    SCCENONE = 0, /**< Drapeau ne provoquant pas d'erreurs. */
    SCCEABORT,    /**< Drapeau de abort(). */
    SCCECALLOC,   /**< Drapeau de calloc(). */
    SCCEPIPE,     /**< Drapeau de pipe(). */
    SCCEFORK,     /**< Drapeau de fork(). */
    SCCEDUP2,     /**< Drapeau de dup2(). */
    SCCECLOSE,    /**< Drapeau de close(). */
    SCCEREAD,     /**< Drapeau de read(). */
    SCCEWRITE,    /**< Drapeau de write(). */
    SCCEMALLOC,   /**< Drapeau de malloc(). */
    SCCEMAX,      /**< Valeur maximale des mocks individuels. */
} SccrollMockFlags;

/**
 * @since 0.1.0
 * @brief Fonction utilisée pour provoquer une erreur dans les
 * simulacres fournis par la bibliothèque.
 * @attention Cette fonction de provoque pas d'erreur par défaut. Elle
 * peut cependant être redéfinie sans problèmes par l'utilisateur afin
 * de provoquer les erreurs voulues selon les conditions voulues.
 * @param mock L'identifiant SccrollMockFlags du mock.
 * @return @c true si le simulacre correspondant à l'identifiant
 * @p mock doit lever une erreur, sinon @c false.
 */
bool sccroll_mockTrigger(SccrollMockFlags mock);

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
