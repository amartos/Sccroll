/**
 * @file        core.c
 * @version     0.1.0
 * @brief       Source code of the Sccroll library core module.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 *
 * @addtogroup Internals Internal structure of Sccroll
 * @{
 * @addtogroup Core Definition, execution and reports for Units Tests
 * @{
 */

#include "sccroll/core.h"

// clang-format off

/******************************************************************************
 * @name Basic messages handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollFonts
 * @since 0.1.0
 * @brief ANSI style codes.
 */
typedef enum SccrollFonts {
    NORMAL     = 0,      /**< default TTY value. */
    RESET      = NORMAL, /**< default TTY value (alias). */
    BOLD       = 1,      /**< Bold characters. */
    DIM        = 2,      /**< Dimmed characters. */
    ITALIC     = 3,      /**< Italic style. */
    UNDERSCORE = 4,      /**< Underlined style. */
}SccrollFonts;

/**
 * @enum SccrollColors
 * @since 0.1.0
 * @brief ANSI color codes.
 */
typedef enum SccrollColors{
    RED   = 1, /**< red. */
    GREEN = 2, /**< green. */
    CYAN  = 6, /**< cyan. */
} SccrollColors;

/**
 * @def COLSTART
 * @since 0.1.0
 * @brief Format string for ANSI style and colors.
 * @param i A SccrollFonts code.
 * @param i A SccrollColors code.
 */
#define COLSTART "\e[0;%i;3%im"

/**
 * @def COLEND
 * @since 0.1.0
 * @brief Reset the message style and colors.
 */
#define COLEND "\e[0m"

/**
 * @def COLSTRFMT
 * @since 0.1.0
 * @brief Format string using ANSI colors and styles codes.
 * @param i A SccrollFonts code.
 * @param i A SccrollColors code.
 * @param s The string to print.
 */
#define COLSTRFMT COLSTART "%s" COLEND

/**
 * @def HEXFMT
 * @since 0.1.0
 * @brief Bytes format string.
 * @param \* The number of digits to print.
 * @param x The byte value.
 */
#define HEXFMT "%0*x"

/**
 * @def COLHEXFMT
 * @since 0.1.0
 * @brief Colored and styled bytes format string.
 * @param i A SccrollFonts code.
 * @param i A SccrollColors code.
 * @param \* The number of digits to print.
 * @param x The byte value.
 */
