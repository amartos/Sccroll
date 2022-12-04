/**
 * @file        sccroll.c
 * @version     0.1.0
 * @brief       Fichier source de la librairie Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 *
 * @addtogroup Sccroll
 *
 * @{
 */

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * @addtogroup General Macros et fonctions d'aides.
 * @brief Commandes d'aides pour la définition des fonctions et autres
 * macros de la librairie.
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def weak_alias
 * @since 0.1.0
 * @brief Créé un alias faible.
 *
 * Définit un alias faible de la fonction donnée. Cette macro est une
 * quasi-copie de la macro @c weak_alias de la librairie C de GNU.
 * @see https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Common-Function-Attributes.html
 *
 * @param name La fonction modèle.
 * @param aliasname Le nom de l'alias.
 */
#define weak_alias(name, aliasname) \
    extern __typeof__(name) aliasname __attribute__((weak, alias(#name)))

/**
 * @since 0.1.0
 * @enum SccrollConstants
 * Constante numériques internes.
 */
enum SccrollConstants {
    PIPEREAD    = 0,            /**< Index du côté lecture d'un pipefd. */
    PIPEWRTE    = PIPEREAD + 1, /**< Index du côté écriture d'un pipefd. */
    REPORTTOTAL = 0,            /**< Index du rapport pour le nombre total de tests exécutés. */
    REPORTFAIL  = 1,            /**< Index du rapport pour le nombre de tests en échec. */
    MAXLINE     = 80,           /**< Longueur maximale des lignes d'un rapport. */
};

#define REPORTFMT "[ %-5s ] %s: %s"
#define LOGICFMT "%i/%i passed, expected %s"
#define EXITFMT "%s: expected %i, got %i"

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Register
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @struct SccrollTest
 * Structure contenant les données pour un test unitaire.
 */
typedef struct SccrollTest {
    SccrollTestFunc test; /**< La fonction de test unitaire. */
    const char* name;     /**< Le nom du test. */
    int status;           /**< Le code de status renvoyé par
                           *   l'exécution du test. */
    int pipefd[2];        /**< Un pipe permettant de récupérer
                           *   l'output du test. */
} SccrollTest;

static SccrollTest* sccroll_gentest(const char* restrict name) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @struct SccrollNode
 * Structure d'un noeud de liste.
 */
typedef struct SccrollNode {
    void* car;               /**< Le car de la liste. */
    struct SccrollNode* cdr; /**< Le cdr de la liste. */
} SccrollNode;

/**
 * @since 0.1.0
 * @typedef SccrollList
 * Structure de liste chaînée simple.
 */
typedef SccrollNode* SccrollList;

/**
 * @since 0.1.0
 * @name Group SccrollList_data_access
 * Pseudo-fonctions d'accès au données d'une SccrollList.
 * @param list la SccrollList dont on veut accéder aux données.
 * @{
 */
#define sccroll_car(list) (list)->car /**< Description */
#define sccroll_cdr(list) (list)->cdr /**< Description */
/**
 * @}
 */

/**
 * @since 0.1.0
 * @brief Créé un nouveau noeud de tête pour la liste de tests.
 * @param car Le car du nouveau noeud de tête.
 * @return Le pointeur de la tête de liste.
 */
static void sccroll_push(void* test);

/**
 * @since 0.1.0
 * @var tests
 * @brief Liste des tests à exécuter.
 */
static SccrollList tests = NULL;

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Execution
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Ne faisant absolument rien.
 *
 * Cette fonction est utilisée dans cette librairie comme base d'alias
 * faibles pour les fonctions dont la définition est laissée à
 * l'utilisateur.
 */
static void sccroll_void(void);

static void* sccroll_popcar(void);
#define popcar(type) (type) sccroll_popcar();

/**
 * @brief Retire le premier noeud de la liste de tests et le renvoie.
 * @return Le premier noeud de la liste.
 */
static SccrollNode* sccroll_pop(void);

/**
 * @brief Fork, exécute la fonction du test, enregistre l'output
 * obtenu sur le descripteur #fd, et stocke le code de status du test.
 *
 * @param current Le test à exécuter.
 * @param fd le descripteur de fichier dont on veut l'output.
 */
static void sccroll_fork(SccrollTest* restrict current, int fd) __attribute__((nonnull (1)));

static bool sccroll_check(SccrollTest* restrict test) __attribute__((nonnull));
static char* sccroll_read_pipe(int pipefd[2], const char* name) __attribute__((nonnull (2)));

static int sccroll_review(void);

static int report[2] = { 0 };

// clang-format off

/******************************************************************************
 * @}
 *
 * Implémentation des fonctions d'enregistrement-exécution.
 ******************************************************************************/
// clang-format on

static void sccroll_void(void) {}
weak_alias(sccroll_void, sccroll_init);
weak_alias(sccroll_void, sccroll_clean);
weak_alias(sccroll_void, sccroll_before);
weak_alias(sccroll_void, sccroll_after);

void sccroll_register(SccrollTestFunc func, const char* name)
{
    SccrollTest* test = sccroll_gentest(name);
    test->test = func;
    test->name = name;
    sccroll_push(test);
}

static SccrollTest* sccroll_gentest(const char* restrict name)
{
    SccrollTest* test = calloc(1, sizeof(SccrollTest));
    if (!test) err(EXIT_FAILURE, "could not generate SccrollTest for %s", name);
    if (pipe(test->pipefd) < 0) err(EXIT_FAILURE, "could not generate pipe for %s", name);
    return test;
}

static void sccroll_push(void* test)
{
    SccrollNode* node = calloc(1, sizeof(SccrollNode));
    if (!node) err(EXIT_FAILURE, "could not push to SccrollList");

    node->car = test;
    node->cdr = tests;
    tests = node;
}

int sccroll_run(void)
{
    sccroll_init();

    SccrollTest* test    = NULL;
    while (tests) {
        test = popcar(SccrollTest*);
        ++report[REPORTTOTAL];

        sccroll_before();
        sccroll_fork(test, STDERR_FILENO);
        sccroll_check(test);
        free(test);
        sccroll_after();
    }

    int failed = sccroll_review();
    sccroll_clean();
    return failed;
}
weak_alias(sccroll_run, main);

static void* sccroll_popcar(void)
{
    SccrollNode* current = sccroll_pop();
    void* data = sccroll_car(current);
    free(current);
    return data;
}

static SccrollNode* sccroll_pop(void)
{
    SccrollNode* popped = tests;
    tests = sccroll_cdr(tests);
    sccroll_cdr(popped) = NULL;
    return popped;
}

static void sccroll_fork(SccrollTest* restrict current, int fd)
{
    pid_t pid = fork();
    if (pid < 0)
        err(EXIT_FAILURE, "fork failed for %s", current->name);
    else if (pid == 0) {
        if (dup2(current->pipefd[PIPEWRTE], fd) < 0)
            err(EXIT_FAILURE, "stderr dup failed for %s", current->name);
        current->test();
        if (close(current->pipefd[PIPEWRTE]) < 0)
            err(EXIT_FAILURE, "pipe close failed in fork for %s", current->name);
        exit(EXIT_SUCCESS);
    }

    int status;
    wait(&status);
    if (WIFSIGNALED(status)) current->status = WTERMSIG(status);
}

static bool sccroll_check(SccrollTest* restrict test)
{
    if (test->status) {
        char* output = sccroll_read_pipe(test->pipefd, test->name);
        ++report[REPORTFAIL];
        fprintf(stderr, REPORTFMT, "FAIL", test->name, output);
        free(output);
        return false;
    }
    return true;
}

static char* sccroll_read_pipe(int pipefd[2], const char* name)
{
    char buffer[BUFSIZ] = { 0 };
    if (close(pipefd[PIPEWRTE]) < 0)
        err(EXIT_FAILURE, "could not close pipe write for %s", name);
    if (read(pipefd[PIPEREAD], buffer, BUFSIZ) < 0)
        err(EXIT_FAILURE, "report read failed for %s", name);
    if (close(pipefd[PIPEREAD]) < 0)
        err(EXIT_FAILURE, "could not close pipe read for %s", name);
    return strdup(buffer);
}

static int sccroll_review(void)
{
    int total     = report[REPORTTOTAL];
    int failed    = report[REPORTFAIL];
    int passed    = total - failed;
    float percent = 100.0 * passed / total;

    char separator[MAXLINE + 1] = { 0 };
    memset(separator, '-', MAXLINE);

    fprintf(stderr, "\n%s\n\n", separator);
    fprintf(stderr, REPORTFMT, "STATUS", failed ? "FAIL" : "PASS", "");
    fprintf(stderr, "%.2f%% [%i/%i] tests passed\n", percent, passed, total);

    return failed;
}

// clang-format off

/******************************************************************************
 * Implémentation des fonctions d'assertion.
 ******************************************************************************/
// clang-format on

void sccroll_assert(int test, const char* restrict fmt, ...)
{
    if (!test) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
    }
}

void sccroll_assertExe(const SccrollProcess* restrict proc)
{
    SccrollTest* test = sccroll_gentest(proc->name);
    test->name = proc->name;
    test->test = proc->wrapper;
    int fd = proc->output.fd > 0 ? proc->output.fd : STDERR_FILENO;

    errno = 0;
    sccroll_fork(test, fd);
    char* output = sccroll_read_pipe(test->pipefd, proc->name);
    char* expected = proc->output.str ? proc->output.str : "";

    assertMsg(errno == proc->errcode, EXITFMT, "errno", proc->errcode, errno);
    assertMsg(test->status == proc->exitcode, EXITFMT, "status", proc->exitcode, test->status);
    assertMsg(!strcmp(expected, output), "output on fd %i: expected '%s', got '%s'", fd, expected, output);

    free(output);
    free(test);
}

/** @} */
