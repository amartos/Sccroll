/**
 * @file        assert.h
 * @version     0.1.0
 * @brief       Ficher en-tête des assertions de Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup API
 * @{
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
 */

#ifndef SCCROLL_ASSERT_H_
#define SCCROLL_ASSERT_H_

#include "sccroll/helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// clang-format off

/******************************************************************************
 * @name Sauvegarde des données de couverture.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fonction sauvegardant les données utilisées par gcov.
 */
extern void __gcov_dump(void);

// clang-format off

/******************************************************************************
 * @}
 * @name Format des messages d'assertion.
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

// clang-format off

/******************************************************************************
 * @}
 * @name Gestions d'erreurs
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Affiche un message sur stderr et lève un signal d'erreur.
 * @param sigint Le signal à lever (@c SIGABRT si le signal est
 * ignoré).
 * @param fmt La chaîne de formatage du message.
 * @param args Les arguments de la chaîne de formatage.
 */
void sccroll_vfatal(int sigint, const char* restrict fmt, va_list args)
    __attribute__((noreturn,format(printf,2,0)));

/**
 * @since 0.1.0
 * @brief Affiche un message sur @c stderr, sauvegarde les données
 * pour gcov et lève le signal donné (puis lève @c SIGABRT).
 * @param sigint Le code du signal.
 * @param fmt La chaîne de formatage du message.
 * @param ... Les arguments de la chaîne de formatage.
 */
void sccroll_fatal(int sigint, const char* restrict fmt, ...)
    __attribute__((noreturn, format(printf,2,3)));


/**
 * @name Try-Catch-Finally
 *
 * Groupe de macros fonctionnant ensemble pour attraper des erreurs.
 *
 * Ce groupe n'utilise **pas** setjmp() ou longjmp(). Il peut être
 * imbriqué, et plusieurs try-catch peuvent être mélangés au même
 * niveau, tant que l'identifiant donné reste unique (ils agissent
 * comme des labels). Les accolades ne sont pas nécessaires pour
 * séparer le code.
 *
 * Pour chaque #catch, un #throw est nécessaire. #try seul est
 * utilisable sans problèmes, cependant sans grand intérêt.
 * @{
 * @param id L'identifiant correspondant au try-catch voulu, donné à
 * #try.
 * @param error Un identifiant d'erreur à lever/attraper. La valeur
 * n'a pas d'importance, seul le mot l'est.
 */

/**
 * @def try
 * @since 0.1.0
 * @brief Démarre le block de code à tester.
 * @param id Un identifiant **unique** (local à la fonction).
 * @note Cette macro ne fait rien, mais permet de bien identifier le
 * début du try-catch donné.
 */
#define try(id)

/**
 * @def throw
 * @since 0.1.0
 * @brief Lève une erreur à attraper.
 */
#define throw(id,error) goto id ## error;

/**
 * @since 0.1.0
 * @brief Attrape une erreur levée et exécute un block de code.
 */
#define catch(id,error) goto id ## finally; id ## error:

/**
 * @since 0.1.0
 * @brief Exécute un bloc de code après tous les autres.
 */
#define finally(id)     id ## finally:
/** @} */

// clang-format off

/******************************************************************************
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
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 */
void sccroll_assert(int expr, const char* restrict fmt, ...)
    __attribute__((nonnull (2), format(printf,2,3)));

/**
 * @def assertMsg
 * @since 0.1.0
 * @brief Alias de sccroll_assert().
 */
#define assertMsg sccroll_assert

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Assertion
 * @{
 * @param expr Expression booléenne indiquant de lever une erreur
 * d'assertion si elle est fausse.
 * @throw SIGABRT si @p expr est fausse.
 ******************************************************************************/
// clang-format on

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

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Assertions inversées
 * @{
 * @param expr Expression booléenne indiquant de lever une erreur
 * d'assertion si elle est vraie.
 * @throw SIGABRT si @p expr est vraie.
 ******************************************************************************/
// clang-format on

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

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Assertions sur l'identité de pointeurs
 * @brief Macros et fonctions levant une erreur d'assertion si la
 * comparaison des **pointeurs** est fausse.
 * @attention Les pointeurs ne sont pas déréférencés, et leur
 * comparaison implique donc de comparer les adresses qu'ils
 * contiennent et non les données correspondantes.
 * @{
 * @param a,b Pointeurs à comparer.
 * @throw SIGABRT si la comparaison attendue est fausse.
 ******************************************************************************/
// clang-format on

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

// clang-format off

/******************************************************************************
 * @}
 *
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
 ******************************************************************************/
// clang-format on

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

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_ASSERT_H_
/** @} @} */