#define COLHEXFMT COLSTART HEXFMT COLEND

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Errors handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def sccroll_err
 * @since 0.1.0
 * @brief Print an error message and exit (#EXIT_FAILURE) if the given
 * expression is @c true.
 * @note This differs from an assertion in the fact that the exit()
 * function is used, not the abort() function.
 * @param expr A boolean expression which raises an error if @c true.
 * @param op A description of the error.
 * @param name A title for the error.
 * @throw #EXIT_FAILURE if @p expr is @c true.
 */
#define sccroll_err(expr, op, name)                             \
    expr                                                        \
    ? err(EXIT_FAILURE, "%s failed for %s", op, name)           \
    : 0

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Tests preparation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Prepare the library.
 */
static void sccroll_atrun(void) __attribute__((constructor));

/**
 * @since 0.1.0
 * @brief Do nothing.
 *
 * This function is used as a weak alias for functions left to the
 * user to define, but called in the library in all cases.
 */
static void sccroll_void(void) __attribute__((unused));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Tests registration
 *
 * The tests are stored in a LIFO queue. However, this fact may
 * change, and thus the ordering of the tests is not guaranteed (and
 * should not be anyway).
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @var tests
 * @brief List of tests to run.
 */
static List* tests = NULL;

// clang-format off

/******************************************************************************
 * @}
 *
 * @name SccrollEffects and members handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Shallow copy the given SccrollEffects.
 * @param effects The SccrollEffects struct to copy.
 * @return A malloc'ed pointer to an @p effects shallow copy.
 */
static SccrollEffects* sccroll_dup(const SccrollEffects* restrict effects) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Malloc a SccrollEffects struct initialised at 0.
 * @return A malloc'ed pointer to a zeroed SccrollEffects struct.
 */
static SccrollEffects* sccroll_gen(void);

/**
 * @since 0.1.0
 * @brief Duplicate a data blob.
 * @param dest The destination structure.
 * @param src The structure to copy.
 * @note If @p src is @c NULL, @p data is filled with an empty string.
 * If @p src->size is @c 0, the @p src blob data is considered to be a
 * string and copied using strdup().
 */
static void sccroll_blobcpy(Data* restrict dest, const Data* restrict src)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Prepare the given test depending on the options.
 * @param effects The test to run.
 * @return A malloc'ed pointer to the prepared test structure. This
 * structure do not have any pointer in common with @p effects
 * (i.e. if no modification is done, the returned struct would be a
 * deep copy of @p effects).
 */
static SccrollEffects* sccroll_prepare(const SccrollEffects* restrict effects) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Give a whitespace-stripped copy of the given string.
 * @param string The string to strip.
 * @return A copy of @p string but stripped of left and right whitespaces.
 */
static char* sccroll_strip(const char* string) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Read the file content and store it.
 * @param file The structure storing the file path and content pointer
 * destination.
 * @param name The parent test name.
 */
static void sccroll_fread(SccrollFile* restrict file, const char* restrict name) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Tests execution
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Predefined main() of the library.
 * @return #EXIT_FAILURE if at least one test failed, #EXIT_SUCCESS
 * otherwise.
 */
static int sccroll_main(void);

/**
 * @since 0.1.0
 * @brief Run the next scheduled test.
 * @return @c 1 if the test failed, @c 0 otherwise.
 */
static int sccroll_test(void);

/**
 * @since 0.1.0
 * @brief Execute the test wrapper and record side effects in the
 * given structure.
 *
 * The recorded data include:
 * - the errno value after the call (errno is reset before the call)
 * - the signals and exit status code if the wrapper is called in a
 *   fork
 * - the outputs on stdout and stderr, if any (defaults to empty
 *   strings)
 * - the theoretical content of any SccrollEffects::files after the
 *   wrapper call
 *
 * @todo prevent side effects on files
 * @attention If #NOFORK is set, the wrapper is directly called. If
 * not, the wrapper is called in a fork.
 * @param result The structure storing the wrapper function pointer
 * and used as a destination for the side effects analysis.
 * @return @p result but now storing the side effects.
 */
static const SccrollEffects* sccroll_exe(SccrollEffects* restrict result) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Pipes handling
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef SccrollPipes
 * @since 0.1.0
 * @brief Internal constants used in pipes handling.
 */
typedef enum SccrollPipes {
    PIPEREAD = 0, /**< Read index of a pipe. */
    PIPEWRTE = 1, /**< Write index of a pipe. */
    PIPEOPEN,     /**< Pipe open operation code. */
    PIPECLOSE,    /**< Pipe close operation code. */
    PIPEDUP,      /**< Pipe dupe operation code. */
    PIPEMAX,      /**< Max index of pipe operations. */
    PIPEERRN = SCCMAXSTD, /**< Index of the errno pipe in an array of pipes. */
    PIPEMAXFD,            /**< Max index of an array of pipes. */
} SccrollPipes;

/**
 * @var PIPEDESC
 * @since 0.1.0
 * @brief Pipes operation descriptions.
 */
const char* const PIPEDESC[PIPEMAX] = {
    "read pipe", "write pipe",
    "open pipe", "close pipe",
    "duplicate pipe"
};

/**
 * @since 0.1.0
 * @brief Handle the pipes operations.
 * @attention This function closes the write side of a pipe after
 * write, and both side after read.
 * @param type A SccrollPipes operation code:
 * @param name The current test name.
 * @param pipefd The current pipe to modify.
 * @param ... Additional arguments depending on @p type:
 * | value      | expected arguments                                                                         |
 * |------------+--------------------------------------------------------------------------------------------|
 * | #PIPEOPEN  | ignored                                                                                    |
 * | #PIPECLOSE | #PIPEREAD or #PIPEWRTE depending on the side to close                                      |
 * | #PIPEWRTE  | a pointer to the data to write in the pipe, the bytes size of the data to write            |
 * | #PIPEREAD  | a pointer to a #SCCMAX string used to store the data to read in the pipe                   |
 * | #PIPEDUP   | #PIPEREAD or #PIPEWRTE depending on the side to duplicate, the destination file descriptor |
 */
static void sccroll_pipes(SccrollPipes type, const char* restrict name, int pipefd[2], ...) __attribute__((nonnull(2, 3)));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Recording of side effects
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Store the error codes of a test.
 * @param result The destination structure.
 * @param pipefd The pipe containing the code value.
 * @param status The wait() (defaults to @c 0 if #NOFORK is set).
 */
static void sccroll_codes(SccrollEffects* restrict result, int pipefd[2], int status) __attribute__((nonnull(1,2)));

/**
 * @since 0.1.0
 * @brief Store the standard outputs of the test.
 * @param result The destination structure.
 * @param pipestd An array of pipes used to capture the standard
 * outputs. The indexes correspond to the standard outputs file
 * descriptors values.
 */
static void sccroll_std(SccrollEffects* restrict result, int pipestd[SCCMAXSTD][2]) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Store the first #SCCMAX-1 characters of the
 * SccollEffects::files content.
 * @param result The destination structure.
 */
static void sccroll_files(SccrollEffects* restrict result) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Side-effects analysis and reports handling.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollReport
 * @since 0.1.0
 * @brief Reports numerical constants.
 */
typedef enum SccrollReport {
    REPORTTOTAL = 0, /**< Index of the total number of tests. */
    REPORTFAIL = 1,  /**< Index of the total number of failed tests. */
    REPORTMAX  = 2,  /**< Max index of the report count array. */
    MAXLINE = 80,    /**< Max line length. */
} SccrollReport;

/**
 * @def BASEFMT
 * @since 0.1.0
 * @brief Report status format string.
 * @param i A SccrollColors code.
 * @param s The status description.
 * @param s The test name.
 */
#define BASEFMT "[ " COLSTRFMT " ] %s"

/**
 * @var SCCSEP
 * @since 0.1.0
 * @brief A text separation line.
 * @note Defined once at the library initialisation, and freed once at
 * its destruction.
 */
static const char* SCCSEP = NULL;

/**
 * @def REPORTFMT
 * @since 0.1.0
 * @brief Final report format string.
 * @param i A SccrollColors code.
 * @param s The status description.
 * @param s The test name.
 * @param f Overall tests success percentage.
 * @param i Total number of passed tests.
 * @param i Total number of tests.
 */
#define REPORTFMT "\n%s\n\n" BASEFMT ": %.2f%% [%i/%i]\n", SCCSEP, BOLD

/**
 * @def DIFFFMT
 * @since 0.1.0
 * @brief Expected/obtained diffs format string.
 * @param name The test name
 * @param s The diff.
 */
#define DIFFFMT BASEFMT ": %s\n", BOLD, CYAN, "DIFF"

/**
 * @def CODEFMT
 * @since 0.1.0
 * @brief Expected/obtained error codes format string.
 * @param s The test name.
 * @param s The test description.
 * @param i The expected code value.
 * @param s The expected code name.
 * @param i The obtained code value.
 * @param s The obtained code name.
 */
#define CODEFMT BASEFMT ": %s: expected %i (%s), got %i (%s)\n", BOLD, CYAN, "DIFF"

/**
 * @since 0.1.0
 * @brief Diff two SccrollEffects.
 *
 * If #NODIFF is **not** defined, the function print a report for any
 * discrepancy found between the two structures.
 *
 * @param expected,result The structures to compare.
 * @return @c true if the @p expected effects and @p results are
 * different, @c false otherwise.
 */
static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Compare two SccrollEffects::codes.
 *
 * If #NODIFF is **not** defined, the function print a report for any
 * discrepancy found between the two structures.
 *
 * @param expected,result The structures to compare.
 * @return @c true if the @p expected codes and @p results codes are
 * different, @c false otherwise.
 */
static bool sccroll_diffCodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Compare two SccrollEffects::std.
 *
 * If #NODIFF is **not** defined, the function print a report for any
 * discrepancy found between the two structures.
 *
 * @param expected,result The structures to compare.
 * @return @c true if any of the @p expected and @p results
 * SccrollEffects::std::content are different, @c false otherwise.
 */
static bool sccroll_diffStd(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));


/**
 * @since 0.1.0
 * @brief Compare two SccrollEffects::files.
 *
 * If #NODIFF is **not** defined, the function print a report for any
 * discrepancy found between the two structures.
 *
 * @param expected,result The structures to compare.
 * @return @c true if any of the @p expected and @p results
 * SccrollEffects::files::content are different, @c false otherwise.
 */
static bool sccroll_diffFiles(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Print an error message describing the difference between the
 * two SccrollEffects::codes.
 * @param expected The expected effects.
 * @param result The obtained effects.
 */
static void sccroll_pcodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @typedef SccrollBlobDiff
 * @since 0.1.0
 * @brief Structure storing the information necessary for a diff of
 * two Data blobs.
 */
typedef struct SccrollBlobDiff {
    const Data* expected; /**< The expected blob. */
    const Data* result;   /**< The obtained blob. */
    const char* name;     /**< The test name. */
    const char* desc;     /**< The blobs description. */
} SccrollBlobDiff;

/**
 * @since 0.1.0
 * @brief Print the diff of two strings.
 * @param infos The structure storing the information on the strings.
 */
static void sccroll_pdiff(const SccrollBlobDiff* restrict infos) __attribute__((nonnull));


/**
 * @since 0.1.0
 * @brief Print the diff of two blobs of bytes.
 * @param infos The structure storing the information on the blobs.
 */
static void sccroll_dump(const SccrollBlobDiff* restrict infos) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Print the final report.
 * @param report An array storing the counts of total number of tests
 * and total number of failed tests.
 * @see SccrollReport
 */
static void sccroll_review(int report[REPORTMAX]) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Post-tests cleanup
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Frees a malloc'ed SccrollEffects structure.
 *
 * The freed items are:
 * - all SccrollEffects::files::content::blob up to the first @c NULL
 *   SccrollEffects::files::path
 * - all SccrollEffects::std::content::blob
 * - @p effects
 * @param effects The struct to free.
 */
static void sccroll_free(const SccrollEffects* restrict effects) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Cleanup the library at exit.
 */
static void sccroll_atexit(void) __attribute__((destructor));

// clang-format off

/******************************************************************************
 * @}
 *
 * Implementation
 *
 * Preparation
 ******************************************************************************/
// clang-format on

static void sccroll_atrun(void)
{
    // Setup the report separating line.
    char sep[MAXLINE+1] = { 0 };
    memset(sep, '-', MAXLINE);
    SCCSEP = strdup(sep);
}

weak_alias(, sccroll_void, sccroll_init);
weak_alias(, sccroll_void, sccroll_clean);
weak_alias(, sccroll_void, sccroll_before);
weak_alias(, sccroll_void, sccroll_after);
static void sccroll_void(void) {}

// clang-format off

/******************************************************************************
 * Registration
 ******************************************************************************/
// clang-format on

void sccroll_register(const SccrollEffects* restrict expected)
{
    tests = lpush(sccroll_prepare(expected), tests);
}

static SccrollEffects* sccroll_prepare(const SccrollEffects* restrict effects)
{
    SccrollEffects* prepared = sccroll_dup(effects);
    int i;
    char *stripped;
    for (i = STDIN_FILENO; i < SCCMAXSTD; ++i)
        if (!sccroll_hasFlags(prepared->flags, NOSTRP)) {
            stripped = sccroll_strip(prepared->std[i].content.blob);
            free(prepared->std[i].content.blob);
            prepared->std[i].content.blob = stripped;
        }

    return prepared;
}

static SccrollEffects* sccroll_dup(const SccrollEffects* restrict effects)
{
    SccrollEffects* copy = sccroll_gen();
    copy->name    = effects->name;
    copy->wrapper = effects->wrapper;
    copy->flags   = effects->flags;
    copy->code    = effects->code;

    for (int i = 0; i < SCCMAX && (effects->files[i].path || i < SCCMAXSTD); ++i) {
        if (i < SCCMAXSTD) {
            if ((copy->std[i].path = effects->std[i].path))
                sccroll_fread(&copy->std[i], effects->name);
            else
                sccroll_blobcpy(&copy->std[i].content, &effects->std[i].content);
        }

        if ((copy->files[i].path = effects->files[i].path))
            sccroll_blobcpy(&copy->files[i].content, &effects->files[i].content);
    }

    return copy;
}

static SccrollEffects* sccroll_gen(void)
{
    SccrollEffects* effects = calloc(1, sizeof(SccrollEffects));
    sccroll_err(!effects, "alloc", "SccrollEffects");
    return effects;
}

static void sccroll_blobcpy(Data* restrict dest, const Data* restrict src)
{
    if (!src->blob) dest->blob = strdup("");
    else if (!src->size) dest->blob = strdup(src->blob);
    else {
        dest->blob = blobdup(src->blob, src->size);
        dest->size = src->size;
    }
}

static char* sccroll_strip(const char* oldstring)
{
    if (!*oldstring) return strdup(oldstring);
    while(isspace(*oldstring)) ++oldstring;
    char* string = strdup(oldstring);
    char* end = string+strlen(string)-1;
    while(isspace(*end)) --end;
    *(end+1) = 0;
    return string;
}

static void sccroll_fread(SccrollFile* restrict file, const char* restrict name)
{
    char buffer[SCCMAX] = { 0 };
    FILE* stream = fopen(file->path, "rb");
    sccroll_err(!stream, file->path, name);
    sccroll_err(
        !(file->content.size = fread(buffer, sizeof(char), SCCMAX, stream))
        && ferror(stream), file->path, name
    );
    fclose(stream);
    // +char to take account of strings comparisons. Since size is not
    // modified, the last byte is null and is hidden to the
    // comparison.
    file->content.blob = blobdup(
        buffer,
        file->content.size < sizeof(buffer)
        ? file->content.size+sizeof(char)
        : sizeof(buffer)
    );
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

// The predefined main is a weak alias to allow any defined override.
weak_alias(, sccroll_main, main);
static int sccroll_main(void)
{
    return sccroll_run() ? EXIT_FAILURE : EXIT_SUCCESS;
}

int sccroll_run(void)
{
    if (!tests) return 0;

    setbuf(stdout, NULL);

    int report[REPORTMAX] = { 0 };
    report[REPORTTOTAL]   = tests->len;

    sccroll_init();
    while (tests->len) {
        sccroll_before();
        report[REPORTFAIL] += sccroll_test();
        sccroll_after();
    }
    sccroll_review(report);
    sccroll_clean();

    lfree(tests);
    tests = NULL;
    return report[REPORTFAIL];
}

static int sccroll_test(void)
{
    const SccrollEffects* expected = lpop(tests);
    const SccrollEffects* result   = sccroll_exe(sccroll_dup(expected));
    int failed = sccroll_diff(expected, result);
    if (failed) {
        fprintf(stderr, BASEFMT "\n", BOLD, RED, "FAIL", expected->name);
        if (!sccroll_hasFlags(expected->flags, NODIFF)) fprintf(stderr, "\n");
    }
    sccroll_free(expected);
    sccroll_free(result);
    return failed;
}

static const SccrollEffects* sccroll_exe(SccrollEffects* restrict result)
{
    bool dofork              = !sccroll_hasFlags(result->flags, NOFORK);
    size_t length            = 0;
    int status               = 0;
    int origstd[SCCMAXSTD]   = { 0 };
    int pipefd[PIPEMAXFD][2] = { 0 };

    for (int i = STDIN_FILENO; i < PIPEMAXFD; ++i)
        sccroll_pipes(PIPEOPEN, result->name, pipefd[i]);

    pid_t pid = dofork ? fork() : 0;
    sccroll_err(pid < 0, "fork", result->name);
    if (pid == 0) {
        for (int i = STDIN_FILENO, p = PIPEREAD; i < SCCMAXSTD; ++i, p = PIPEWRTE) {
            if (!dofork) sccroll_err((origstd[i] = dup(i)) < 0, "dup save of standard", result->name);
            sccroll_pipes(PIPEDUP, result->name, pipefd[i], p, i);
        }

        errno = 0;
        length = sizeof(char)*strlen(result->std[STDIN_FILENO].content.blob);
        sccroll_pipes(PIPEWRTE, result->name, pipefd[STDIN_FILENO], result->std[STDIN_FILENO].content.blob, length);
        result->wrapper();
        sccroll_pipes(PIPEWRTE, result->name, pipefd[PIPEERRN], &errno, sizeof(int));

        for (int i = STDIN_FILENO, p = PIPEREAD; i < SCCMAXSTD; ++i, p = PIPEWRTE) {
            if (!dofork) {
                sccroll_err(dup2(origstd[i], i) < 0, "original std fd restoration", result->name);
                sccroll_err(close(origstd[i]) < 0, "could not close original std fd", result->name);
            }
            sccroll_pipes(PIPECLOSE, result->name, pipefd[i], p);
        }

        if (dofork) exit(EXIT_SUCCESS);
    }

    if (dofork) {
        for (int i = STDIN_FILENO; i < PIPEMAXFD; ++i)
            sccroll_pipes(PIPECLOSE, result->name, pipefd[i], PIPEWRTE);
        sccroll_pipes(PIPECLOSE, result->name, pipefd[STDIN_FILENO], PIPEREAD);
        wait(&status);
    }
    sccroll_codes(result, pipefd[PIPEERRN], status);
    sccroll_std(result, pipefd);
    sccroll_files(result);

    for (int i = STDIN_FILENO; i < PIPEMAXFD; ++i) {
        sccroll_pipes(PIPECLOSE, result->name, pipefd[i], PIPEREAD);
        sccroll_pipes(PIPECLOSE, result->name, pipefd[i], PIPEWRTE);
    }
    return result;
}

static void sccroll_pipes(SccrollPipes type, const char* restrict name, int pipefd[2], ...)
{
    int status = 0, pipeside = 0, fd = 0;
    void* buf;
    size_t size;
    va_list args;
    va_start(args, pipefd);
    switch (type) {
    case PIPEOPEN: status = pipe(pipefd); break;
    case PIPEDUP:
        pipeside = va_arg(args, int);
        fd = va_arg(args, int);
        status = dup2(pipefd[pipeside], fd);
        break;
    case PIPEREAD:
        buf = va_arg(args, void*);
        size = va_arg(args, size_t);
        status = read(pipefd[PIPEREAD], buf, size);
        if (status >= 0) sccroll_pipes(PIPECLOSE, name, pipefd, PIPEREAD);
        break;
    case PIPEWRTE:
        buf = va_arg(args, void*);
        size = va_arg(args, size_t);
        status = write(pipefd[PIPEWRTE], buf, size);
        if (status >= 0) sccroll_pipes(PIPECLOSE, name, pipefd, PIPEWRTE);
        break;
    case PIPECLOSE:
        pipeside = va_arg(args, int);
        if (!pipefd[pipeside]) break;
        status = close(pipefd[pipeside]);
        pipefd[pipeside] = 0;
        break;
    default: break;
    }
    va_end(args);
    sccroll_err(status < 0, PIPEDESC[type], name);
}

// clang-format off

/******************************************************************************
 * Recording of side-effects
 ******************************************************************************/
// clang-format on

static void sccroll_codes(SccrollEffects* restrict result, int pipefd[2], int status)
{
    switch(result->code.type)
    {
    case SCCERRNUM:
        sccroll_pipes(PIPEREAD, result->name, pipefd, &result->code.value, sizeof(int));
        break;
    case SCCSTATUS:
        if (sccroll_hasFlags(result->flags, NOFORK))
            result->code.value = status;
        else if (WIFEXITED(status))
            result->code.value = WEXITSTATUS(status);
        break;
    default: // SCCSIGNAL
        if (sccroll_hasFlags(result->flags, NOFORK))
            result->code.value = status;
        else if (WIFSIGNALED(status))
            result->code.value = WTERMSIG(status);
        break;
    }
}

static void sccroll_std(SccrollEffects* restrict result, int pipefd[SCCMAXSTD][2])
{
    // expected and result share the same pointer, freeing both would
    // raise an error. This one is thus reset to avoid the situation.
    result->std[STDIN_FILENO].content.blob = NULL;

    char buffer[SCCMAX] = { 0 };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i, memset(buffer, 0, strlen(buffer))) {
        sccroll_pipes(PIPEREAD, result->name, pipefd[i], buffer, SCCMAX);
        result->std[i].content.blob =
            sccroll_hasFlags(result->flags, NOSTRP)
            ? strdup(buffer)
            : sccroll_strip(buffer);
    }
}

static void sccroll_files(SccrollEffects* restrict result)
{
    for (int i = 0; i < SCCMAX && result->files[i].path; ++i)
        sccroll_fread(&result->files[i], result->name);
}

// clang-format off

/******************************************************************************
 * Reports
 ******************************************************************************/
// clang-format on

static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    // We want to compare all data before returning the result of
    // comparison, hence the not-directly-or'ed.
    bool diff = sccroll_diffCodes(expected, result);
    diff |= sccroll_diffStd(expected, result);
    diff |= sccroll_diffFiles(expected, result);
    return diff;
}

static bool sccroll_diffCodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    if (expected->code.value != result->code.value) {
        if(!sccroll_hasFlags(expected->flags, NODIFF))
            sccroll_pcodes(expected, result);
        return true;
    }
    return false;
}

