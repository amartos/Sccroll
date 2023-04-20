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

#include <stdbool.h>
#include <stdio.h>
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
 * Le comportement des simulacres fournis est déterminé par la
 * structure SccrollMockTrigger passée à sccroll_mockTrigger(). Des
 * options définies par SccrollMockOptions sont disponibles en les
 * passant à SccrollMockTrigger::opts à l'aide d'un OR.
 *
 * Par défaut, le module conserve le dernier SccrollMockTrigger passé
 * à sccroll_mockTrigger(). Une option permet de l'oublier à la
 * première erreur, mais l'appel de sccroll_mockFlush() est également
 * dédié à cette opération.
 *
 * Certains simulacres fournissent également d'autres options et
 * capacités décrites dans leur documentation.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Drapeaux pour SccrollMockTrigger::mock afin d'indiquer quel
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
 * @enum SccrollMockOptions
 * @since 0.1.0
 * @brief Options pour les simulacres prédéfinis.
 */
typedef enum SccrollMockOptions {
    SCCMNONE  = 0, /**< Pas d'options. */
    SCCMFLUSH = 2, /**< Effacer du module le pointeur de la structure
                    * SccrollMockTrigger après le premier appel de simulacre
                    * en erreur. */
    SCCMABORT = 4, /**< Lever une erreur au prochain appel de
                    * simulacre après celui en erreur (ou si delay est
                    * négatif). */
} SccrollMockOptions;

/**
 * @struct SccrollMockTrigger
 * @since 0.1.0
 * @brief Structure contenant les informations nécessaires au
 * déclenchement d'un simulacre prédéfini.
 * @note SccrollMockTrigger::abort à @c true déclenche une erreur si
 * SccrollMockTrigger::delay est négatif.
 */
typedef struct SccrollMockTrigger {
    SccrollMockFlags mock;   /**< Drapeau correspondant au simulacre à déclencher. */
    SccrollMockOptions opts; /**< Options pour le déclenchement des simulacres. */
    int delay;               /**< Nombre d'appels du simulacre à ignorer. */
} SccrollMockTrigger;

/**
 * @since 0.1.0
 * @brief Fonction utilisée pour provoquer une erreur dans le
 * simulacre fourni par la bibliothèque et correspondant à la valeur
 * de @p mock.
 * @param trigger SccrollMockTrigger contenant les informations sur le
 * déclenchement de l'erreur, ou NULL pour ne rien déclencher.
 * @attention La structure @p trigger est réutilisée tant que le
 * pointeur est encore valable, sauf si SccrollMockTrigger::abort vaut
 * @c true et lève une erreur ou que la fonction @c abort est
 * utilisée. Ces deux cas reviennent à utiliser sccroll_mockFlush().
 */
void sccroll_mockTrigger(SccrollMockTrigger * trigger);

/**
 * @since 0.1.0
 * @brief Efface le pointeur de la dernière structure
 * SccrollMockTrigger donnée *via* sccroll_mockTrigger().
 * @note La structure en soi n'est pas effacée, juste sa référence
 * pour le module.
 */
void sccroll_mockFlush(void);

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
