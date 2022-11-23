/**
 * @file        sccroll.h
 * @version     0.1.0
 * @brief       Librairie Sccroll, framework de tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 \
 *     -fpic -shared sccroll.c \
 *     -o libsccroll.so
 * @endcode
 *
 * @details
 * @parblock
 * Ce module est un framework de tests unitaires. Le fichier header
 * est à importer dans le fichier source contenant les tests unitaires
 * à effectuer. Les tests sont définis selon la syntaxe suivante:
 *
 * @code{.c}
 * SCCROLL_TEST(nom_du_test) { assert(...); }
 * @endcode
 *
 * La macro s'occupe d'inscrire le test pour exécution et rapport.
 * La macro prend également en charge les attributs optionnels du
 * compilateur GCC (et compatibles) si placé en en-tête de la
 * définition (cf. documentation).
 *
 * Le fichier de tests ne nécessite aucune fonction @c main(), puisque la
 * librairie en fournit une. Toutefois, si un @c main() personnalisé est
 * nécessaire, il est possible de désactiver la fonction par défaut en
 * définissant la macro @c SCCROLL_NOMAIN à la compilation.
 * Les fonctions sccroll_register() et sccroll_run() permettent
 * respectivement d'inscrire une fonction de test non définie par
 * #SCCROLL_TEST et d'exécuter les tests.
 *
 * L'ordre d'exécution des tests n'est pas garantit, il est donc
 * déconseillé de faire dépendre plusieurs tests entre eux.
 *
 * La librairie fournit également des fonctions d'assertion:
 *
 *
 * @endparblock
 *
 * @addtogroup Sccroll
 *
 * @{
 */

#ifndef SCCROLL_H_
#define SCCROLL_H_