static bool sccroll_diffStd(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    bool diff = false;
    SccrollBlobDiff infos = { .name = expected->name };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
        if (strcmp(expected->std[i].content.blob, result->std[i].content.blob)) {
            if (!sccroll_hasFlags(expected->flags, NODIFF)) {
                infos.expected = &expected->std[i].content;
                infos.result = &result->std[i].content;
                infos.desc = i == STDOUT_FILENO ? "stdout" : "stderr";
                sccroll_pdiff(&infos);
            }
            diff = true;
        }
    return diff;
}

static bool sccroll_diffFiles(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    bool diff = false;
    size_t explen = 0, reslen = 0;
    SccrollBlobDiff infos = { .name = expected->name };

    for (int i = 0; i < SCCMAX && (bool)expected->files[i].path; ++i, explen = 0, reslen = 0) {
        if (expected->files[i].content.size) {
            explen = expected->files[i].content.size;
            reslen = result->files[i].content.size;
        }
        else {
            explen = strlen(expected->files[i].content.blob);
            reslen = strlen(result->files[i].content.blob);
        }

        if (explen != reslen
            || memcmp(expected->files[i].content.blob, result->files[i].content.blob, explen)) {
            diff = true;
            if (!sccroll_hasFlags(expected->flags, NODIFF)) {
                infos.expected = &expected->files[i].content;
                infos.result = &result->files[i].content;
                infos.desc = expected->files[i].path;
                expected->files[i].content.size
                    ? sccroll_dump(&infos)
                    : sccroll_pdiff(&infos);
            }
        }
    }
    return diff;
}

