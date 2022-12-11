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
#include <ctype.h>
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
 * @typedef SccrollFunc
 * @since 0.1.0
 * @brief Prototype des fonctions de test unitaires.
 */
typedef void (*SccrollFunc)(void);

/**
 * @enum SccrollIndexes
 * @since 0.1.0
 * @brief Index des tables de SccrollEffects.
 * @note #SCCMAX est généralement utilisé comme valeur d'index maximal
 * dans la librairie.
 * @attention Les index utilisables de SccrollEffects::std sont
 * #STDOUT_FILENO et #STDERR_FILENO.
 */
typedef enum SccrollIndexes {
    /**
     * @name Codes
     * @brief codes d'erreur de la fonction de test.
     * @{ */
    SCCERRNUM = 0, /**< Code errno. */
    SCCSIGNAL = 1, /**< Code de signal. */
    SCCSTATUS = 2, /**< Code de status/exit. */
    SCCMAXSIG = 3, /**< Index maximal de SccrollEffects::codes. */
    /** @} */
    SCCMAXSTD = STDERR_FILENO + 1, /**< Index maximal de SccrollEffects::std. */
    SCCMAX    = BUFSIZ,            /**< Index maximal de SccrollEffects::files. */
} SccrollIndexes;

/**
 * @enum SccrollFlags
 * @since 0.1.0
 * @brief Drapeaux d'options pour un test.
 */
typedef enum SccrollFlags {
    NOSTRP = 1, /**< Ne pas réduire les espaces autour des sorties standard.*/
    NOFORK = 2, /**< Ne pas @c fork avant d'exécuter le test. */
    NODIFF = 4, /**< Ne pas afficher les différences attendu/obtenu. */
} SccrollFlags;

/**
 * @def sccroll_hasFlags
 * @since 0.1.0
 * @brief Détermine si les drapeaux @p values sont contenus dans flags.
 * @param flags Les drapeaux combinés avec OR.
 * @param values Les drapeaux recherchés combinés avec OR.
 * @return @p values si @p flags les contient, sinon 0.
 * @throw SIGABRT si @p value a une valeur impaire si > 1.
 */
#define sccroll_hasFlags(flags, values) ((flags) & (values))

/**
 * @struct SccrollEffects
 * @since 0.1.0
 * @brief Gère les informations sur les effets secondaires de
 * l'exécution d'une fonction.
 *
 * @parblock
 * Cette structure est la structure principale de la librairie. Elle
 * sert à passer au programme les informations sur les effets que l'on
 * attend d'un test, qui échouera si l'un d'eux diffère en réalité.
 *
 * La structure est également versatile. Elle permet de tester une
 * fonction "généraliste", contenant par exemple de multiples
 * assertions, ou encore de tester les effets attendu d'une unique
 * fonction appelée seule (et sans assertions) dans la fonction
 * SccrollEffects::wrapper.
 *
 * À l'exception de SccrollEffects::files, toutes les valeurs de
 * #expected sont vérifiées, même si elles sont nulles. Les chaînes de
 * caractères NULL équivalent à une chaîne vide. De plus, la valeur de
 * #errno est remise à 0 juste avant l'exécution du test.
 *
 * Pour SccrollEffects::files, seuls les fichiers ayant un
 * SccrollEffects::files::path défini (non @c NULL ) sont testés.
 * @endparblock
 */
typedef struct SccrollEffects {
    struct {
        const char* path; /**< Le chemin du fichier. */
        char* content;    /**< Le contenu du fichier. */
    } files[SCCMAX];      /**< Vérification du  contenu de fichiers. */
    char* std[SCCMAXSTD]; /**< Vérification des outputs sur les sorties standard. */
    int codes[SCCMAXSIG]; /**< Vérification des codes d'erreur, signal et status. */
    int flags;            /**< Drapeaux d'options SccrollFlags. */
    SccrollFunc wrapper;  /**< La fonction de test unitaire. */
    const char* name;     /**< Nom descriptif du test. */
} SccrollEffects;

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
 * @brief Inscrit le test décrit par #expected pour exécution.
 * @attention Cette fonction n'a pas besoin d'être appelée par
 * l'utilisateur s'il définit les tests à l'aide de la macro
 * #SCCROLL_TEST. Un test défini par cette dernière et inscrit avec
 * sccroll_register() sera exécuté deux fois.
 * @param expected Les informations nécessaires pour l'exécution d'un
 * test et comparaison des résultats.
 */
