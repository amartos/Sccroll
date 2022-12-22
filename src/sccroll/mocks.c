/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Fichier source des simulacres pré-fournis.
 * @date        2022
 * @author      Alexandre Martos
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
 * @def sccroll_mockError
 * @since 0.1.0
 * @brief Renvoie la valeur d'erreur si sccroll_mockTrigger() renvoie
 * @c true, sinon renvoie la valeur de @c __real_name(...).
 * @param name Le nom de la fonction originale.
 * @param errtrig Le code SccrollMockFlags du simulacre.
 * @param errval La valeur à renvoyer pour simuler l'erreur de @p name.
 * @param ... Les arguments pour la fonction originale (vide pour
 * "sans arguments").
 * @return @p errval si sccroll_mockTrigger() renvoie @p true et
 * qu'aucun drapeau incompatible n'est donné, sinon la valeur renvoyée
 * par @p __real_name(...).
 */
#define sccroll_mockError(name, errtrig, errval,...)                    \
    sccroll_mockTrigger(errtrig) && !sccroll_mockAreIncompats(errtrig)  \
        ? errval                                                        \
        : __real_##name(__VA_ARGS__)

/**
 * @since 0.1.0
 * @brief Détermine si d'autre drapeaux incompatibles ont été donné
 * avec celui du simulacre.
 * @param mock Le drapeau du simulacre concerné.
 * @return @c true si d'autres drapeaux incompatibles ont été donné,
 * sinon @c false.
 */
static bool sccroll_mockAreIncompats(SccrollMockFlags mock);

/**
 * @since 0.1.0
 * @brief Fonction renvoyant toujours #SCCENONE.
 * @note est utilisée comme alias faible de sccroll_mockTrigger().
 * @param mock Paramètre inutilisé.
 * @return @c false.
 */
static bool sccroll_enone(SccrollMockFlags mock) __attribute__((unused));

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

const char* sccroll_mockName(SccrollMockFlags mock)
{
    switch(mock)
    {
    default:         return "none";
    case SCCEABORT:  return "abort";
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

static bool sccroll_mockAreIncompats(SccrollMockFlags mock)
{
    switch(mock)
    {
    default: return false;
    case SCCEMALLOC: return sccroll_mockTrigger(SCCEABORT);
    }
}

weak_alias(sccroll_enone, sccroll_mockTrigger);
static bool sccroll_enone(SccrollMockFlags mock)
{
    sccroll_unused(mock);
    return false;
}

SCCROLL_MOCK(void, abort, void)
{
    // La fonction doit quitter. Mais une erreur possible pour elle
    // est de quitter de la mauvaise manière: au lieu de s'arrêter
    // avec un signal SIGABRT et un status EXIT_SUCCESS, la fonction
    // s'arrête avec exit et un status d'erreur.
    sccroll_mockTrigger(SCCEABORT)
        && !sccroll_mockAreIncompats(SCCEABORT)
        ? (__gcov_dump(), exit(SIGABRT))
        : (__gcov_dump(), __real_abort());
}

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

/** @} @} */