static void sccroll_pcodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    int exp = expected->code.value, res = result->code.value;
    char expdesc[MAXLINE] = { 0 };
    char resdesc[MAXLINE] = { 0 };
    char* desc = NULL;

    switch(expected->code.type)
    {
    case SCCERRNUM:
        desc = "errno";
        sprintf(expdesc, "%s", strerrorname_np(exp));
        sprintf(resdesc, "%s", strerrorname_np(res));
        break;
    case SCCSTATUS:
        desc = "status";
        sprintf(expdesc, exp ? "error" : "no error");
        sprintf(resdesc, res ? "error" : "no error");
        break;
    default: // SCCSIGNAL
        desc = "signal";
        exp
            ? sprintf(expdesc, "SIG%s", sigabbrev_np(exp))
            : sprintf(expdesc, "no signal");
        res
            ? sprintf(resdesc, "SIG%s", sigabbrev_np(res))
            : sprintf(resdesc, "no signal");
        break;
    }
    fprintf(stderr, CODEFMT, expected->name, desc, exp, expdesc, res, resdesc);
}

static void sccroll_pdiff(const SccrollBlobDiff* restrict infos)
{
    size_t expc, resc;
    char *expz = NULL, *resz = NULL;
    char *expn = NULL, *resn = NULL;

    sccroll_err(argz_create_sep(infos->expected->blob, '\n', &expz, &expc), infos->desc, infos->name);
    sccroll_err(argz_create_sep(infos->result->blob, '\n', &resz, &resc), infos->desc, infos->name);

    fprintf(stderr, DIFFFMT, infos->name, infos->desc);
    for (
        expn = argz_next(expz, expc, expn), resn=argz_next(resz, resc, resn);
        expn || resn;
        expn = expn ? argz_next(expz, expc, expn) : expn,
        resn = resn ? argz_next(resz, resc, resn) : resn
        )
        if (((bool)expn ^ (bool)resn) || (expn && strcmp(expn, resn)))
        {
            fprintf(stderr, "exp: " COLSTRFMT "\n", NORMAL, GREEN, expn ? expn : "");
            fprintf(stderr, "res: " COLSTRFMT "\n", NORMAL, RED, resn ? resn : "");
        }

    free(expz);
    free(resz);
}

