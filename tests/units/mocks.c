/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Mocks unit tests suorce code.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include <assert.h>
#include <fcntl.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

// Constants
enum {
    MAX = 10,               // Max delay used
    SCCESCCRUN = SCCEMAX,   // sccroll_run user mock ID
    SCCEFREE   = SCCEMAX+1, // free user mock ID
};

static unsigned delay = 0;
static int none_status = 0;
static unsigned dummy_flag = SCCENONE;

// Test the mock trigger, then reset the schedule. This allows to test
// multiple mocks in a row.
#define testMock(flag, delay, testOK, testFail)                         \
    assert(testOK);                                                     \
    sccroll_mockTrigger(flag, delay);                                   \
    assert(testFail);                                                   \
    sccroll_mockFlush()

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

// This mock does not interfere with the function. It was not
// implemented within the predefined mocks of the library as it does
// not raise any errors on its own.
SCCROLL_MOCK(
    dummy_flag == SCCEFREE,
    puts("free mocked") SCCCOMMA libfree(ptr),
    void, free, void* ptr, ptr
);
#define free sccroll_mockfree

// Cancels the functions effects only if #dummy_flag is defined.
// The return value is a hack to print a message and force the return
// of 0 instead of calling the original function.
SCCROLL_MOCK(
    dummy_flag == SCCESCCRUN,
    puts("sccroll_run mocked: flag seen.") SCCCOMMA 0
    : puts("sccroll_run mocked: nothing executed.") SCCCOMMA 0;
    true ? 0,
    int, sccroll_run, void,
);
#define sccroll_run sccroll_mocksccroll_run

// This mock will make all tests fail only if the sccroll_run mock
// does not work.
SCCROLL_MOCK(
    true,
    assert(false && "sccroll_before mocked, but should not be executed..."),
    void, sccroll_before, void
);
#define sccroll_before sccroll_mocksccroll_before


void test_getters(void)
{
    void* blob = NULL;
    sccroll_mockFlush();

    assert(sccroll_mockGetTrigger() == SCCENONE);
    assert(sccroll_mockGetCalls() == 0);

    sccroll_mockTrigger(SCCEMALLOC, 0);
    assert(sccroll_mockGetTrigger() == SCCEMALLOC);
    assert(sccroll_mockGetCalls() == 0);
    sccroll_mockFlush();
    assert(sccroll_mockGetTrigger() == SCCENONE);
    assert(sccroll_mockGetCalls() == 0);

    sccroll_mockTrigger(SCCEMALLOC, 13);
    assert(sccroll_mockGetTrigger() == SCCEMALLOC);
    assert(sccroll_mockGetCalls() == 13);
    sccroll_mockFlush();

    sccroll_mockTrigger(SCCEMALLOC, 1);
    assert(sccroll_mockGetTrigger() == SCCEMALLOC);
    assert(sccroll_mockGetCalls() == 1);
    assert((blob = malloc(1)));
    free(blob);
    assert(sccroll_mockGetCalls() == 0);
    assert(!malloc(1));
    assert(sccroll_mockGetCalls() == -1);
    sccroll_mockFlush();
}

