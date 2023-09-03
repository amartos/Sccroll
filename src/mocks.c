/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Mocks module source code.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Mocks Mocks module internals.
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
 * @brief Print an error message for the predefined mocks.
 * @param name The original function name.
 * @param calls The total number of the mock calls.
 * @param msg The error message.
 */
#define SCCROLL_MOCKERROR(name, calls, msg)                         \
    "%s (call #%u in %s::%s(), l. %i): %s",                         \
        name,                                                       \
        calls,                                                      \
        trace.source,                                               \
        trace.caller,                                               \
        trace.line,                                                 \
        msg

/**
 * @since 0.1.0
 * @brief Check if an error must be raised, either by the mock or by
 * the module (if the previous mock error was not handled).
 * @param mock The mock identifier code.
 * @return @c true if the mock must raise an error, @c false
 * otherwise.
 * @throw #SIGABRT if the previous mock error trigger was not
 * handled.
 */
static bool sccroll_mockFire(SccrollMockFlags mock);

/**
 * @since 0.1.0
 * @brief Assert that a mock error trigger has been handled.
 * @throw #SIGABRT if a mock trigger was not handled.
 */
static void sccroll_mockAssert(void);

/**
 * @struct SccrollMockTrace
 * @since 0.1.0
 * @brief Structure storing traces for mock calls.
 */
typedef struct SccrollMockTrace {
    const char* source; /**< The caller source file path. */
    const char* caller; /**< The caller name. */
    int line;           /**< The line of call. */
    SccrollMockFlags mock; /**< The mock code. */
    int calls;             /**< The number of calls made. */
} SccrollMockTrace;

/**
 * @var trace
 * @since 0.1.0
 * @brief Store the mocks calls trace.
 */
static SccrollMockTrace trace = {0};

/**
 * @since 0.1.0
 * @brief Execute a wrapper function and check that the raised mock
 * errors have been handled by it.
 * @param wrapper The wrapper to execute.
 * @param mock The mock to schedule a trigger from.
 * @param delay The delay to use for the trigger.
 * @return @c true if the mock trigger emitted an error that was
 * handled, @c false otherwise. This value is used as an indicator of
 * remaining calls to tests; a value of @c false for any other mock
 * than #SCCENONE indicates that no other mock calls is to be
 * expected.
 */
static bool sccroll_mockCrashTest(SccrollFunc wrapper, SccrollMockFlags mock, unsigned delay)
    __attribute__((nonnull (1)));

// clang-format off

/******************************************************************************
 * Implementation
 ******************************************************************************/
// clang-format on

void sccroll_mockTrigger(SccrollMockFlags mock, unsigned delay) {
    trace.mock  = mock;
    trace.calls = delay;
}

void sccroll_mockFlush(void) { trace.mock = 0, trace.calls = 0; }

SccrollMockFlags sccroll_mockGetTrigger(void) { return trace.mock; }
int sccroll_mockGetCalls(void) { return trace.calls; }

void sccroll_mockTrace(const char* source, const char* funcname, int line, SccrollMockFlags mock)
{
    if (trace.mock == mock) {
        trace.source = source;
        trace.caller = funcname;
        trace.line   = line;
    }
}

static bool sccroll_mockFire(SccrollMockFlags mock)
{
    if (!trace.mock) return false;
    else if (trace.mock != mock)
    {
        // Coordinated functions mocked.
        switch(mock)
        {
        default: sccroll_mockAssert(); break;
        case SCCEFERROR:
            // The f* file functions of the library set some internal
            // FILE* values reported by ferror. Thus, in case of an
            // error for these, ferror must also report an
            // error. Hence the coordinated trigger.
            switch(trace.mock)
            {
            default: break;
            case SCCEFSCANF: __attribute__((fallthrough));
            case SCCEFWRITE: __attribute__((fallthrough));
            case SCCEFREAD:  __attribute__((fallthrough));
            case SCCEFTELL: __attribute__((fallthrough));
            case SCCEFSEEK: __attribute__((fallthrough));
            case SCCEFOPEN:
                return trace.calls <= 0;
            }
            break;
        }
        return false;
    }
    else if (trace.calls < 0) sccroll_mockAssert();

    return !trace.calls--;
}

static void sccroll_mockAssert(void)
{
    if (trace.calls >= 0) return;

    const char* name = sccroll_mockName(trace.mock);
    int calls        = -1*trace.calls;
    sccroll_mockFlush();
    sccroll_fatal(SIGABRT, SCCROLL_MOCKERROR(name, calls, "error not handled"));
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
    case SCCEFSCANF: return "fscanf";
    case SCCEFILENO: return "fileno";
    case SCCEHCREATE: return "hcreate";
    case SCCEHSEARCH: return "hsearch";
    }
}

void sccroll_mockPredefined(SccrollFunc wrapper)
{
    SccrollMockFlags mock;
    unsigned delay;
    for (mock = SCCENONE; mock < SCCEMAX; ++mock) {
        // In case an error is raised (code or signal), we can assume
        // that other calls remain to be checked. If no errors are
        // raised, the wrapper call is complete and the mock code is
        // not SCCENONE, then there will be no more calls of the given
        // mock. Hence the exit condition of the loop.
        for (delay = 0; sccroll_mockCrashTest(wrapper, mock, delay); ++delay);
    }
}

static bool sccroll_mockCrashTest(SccrollFunc wrapper, SccrollMockFlags mock, unsigned delay)
{
    bool error = false;
    int status = 0, code = 0, signal = 0;
    const char* name = sccroll_mockName(mock);
    const char* sigstr = NULL;

    // Fork to avoid a premature crash.
    sccroll_mockTrigger(mock, delay);
    status = sccroll_simplefork(name, wrapper);
    sccroll_mockFlush();
    code   = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    sigstr = sigabbrev_np(signal);
    error  = code || signal;

    // Check that no errors have been raised while no mock is
    // triggered, or that no signals have been sent (SIGABRT, SIGSEGV,
    // ...).
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
 * Predefined mocks
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

// The variadic nature of fscanf prevents its definition as a mock,
// but its source code uses vfscanf anyway.
// SEE (link truncated for readability)
// https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/fscanf.c
// ;h=caca780f0982cbb6d46aa41a79460a01b906eec8;hb=dee2bea048b688b643a9a3b44b26ca9f7a706fe8#l36
int sccroll_mockfscanf(FILE* restrict stream, const char* restrict format, ...)
{

    if (sccroll_mockFire(SCCEFSCANF)) return EOF;
    va_list args;
    va_start(args, format);
    int status = vfscanf(stream, format, args);
    va_end(args);
    return status;
}

SCCROLL_MOCK(sccroll_mockFire(SCCEFILENO), -1, int, fileno, FILE* stream, stream);

SCCROLL_MOCK(sccroll_mockFire(SCCEHCREATE), 0, int, hcreate, size_t nel, nel);

SCCROLL_MOCK(
    sccroll_mockFire(SCCEHSEARCH),
    NULL, ENTRY*, hsearch, ENTRY item SCCCOMMA ACTION action,
    item, action
);

// clang-format off

/******************************************************************************
 * Redefinitions.
 ******************************************************************************/
// clang-format on

// abort does not dump gcov() data, which is a problem for assertions
// that are expected to raise assertion errors.
// The final exit is there to please the compilers.
void abort(void) { sccroll_mockFlush(), __gcov_dump(), raise(SIGABRT), exit(SIGABRT); }

void exit(int status)
{
    if (!status) sccroll_mockAssert();
    __gcov_dump(), _exit(status);
}
/** @} @} */
