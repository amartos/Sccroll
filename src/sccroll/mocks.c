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
 * @def SCCROLL_MOCKERROR
 * @since 0.1.0
 * @brief Message d'erreur des simulacres prédéfinis.
 * @param callee Le simulacre en erreur.
 * @param calls Le nombre d'appels du simulacre effectués.
 * @param caller La fonction appelante du simulacre.
 * @param line La ligne d'appel.
 * @param msg Un message d'erreur.
 */
#define SCCROLL_MOCKERROR(callee, calls, caller, line, msg) \
    "%s (call #%u in %s l. %i): %s",                        \
        callee,calls,caller,line,msg

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
 * @since 0.1.0
 * @brief Vérifie qu'une éventuelle erreur d'un simulacre déclenché a
 * été gérée, et si non termine le programme (@c SIGABRT).
 */
static void sccroll_mockAssert(void);

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
 * @struct SccrollMockTrace
 * @since 0.1.0
 * @brief Permet de conserver la trace d'appelants de fonctions.
 */
typedef struct SccrollMockTrace {
    const char* caller; /**< Le nom de la fonction appelante. */
    int line;           /**< La ligne d'appel. */
} SccrollMockTrace;

/**
 * @var trace
 * @since 0.1.0
 * @brief Conserve la trace du dernier appelant d'un simulacre.
 */
static SccrollMockTrace trace = {0};

/**
 * @since 0.1.0
 * @brief Exécute une fonction en provoquant une erreur de simulacre
 * prédéfini, et vérifie que l'erreur a bien été gérée.
 *
 * La valeur renvoyée sert d'indicateur de s'il reste des appels à
 * effectuer pour un simulacre donné. Tant que des erreurs de
 * simulacres sont émises et gérées, la fonction renverra @c true,
 * indiquant par là que le simulacre testé a été appelé par
 * @p wrapper. @c false indiquera que le simulacre n'a pas été appelé,
 * et on peut donc supposer qu'aucun autre appel n'interviendra par la
 * suite.
 *
 * @param trigger La structure contenant les informations du test du
 * simulacre.
 * @param wrapper Le wrapper de la fonction à tester.
 * @return true si le simulacre testé a émit une erreur qui a été
 * gérée, ou false si aucune erreur n'a été émise (ni même par une
 * absence de gestion d'erreur).
 */
static bool sccroll_mockCrashTest(SccrollFunc wrapper, SccrollMockFlags mock, unsigned delay)
    __attribute__((nonnull (1)));

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

SccrollMockFlags sccroll_mockGetTrigger(void) { return trigger[SCCMMOCK]; }
unsigned sccroll_mockGetDelay(void) { return trigger[SCCMDELAY]; }
unsigned sccroll_mockGetCalls(void) { return trigger[SCCMCALLS]; }

void sccroll_mockTrace(const char* funcname, int line, SccrollMockFlags mock)
{
    if (trigger[SCCMMOCK] == mock) {
        trace.caller = funcname;
        trace.line   = line;
    }
}

static bool sccroll_mockFire(SccrollMockFlags mock)
{
    if (!trigger[SCCMMOCK]) return false;
    else if (trigger[SCCMMOCK] != mock)
    {
        // Actions coordonnées entre simulacres.
        switch(mock)
        {
        default: sccroll_mockAssert(); break;
        case SCCEFERROR:
            // On déclenche aussi le simulacre avec les autres fonctions
            // f* de la librairie standard, car ferror doit être synchrone
            // avec leur déclenchement.
            switch(trigger[SCCMMOCK])
            {
            default: break;
            case SCCEFWRITE: __attribute__((fallthrough));
            case SCCEFREAD:  __attribute__((fallthrough));
            case SCCEFTELL: __attribute__((fallthrough));
            case SCCEFSEEK: __attribute__((fallthrough));
            case SCCEFOPEN:
                return trigger[SCCMCALLS] > trigger[SCCMDELAY];
            }
            break;
        }
    }
    else if (trigger[SCCMDELAY] > 0)
        --trigger[SCCMDELAY];
    else if (!trigger[SCCMDELAY] && !trigger[SCCMCALLS])
        ++trigger[SCCMCALLS];
    else
        sccroll_mockAssert();

    return trigger[SCCMMOCK] == mock && trigger[SCCMCALLS];
}

static void sccroll_mockAssert(void)
{
    if (trigger[SCCMCALLS])
        sccroll_mockFatal(SIGABRT, "error not handled");
}

