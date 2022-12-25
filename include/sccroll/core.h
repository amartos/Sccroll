/**
 * @file        core.h
 * @version     0.1.0
 * @brief       Ficher en-tête de la gestion de tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup API API de Sccroll
 * @{
 * @addtogroup CoreAPI Création, exécution et analyse de tests unitaires
 * @{
 */

#ifndef SCCROLL_CORE_H_
#define SCCROLL_CORE_H_

#define _GNU_SOURCE

#include "sccroll/helpers.h"

#include <argz.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
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
 ******************************************************************************/
// clang-format on

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

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Création de tests unitaires
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
} SccrollFlags;

/**
 * @struct SccrollFile
 * @since 0.1.0
 * @brief Structure stockant le chemin d'un fichier et son contenu.
 */
typedef struct SccrollFile {
    const char* path; /**< Le chemin du fichier. */
    char* content;    /**< Le contenu du fichier. */
} SccrollFile;

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
 * Si un chemin de fichier est passé à SccrollEffects::std::path, il
 * est considéré que les #SCCMAX-1 premiers caractères de son contenu
 * sont ceux à enregistrer pour SccrollEffects::std::content. Ce
 * dernier sera remplacé dans tous les cas si
 * SccrollEffects::std::path est non @c NULL. Si les deux membres de
 * SccrollEffects::std sont @c NULL, la comparaison sera effctuée avec
 * une chaîne vide.
 *
 * Une entrée *via* stdin peut être simulée en passant une chaîne de
 * caractères à SccrollEffects::std[STDIN_FILENO].
 *
 * Les codes SccrollEffects::codes ne sont pas tous comparables en
 * même temps; SccrollEffects::codes[SCCERRNUM] ne peut être récupérée
 * que si la fonction de test ne termine pas le programme; à
 * l'inverse, les codes SccrollEffects::codes[SCCSIGNAL] et
 * SccrollEffects::codes[SCCSTATUS] ne seront récupérables que si la
 * fonction termine le programme.
 *
 * Si le contenu de fichiers doit être testé, leur chemin doit être
 * passé à SccrollEffects::files::path. L'analyse s'arrête à la
 * première occurrence de SccrollEffects::files::path de valeur
 * @c NULL.
 *
 * Les options de test, décrites dans SccrollFlags, sont à passer à
 * SccrollEffects::flags en les groupant avec OR.
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
    SccrollFile files[SCCMAX];  /**< Vérification du  contenu de fichiers. */
    SccrollFile std[SCCMAXSTD]; /**< I/O des sorties standard. */
    int codes[SCCMAXSIG]; /**< Vérification des codes d'erreur, signal et status. */
    SccrollFlags flags;   /**< Drapeaux d'options SccrollFlags. */
    SccrollFunc wrapper;  /**< La fonction de test unitaire. */
    const char* name;     /**< Nom descriptif du test. */
} SccrollEffects;

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Enregistrement de tests
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
 */
void sccroll_register(const SccrollEffects* restrict expected)
    __attribute__((nonnull));

/**
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
 * @name Exécution des tests enregistrés
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
 */
int sccroll_run(void);

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CORE_H_
/** @} @} */
