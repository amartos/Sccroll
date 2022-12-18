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

#include <argz.h>
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
    char* std[SCCMAXSTD]; /**< Vérification des outputs sur les sorties standard. */
    int codes[SCCMAXSIG]; /**< Vérification des codes d'erreur, signal et status. */
    unsigned flags;       /**< Drapeaux d'options SccrollFlags. */
    SccrollFunc wrapper;  /**< La fonction de test unitaire. */
    const char* name;     /**< Nom descriptif du test. */
} SccrollEffects;

/**
 * @name Générateurs de données
 * @{
 */

/**
 * @def sccroll_monkey
 * @since 0.1.0
 * @brief Rempli un espace mémoire de données aléatoires.
 * @param blob Un espace mémoire à remplir.
 * @param size Le nombre d'octets à remplir.
 */
#define sccroll_monkey arc4random_buf
/** @} */

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
 * @}
 *
 * @addtogroup AssertAPI Assertions
 *
 * La bibliothèque définit sa propre macro assert() et/ou remplace celle
 * de la bibliothèque standard @c assert.h dans deux cas:
 * - la librairie @c assert.h n'est pas incluse;
 * - la librairie @c assert.h est incluse, mais @c NDEBUG est défini.
 *
 * Ceci assure que l'ensemble des assertions des tests unitaires
 * seront bien effectuées dans tous les cas. De même, les tests
 * unitaires deviennent donc insensibles à la définition de @c NDEBUG.
 *
 * La macro assert(), bien que mimant le comportement de celle de la
 * bibliothèque standard, affiche un message différent de cette
 * dernière.
 *
 * La librairie s'assure aussi que les données de couverture pour gcov
 * sont bien récupérées avant l'appel à @c abort; la fonction
 * sccroll_abort() est spécifiquement conçue dans cette optique.
 * Dans le cas où la macro @c assert de la librairie standard est
 * utilisée, un mock de @c abort est doit être généré par la librairie
 * dans ce but.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def SCCASSERTFMT
 * @since 0.1.0
 * @brief Format des messages d'erreurs d'assertion.
 * @see sccroll_assert()
 * @param s Le nom du fichier.
 * @param i Le numéro de ligne.
 * @param s Le nom de la fonction.
 * @param s L'expression testée.
 */
#define SCCASSERTFMT "%s (l. %i): Assertion `%s' failed."

/**
 * @name Assertions avec messages personnalisés
 *
 * Les macros et fonctions suivantes ne génèrent pas de message
 * d'erreur automatiquement, laissant sa définition à l'utilisateur.
 *
 * @note Ces macros et fonctions appellent __gcov_dump() *avant*
 * d'appeler abort().
 * @{
 * @param expr Expression booléenne indiquant de lever une erreur
 * d'assertion si elle est fausse.
 * @param fmt Chaîne de formatage du message d'erreur.
 * @param ... Paramètres de la chaîne de formatage.
 * @throw SIGABRT si @p expr est fausse.
 */

/**
 * @since 0.1.0
 */
void sccroll_assert(int expr, const char* restrict fmt, ...)
    __attribute__((nonnull(2)));

/**
 * @def assertMsg
 * @since 0.1.0
 * @brief Alias de sccroll_assert().
 */
#define assertMsg sccroll_assert
/** @} */

/**
 * @name Assertion
 * @{
 * @param expr Expression booléenne indiquant de lever une erreur
 * d'assertion si elle est fausse.
 * @throw SIGABRT si @p expr est fausse.
 */