void sccroll_mockFatal(int sigint, const char* restrict fmt, ...)
{
    int calls = trigger[SCCMCALLS];
    const char* name = sccroll_mockName(trigger[SCCMMOCK]);
    char msg[BUFSIZ] = {0};
    sccroll_mockFlush();
    sccroll_variadic(fmt, list, vsprintf(msg, fmt, list));
    sccroll_fatal(
        sigint,
        SCCROLL_MOCKERROR(name, calls, trace.caller, trace.line, msg)
    );
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
    case SCCEFERROR: return "ferror";
    case SCCEFOPEN:  return "fopen";
    case SCCEFSEEK:  return "fseek";
    case SCCEFTELL:  return "ftell";
    case SCCEFREAD:  return "fread";
    case SCCEFWRITE: return "fwrite";
    }
}

void sccroll_mockPredefined(SccrollFunc wrapper)
{
    SccrollMockFlags mock;
    unsigned delay;
    for (mock = SCCENONE; mock < SCCEMAX; ++mock) {
        // Si une erreur est levée (code ou signal), on peut supposer
        // qu'il reste encore des appels à vérifier, d'où la condition
        // de sortie. Si aucune erreur n'est levée par le simulacre,
        // il ne le sera plus, et donc on passe au suivant.
        for (delay = 0; sccroll_mockCrashTest(wrapper, mock, delay); ++delay);
    }
}

static bool sccroll_mockCrashTest(SccrollFunc wrapper, SccrollMockFlags mock, unsigned delay)
{
    bool error = false;
    int status = 0, code = 0, signal = 0;
    const char* name = sccroll_mockName(mock);
    const char* sigstr = NULL;

    // On effectue le test dans un fork pour éviter de crasher le
    // programme prématurément.
    sccroll_mockTrigger(mock, delay);
    status = sccroll_simplefork(name, wrapper);
    sccroll_mockFlush();
    code   = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    sigstr = sigabbrev_np(signal);
    error  = code || signal;

    // On vérifie qu'il n'y a pas d'erreur si aucun simulacre n'est
    // déclenché, ou que le simulacre n'a pas envoyé de signaux
    // d'erreurs (SIGABRT, SIGSEGV, ...).
    assertMsg(
        !signal && (mock || !error),
        "Predefined %s mock error (status %i, signal %s)",
        name,
        code, sigstr ? sigstr : "0"
    );

    return error;
}

// clang-format off

/******************************************************************************
 * Simulacres prédéfinis
 ******************************************************************************/
// clang-format on

SCCROLL_MOCK(
    sccroll_mockFire(SCCECALLOC),
    NULL, void*, calloc,
    size_t nmemb SCCCOMMA size_t size,
    nmemb, size
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEMALLOC),
    NULL, void*, malloc, size_t size,
    size
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEPIPE),
    -1, int, pipe, int pipefd[2],
    pipefd
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFORK),
    -1, int, fork, void,
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEDUP2),
    -1, int, dup2, int oldfd SCCCOMMA int newfd,
    oldfd, newfd
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCECLOSE),
    -1, int, close, int fd, fd
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEREAD),
    -1, ssize_t, read, int fd SCCCOMMA void* buf SCCCOMMA size_t count,
    fd, buf, count
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEWRITE),
    -1, ssize_t, write, int fd SCCCOMMA const void* buf SCCCOMMA size_t count,
    fd, buf, count
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFERROR),
    1, int, ferror, FILE* stream, stream
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFOPEN),
    NULL, FILE*, fopen, const char* restrict pathname SCCCOMMA const char* restrict mode,
    pathname, mode
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFSEEK),
    -1, int, fseek,
    FILE* stream SCCCOMMA long offset SCCCOMMA int whence,
    stream, offset, whence
);

SCCROLL_MOCK(sccroll_mockFire(SCCEFTELL), -1, long, ftell, FILE* stream, stream);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFREAD),
    0, size_t, fread,
    void* ptr SCCCOMMA size_t size SCCCOMMA size_t nmemb SCCCOMMA FILE* restrict stream,
    ptr, size, nmemb, stream
);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEFWRITE),
    0, size_t, fwrite,
    const void* ptr SCCCOMMA size_t size SCCCOMMA size_t nmemb SCCCOMMA FILE* restrict stream,
    ptr, size, nmemb, stream
);

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

void exit(int status)
{
    if (!status) sccroll_mockAssert();
    __gcov_dump(), _exit(status);
}
/** @} @} */