void sccroll_register(const SccrollEffects* restrict expected)
    __attribute__((nonnull));

/**
 * @ingroup Definition
 * @def SCCROLL_TEST
 * @since 0.1.0
 * @brief Définit et enregistre un test pour exécution.
 *
 * @parblock
 * Créé et inscrit une fonction de test unitaire #testname ayant
 * pour code la suite d'instructions située dans les accolades suivant
 * directement la macro (comme pour la définition d'une fonction), et
 * l'inscrit pour exécution. Le nom du test est identique à celui de
 * la fonction.
 *
 * Il est possible de fournir les valeurs attendues pour les
 * différents membres de SccrollEffects (à l'exception de
 * SccrollEffects::wrapper et SccrollEffects::name), comme lors d'une
 * initialisation de la structure.
 *
 * Toute donnée attendue non fournie équivaut à une donnée attendue
 * valant 0 (ou vide pour les chaînes de caractères).
 *
 * Exemples de définition de tests:
 * @include tests/sccroll_template_tests.c
 * @enparblock
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
#define SCCASSERTFMT "%s (l. %i): Assertion `%s' failed."

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
void sccroll_assert(int expr, const char* restrict fmt, ...)
    __attribute__((nonnull(2)));

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

#if !defined(_ASSERT_H) || (defined(_ASSERT_H) && defined(NDEBUG))
#undef assert

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
            SCCASSERTFMT, __FILE__, __LINE__, #expr)
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
 * @def sccroll_unused
 * @since 0.1.0
 * @brief Indique à la fonction qu'un paramètre n'est pas utilisé.
 *
 * Cette macro est utile pour les mocks pour éviter les erreurs de
 * compilation si un des paramètres n'est pas utilisé.
 */
#define sccroll_unused(var) (void) var

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

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Drapeaux utilisables par sccroll_mockTrigger() pour indiquer
 * quel simulacre pré-fourni doit être en erreur.
 */
typedef enum SccrollMockFlags {
    SCCENONE  = 0, /**< Drapeau ne provoquant pas d'erreurs. */
    SCCEABORT = 2, /**< Drapeau de abort(). */
} SccrollMockFlags;

/**
 * @since 0.1.0
 * @brief Fonction utilisée pour provoquer une erreur dans les
 * simulacres fournis par la bibliothèque.
 * @attention Cette fonction de provoque pas d'erreur par défaut. Elle
 * peut cependant être redéfinie sans problèmes par l'utilisateur afin
 * de provoquer les erreurs voulues selon les conditions voulues.
 * @see sccroll_hasFlags
 * @return Un ensemble de SccrollMockFlags combinés avec OR; tous les
 * simulacres correspondants aux drapeaux entreront en erreur.
 */
unsigned sccroll_mockTrigger(void);

/**
 * @since 0.1.0
 * @brief Simulacre de @c abort permettant de sauvegarder les données
 * de couverture de gcov.
 *
 * La fonction @c abort interromp la récolte de données entamée par @c
 * gcov , provoquant ainsi un biais de couverture. L'utilisation de ce
 * simulacre corrige ces biais.
 *
 * Ce simulacre peut provoquer une erreur si #SCCEABORT est contenu
 * dans les drappeaux renvoyés par sccroll_mockTrigger(). L'erreur
 * provoquée est que le programme quittera avec `exit(SIGABRT)` au
 * lieu d'utiliser la fonction __real_abort().
 */
void __wrap_abort(void) __attribute__((noreturn));

// clang-format off
/******************************************************************************
 * @}
 * @}
 ******************************************************************************/
// clang-format on

#endif // @} SCCROLL_H_
