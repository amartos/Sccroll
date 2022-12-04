/**
 * @file        sccroll.h
 * @version     0.1.0
 * @brief       Librairie Sccroll, framework de tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -Wextra -std=gnu99 \
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
#include <time.h>

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

/**
 * @since 0.1.0
 * @brief Rempli un espace mémoire de données aléatoires.
 *
 * Remmplit l'espace mémoire pointé par #blob sur #size octets de
 * données aléatoires.
 *
 * @param blob Un espace mémoire à remplir.
 * @param size Le nombre d'octets à remplir.
 */
#define sccroll_monkey arc4random_buf

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
    static void name(void);                                         \
    __attribute__((constructor)) void sccroll_register_##name(void) \
    {                                                               \
        sccroll_register(name, #name);                              \
    }                                                               \
    static void name(void)

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

/**
 * @ingroup FormatStrings
 * @def SCCASSERTFMT
 * @since 0.1.0
 * @brief Format des messages d'erreurs d'assertion.
 * @param s Le nom du fichier.
 * @param s Le numéro de ligne.
 * @param s Le nom de la fonction.
 * @param s L'expression testée.
 */
#define SCCASSERTFMT "(%s line %i) %s: Assertion `%s' failed."

/**
 * @name AssertMsg
 * @brief Si #test vaut 0, lève une erreur d'assertion, affiche un
 * message d'erreur et termine le programme.
 * @param test Indique de lever une erreur d'assertion si sa valeur
 * est nulle. N'importe quelle autre valeur évite l'erreur.
 * @param fmt Chaîne de formatage du message d'erreur.
 * @param ... Paramètres de la chaîne de formatage.
 * @throw AssertionError si #test vaut 0.
 * @{
 */

/**
 * @since 0.1.0
 */
void sccroll_assert(int test, const char* restrict fmt, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @def assertMsg
 * @since 0.1.0
 * Cette macro est un alias simplifié de sccroll_assert().
 */
#define assertMsg sccroll_assert
/** @} */

/**
 * @name Assertion
 * @brief Si l'expression #expr est fausse, lève une erreur
 * d'assertion, affiche un message d'erreur et termine le programme.
 * @param expr Une expression à évaluer.
 * @throw AssertionError si #expr est fausse.
 */

#ifndef _ASSERT_H
    /**
     * @def assert
     * @since 0.1.0
     *
     * Cette macro est une alternative à la macro @c assert de la
     * librairie standard.
     *
     * @attention Cette macro n'est pas chargée si le fichier header
     * @c assert.h est inclus dans un fichier de tests. De plus, au
     * contraire de celle de la librairie standard, cette macro-ci n'est
     * pas sensible à la définition de la macro #NDEBUG.
     */
    #define assert(expr)             \
        sccroll_assert((bool)(expr), \
            SCCASSERTFMT, __FILE__, __LINE__, __FUNCTION__, #expr)
#endif // _ASSERT_H

/**
 * @def assertTrue
 * @since 0.1.0
 * Alias de assert pour plus de clarté.
 */
#define assertTrue assert
/** @} */

/**
 * @name AssertFalse
 * @brief Assertion vérifiant que l'expression #expr est fausse.
 * @param expr Une expression à évaluer.
 * @throw AssertionError si l'expression #expr est vraie.
 * @{
 */

/**
 * @def assertFalse
 * @since 0.1.0
 */
#define assertFalse(expr) assertTrue(!(expr))

/**
 * @def assertNot
 * @since 0.1.0
 * Alias de #assertFalse.
 */
#define assertNot assertFalse

/**
 * @def assertNull
 * @since 0.1.0
 * Alias de #assertFalse.
 */
#define assertNull assertFalse
/** @} */

/**
 * @name AssertPointers
 * @brief Lève une erreur d'assertion si la comparaison des pointeurs
 * #a et #b est fausse.
 * @param a,b Deux pointeurs à comparer.
 * @throw AssertionError si la comparaison est fausse.
 * @{
 */

/**
 * @def assertEql
 * @since 0.1.0
 * Vérifie que @verbatim a == b @endverbatim.
 */
#define assertEql(a, b) assert(a == b)

/**
 * @brief assertNotEql
 * @since 0.1.0
 * Vérifie que @verbatim a != b @endverbatim.
 */
#define assertNotEql(a, b) assert(a != b)
/** @} */

/**
 * @name AssertComp
 * @brief Assertion vérifiant que la comparaison des données des deux
 * variables indiquée est vraie.
 * @attention Au contraire de #assertEql, ce sont les données de #a et
 * #b qui sont comparées, et non les pointeurs.
 * @param a,b Pointeurs des variables à comparer.
 * @param cmp Fonction de comparaison prenant au moins les deux
 * pointeurs en arguments et renvoyant un nombre négatif, nul ou
 * positif selon que, respectivement, @verbatim a < b @endverbatim,
 * @verbatim a == b @endverbatim ou @verbatim a > b @endverbatim. Le
 * modèle est celui des fonction de type #comparison_fn_t (utilisé par
 * qsort()).
 * @param ... Arguments supplémentaires optionnels pour #cmp qui lui
 * sont passés tels quels.
 * @throw AssertionError si la comparaison est fausse.
 */

/**
 * @def assertCmp
 * @since 0.1.0
 * @param sign Signe de comparaison des variables parmi
 * @verbatim < <= == => > != @enverbatim; la comparaison est toujours
 * dans le sens @verbatim a sign b @enverbatim.
 */
#define assertCmp(a, sign, b, cmp, ...) assert(cmp(a, b, ##__VA_ARGS__) sign 0)

/**
 * @def assertEqual
 * @since 0.1.0
 *
 * Vérifie que les données de #a et #b sont identiques.
 */
#define assertEqual(a, b, cmp, ...) assertCmp(a, ==, b, cmp, ##__VA_ARGS__)

/**
 * @def assertNotEqual
 * @since 0.1.0
 *
 * Vérifie que les données de #a et #b sont différentes.
 */
#define assertNotEqual(a, b, cmp, ...) assertCmp(a, !=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmaller
 * @since 0.1.0
 *
 * Vérifie que les données de #a sont toutes plus petites que delles
 * de #b.
 */
#define assertSmaller(a, b, cmp, ...) assertCmp(a, <, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreater
 * @since 0.1.0
 *
 * Vérifie que les données de #a sont toutes plus grandes que celles
 * de #b.
 */
#define assertGreater(a, b, cmp, ...) assertCmp(a, >, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmallerOrEqual
 * @since 0.1.0
 *
 * Vérifie que les données de #a sont toutes plus petites que ou
 * égales à celles de #b.
 */
#define assertSmallerOrEqual(a, b, cmp, ...) assertCmp(a, <=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreaterOrEqual
 * @since 0.1.0
 *
 * Vérifie que les données de #a sont toutes plus grandes que ou
 * égales à celles de #b.
 */
#define assertGreaterOrEqual(a, b, cmp, ...) assertCmp(a, >=, b, cmp, ##__VA_ARGS__)
/** @} */

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

// clang-format off

/******************************************************************************
 * @addtogroup LibMocks Génération de mocks.
 *
 * Afin de pouvoir générer des résultats prévisibles pour certaines
 * assertions (par exemple, les captures d'erreurs conditionnelles),
 * il est parfois nécessaire de générer des mocks des fonctions
 * d'autres librairies. Les macros de cette section sont utilisées à
 * ces fins.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def SCCROLL_MOCK
 * @since 0.1.0
 * @brief Génère un un mock d'une fonction.
 *
 * @parblock
 * Cette macro est un simple alias pour la définition d'un @c wrapper
 * par le linker de GCC (@c ld ). Elle génère une fonction appelée à
 * la place de l'originale #name, cette dernière restant accessible
 * via l'alias @c __real_name().
 *
 * La macro s'utilise comme pour la définition d'une fonction.
 * Voici un exemple de mock, ici de la fonction calloc() de la
 * librairie standard C:
 *
 * @code{.c}
 * SCCROLL_MOCK(void*, calloc, size_t nmemb, size_t size)
 * {
 *     // ... mon code, exemple:
 *     if (my_error_trigger) return NULL;
 *     // On utilise ici la fonction calloc originale.
 *     return __real_calloc(nmemb, size);
 * }
 * @endcode
 * @endparblock
 *
 * @attention Nécessite l'option
 * @verbatim -Wl,--wrap,name,--wrap,... @endverbatim
 * (un @verbatim --wrap,name @enverbatim pour chaque fonction mockée)
 * à la compilation. La syntaxe est celle indiquée dans le manuel de
 * GCC (option @c -Wl ).
 *
 * @param rettype Le type des données renvoyées par la fonction.
 * @param name Le nom de la fonction mockée.
 * @param ... Les paramètres de la fonction mockée
 * (syntaxe: @verbatim rettype, name, typea variablea,
 * typeb variableb, ... @endverbatim) ou @c void si aucun paramètre.
 */
#define SCCROLL_MOCK(retval, name, ...)         \
    extern __typeof__(name) __real_##name;      \
    retval __wrap_##name(__VA_ARGS__)

// clang-format off
/******************************************************************************
 * @}
 * @}
 ******************************************************************************/
// clang-format on

#endif // @} SCCROLL_H_
