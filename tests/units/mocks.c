/**
 * @file        mocks.c
 * @version     0.1.0
 * @brief       Test unitaire de définitions de mocks.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/mocks.c -o build/objs/tests/units/mocks.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/mocks.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c test/units/mocks.c) \
 *     -o build/bin/tests/mocks
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparation des tests.
 ******************************************************************************/
// clang-format on

// Constantes du test
enum {
    MAX = 10,       // Nombre de tests de délai max
    SCCESCCRUN = 2, // identifiant du simulacre interne de sccroll_run
    SCCEFREE = 4,   // identifiant du simulacre interne de free
};


// Variable utilisée comme drapeau pour déclencher les erreurs des
// simulacres interne au test.
static unsigned dummy_flag = SCCENONE;

// Teste le déclenchement de l'erreur du simulacre, puis le reset du
// déclencheur.
#define testMock(flag, testOK, testFail)        \
    assert(testOK);                             \
    trigger.mock = flag;                        \
    trigger.delay = 0;                          \
    assert(testFail);                           \
    trigger.mock = SCCENONE

// Fonctions utilisées pour maintenance du test (on ne cherche pas à
// les tester).
extern __typeof__(fork) __real_fork;
extern __typeof__(close) __real_close;
extern __typeof__(pipe) __real_pipe;

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

// simulacre de free n'interférant pas avec la fonction.
// free na pas été intégré avec les simulacres préfournis car la
// fonction ne lève aucune erreur qui puisse être prévue.
SCCROLL_MOCK(void, free, void* ptr)
{
    if (sccroll_hasFlags(dummy_flag, SCCEFREE))
        puts("free mocked");
    __real_free(ptr);
}

// mock de sccroll_run qui annule les effets de la fonction si
// #dummy_flag est défini.
SCCROLL_MOCK(int, sccroll_run, void)
{
    sccroll_hasFlags(dummy_flag, SCCESCCRUN)
        ? puts("sccroll_run mocked: flag seen.")
        : puts("sccroll_run mocked: nothing executed.");
    return 0;
}

// mock de sccroll_before, mais qui provoque une erreur en cas
// d'exécution. Puisque sccroll_run est mockée et n'exécute aucun
// test, sccroll_before ne devrait pas être exécutée.
SCCROLL_MOCK(void, sccroll_before, void)
{
    assert(false && "sccroll_before mocked, but should not be executed...");
}

// Tests des mocks prédéfinis.
void test_predefined_mocks(void)
{
    pid_t pid = -1;
    int signal, code, status;
    int pipefd[2] = { 0 };
    char* dummy = NULL;
    char buf[SCCMAX] = { 0 };
    const char* teststr = "foobar test 123";
    ssize_t lenstr = strlen(teststr);

    SccrollMockTrigger trigger = { 0 };
    sccroll_mockTrigger(&trigger);

    testMock(SCCECALLOC, (dummy=calloc(1,sizeof(char))), calloc(1, sizeof(char)) == NULL);
    __real_free(dummy);

    testMock(SCCEMALLOC, (dummy=malloc(1)), malloc(1) == NULL);
    __real_free(dummy);

    testMock(SCCEPIPE, pipe(pipefd) >= 0, pipe(pipefd) < 0);
    __real_close(pipefd[0]), __real_close(pipefd[1]);

    testMock(SCCEFORK, (pid = fork()) >= 0, fork() < 0);
    if (pid == 0) exit(0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(!code && ! signal);

    trigger.mock = SCCENONE;
    __real_pipe(pipefd);
    pid = __real_fork();
    if (pid == 0) {
        testMock(SCCEDUP2,
                 dup2(pipefd[1], STDOUT_FILENO) >= 0,
                 dup2(STDERR_FILENO, STDOUT_FILENO) < 0);
        testMock(SCCEWRITE,
                 write(STDOUT_FILENO, teststr, strlen(teststr)) == lenstr,
                 write(pipefd[1], teststr, lenstr) < 0);
        testMock(SCCECLOSE, close(pipefd[1]) >= 0, close(pipefd[0]) < 0);
        testMock(SCCEREAD,
                 read(pipefd[0], buf, SCCMAX) == lenstr,
                 read(pipefd[0], buf, SCCMAX) < 0);

        sccroll_mockTrigger(NULL);
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

    pid = __real_fork();
    if (pid == 0) {
        trigger.mock = SCCEMALLOC;
        trigger.delay = 0;
        abort();
        raise(SIGTERM); // au cas où le simulacre échoue à quitter.
    }
    assert(pid > 0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(code == 0 && signal == SIGABRT);

    pid = __real_fork();
    if (pid == 0) {
        trigger.mock = SCCEABORT;
        // delay ne devrait pas influer sur abort.
        trigger.delay = 150;
        abort();
        raise(SIGTERM); // au cas où le simulacre échoue à quitter.
    }
    assert(pid > 0);
    wait(&status);
    code = WEXITSTATUS(status);
    signal = WTERMSIG(status);
    assert(trigger.mock != SCCEABORT);
    assert(code == SIGABRT && !signal);
}

void test_notrigger(void)
{
    void* blob;
    sccroll_mockTrigger(NULL);
    assert((blob = malloc(1)));
    __real_free(blob);
}

void test_delay(int delay)
{
    SccrollMockTrigger trigger = {
        .mock = SCCEMALLOC,
        .delay = delay,
    };
    sccroll_mockTrigger(&trigger);
    void* blob = NULL;
    for (int i = 0; i <= delay; ++i) {
        if (i == delay) assert(malloc(1) == NULL);
        assert((blob = malloc(1)));
        __real_free(blob);
    }
}

void test_abort(void)
{
    SccrollMockTrigger trigger = {
        .mock = SCCEMALLOC,
        .delay = 0,
        .opts = SCCMABORT,
    };
    sccroll_mockTrigger(&trigger);
    assert(malloc(1) == NULL);
    assert(trigger.delay == -1);

    pid_t pid = __real_fork();
    if (pid == 0) free(malloc(1)), exit(1);
    assert(pid > 0);

    int status;
    wait(&status);
    assert(WTERMSIG(status) == SIGABRT);
}

void test_flush(void)
{
    void* blob;
    SccrollMockTrigger trigger = {
        .mock = SCCEMALLOC,
        .delay = -1,
        .opts = SCCMABORT,
    };
    sccroll_mockTrigger(&trigger);
    sccroll_mockFlush();
    assert((blob = malloc(1)));
    free(blob);

    trigger.delay = 0;
    trigger.opts |= SCCMFLUSH;
    sccroll_mockTrigger(&trigger);
    assert(malloc(1) == NULL);
    // Cet appel devrait lancer un abort.
    assert((blob = malloc(1)));
    free(blob);
}

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    // On teste les réactions des mocks prédéfinis.
    test_predefined_mocks();
    test_notrigger();
    test_abort();
    test_flush();
    for (unsigned delay = 0; delay < MAX; ++delay) test_delay(delay);

    // On teste les constructions de mocks

    // On s'assure que les fonctions mockées de la librairie peuvent
    // être appelées avec leur nom original.
    dummy_flag = SCCEFREE;
    free(strdup("test"));

    // Si le mock de sccroll_run() ne fonctionne pas, il provoquera
    // une erreur en appelant sccroll_before(), et l'assert assure un
    // second niveau de vérification (puisque le seul test enregistré
    // est en échec, et non en réussite comme testé ici).
    dummy_flag = SCCENONE;
    assert(!sccroll_run());

    // Un changement d'état du drapeau affichera un nouveau message.
    dummy_flag = SCCESCCRUN;
    sccroll_run();

    return EXIT_SUCCESS;
}