#if !defined(_ASSERT_H) || (defined(_ASSERT_H) && defined(NDEBUG))
#undef assert

    /**
     * @def assert
     * @since 0.1.0
     * @brief Alternative à la macro @c assert de la librairie
     * standard.
     * @attention Cette macro n'est pas chargée si le fichier header
     * @c assert.h est inclus dans un fichier de tests. De plus, au
     * contraire de celle de la librairie standard, cette macro-ci n'est
     * pas sensible à la définition de la macro @c NDEBUG .
     */
    #define assert(expr)             \
        sccroll_assert((bool)(expr), \
            SCCASSERTFMT, __FILE__, __LINE__, #expr)
#endif // _ASSERT_H

/**
 * @def assertTrue
 * @since 0.1.0
 * @brief Alias de assert().
 */
#define assertTrue assert
/** @} */

/**
 * @name Assertions inversées
 * @{
 * @param expr Expression booléenne indiquant de lever une erreur
 * d'assertion si elle est vraie.
 * @throw SIGABRT si @p expr est vraie.
 */

/**
 * @def assertFalse
 * @since 0.1.0
 */
#define assertFalse(expr) assertTrue(!(expr))

/**
 * @def assertNot
 * @since 0.1.0
 * @brief Alias de assertFalse().
 */
#define assertNot assertFalse

/**
 * @def assertNull
 * @since 0.1.0
 * @brief Alias de assertFalse().
 */
#define assertNull assertFalse
/** @} */

/**
 * @name Assertions sur l'identité de pointeurs
 * @brief Macros et fonctions levant une erreur d'assertion si la
 * comparaison des **pointeurs** est fausse.
 * @attention Les pointeurs ne sont pas déréférencés, et leur
 * comparaison implique donc de comparer les adresses qu'ils
 * contiennent et non les données correspondantes.
 * @{
 * @param a,b Pointeurs à comparer.
 * @throw SIGABRT si la comparaison attendue est fausse.
 */

/**
 * @def assertEql
 * @since 0.1.0
 * @brief Vérifie que `a == b`
 */
#define assertEql(a, b) assert(a == b)

/**
 * @def assertNotEql
 * @since 0.1.0
 * @brief Vérifie que `a != b`
 */
#define assertNotEql(a, b) assert(a != b)
/** @} */

/**
 * @name Assertions sur les données
 * @brief Macros et fonctions levant une erreur d'assertion si la
 * comparaison des **données pointées** est fausse.
 * @{
 * @param a,b Pointeurs de données à comparer.
 * @param sign Signe de comparaison d'entiers symbolisant la
 * comparaison attendue entre @p a et @p b. La comparaison est
 * toujours effectuée dans le sens `a sign b`
 * @param cmp Fonction de comparaison prenant au moins les deux
 * pointeurs @p a et @p b en arguments et renvoyant un nombre négatif,
 * nul ou positif selon que, respectivement: `a < b`, `a == b` ou
 * `a > b`. Le prototype attendu est analogue à celui des fonctions
 * utilisées par
 * [qsort](https://www.gnu.org/software/libc/manual/html_node/Comparison-Functions.html),
 * bien que le type de @p a et @p b ne soit pas restreint
 * à @c void* .
 * @param ... Arguments supplémentaires **optionnels** pour @p cmp qui
 * lui sont passés tels quels. Le prototype de @p cmp doit alors être
 * celui d'une fonction de comparaison, mais dont les arguments
 * supplémentaires sont situés *après* les pointeurs à comparer:
 * `int cmp(type a, type b, type arg1, type arg2, etc);`
 * @throw SIGABRT si la comparaison attendue est fausse
 */

/**
 * @def assertCmp
 * @since 0.1.0
 * @brief Effectue la comparaison `a sign b`.
 */
#define assertCmp(a, sign, b, cmp, ...) assert(cmp(a, b, ##__VA_ARGS__) sign 0)

/**
 * @def assertEqual
 * @since 0.1.0
 * @brief Vérifie que les données de @p a et @p b sont identiques.
 */
#define assertEqual(a, b, cmp, ...) assertCmp(a, ==, b, cmp, ##__VA_ARGS__)

/**
 * @def assertNotEqual
 * @since 0.1.0
 * @brief Vérifie que les données de @p a et @p b sont différentes.
 */
#define assertNotEqual(a, b, cmp, ...) assertCmp(a, !=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmaller
 * @since 0.1.0
 * @brief Vérifie que les données de @p a sont plus petites que celles de
 * @p b selon @p cmp.
 */
#define assertSmaller(a, b, cmp, ...) assertCmp(a, <, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreater
 * @since 0.1.0
 * @brief Vérifie que les données de @p a sont plus grandes que celles de
 * @p b selon @p cmp.
 */
#define assertGreater(a, b, cmp, ...) assertCmp(a, >, b, cmp, ##__VA_ARGS__)

/**
 * @def assertSmallerOrEqual
 * @since 0.1.0
 * @brief Vérifie que les données de @p a sont plus petites ou égales que
 * celles de @p b selon @p cmp.
 */
#define assertSmallerOrEqual(a, b, cmp, ...) assertCmp(a, <=, b, cmp, ##__VA_ARGS__)

/**
 * @def assertGreaterOrEqual
 * @since 0.1.0
 * @brief Vérifie que les données de @p a sont plus grandes ou égales que
 * celles de @p b selon @p cmp.
 */
#define assertGreaterOrEqual(a, b, cmp, ...) assertCmp(a, >=, b, cmp, ##__VA_ARGS__)
/** @} */

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup MocksAPI Simulacres
 *
 * En C, la définition de simulacres (*mocks* en anglais) n'a pas la
 * même signification que dans les langages orientés objets. Ici, un
 * simulacre sera principalement une fonction (et non plus un objet)
 * altérée de manière à pouvoir contrôler finement son comportement
 * lors de l'exécution du programme.
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
    retval __wrap_##name(__VA_ARGS__)

/**
 * @name Simulacres prédéfinis
 * @{
 */

/**
 * @enum SccrollMockFlags
 * @since 0.1.0
 * @brief Drapeaux utilisables par sccroll_mockTrigger() pour indiquer
 * quel simulacre pré-fourni doit être en erreur.
 */
typedef enum SccrollMockFlags {
    SCCENONE  = 0, /**< Drapeau ne provoquant pas d'erreurs. */
    SCCEABORT = 2, /**< Drapeau de __wrap_abort(). */
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
/** @} */


// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_H_
/** @} */
