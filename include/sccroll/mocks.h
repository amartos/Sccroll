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

#define _GNU_SOURCE
#include <dlfcn.h>
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
 * Générer un simulacre se fait avec deux définitions.
 *
 * La première est celle de cette macro, qui donne les informations
 * nécessaires au fonctionnement du simulacre et à son déclenchement
 * d'erreurs.
 *
 * La seconde est celle d'une macro de remplacement dans une en-tête
 * importée par le code à tester. Cette macro redéfinit la fonction
 * remplacée par l'équivalente générée par la macro #SCCROLL_MOCK ; la
 * syntaxe est la suivante: `#define fonction sccroll_mockfonction`.
 *
 * Le simulacre est ensuite déclenché quand @p expr vaut @c true, et
 * renvoie @p errval. Il est déconseillé d'utiliser la fonction
 * `libfonction` (qui correspond à la fonction originale), car elle
 * n'est initialisée qu'après le premier appel du simulacre.
 *
 * @param expr Une expression booléenne déclenchant une erreur du
 * simulacre quand @c true.
 * @param errval La valeur renvoyée en cas d'erreur.
 * @param retval Le type des données renvoyées par la fonction
 * originelle.
 * @param name Le nom de la fonction originelle.
 * @param protoargs Les paramètres du prototype ; la macro #SCCCOMMA
 * facilite la construction de cet argument.
 * @param ... Les paramètres de la fonction originelle sans les type
 * de données (rien pour @c void).
 * @return @p errval si @p expr vaut @c true, sinon la valeur renvoyée
 * par la fonction originelle.
 */
#define SCCROLL_MOCK(expr, errval, retval, name, protoargs, ...)        \
    __typeof__(name) (*lib##name) = NULL;                               \
    retval sccroll_mock##name(protoargs)                                \
    {                                                                   \
        if (!lib##name) {                                               \
            lib##name = dlsym(RTLD_NEXT, #name);                        \
            if (!lib##name) err(EXIT_FAILURE, "%s", dlerror());         \
        }                                                               \
        return (expr) ? errval : lib##name(__VA_ARGS__);                \
    }

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
 * @attention Certains drapeaux déclenchent l'erreur d'un autre.
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
    SCCEFERROR,   /**< Drapeau de ferror(). */
    SCCEFOPEN,    /**< Drapeau de fopen() et ferror(). */
    SCCEFSEEK,    /**< Drapeau de fseek() et ferror(). */
    SCCEFTELL,    /**< Drapeau de ftell() et ferror(). */
    SCCEFREAD,    /**< Drapeau de fread() et ferror(). */
    SCCEFWRITE,   /**< Drapeau de fwrite() et ferror(). */
    SCCEFSCANF,   /**< Drapeau de fscanf() et ferror(). */
    SCCEFILENO,   /**< Drapeau de fileno(). */
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
 * sauvegarde les données pour gcov et lève un signal.
 * @param sigint Le signal d'erreur.
 * @param fmt La chaîne de formatage du message
 * @param ... Les arguments de la chaîne de formatage.
 */
void sccroll_mockFatal(int sigint, const char* restrict fmt, ...)
    __attribute__((noreturn,format(printf,2,3)));

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
 * @name Prototypes et macros des simulacres prédéfinis.
 *
 * Cette méthode est utilisée à la place de #SCCROLL_MOCK car des
 * simulacres de ces fonctions de la librairie standard provoquent des
 * erreurs lors des appels dans les librairies partagées, notamment la
 * standard.
 *
 * Cette section n'est pas destinée à être utilisée directement, mais
 * sert à rediriger les simulacres prédéfinis vers les fonctions de la
 * librairie.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_mockPrototype
 * @since 0.1.0
 * @brief Définit les prototypes des simulacres.
 */
#define sccroll_mockPrototype(name)           \
    extern __typeof__(name) (*lib##name);     \
    __typeof__(name) sccroll_mock##name

/**
 * @since 0.1.0
 * @brief Sauvegarde les informations sur le dernier appel d'un
 * simulacre.
 * @param funcname @c __FUNCTION__.
 * @param line @c __LINE__.
 * @param mock Le drapeau SccrollMockFlags du simulacre.
 */
void sccroll_mockTrace(const char* funcname, int line, SccrollMockFlags mock);

/**
 * @def sccroll_mockCall
 * @since 0.1.0
 * @brief Macro facilitant la construction des simulacres avec
 * traçage.
 * @param name Le nom de la fonction remplacée.
 * @param flag Le drapeau SccrollMockFlags qui correspond au
 * simulacre.
 * @return Le résultat de `sccroll_mockname`.
 */
#define sccroll_mockCall(name, flag, ...)                               \
    (                                                                   \
        sccroll_mockTrace(__FUNCTION__, __LINE__, flag),                \
        sccroll_mock##name(__VA_ARGS__)                                 \
    )

/**
 * @name Prototypes des simulacres prédéfinis.
 * @{
 */
sccroll_mockPrototype(malloc);
sccroll_mockPrototype(calloc);
sccroll_mockPrototype(pipe);
sccroll_mockPrototype(fork);
sccroll_mockPrototype(dup2);
sccroll_mockPrototype(close);
sccroll_mockPrototype(read);
sccroll_mockPrototype(write);
sccroll_mockPrototype(ferror);
sccroll_mockPrototype(fopen);
sccroll_mockPrototype(fseek);
sccroll_mockPrototype(ftell);
sccroll_mockPrototype(fread);
sccroll_mockPrototype(fwrite);
sccroll_mockPrototype(fscanf);
sccroll_mockPrototype(fileno);
/** @} */

/**
 * @name Définition des simulacres *via* macros.
 * @{
 */
#define malloc(...) sccroll_mockCall(malloc, SCCEMALLOC, __VA_ARGS__)
#define calloc(...) sccroll_mockCall(calloc, SCCECALLOC, __VA_ARGS__)
#define pipe(...)   sccroll_mockCall(pipe, SCCEPIPE, __VA_ARGS__)
#define fork(...)   sccroll_mockCall(fork, SCCEFORK, __VA_ARGS__)
#define dup2(...)   sccroll_mockCall(dup2, SCCEDUP2, __VA_ARGS__)
#define close(...)  sccroll_mockCall(close, SCCECLOSE, __VA_ARGS__)
#define read(...)   sccroll_mockCall(read, SCCEREAD, __VA_ARGS__)
#define write(...)  sccroll_mockCall(write, SCCEWRITE, __VA_ARGS__)
#define ferror(...) sccroll_mockCall(ferror, SCCEFERROR, __VA_ARGS__)
#define fopen(...)  sccroll_mockCall(fopen, SCCEFOPEN, __VA_ARGS__)
#define fseek(...) sccroll_mockCall(fseek, SCCEFSEEK, __VA_ARGS__)
#define ftell(...)  sccroll_mockCall(ftell, SCCEFTELL, __VA_ARGS__)
#define fread(...)  sccroll_mockCall(fread, SCCEFREAD, __VA_ARGS__)
#define fwrite(...) sccroll_mockCall(fwrite, SCCEFWRITE, __VA_ARGS__)
#define fscanf(...) sccroll_mockCall(fscanf, SCCEFSCANF, __VA_ARGS__)
#define fileno(...) sccroll_mockCall(fileno, SCCEFILENO, __VA_ARGS__)
/** @} */

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_MOCKS_H_
/** @} @} */