#define _GNU_SOURCE

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <search.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @addtogroup Definition Les tests unitaires.
 * @brief Gestion de l'inscription, exécution et rapport de tests
 * unitaires.
 *
 * Les structures, macros et fonctions du groupe permettent de
 * préparer les tests, de les définir, de les enregistrer pour
 * éxécution, et d'effectuer un nettoyage après exécution.
 *
 * @{
 *****************************************************************************/
// clang-format on

/**
 * @typedef SccrollTestFunc
 * @since 0.1.0
 * @brief Prototype des fonctions de test unitaires.
 */
typedef void (*SccrollTestFunc)(void);

// clang-format off

/******************************************************************************
 * @addtogroup TestPrep Préparation des tests.
 * @brief Commandes exécutées avant ou après les tests unitaires.
 *
 * @parblock
 * Les fonctions sccroll_init() et sccroll_clean() sont exécutées
 * respectivement avant @b l'ensemble des tests unitaires et juste
 * avant la fin du programme (cf. atexit()).
 *
 * Les fonctions sccroll_before() et sccroll_after() sont appelées
 * respectivement avant et après @b chacune des fonctions de
 * test.
 * @endparblock
 *
 * @attention Toutes ces fonctions sont à définir par
 * l'utilisateur. Elles sont appelées automatiquement par la fonction
 * sccroll_run().
 *
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
 * @brief Dernière fonction exécutée par le main fournit.
 * @note Cette fonction est inscrite pour exécution lors d'un exit
 * avec atexit.
 */
void sccroll_clean(void);

/**
 * @since 0.1.0
 * @brief Prépare un test unitaire.
 *
 * Fonction appelée avant chaque test dont elle permet la
 * préparation (initialisation de variables, etc...).
 */
void sccroll_before(void);

/**
 * @since 0.1.0
 * @brief Nettoie après un test unitaire.
 *
 * Fonction appelée après chaque test dont elle permet le
 * nettoyage (libération de mémoire, etc...).
 */
void sccroll_after(void);

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Register Enregistrement des tests.
 * @brief Commandes enregistrant les tests pour exécution.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Inscrit une fonction de test unitaire pour exécution.
 *
 * Inscrit la fonction #func correspondant au test #name pour
 * exécution lors de l'appel de sccroll_run(). Les deux arguments ne
 * peuvent être @c NULL.
 *
 * @attention Cette fonction n'a pas besoin d'être appelée par
 * l'utilisateur s'il définit les tests à l'aide de la macro
 * #SCCROLL_TEST.
 *
 * @param func Le pointeur de la fonction de test unitaire.
 * @param name Le nom du test à associer à la fonction.
 */
void sccroll_register(SccrollTestFunc func, const char* name) __attribute__((nonnull));

/**
 * @def SCCROLL_TEST
 * @since 0.1.0
 * @brief Définit et enregistre un test pour exécution.
 *
 * @parblock
 * Créé et inscrit une fonction de test unitaire #name ayant
 * pour code la suite d'instructions située dans les accolades suivant
 * directement la macro, et l'inscrit pour exécution. Le nom du test
 * est identique à celui de la fonction.
 *
 * L'exemple ci-dessous créé la fonction @c my_test_name() et l'appelle
 * lors de l'exécution sous le nom @c "my_test_name".
 *
 * @code{.c}
 * SCCROLL_TEST(my_test_name)
 * {
 *     assert(a() && b() && c());
 *     assert(d());
 *     assert(e());
 * }
 * @endcode
 *
 * Si besoin, des attributs peuvent être ajoutés à la fonction. Ils
 * doivent toutefois être placés juste avant la macro et doivent
 * prendre en compte le fait que la fonction sera appelée quel que
 * soit l'attribut défini. Exemple:
 *
 * @code{.c}
 * __attribute__((warn ("message")))
 * SCCROLL_TEST(dummy)
 * {
 *     assert(true);
 * }
 * @endcode
 * @enparblock
 * @see https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 *
 * @param name Le nom de la fonction du test unitaire, qui est aussi
 * le nom du test.
 */
#define SCCROLL_TEST(name)                                          \
    void name(void);                                                \
    __attribute__((constructor)) void sccroll_register_##name(void) \
    {                                                               \
        sccroll_register(name, #name);                              \
    }                                                               \
    void name(void)

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Execution Exécution des tests unitaires.
 * @brief Commandes gérant l'exécution et l'affichage des rapports des
 * tests unitaires.
 *
 * @parblock
 * <b>L'ordre d'exécution des tests n'est pas garantit</b>; aucun test
 * ne peut donc dépendre de manière fiable de l'exécution d'un
 * autre. L'ordre d'exécution garanti des fonctions de la librairie
 * (fonction sccroll_run() ou la fonction main() fournie) est le
 * suivant:
 *
 * 1. sccroll_init()
 * 2. sccroll_run()
 *   - Pour chaque test unitaire:
 *   1. sccroll_before()
 *   2. @c fonction_du_test()
 *   3. sccroll_after()
 * 3. sccroll_clean() (lors d'un exit).
 *
 * Tous les rapports sont affichés par défault sur stderr.
 * @endparblock
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute les tests unitaires et affiche un rapport.
 *
 * Exécute tous les tests définis avec la macro #SCCROLL_TEST (ou
 * enregistrés avec la fonction sccroll_register() ), et affiche un
 * rapport détaillé sur le résultat.
 *
 * @attention Cette fonction est appelée automatiquement par la
 * fonction main() fournie. Elle n'est à utiliser que lors d'une
 * redéfinition de cette dernière.
 * @return le nombre de tests ayant échoué.
 */
int sccroll_run(void);

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Asserts Fonctions d'assertion.
 *
 * @{
 ******************************************************************************/
// clang-format on

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup AssertGroup Assertion sur des groupes de tests.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollGroupLogic
 * @since 0.1.0
 * @brief Résultats de logique booléenne attendus sur un groupe de
 * tests.
 *
 * Les mots clés agissent comme des drapeaux définissant le résultat
 * attendu d'un groupe de tests, décrit dans le tableau suivant
 * (@c success pour succès, @c failure pour échec, les en-têtes
 * décrivent le nombre de tests réussis @c n):
 *
 * |         | 0          | 1          | 2 <= n < all | tous       |
 * |---------+------------+------------+--------------+------------|
 * | @c NONE | @c success | @c failure | @c failure   | @c failure |
 * | @c XOR  | ^          | ^          | ^            | @c success |
 * | @c ALL  | @c failure | ^          | ^            | ^          |
 * | @c MANY | ^          | ^          | @c success   | ^          |
 * | @c ANY  | ^          | @c success | ^            | ^          |
 * | @c SOME | ^          | ^          | ^            | @c failure |
 * | @c MULT | ^          | @c failure | ^            | ^          |
 * | @c ONE  | ^          | @c success | @c failure   | ^          |
 */
typedef enum SccrollGroupLogic {
    NONE = 1,          /**< Tous les tests sont faux. */
    ONE  = 2,          /**< Un seul test est vrai. */
    MULT = 4,          /**< Au moins deux tests sont vrais, mais pas tous. */
    ALL  = 8,          /**< Tous les tests sont vrais. */
    MANY = MULT | ALL, /**< Au moins deux tests sont vrais. */
    ANY  = ONE | MANY, /**< Au moins un test est vrai. */
    SOME = ONE | MULT, /**< Au moins un test est vrai, mais pas tous. */
    XOR  = NONE | ALL, /**< Tous les tests sont soit vrais soit faux. */
} SccrollGroupLogic;

/**
 * @def SCCROLL_SENTRY
 * @since 0.1.0
 * @brief Valeur sentinelle des fonctions à nombre d'arguments
 * variable.
 *
 * Donne une valeur sentinelle utilisable par les fonctions à nombre
 * d'arguments variable.
 */
#define SCCROLL_SENTRY -1

/**
 * @{
 * @since 0.1.0
 * @brief Vérifie si le groupe de tests donnés répondent au critère
 * SccrollGroupLogic attendu.
 *
 * @attention Le dernier argument sentinelle doit être la macro
 * #SCCROLL_SENTRY (qui peut être redéfinie au besoin). Les macros
 * @c assert* de ce groupe fournissent directement la valeur à la
 * fonction.
 *
 * @throw AssertionError si l'ensemble des tests ne correspondent pas
 * à la logique attendue.
 * @param logic Drapeau type SccrollGroupLogic donnant les critères
 * attendus pour l'ensemble des tests.
 * @param ... L'ensemble des tests à analyser, qui doivent tous
 * renvoyer 0 (échec) ou une valeur positive (réussite).
 */
void sccroll_assertGroup(SccrollGroupLogic logic, ...);
#define assertGroup(logic, ...) sccroll_assertGroup(logic, __VA_ARGS__, SCCROLL_SENTRY)
#define assertNone(...) assertGroup(NONE, __VA_ARGS__)
#define assertOne(...)  assertGroup(ONE, __VA_ARGS__)
#define assertMult(...) assertGroup(MULT, __VA_ARGS__)
#define assertAll(...)  assertGroup(ALL, __VA_ARGS__)
#define assertMany(...) assertGroup(MANY, __VA_ARGS__)
#define assertAny(...)  assertGroup(ANY, __VA_ARGS__)
#define assertSome(...) assertGroup(SOME, __VA_ARGS__)
#define assertXor(...)  assertGroup(XOR, __VA_ARGS__)
/** @} */

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup AssertSingle Alias clarifiant les assertions simples.
 *
 * @{
 ******************************************************************************/
// clang-format on

#define assertTrue(test)  assert((bool)test)
#define assertFalse(test) assertTrue(!(bool)test)
#define assertNull        assertFalse

void sccroll_assertMsg(int test, const char* restrict format, ...);
#define assertMsg sccroll_assertMsg

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup AssertProcess Assertions sur les effets secondaires de
 * l'exécution de fonctions.
 *
 * Cette section est surtout conçue pour tester les fonctions qui
 * peuvent quitter le programme, qui modifient la valeur de errno, ou
 * qui inscrivent des messages dans un fichier ou une sortie standard.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct SccrollProcess
 * @since 0.1.0
 * @brief Gère les informations sur les effets secondaires de
 * l'exécution d'une fonction.
 */
typedef struct SccrollProcess {
    struct {
        int fd;              /**< Descripteur de fichier altéré par la fonction. */
        char* str;           /**< Texte inscrit par la fonction dans file. */
    } output;                /**< Vérification des outputs générés. */
    int errcode;             /**< Code d'erreur attendu. */
    int exitcode;            /**< Code de status attendu. */
    SccrollTestFunc wrapper; /**< Wrapper de la fonction à tester. */
    const char* name;        /**< Nom descriptif de l'assertion. */
} SccrollProcess;

/**
 * @since 0.1.0
 * @brief Vérifie les effets de la fonction à tester.
 *
 * @parblock
 * Exécute la fonction à tester et détermine si les effets secondaires
 * sont ceux attendus, décrits dans la structure SccrollProcess.
 *
 * La valeur de #errno est remise à 0 avant exécution de la fonction.
 * @endparblock
 *
 * @param proc La structure SccrollProcess contenant les informations
 * sur les effets attendus.
 * @throw AssertionError si les effets attendus ne sont pas ceux
 * obtenus.
 */
void sccroll_assertExe(const SccrollProcess* restrict proc) __attribute__((nonnull));

/**
 */
#define assertExe(w, ...)                                                  \
    {                                                                      \
        SccrollProcess proc = { .name = #w, .wrapper = w, ##__VA_ARGS__ }; \
        sccroll_assertExe(proc);                                           \
    }

/**
 * @{
 * @since 0.1.0
 * @brief Teste la valeur du code d'erreur obtenu après éxécution de
 * la fonction.
 *
 * Effectue une assertion sur le code d'erreur (ou de status pour une
 * fonction qui termine le programme) obtenu après exécution de la
 * fonction testée. #errno est toujours remise à 0 lors de ce test.
 *
 * @throw AssertionError si le code ne correspond pas à celui attendu.
 * @param wrapper La fonction wrapper de la fonction à tester.
 * @param code Le code d'erreur attendu.
 */
#define assertErrno(wrapper, code) assertExe(wrapper, .errcode = code)
#define assertExit(wrapper, code)  assertExe(wrapper, .exitcode = code)
/** @} */

/**
 * @since 0.1.0
 * @brief Teste la modification d'un fichier par la fonction.
 *
 * Détermine si le fichier décrit par le descripteur #fd aurait été
 * modifié par la fonction testée (exécutée via le wrapper #w) avec la
 * chaîne #string. La sortie est capturée par la fonction, et donc la
 * modification n'est pas réellement effectuée au cours du test.
 *
 * @throw AssertionError si la chaîne donnée ne correspond pas à celle
 * attendue pour le descripteur de fichier donné.
 * chaîne donnée.
 * @param wrapper La fonction wrapper de la fonction à tester.
 * @param fd Le descripteur de fichier modifié par la fonction.
 * @param string La chaîne attendue pour inscription dans le fichier.
 */
#define assertOutput(wrapper, fd, string) \
    assertExe(wrapper, .output = { .fd = fd, .str = string })

/**
 * @{
 * @since 0.1.0
 * @brief Vérifie si la fonctions affiche un message sur la sortie
 * standard.
 *
 * Détermine quels messages sont affichés par la fonction testée sur
 * la sortie standard, et vérifie qu'ils correspondent à #string.
 *
 * @param wrapper La fonction wrapper de la fonction à tester.
 * @param string Les messages affichés attendus.
 */
#define assertStdout(wrapper, string) assertOutput(wrapper, STDOUT_FILENO, string)
#define assertStderr(wrapper, string) assertOutput(wrapper, STDERR_FILENO, string)
/** @} */

// clang-format off

/******************************************************************************
 * @}
 * @}
 ******************************************************************************/
// clang-format on

#endif // @} SCCROLL_H_
