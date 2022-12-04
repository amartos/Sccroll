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
 * @name Alias
 * @brief Macros générant des alias de fonctions.
 * @param name Nom de la fonction d'origine.
 * @param aliasname Nom de l'alias.
 * @{
 */

/**
 * @def attr_alias
 * @since 0.1.0
 *
 * Macro créant un alias, et qui permet de fournir à l'alias d'autres
 * attributs. C'est une quasi-copie de la macro @c weak_alias de la
 * librairie C de GNU.
 *
 * @param ... Attributs supplémentaires pour l'alias.
 */
#define attr_alias(name, aliasname, ...) \
    extern __typeof__(name) aliasname __attribute__((alias(#name), ##__VA_ARGS__))

/**
 * @def strong_alias
 * @since 0.1.0
 *
 * Définit un alias fort #aliasname de la fonction #name.
 */
#define strong_alias(name, aliasname) attr_alias(name, aliasname)

/**
 * @def weak_alias
 * @since 0.1.0
 *
 * Définit un alias faible #aliasname de la fonction #name.
 */
#define weak_alias(name, aliasname) attr_alias(name, aliasname, weak)
/** @} */

/**
 * @def sccroll_err
 * @since 0.1.0
 * @brief Si #expr est vrai, affiche un message d'erreur et termine le
 * programme.
 *
 * Cette macro est utilisée pour les vérification de fonctions
 * modifiant #errno en cas d'erreur.
 * @param expr Une expression booléenne valant @c true pour une
 * erreur, sinon valant @c false.
 * @param op L'opération effectuée ayant échoué.
 * @param name Un nom d'étape pour une description plus fine de
 * l'erreur.
 * @exit EXIT_FAILURE si #expr est vrai.
 */
#define sccroll_err(expr, op, name) if ((expr)) err(EXIT_FAILURE, "%s failed for %s", op, name);

/**
 * @enum SccrollConstant
 * @since 0.1.0
 * @brief Constantes numériques internes.
 */
typedef enum SccrollConstant {
    /**
     * @ingroup Report
     * @name ReportConstants
     * @since 0.1.0
     * @brief Constantes numériques utilisée pour la génération de
     * rapports.
     */
    REPORTTOTAL = 0, /**< Index du nombre de tests totaux dans la
                      * table des rapport. */
    REPORTFAIL = 1,  /**< Index du nombre de tests échoués dans la
                      * table des rapports. */
    MAXLINE = 80,    /**< Longueur maximale des lignes d'un rapport. */
    /** @} */
    /**
     * @ingroup Execution
     * @name Pipes
     * @since 0.1.0
     * @brief Index et drapeaux de gestion des pipes.
     */
    PIPEREAD = 0, /**< Index du côté lecture d'un pipe. */
    PIPEWRTE = 1, /**< Index du côté écriture d'un pipe. */
    PIPEOPEN,     /**< Drapeau d'ouverture d'un pipe. */
    PIPECLOSE,    /**< Drapeau de fermeture d'un côté d'un pipe. */
    PIPEDUP,      /**< Drapeau de duplication d'un pipe. */
    PIPEMAX,      /**< Index maximal des drapeaux d'opérations de pipe. */
    /** @} */
} SccrollConstant;

/**
 * @ingroup Report
 * @var PIPEDESC
 * @since 0.1.0
 * @brief Description des drapeaux de pipes de SccrollConstant.
 */
const char* const PIPEDESC[PIPEMAX] = {
    "read pipe", "write pipe",
    "open pipe", "close pipe",
    "duplicate pipe"
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
 * @brief Enregistre le #test dans la liste de tests.
 * @param test Un test à exécuter.
 * @param name le nom du test.
 */
static void sccroll_push(SccrollTestFunc test, const char* name) __attribute__((nonnull));

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

/**
 * @since 0.1.0
 * @brief Gère l'ouverture, la duplication, l'écriture, la lecture et
 * la fermeture de pipes.
 *
 * @attention Cette fonction ferme le pipe d'écriture après écriture,
 * et les deux côtés du pipe à la lecture.
 *
 * @param type Une valeur SccrollConstant:
 * - #PIPEOPEN  pour l'ouverture;
 * - #PIPECLOSE pour la fermeture d'un côté du pipe;
 * - #PIPEWRTE  pour l'écriture et fermeture du côté écriture;
 * - #PIPEREAD  pour la lecture et fermeture complète du pipe;
 * - #PIPEDUP   pour dupliquer le côté écriture du pipe sur un
 *              descripteur de fichier.
 * @param name Le nom du test courant.
 * @param pipefd Le pipe du test courant à modifier.
 * @param ... Arguments supplémentaires dépendant de la valeur de
 * #type:
 * - #PIPEOPEN:  arguments supplémentaires ignorés;
 * - #PIPECLOSE: #PIPEREAD <b>ou</b> #PIPEWRTE selon le côté à fermer;
 * - #PIPEWRTE:  la chaîne de #SCCMAX caractères à écrire dans le pipe;
 * - #PIPEREAD:  une chaîne de #SCCMAX caractères comme destination de
 *               la lecture du pipe;
 * - #PIPEDUP:   le descripteur de fichier où dupliquer le pipe.
 */
static void sccroll_pipes(SccrollConstant type, const char* restrict name, int pipefd[2], ...) __attribute__((nonnull(2, 3)));


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

strong_alias(sccroll_push, sccroll_register);
static void sccroll_push(SccrollTestFunc test, const char* name)
{
    SccrollNode* node = calloc(1, sizeof(SccrollNode));
    sccroll_err(!node, "test registration", name);

    SccrollTest* nexttest = sccroll_gentest(name);
    nexttest->test = test;
    nexttest->name = name;

    node->car = nexttest;
    node->cdr = tests;
    tests = node;
}

static SccrollTest* sccroll_gentest(const char* restrict name)
{
    SccrollTest* test = calloc(1, sizeof(SccrollTest));
    sccroll_err(!test, "SccrollTest alloc", name);
    sccroll_pipes(PIPEOPEN, name, test->pipefd);
    return test;
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
    sccroll_err(pid < 0, "fork", current->name);
    if (pid == 0) {
        sccroll_pipes(PIPEDUP, current->name, current->pipefd, fd);
        current->test();
        sccroll_pipes(PIPECLOSE, current->name, current->pipefd, PIPEWRTE);
        exit(EXIT_SUCCESS);
    }

    int status;
    wait(&status);
    if (WIFSIGNALED(status)) current->status = WTERMSIG(status);
}

static bool sccroll_check(SccrollTest* restrict test)
{
    if (test->status) {
        char output[BUFSIZ] = { 0 };
        sccroll_pipes(PIPEREAD, test->name, test->pipefd, output);
        ++report[REPORTFAIL];
        fprintf(stderr, REPORTFMT, "FAIL", test->name, output);
        return false;
    }
    return true;
}

static void sccroll_pipes(SccrollConstant type, const char* name, int pipefd[2], ...)
{
    int status = 0, index = -1;
    va_list args;
    va_start(args, pipefd);
    switch (type) {
    case PIPEOPEN: status = pipe(pipefd); break;
    case PIPEDUP: status = dup2(pipefd[PIPEWRTE], va_arg(args, int)); break;
    case PIPEREAD:
        sccroll_pipes(PIPECLOSE, name, pipefd, PIPEWRTE);
        status = read(pipefd[PIPEREAD], va_arg(args, char*), BUFSIZ);
        if (status >= 0) sccroll_pipes(PIPECLOSE, name, pipefd, PIPEREAD);
        break;
    case PIPEWRTE:
        status = write(pipefd[PIPEWRTE], va_arg(args, char*), BUFSIZ);
        if (status >= 0) sccroll_pipes(PIPECLOSE, name, pipefd, PIPEWRTE);
        break;
    case PIPECLOSE:
        index  = index < 0 ? va_arg(args, int) : index;
        status = close(pipefd[index]);
        break;
    default: break;
    }
    va_end(args);
    sccroll_err(status < 0, PIPEDESC[type], name);
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
    char output[BUFSIZ] = { 0 };
    sccroll_pipes(PIPEREAD, proc->name, test->pipefd, output);
    char* expected = proc->output.str ? proc->output.str : "";

    assertMsg(errno == proc->errcode, EXITFMT, "errno", proc->errcode, errno);
    assertMsg(test->status == proc->exitcode, EXITFMT, "status", proc->exitcode, test->status);
    assertMsg(!strcmp(expected, output), "output on fd %i: expected '%s', got '%s'", fd, expected, output);

    free(test);
}

/** @} */
