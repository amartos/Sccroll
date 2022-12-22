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
 * gcc -L build/libs -lsccroll build/objs/tests/units/mocks \
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

// Variable utilisée comme drapeau pour certains mocks.
static int dummy_flag = SCCENONE;

// Drapeaux des mocks du test.
enum {
    SCCESCCRUN = 2,
};

// On utilise des fork pour tester certains mocks. On s'assure
// d'utiliser la vraie fonction.
extern __typeof__(fork) __real_fork;

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

// Redéfinition prévue par l'API.
bool sccroll_mockTrigger(SccrollMockFlags mock)
{
    return sccroll_hasFlags(dummy_flag, mock);
}

// simulacre de free n'interférant pas avec la fonction.
// free na pas été intégré avec les simulacres préfournis car la
// fonction ne lève aucune erreur qui puisse être prévue.
SCCROLL_MOCK(void, free, void* ptr)
{
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
    int status = -1;
    int pipefd[2] = { 0 };
    char* dummy = NULL;
    char buf[SCCMAX] = { 0 };
    const char* teststr = "foobar test 123";
    ssize_t lenstr = strlen(teststr);

    // Pas d'erreurs des mocks.
    dummy_flag = SCCENONE;

    assert((dummy=calloc(1,sizeof(char))));
    __real_free(dummy);

    assert((dummy=malloc(1)));
    __real_free(dummy);

    assert(pipe(pipefd) >= 0);
    pid = fork();
    if (pid == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) exit(1);
        if (write(STDOUT_FILENO, teststr, strlen(teststr)) != lenstr) exit(2);
        if (close(pipefd[1]) < 0) exit(3);
        if (read(pipefd[0], buf, SCCMAX) != lenstr) exit(4);
        if (strcmp(buf, teststr)) exit(5);
        abort();
    }
    assert(pid > 0);
    wait(&status);
    assert(WEXITSTATUS(status) == 0);
    assert(WTERMSIG(status) == SIGABRT);
    assert(close(pipefd[0]) >= 0);
    assert(close(pipefd[1]) >= 0);

    // Erreurs individuelles.
    dummy_flag = SCCECALLOC;
    assert(calloc(1, sizeof(char)) == NULL);
    assert((dummy=malloc(1)));
    __real_free(dummy);

    dummy_flag = SCCEMALLOC;
    assert(malloc(1) == NULL);
    assert((dummy=calloc(1,sizeof(char))));
    __real_free(dummy);

    dummy_flag = SCCEALLOC;
    assert(calloc(1, sizeof(char)) == NULL);
    assert(malloc(1) == NULL);

    dummy_flag = SCCEPIPE;
    assert(pipe(pipefd) < 0);

    dummy_flag = SCCEFORK;
    assert(fork() < 0);

    dummy_flag = SCCEDUP2;
    assert(dup2(STDERR_FILENO, STDOUT_FILENO) < 0);

    dummy_flag = SCCEWRITE;
    assert(pipe(pipefd) >= 0);
    assert(write(pipefd[1], teststr, lenstr) < 0);
    assert(close(pipefd[1]) >= 0);
    memset(buf, 0, strlen(buf));
    assert(read(pipefd[0], buf, lenstr) >= 0);
    assert(strlen(buf) == 0);
    assert(close(pipefd[0]) >= 0);

    dummy_flag = SCCECLOSE;
    assert(pipe(pipefd) >= 0);
    assert(close(pipefd[1]) < 0);

    dummy_flag = SCCEREAD;
    assert(write(pipefd[1], teststr, lenstr) >= 0);
    assert(close(pipefd[1]) >= 0);
    memset(buf, 0, strlen(buf));
    assert(read(pipefd[0], buf, lenstr) < 0);
    assert(strlen(buf) == 0);
    assert(close(pipefd[0]) >= 0);

    dummy_flag = SCCEABORT;
    pid = __real_fork();
    if (pid == 0) abort();
    assert(pid > 0);
    wait(&status);
    assert(WEXITSTATUS(status) == SIGABRT);
    assert(WTERMSIG(status) == 0);

    // Groupes d'erreurs.
    dummy_flag = SCCENONE |                     \
        SCCEABORT | SCCECALLOC | SCCEPIPE  |    \
        SCCEFORK  | SCCEDUP2   | SCCECLOSE |    \
        SCCEREAD  | SCCEWRITE;

    assert(calloc(1, sizeof(char)) == NULL);
    assert(pipe(pipefd) < 0);
    assert(fork() < 0);
    assert(dup2(STDERR_FILENO, STDOUT_FILENO) < 0);
    assert(write(pipefd[1], teststr, lenstr) < 0);
    assert(close(pipefd[1]) < 0);
    assert(read(pipefd[0], buf, lenstr) < 0);

    pid = __real_fork();
    if (pid == 0) abort();
    assert(pid > 0);
    wait(&status);
    assert(WEXITSTATUS(status) == SIGABRT);
    assert(WTERMSIG(status) == 0);

    // tests des incompatibilités

    dummy_flag = SCCEABORT | SCCEMALLOC;
    // Malloc est appelé si abort est en erreur. Si les deux le sont,
    // le résultat n'est pas celui attendu.
    assert((dummy = malloc(1)));
    __real_free(dummy);
    pid = __real_fork();
    if (pid == 0) abort();
    assert(pid > 0);
    wait(&status);
    assert(WEXITSTATUS(status) == SIGABRT);
    assert(WTERMSIG(status) == 0);
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

    // Les fonctions mockées doivent afficher un message si le mock
    // est réussi. Sinon, sccroll_run() provoquera une erreur en
    // appelant sccroll_before(), et l'assert assure un second niveau
    // de vérification (puisque le seul test enregistré est en échec,
    // et non en réussite comme testé ici).
    dummy_flag = SCCENONE;
    assert(!sccroll_run());

    // On s'assure que les fonctions mockées peuvent être appelées
    // avec leur nom original.
    free(strdup("test"));

    // Un changement d'état du drapeau affichera un nouveau message.
    dummy_flag = SCCESCCRUN;
    sccroll_run();

    return EXIT_SUCCESS;
}