static void sccroll_dump(const SccrollBlobDiff* restrict infos)
{
    const int digits         = sizeof(char)*2;
    char expbuffer[SCCMAX/2] = { 0 };
    char resbuffer[SCCMAX/2] = { 0 };
    char expected[SCCMAX*2]  = "exp (bytes): ";
    char result[SCCMAX*2]    = "res (bytes): ";
    char* expdata            = infos->expected->blob;
    char* resdata            = infos->result->blob;
    bool same                = false;

    fprintf(stderr, DIFFFMT, infos->name, infos->desc);
    for (size_t i = 0; i <= infos->expected->size || i <= infos->result->size; ++i) {
        same =
            i <= infos->expected->size
            && i <= infos->result->size
            && expdata[i] == resdata[i];

        if (same && i <= infos->expected->size)
            sprintf(expbuffer, HEXFMT, digits, expdata[i]);
        else if (i <= infos->expected->size)
            sprintf(expbuffer, COLHEXFMT, NORMAL, GREEN, digits, expdata[i]);
        else if (*expbuffer)
            memset(expbuffer, 0, sizeof(char)*strlen(expbuffer));

        if (same && i <= infos->result->size)
            sprintf(resbuffer, HEXFMT, digits, resdata[i]);
        else if (i <= infos->result->size)
            sprintf(resbuffer, COLHEXFMT, NORMAL, RED, digits, resdata[i]);
        else if (*resbuffer)
            memset(resbuffer, 0, sizeof(char)*strlen(resbuffer));

        strcat(expected, expbuffer);
        strcat(result, resbuffer);
    }
    fprintf(stderr, "%s\n%s\n", expected, result);
}

static void sccroll_review(int report[REPORTMAX])
{
    int passed    = report[REPORTTOTAL] - report[REPORTFAIL];
    float percent = 100.0 * passed / report[REPORTTOTAL];
    fprintf(stderr, REPORTFMT,
        report[REPORTFAIL] ? RED : GREEN,
        report[REPORTFAIL] ? "FAIL" : "PASS",
        "success rate", percent, passed, report[REPORTTOTAL]);
}

// clang-format off

/******************************************************************************
 * Cleanup
 ******************************************************************************/
// clang-format on

static void sccroll_free(const SccrollEffects* restrict effects)
{
    for (int i = 0; i < SCCMAX && (effects->files[i].path || i < SCCMAXSTD); ++i) {
        if (i < SCCMAXSTD) free(effects->std[i].content.blob);
        free(effects->files[i].content.blob);
    }

    free((void*)effects);
}

static void sccroll_atexit(void)
{
    // Freeing the reports separator line.
    free((void*)SCCSEP);
}

/** @} @} **/
