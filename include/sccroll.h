/**
 * @file        sccroll.h
 * @version     0.1.0
 * @brief       Ficher en-tête de Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -fpic -shared -Wl,--wrap,abort \
 *     -o build/libs/libsccroll.so
 * @endcode
 *
 * @addtogroup API API de Sccroll
 * @{
 */

#ifndef SCCROLL_H_
#define SCCROLL_H_

#define _GNU_SOURCE

#include "sccroll/helpers.h"
#include "sccroll/data.h"
#include "sccroll/assert.h"
#include "sccroll/mocks.h"

#include <argz.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>
#include <search.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// clang-format off

/******************************************************************************
 * @addtogroup TestsAPI Tests unitaires
 * @{
 *
 * @addtogroup PrepAPI Préparation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @name Fonctions exécutée à des moments précis lors des tests
 *
 * Ces fonctions de préparation sont exécutées à des moments
 * prédéterminés autour de l'exécution d'un test.
 *
 * Cette section ne représente qu'une interface disponible, la
 * définition de ses fonctions sont laissées à l'utilisateur. Si l'une
 * d'elles n'est pas définie, elle n'a aucun effet.
 *
 * @internal
 * @note Toutes les fonctions de cette section sont des alias faibles d'une
 * fonction qui n'a aucun effet.
 * @endinternal
 * @{
 */

/**
 * @since 0.1.0
 * @brief Première fonction exécutée par sccroll_run().
 */
void sccroll_init(void);

/**
 * @since 0.1.0
 * @brief Dernière fonction exécutée par sccroll_run().
 */
void sccroll_clean(void);
/** @} */

/**
 * @name Fonctions exécutées avec chaque test
 * @{
 */

/**
 * @since 0.1.0
 * @brief Fonction appelée juste avant l'exécution de *chaque*
 * SccrollEffects::wrapper.
 */
void sccroll_before(void);

/**
 * @since 0.1.0
 * @brief Fonction appelée juste après l'exécution de *chaque*
 * SccrollEffects::wrapper.
 */
