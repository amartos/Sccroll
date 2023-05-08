/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Fichier source des simulacres pré-fournis.
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
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Mocks Simulacres pré-fournis.
 * @{
 */

#include "sccroll/mocks.h"

// clang-format off

/******************************************************************************
 * Documentation
 ******************************************************************************/
// clang-format on

/**
 * @def SCCROLL_MOCKERRFMT
 * @since 0.1.0
 * @brief Le format des messages de sccroll_mockPredefined().
 * @param s Le nom du simulacre testé.
 * @param i Le nombre d'appels du simulacre au moment du message.
 * @param s Le message.
 */
#define SCCROLL_MOCKERRFMT "%s (call #%u): %s"

/**
 * @since 0.1.0
 * @brief Détermine s'il faut déclencher une erreur du simulacre
 * ou lever une erreur générale si l'erreur précédente n'a pas été
 * gérée.
 * @param mock Le code SccrollMockFlags du simulacre.
 * @return true si le simulacre doit déclencher une erreur, sinon
 * false.
 */
static bool sccroll_mockFire(SccrollMockFlags mock);

/**
 * @enum SccrollMockIndex
 * @since 0.1.0
 * @brief Liste des index de #trigger.
 */
typedef enum SccrollMockIndex {
    SCCMMOCK,  /**< Index du code du simulacre à déclencher. */
    SCCMDELAY, /**< Index du délai à applique. */
    SCCMCALLS, /**< Index du nombre d'appels effectués à partir de
                * l'erreur du simulacre. */
    SCCMMAX,   /**< Valeur maximale des index. */
} SccrollMockIndex;

/**
 * @var trigger
 * @since 0.1.0
 * @brief Variable contenant les informations sur le simulacre à
 * déclencher.
 */
static unsigned trigger[SCCMMAX] = {0};

/**
 * @def sccroll_mockError
 * @since 0.1.0
 * @brief Renvoie la valeur d'erreur si sccroll_mockTrigger correspond
 * au simulacre, sinon renvoie la valeur de @c __real_name(...).
 * @param name Le nom de la fonction originale.
 * @param errtrig Le code SccrollMockFlags du simulacre.
 * @param errval La valeur à renvoyer pour simuler l'erreur de @p name.
 * @param ... Les arguments pour la fonction originale (vide pour
 * "sans arguments").
 * @return @p errval si sccroll_mockTrigger correspond au simulacre,
 * sinon la valeur renvoyée par @p __real_name(...).
 */
#define sccroll_mockError(name, errtrig, errval, ...)               \
    sccroll_mockFire(errtrig) ? errval : __real_##name(__VA_ARGS__)

/**
 * @since 0.1.0
 * @brief Fonction sauvegardant les données utilisées par gcov.
 */
extern void __gcov_dump(void);

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

void sccroll_mockTrigger(SccrollMockFlags mock, unsigned delay) {
    trigger[SCCMMOCK]  = mock;
    trigger[SCCMDELAY] = delay;
}

void sccroll_mockFlush(void) { memset(trigger, 0, sizeof(trigger)); }

static bool sccroll_mockFire(SccrollMockFlags mock)
{
    if (!trigger[SCCMMOCK]) return false;
    else if (trigger[SCCMMOCK] != mock)
    {
        // Actions coordonnées entre simulacres.
        switch(mock)
        {
        default: return false;
        }
    }
    else if (trigger[SCCMDELAY] > 0)
        --trigger[SCCMDELAY];
    else if (trigger[SCCMCALLS] > trigger[SCCMDELAY])
        sccroll_mockFatal("mock error not handled");
    else if (trigger[SCCMDELAY] == 0 && trigger[SCCMCALLS] == 0)
        return ++trigger[SCCMCALLS];

    return false;
}

void sccroll_mockFatal(const char* restrict fmt, ...)
{
    int calls = trigger[SCCMCALLS];
    const char* name = sccroll_mockName(trigger[SCCMMOCK]);
    char msg[BUFSIZ] = {0};
    sccroll_mockFlush();
    sccroll_variadic(fmt, list, vsprintf(msg, fmt, list));
    sccroll_fatal(SCCROLL_MOCKERRFMT, name, calls, msg);
}

const char* sccroll_mockName(SccrollMockFlags mock)
{
    switch(mock)
    {
    default:         return "none";
    case SCCECALLOC: return "calloc";
    case SCCEPIPE:   return "pipe";
    case SCCEFORK:   return "fork";
    case SCCEDUP2:   return "dup2";
    case SCCECLOSE:  return "close";
    case SCCEREAD:   return "read";
    case SCCEWRITE:  return "write";
    case SCCEMALLOC: return "malloc";
    }
}

// clang-format off

/******************************************************************************
 * Simulacres prédéfinis
 ******************************************************************************/
// clang-format on

SCCROLL_MOCK(void*, calloc, size_t nmemb, size_t size)
{
    return sccroll_mockError(calloc, SCCECALLOC, NULL, nmemb, size);
}

SCCROLL_MOCK(void*, malloc, size_t size)
{
    return sccroll_mockError(malloc, SCCEMALLOC, NULL, size);
}

SCCROLL_MOCK(int, pipe, int pipefd[2])
{
    return sccroll_mockError(pipe, SCCEPIPE, -1, pipefd);
}

SCCROLL_MOCK(pid_t, fork, void)
{
    return sccroll_mockError(fork, SCCEFORK, -1,);
}

SCCROLL_MOCK(int, dup2, int oldfd, int newfd)
{
    return sccroll_mockError(dup2, SCCEDUP2, -1, oldfd, newfd);
}

SCCROLL_MOCK(int, close, int fd)
{
    return sccroll_mockError(close, SCCECLOSE, -1, fd);
}

SCCROLL_MOCK(ssize_t, read, int fd, void* buf, size_t count)
{
    return sccroll_mockError(read, SCCEREAD, -1, fd, buf, count);
}

SCCROLL_MOCK(ssize_t, write, int fd, const void* buf, size_t count)
{
    return sccroll_mockError(write, SCCEWRITE, -1, fd, buf, count);
}

// clang-format off

/******************************************************************************
 * Redéfinitions des fonctions de la librairie.
 ******************************************************************************/
// clang-format on

// abort() ne gère pas les données de gcov(), ce qui pose problème
// pour la couverture. Cette redéfinition est donc là pour régler le
// problème. L'exit final est là pour contenter le compilateur, mais
// ne sera jamais appelé.
void abort(void) { sccroll_mockFlush(), __gcov_dump(), raise(SIGABRT), exit(SIGABRT); }

/** @} @} */