void test_predefined_mocks(void)
{
    sccroll_mockFlush();
    pid_t pid = -1;
    int signal, code, status;
    int pipefd[2] = { 0 };
    char* dummy = NULL;
    char buf[SCCMAX] = { 0 };
    const char* teststr = "foobar test 123";
    ssize_t lenstr = strlen(teststr);
    const char* textfilepath = "tests/assets/blobs/textfile";
    char template[] = "/tmp/sccroll.errors.XXXXXX";
    char text[SCCMAX] = { 0 };
    FILE* textfile = fopen(textfilepath, "r");
    if (!textfile) err(EXIT_FAILURE, "%s", textfilepath);
    size_t textlen;
    assert((textlen = fread(text, sizeof(char), SCCMAX, textfile)) > 0);
    fclose(textfile), textfile = NULL;
    ENTRY item = {0};
    ENTRY* retitem = NULL;

    testMock(SCCEFOPEN, 0, (textfile=fopen(textfilepath, "r")), (fopen(textfilepath, "r") == NULL));
    assert(textfile);
    clearerr(textfile);
    rewind(textfile);
    testMock(
        SCCEFSCANF, 0,
        (clearerr(textfile), rewind(textfile),
         fscanf(textfile, "%s", buf) == 1   && ferror(textfile) == 0),
        (clearerr(textfile), rewind(textfile),
         fscanf(textfile, "%s", buf) == EOF && ferror(textfile) != 0)
    );
    assert(buf[0]);
    buf[strlen(buf)] = '\n';
    assert(!strcmp(buf, text));
    memset(buf, 0, sizeof(buf));
    clearerr(textfile);

    testMock(SCCEFERROR, 0, (ferror(textfile) == 0), (ferror(textfile) != 0));
    testMock(SCCEFORK, 0, (ferror(textfile) == 0), (ferror(textfile) == 0));
    // this ensures that ferror is indeed returning an error even if
    // it is not supposed to trigger.
    testMock(SCCEFOPEN, 0, true, (fopen(textfilepath, "r"), ferror(textfile) != 0));
    assert(fread(buf, sizeof(char), SCCMAX, textfile) == textlen);
    assert(!strcmp(buf, text));
    memset(buf, 0, sizeof(buf));
    testMock(SCCEFSEEK, 0, true, (fseek(textfile, 1L, SEEK_SET), ferror(textfile) != 0));
    testMock(SCCEFTELL, 0, true, (ftell(textfile), ferror(textfile) != 0));
    testMock(
        SCCEFSEEK, 0,
        (fseek(textfile, 1L, SEEK_SET) == 0),
        (fseek(textfile, 1L, SEEK_SET) == -1)
    );
    testMock(SCCEFTELL, 0, (ftell(textfile) == 1), (ftell(textfile) == -1));

    testMock(SCCEFREAD, 0, true, (fread(buf, sizeof(char), SCCMAX, textfile), ferror(textfile) != 0));
    testMock(SCCEFWRITE, 0, true, (fwrite(text, sizeof(char), SCCMAX, textfile), ferror(textfile) != 0));
    assert(textfile);
    rewind(textfile);
    testMock(
        SCCEFREAD, 0,
        (fread(buf, sizeof(char), SCCMAX, textfile) == textlen),
        (fread(buf, sizeof(char), SCCMAX, textfile) == 0)
    );
    assert(!strcmp(buf, text));
    memset(buf, 0, sizeof(buf));
    fclose(textfile), textfile = NULL;

    mkstemp(template);
    textfile = fopen(template, "w+");
    if (!textfile) err(EXIT_FAILURE, "%s", textfilepath);
    testMock(
        SCCEFWRITE, 0,
        (fwrite(text, sizeof(char), textlen, textfile) == textlen),
        (fwrite(text, sizeof(char), textlen, textfile) == 0)
    );
    fseek(textfile, 0L, SEEK_SET);
    assert(fread(buf, sizeof(char), textlen, textfile) == textlen);

    testMock(SCCECALLOC, 0, (dummy=calloc(1,sizeof(char))), calloc(1, sizeof(char)) == NULL);
    free(dummy);

    testMock(SCCEMALLOC, 0, (dummy=malloc(1)), malloc(1) == NULL);
    free(dummy);

    testMock(SCCEPIPE, 0, pipe(pipefd) >= 0, pipe(pipefd) < 0);
    close(pipefd[0]), close(pipefd[1]);

    testMock(SCCEFORK, 0, (pid = fork()) >= 0, fork() < 0);
    if (pid == 0) exit(0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(!code && ! signal);

    pipe(pipefd);
    pid = fork();
    if (pid == 0) {
        testMock(SCCEDUP2, 0,
                 dup2(pipefd[1], STDOUT_FILENO) >= 0,
                 dup2(STDERR_FILENO, STDOUT_FILENO) < 0);
        testMock(SCCEWRITE, 0,
                 write(STDOUT_FILENO, teststr, strlen(teststr)) == lenstr,
                 write(pipefd[1], teststr, lenstr) < 0);
        testMock(SCCECLOSE, 0, close(pipefd[1]) >= 0, close(pipefd[0]) < 0);
        testMock(SCCEREAD, 0,
                 read(pipefd[0], buf, SCCMAX) == lenstr,
                 read(pipefd[0], buf, SCCMAX) < 0);
        assert(!strcmp(buf, teststr));
        exit(5);
    }
    assert(pid > 0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(code == 5 && !signal);
    assert(close(pipefd[0]) >= 0);
    assert(close(pipefd[1]) >= 0);

    testMock(SCCEHCREATE, 0, hcreate(BUFSIZ) > 0, (hdestroy(), hcreate(BUFSIZ) == 0));
    libhcreate(BUFSIZ);
    item.key  = template;
    item.data = (void*)42;
    testMock(
        SCCEHSEARCH, 0,
        hsearch(item, ENTER) != NULL,
        hsearch(item, FIND) == NULL
    );
    testMock(
        SCCEHSEARCH, 0,
        (retitem = hsearch(item, FIND)) != NULL,
        (item.key = (char*)teststr, hsearch(item, ENTER) == NULL)
    );
    assert((size_t)retitem->data == 42);
    hdestroy();

    pid = fork();
    if (pid == 0) {
        abort();
        raise(SIGTERM); // in case the mock fails to quit.
    }
    assert(pid > 0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(code == 0 && signal == SIGABRT);

}

void test_notrigger(void)
{
    sccroll_mockFlush();

    void* blob;
    sccroll_mockTrigger(0,0);
    assert((blob = malloc(1)));
    free(blob);
}

void test_delay(void)
{
    sccroll_mockFlush();

    sccroll_mockTrigger(SCCEMALLOC, delay);
    for (unsigned i = 0; i <= delay+1; ++i)
        free(malloc(1));
    exit(1);
}

void test_abort_atexit(void)
{
    sccroll_mockFlush();

    sccroll_mockTrigger(SCCEMALLOC, 0);
    // This **must** raise a failure of the test, whatever the context.
    void* blob = malloc(1);
    (void) blob;
}

void test_flush(void)
{
    void* blob = NULL;
    sccroll_mockTrigger(SCCEMALLOC, 0);
    sccroll_mockFlush();
    assert((blob = malloc(1)));
    free(blob);
}

void test_fullerrors(void)
{
    bool fatal       = false;
    int fd           = 0;
    int oldfd        = 0;
    int pipefd[2]    = {0};
    char buf[10]     = {0};
    void* blob       = NULL;
    char template[]  = "/tmp/sccroll.errors.XXXXXX";
    char* errmsg     = NULL;
    FILE* tmp        = NULL;
    ENTRY dummy      = {0};

    // We want to fine control the error trigger time, thus this is to
    // sync with it.
    if (dummy_flag != sccroll_mockGetTrigger()) exit(0);

    try(test_fullerrors) {
        if (dummy_flag > SCCEPIPE) {
            // All mocks above this code need a valid opened file.
            fd     = mkstemp(template);
            errmsg = template;
            if (fd < 0) throw(test_fullerrors, INDEPERROR);
            tmp    = libfopen(template, "r+");
            if (!tmp) throw(test_fullerrors, INDEPERROR);
            blob   = libcalloc(1,1);
            errmsg = "could not allocate for blob";
            if (!blob) throw(test_fullerrors, INDEPERROR);
        }

        switch(dummy_flag)
        {
        case SCCEMALLOC: free(malloc(1)); break;
        case SCCECALLOC: free(calloc(1, 1)); break;
        case SCCEFORK:   fork() == 0 ? exit(0) : 0; break;
        case SCCEPIPE:   pipe(pipefd); break;
        case SCCEDUP2:
            oldfd  = dup(STDOUT_FILENO);
            errmsg = "could not dupsave STDOUT";
            if (oldfd < 0) throw(test_fullerrors, INDEPERROR);
            if (dup2(STDOUT_FILENO, fd) > 0) {
                // on évite de re-déclencher le simulacre en utilisant la
                // version originale.
                errmsg = "could not reset STDOUT";
                if (libdup2(STDOUT_FILENO, oldfd) < 0)
                    throw(test_fullerrors, INDEPERROR);
            }
            break;
        case SCCECLOSE:  close(fd); break;
        case SCCEREAD:   read(fd, blob, 1); break;
        case SCCEWRITE:  write(fd, blob, 1); break;
        case SCCEFERROR: (void)ferror(tmp); break;
        case SCCEFOPEN:  fclose(tmp), tmp = fopen(template, "r+"); break;
        case SCCEFSEEK:  fseek(tmp, 0L, SEEK_SET); break;
        case SCCEFTELL:  ftell(tmp); break;
        case SCCEFREAD:  fread(blob,1,1,tmp); break;
        case SCCEFWRITE: fwrite(blob,1,1,tmp); break;
        case SCCEFSCANF: fscanf(blob, "%c", buf); break;
        case SCCEFILENO: close(fd), fd = fileno(tmp); break;
        case SCCEHCREATE: if (hcreate(BUFSIZ)) hdestroy(); break;
        case SCCEHSEARCH:
            errmsg = "could not create hash table";
            if (!hcreate(BUFSIZ))
                throw(test_fullerrors, INDEPERROR);
            dummy.key = template;
            hsearch(dummy, ENTER);
            hdestroy();
            break;
        case SCCENONE:   throw(test_fullerrors, IGNORE); break;
        default:
            // default s'assure qu'on oublie pas de tests
            throw(test_fullerrors, MISSINGTEST);
            break;
        }
    }
    catch(test_fullerrors, IGNORE) {}
    catch(test_fullerrors, INDEPERROR) {
        warn("%s", errmsg);
        fatal = true;
        errmsg = "external error";
    }
    catch(test_fullerrors, MISSINGTEST) {
        fatal = true;
        errmsg = "missing test";
    }
    finally(test_fullerrors) {
        if (fd) libclose(fd);
        if (pipefd[0]) libclose(pipefd[0]), libclose(pipefd[1]);
        free(blob);
        if (tmp) fclose(tmp);
        if (fatal)
            // SIGABRT is used by the mocks, but the present test is
            // to check for errors not raised by them. We raise
            // another signal for this.
            sccroll_fatal(SIGTERM, "%s: %s", sccroll_mockName(dummy_flag), errmsg);
        exit(none_status);
    }
}

void test_mockPredefined(void)
{
    sccroll_mockPredefined(test_fullerrors);
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    test_predefined_mocks();
    test_notrigger();
    test_getters();
    int status = sccroll_simplefork("test_abort_atexit", test_abort_atexit);
    assert(WTERMSIG(status) == SIGABRT);
    test_flush();
    for (delay = 0; delay < MAX; ++delay) {
        status = sccroll_simplefork("test delay", test_delay);
        assert(WTERMSIG(status) == SIGABRT);
    }
    sccroll_mockFlush();
    for (dummy_flag = SCCENONE; dummy_flag < SCCEMAX; ++dummy_flag) {
        status = sccroll_simplefork("test predefined", test_mockPredefined);
        assert((!dummy_flag && !status) || WTERMSIG(status) == SIGABRT);
        // Ensure the function does not affect anything else than
        // itself.
        // TODO: check if this is necessary, the fork should not
        // affect anything.
        assert(!sccroll_mockGetTrigger());
    }

    sccroll_mockFlush();
    dummy_flag  = 0;
    none_status = 1;
    status = sccroll_simplefork("test errors not predefined", test_mockPredefined);
    assert(WIFSIGNALED(status));
    assert(WTERMSIG(status) == SIGABRT);

    // User-defined mocks tests

    // Ensures the original functions can be called with their
    // original names.
    dummy_flag = SCCEFREE;
    free(strdup("test"));

    // If the sccroll_run mock does not work, the sccroll_before will
    // ensure a fail.
    dummy_flag = SCCENONE;
    assert(!sccroll_run());

    // A state change should print a new message.
    dummy_flag = SCCESCCRUN;
    sccroll_run();

    // Avoid exit errors, just in case.
    sccroll_mockFlush();
    return EXIT_SUCCESS;
}