void sccroll_after(void);
/** @} */

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup DefAPI Définition
 *
 * Les tests unitaires et leurs effets sont décris à la librairie à
 * l'aide de la structure SccrollEffects. Leur définition est
 * facilitée par la macro SCCROLL_TEST() dont l'utilisation est
 * similaire à celle d'une définition de fonction.
 * @{
 *****************************************************************************/
// clang-format on

/**
 * @typedef SccrollFunc
 * @since 0.1.0
 * @brief Prototype des fonctions de test unitaires.
 */
typedef void (*SccrollFunc)(void);

/**
 * @enum SccrollIndexes
 * @since 0.1.0
 * @brief Index des tables de SccrollEffects.
 */
typedef enum SccrollIndexes {
    SCCERRNUM = 0,                 /**< Index du code errno. */
    SCCSIGNAL = 1,                 /**< Index du code de signal. */
    SCCSTATUS = 2,                 /**< Index du code de status/exit. */
    SCCMAXSIG = 3,                 /**< Index maximal de SccrollEffects::codes. */
    SCCMAXSTD = STDERR_FILENO + 1, /**< Index maximal de SccrollEffects::std. */
    SCCMAX    = BUFSIZ,            /**< Index maximal de SccrollEffects::files. */
} SccrollIndexes;

/**
 * @enum SccrollFlags
 * @since 0.1.0
 * @brief Drapeaux d'options pour un test.
 * @attention Le comportement par défaut du programme est l'inverse de
 * toutes les options définies ici.
 */
typedef enum SccrollFlags {
    NOSTRP = 1, /**< Ne pas réduire les espaces autour des sorties standard.*/
    NOFORK = 2, /**< Ne pas @c fork avant d'exécuter le test. */
    NODIFF = 4, /**< Ne pas afficher les différences attendu/obtenu. */
    EXPATH = 8, /**< Interpréter les chemins absolus de fichiers passés à
                  * SccrollEffects::std et SccrollEffects::files comme
                  * contennant les textes attendus. */
} SccrollFlags;

/**
 * @struct SccrollEffects
 * @since 0.1.0
 * @brief Gère les informations sur les effets secondaires de
 * l'exécution d'une fonction.
 *
 * Cette structure permet de décrire les effets attendus d'un
 * test, et de passer certaines options au programme pour un test
 * spécifique.
 *
 * Le pointeur de la fonction de test est tocké dans
 * SccrollEffects::wrapper. Lors de son exécution, si l'un des effets
 * attendus diffère de celui observé, un message d'erreur est levé. Le
 * message utilise le nom défini dans SccrollEffects::name pour une
 * bonne identification du test en échec.
 *
 * Une entrée *via* stdin peut être simulée en passant une chaîne de
 * caractères à SccrollEffects::std[STDIN_FILENO].
 *
 * Cette structure est très versatile, dans le sens où elle permet
 * soit d'effectuer une série de tests, soit de tester les effets
 * d'une unique fonction, la seule différence résidant dans le code de
 * la fonction SccrollEffects::wrapper et dans les effets attendus
 * indiqués.
 *
 * Les options disponibles pour un test sont listées dans la structure
 * SccrollFlags, et doivent être données par combinaison OR.
 */
typedef struct SccrollEffects {
    struct {
        const char* path; /**< Le chemin du fichier. */
        char* content;    /**< Le contenu du fichier. */
    } files[SCCMAX];      /**< Vérification du  contenu de fichiers. */
    char* std[SCCMAXSTD]; /**< I/O des sorties standard. */
    int codes[SCCMAXSIG]; /**< Vérification des codes d'erreur, signal et status. */
    unsigned flags;       /**< Drapeaux d'options SccrollFlags. */
    SccrollFunc wrapper;  /**< La fonction de test unitaire. */
    const char* name;     /**< Nom descriptif du test. */
} SccrollEffects;

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup RecAPI Enregistrement
 *
 * Il existe deux manières d'inscrire un test unitaire pour exécution:
 * soit la structure SccrollEffects correspondante est confiée à
 * sccroll_register(), soit le test est défini à l'aide de la macro
 * SCCROLL_TEST(). Les deux méthodes peuvent être utilisées dans un
 * même fichier source de tests.
 *
 * @attention Un test défini avec SCCROLL_TEST() et enregistré avec
 * sccroll_register() sera exécuté **deux** fois.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Inscrit le test décrit par @p expected pour exécution.
 * @attention Un test défini par SCCROLL_TEST() et inscrit avec
 * sccroll_register() sera exécuté **deux** fois.
 * @param expected Les informations nécessaires pour l'exécution d'un
 * test et comparaison des résultats.
 * @throw SIGABRT si la fonction est exécutée au
 * sein d'une fonction de test.
 */
void sccroll_register(const SccrollEffects* restrict expected)
    __attribute__((nonnull));

/**
 * @ingroup DefAPI
 * @def SCCROLL_TEST
 * @since 0.1.0
 * @brief Définit et enregistre un test pour exécution.
 *
 * La macro s'utilise de manière similaire à la définition d'une
 * fonction. Le code entre crochets situé directement après la macro
 * constitue le code de la fonction de test @p testname , qui est
 * automatiquement enregistrée pour exécution.
 *
 * Les paramètres suivants sont ceux donnés à la structure
 * SccrollEffects afin de décrire les effets attendus (même syntaxe
 * que pour l'initialisation de la structure). Si aucun paramètre
 * n'est donné, les valeurs des effets attendus seront @c 0 ou @c ""
 * pour l'ensemble des éléments testés.
 *
 * @example SCCROLL_TEST.c
 *
 * @param testname Le nom du test unitaire.
 * @param ... Les données SccrollEffects attendues (syntaxe d'une
 * initialisation de la structure).
 */
#define SCCROLL_TEST(testname, ...)                                            \
    static void testname(void);                                                \
    __attribute__((constructor)) static void sccroll_register_##testname(void) \
    {                                                                          \
        const SccrollEffects expected = {                                      \
            .wrapper = testname,                                               \
            .name    = #testname,                                              \
            ##__VA_ARGS__                                                      \
        };                                                                     \
        sccroll_register(&expected);                                           \
    }                                                                          \
    static void testname(void)

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup ExeAPI Exécution
 *
 * La librairie fournit une fonction main exécutée par
 * défaut. Un fichier source de tests unitaires n'a donc besoin que de
 * la définition des tests avec SCCROLL_TEST().
 *
 * Cependant, si une fonction main est redéfinie par l'utilisateur, il
 * est possible de lancer l'exécution des tests à l'aide de
 * sccroll_run().
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute les tests unitaires et affiche un rapport.
 * @attention La librairie fournit une fonction main par défaut qui
 * exécute sccroll_run(). Si la fonction main est redéfinie, il est
 * nécessaire d'appeler sccroll_run() pour lancer les tests. À
 * l'inverse, si aucun main n'est défini pour les tests, l'appel de
 * sccroll_run() est inutile.
 * @return le nombre de tests en échec.
 * @throw SIGABRT si la fonction est exécutée au
 * sein d'une fonction de test.
 */
int sccroll_run(void);

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_H_
/** @} */
