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
    REPORTMAX  = 2,  /**< Index maximal de la table des rapports. */
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
 * @struct SccrollNode
 * @since 0.1.0
 * @brief Structure d'un noeud de la liste de tests.
 *
 * SccrollNode::nth est utilisable pour déterminer le nombre total de
 * tests: le dernier test inscrit étant en tête de liste, son numéro
 * correspond au nombre total de tests.
 */
typedef struct SccrollNode {
    int nth;                   /**< Numéro du noeud dans la liste. */
    const SccrollEffects* car; /**< Le test et ses effets attendus. */
    struct SccrollNode* cdr;   /**< Le prochain noeud de la liste. */
} SccrollNode;

/**
 * @typedef SccrollList
 * @since 0.1.0
 * @brief Structure de la liste de tests.
 */
typedef SccrollNode* SccrollList;

/**
 * @name SccrollListAccess
 * @brief Pseudo-fonctions d'accès au données d'une SccrollList.
 * @param list la SccrollList dont on veut accéder aux données.
 * @{
 */

/**
 * @def sccroll_car
 * @since 0.1.0
 *
 * Accède au test du noeud.
 */
#define sccroll_car(list) (list)->car

/**
 * @def sccroll_nth
 * @since 0.1.0
 *
 * Accède au numéro du noeud.
 */
#define sccroll_nth(list) (list)->nth

/**
 * @def sccroll_cdr
 * @since 0.1.0
 *
 * Accède au reste de la liste.
 */
#define sccroll_cdr(list) (list)->cdr
/** @} */

/**
 * @since 0.1.0
 * @brief Ajoute le test en tête de la liste #tests.
 * @param expected Le SccrollEffects attendu pour le test.
 * @{
 */
static void sccroll_push(const SccrollEffects* restrict expected) __attribute__((nonnull));
/** @} */

/**
 * @since 0.1.0
 * @brief Génère une copie de #effects.
 * @attention Utilise calloc.
 * @param effects Le SccrollEffects à copier.
 * @return Le pointeur d'une copie de #effects.
 */
static SccrollEffects* sccroll_dup(const SccrollEffects* restrict effects) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Génère un SccrollEffects initialisé à 0.
 * @attention utilise calloc.
 * @return Le pointeur d'un SccrollEffects initialisé à 0.
 */
static SccrollEffects* sccroll_gen(void);

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
static void sccroll_void(void) __attribute__((unused));

/**
 * @since 0.1.0
 * @brief Exécute le prochain test de la liste et détermine son
 * résultat.
 * @return 0 si le test réussit, 1 s'il échoue.
 */
static int sccroll_test(void);

/**
 * @since 0.1.0
 * @brief Indique si #flag a été donné pour le test.
 * @param testflags Les drapeaux donnés pour le test.
 * @param flag Le drapeau à tester.
 * @return true si #flags est donné dans #testflags, sinon false.
 */
static bool sccroll_flag(int testflags, SccrollFlags flag);

/**
 * @since 0.1.0
 * @brief Retire le premier noeud de la liste de tests et renvoie le
 * pointeur du test qui y est stocké.
 * @return Le test du premier noeud de la liste.
 */
static const SccrollEffects* sccroll_pop(void);

/**
 * @since 0.1.0
 * @brief exécute la fonction du test dans un fork et enregistre les
 * effets dans la structure SccrollEffects donnée.
 *
 * Les données enregistrées comprennent la valeur de errno après le
 * test (elle est remise à 0 avant le test), les valeurs de signal et
 * status émises par le fork, les affichages des sorties standard
 * stderr et stdout, ainsi que le contenu des fichiers
 * SccrollEffects::files après exécution du test.
 *
 * @param result Le test à exécuter avec ses effets attendus.
 * @return Toujours result, mais modifié avec les effets obtenus.
 */
static const SccrollEffects* sccroll_fork(SccrollEffects* restrict result) __attribute__((nonnull));

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

/**
 * @since 0.1.0
 * @brief Lis les différents codes de status obtenus au cours de
 * l'exécution d'un test et les stocke dans SccrollEffects::codes.
 * @param result La structure SccrollEffects de destination.
 * @param pipefd Le pipe contenant la valeur de errno obtenu dans le
 * fork du test.
 * @param status Le wstatus obtenu avec la fonction wait().
 */
static void sccroll_codes(SccrollEffects* restrict result, int pipefd[2], int status) __attribute__((nonnull(2)));

/**
 * @since 0.1.0
 * @brief Lis les outputs sur les sorties standard obtenus lors de
 * l'exécution d'un test et les stockes dans SccrollEffects::std.
 * @param result La structure SccrollEffects de destination.
 * @param pipestd Une table de pipes utilisés pour capter les sorties;
 * l'index des pipes correspond au descripteur de fichier des sorties
 * standard.
 */
static void sccroll_std(SccrollEffects* restrict result, int pipestd[SCCMAXSTD][2]) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Lis le contenu des fichiers de SccollEffects::files::path et
 * stocke les #SCCMAX premiers caractères dans
 * SccrollEffects::files::content.
 * @param result La structure SccrollEffects de destination.
 */
static void sccroll_files(SccrollEffects* restrict result) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief exécute la fonction du test et enregistre les
 * effets dans la structure SccrollEffects donnée.
 *
 * Les données enregistrées comprennent la valeur de errno après le
 * test (elle est remise à 0 avant le test), les affichages des
 * sorties standard stderr et stdout, ainsi que le contenu des
 * fichiers SccrollEffects::files après exécution du test.
 * @attention Toute erreur qui terminerait le programme levée dans le test
 * @param result Le test à exécuter avec ses effets attendus.
 * @return Toujours result, mais modifié avec les effets obtenus.
 */
static const SccrollEffects* sccroll_nofork(SccrollEffects* restrict result) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Report
 *
 * description
 *
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollColors
 * @since 0.1.0
 * @brief Couleur pour les codes ANSI.
 */
typedef enum SccrollColors{
    RED   = 1, /**< Rouge. */
    GREEN = 2, /**< Vert. */
    CYAN  = 6, /**< Cyan. */
} SccrollColors;

/**
 * @name FormatStrings
 * @brief Chaînes de formatage pour les messages des tests.
 * @{
 */

/**
 * @def BASEFMT
 * @since 0.1.0
 *
 * Formatage de l'indicateur de status.
 *
 * @param i Un chiffre de code de couleur ANSI (SccrollColors est
 * adapté pour ce paramètre).
 * @param s Status
 * @param s Nom du test.
 */
#define BASEFMT "[ \e[0;1;3%im%-4s\e[0m ] %s"

/**
 * @def REPORTFMT
 * @since 0.1.0
 *
 * Formatage du rapport final.
 *
 * @param s Ligne de séparation
 * @param s Status.
 * @param s Nom du test.
 * @param f Pourcentage de réussite des tests.
 * @param i Nombre de tests réussis.
 * @param i Nombre total de tests.
 */
#define REPORTFMT "\n%s\n\n" BASEFMT ": %.2f%% [%i/%i]\n"

/**
 * @def DIFFFMT
 *
 * Formatage de l'affichage des différences attendu/obtenu.
 *
 * @param
 * @param i Un chiffre de code de couleur ANSI (SccrollColors est
 * adapté pour ce paramètre).
 * @param s Status
 * @param s Nom du test.
 * @param s Description de la différence.
 */
#define DIFFFMT BASEFMT " (%s): "

/**
 * @def OUTPUTFMT
 * @since 0.1.0
 *
 * Formatage d'affichage des différences des sorties standard.
 *
 * @param s Status.
 * @param s Nom du test.
 * @param s Description de la différence.
 * @param s Chaîne obtenue.
 */
#define OUTPUTFMT DIFFFMT "%s\n"

/**
 * @def FILESFMT
 * @since 0.1.0
 *
 * Formatage d'affichage de différences entre fichiers.
 * @param s Status.
 * @param s Nom du test.
 * @param s Description de la différence.
 * @param s Chaîne attendue.
 * @param s Chaîne obtenue.
 */
#define FILESFMT DIFFFMT "expected '%s', read '%s'\n"

/**
 * @def EXITFMT
 * @since 0.1.0
 *
 * Formatage d'affichage d'erreurs concernant les codes d'erreur et
 * sortie attendus.
 *
 * @param s1 Status.
 * @param s2 Nom du test.
 * @param s3 Description du test.
 * @param s4 code attendu.
 * @param s5 code obtenu.
 */
#define EXITFMT DIFFFMT "expected %i, got %i\n"
/** @} */

/**
 * @since 0.1.0
 * @brief Compare deux SccrollEffects et indique si les valeurs des
 * données du test qu'elles contiennent sont différentes.
 *
 * @parblock
 * Si la macro #SCCROLL_NODIFF est définie, la fonction ne fait que
 * renvoyer le résultat. Si elle ne l'est pas, la fonction affiche
 * égalemnt un message pour chaque élément différent entre les effets
 * attendus et obtenus.
 *
 * Les données comparées sont:
 * - Les valeur de SccrollEffects::codes;
 * - Les chaînes de caractères de SccrollEffects::std des index
 * #STDOUT_FILENO et #STDERR_FILENO;
 * - Les chaînes de caractères SccrollEffects::files::content jusqu'au
 * premier SccrollEffects::files::path valant @c NULL .
 * @endparblock
 *
 * @param expected,result Les structures à comparer.
 * @return true si au moins une des données comparées diffère entre
 * les effets attendu et ceux obtenus, sinon false.
 */
static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @def sccroll_pdiff
 * @since 0.1.0
 * @brief Affiche un message d'erreur sur stderr sauf si le drapeau
 * #NODIFF est donné pour le test.
 * @param flags Les drapeaux donnés pour le test.
 * @param fmt La chaîne de formatage du message.
 * @param ... Les arguments de la chaîne de formatage.
 */
#define sccroll_pdiff(flags, fmt, ...)                                  \
    if (!sccroll_flag(flags, NODIFF)) fprintf(stderr, fmt, ##__VA_ARGS__)

/**
 * @def sccroll_strcmp
 * @since 0.1.0
 * @brief Compare deux chaînes et affiche un message si elles sont
 * différences.
 * @attention Cette macro est spécifique à la fonction sccroll_diff().
 * @param exp La chaîne attendue.
 * @param res La chaîne obtenue.
 * @param fmt La chaîne de formatage du message.
 * @param ... Les arguments de la chaîne de formatage.
 */
#define sccroll_strcmp(exp, res, fmt, ...)                              \
    if (strcmp(exp, res)) {                                             \
        sccroll_pdiff(flags, fmt, CYAN, "DIFF",                         \
                      expected->name, ##__VA_ARGS__);                   \
        diff = true;                                                    \
    }

/**
 * @since 0.1.0
 * @brief Si #NOSTRIP n'a pas été donné pour le test, renvoie une
 * copie de la chaîne où les espaces en amont et aval ont été retirés.
 * @attention Utilise malloc.
 * @param flags Les drapeaux du test.
 * @param string La chaîne à traiter.
 * @return Une copie de la chaîne, avec les espaces amonts et avals
 * retirés si le drapeau #NOSTRIP n'est pas donné; si #string est
 * @c NULL , renvoie une chaîne vide.
 */
static const char* sccroll_strip(int flags, const char* restrict string);

/**
 * @since 0.1.0
 * @brief Affiche un rapport final.
 * @param report Une table contenant le nombre total de tests (index
 * #REPORTTOTAL) et le nombre de tests en échec (index #REPORTFAIL).
 */
static void sccroll_review(int report[REPORTMAX]) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Clean Fonctions internes de nettoyage.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Libère une structure SccrollEffects allouée.
 *
 * Les éléments libérés sont:
 * - Tous les SccrollEffects::files::content jusqu'au premier
 * SccrollEffects::files::path valant NULL;
 * - Tous les SccrollEffects::std;
 * - #effects.
 * @param effects La structure à libérer.
 */
static void sccroll_free(const SccrollEffects* restrict effects) __attribute__((nonnull));

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
static void sccroll_push(const SccrollEffects* restrict expected)
{
    SccrollNode* node = calloc(1, sizeof(SccrollNode));
    sccroll_err(!node, "test registration", expected->name);

    node->car = sccroll_dup(expected);
    node->nth = !tests ? 1 : sccroll_nth(tests) + 1;
    node->cdr = tests;
    tests = node;
}

static SccrollEffects* sccroll_dup(const SccrollEffects* restrict effects)
{
    SccrollEffects* copy = sccroll_gen();
    memcpy(copy, effects, sizeof(SccrollEffects));
    return copy;
}

static SccrollEffects* sccroll_gen(void)
{
    SccrollEffects* effects = calloc(1, sizeof(SccrollEffects));
    sccroll_err(!effects, "alloc", "SccrollEffects");
    return effects;
}

int sccroll_run(void)
{
    if (!tests) return 0;

    int report[REPORTMAX] = { 0 };
    report[REPORTTOTAL]   = sccroll_nth(tests);

    sccroll_init();
    while (tests) {
        sccroll_before();
        report[REPORTFAIL] += sccroll_test();
        sccroll_after();
    }
    sccroll_review(report);
    sccroll_clean();

    return report[REPORTFAIL];
}
weak_alias(sccroll_run, main);

static int sccroll_test(void)
{
    const SccrollEffects* expected = sccroll_pop();
    const SccrollEffects* result   =
        sccroll_flag(expected->flags, NOFORK)
        ? sccroll_nofork(sccroll_dup(expected))
        : sccroll_fork(sccroll_dup(expected));
    int failed = sccroll_diff(expected, result);
    if (failed) fprintf(stderr, BASEFMT "\n", RED, "FAIL", expected->name);
    free((void*)expected);
    sccroll_free(result);
    return failed;
}

static bool sccroll_flag(int testflags, SccrollFlags flag)
{
    return (bool)(testflags & flag);
}

static const SccrollEffects* sccroll_pop(void)
{
    const SccrollEffects* expected = sccroll_car(tests);
    SccrollNode* popped = tests;
    tests = sccroll_cdr(popped);
    free(popped);
    return expected;
}

static const SccrollEffects* sccroll_fork(SccrollEffects* restrict result)
{
    int status               = 0;
    char buffer[SCCMAX]      = { 0 };
    int pipefd[SCCMAXSTD][2] = { 0 };
    for (int i = 0; i < SCCMAXSTD; ++i)
        sccroll_pipes(PIPEOPEN, result->name, pipefd[i]);

    pid_t pid = fork();
    sccroll_err(pid < 0, "fork", result->name);
    if (pid == 0) {
        for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
            sccroll_pipes(PIPEDUP, result->name, pipefd[i], i);
        errno = 0;
        result->wrapper();
        sprintf(buffer, "%i", errno);
        sccroll_pipes(PIPEWRTE, result->name, pipefd[0], buffer);
        exit(EXIT_SUCCESS);
    }

    wait(&status);
    sccroll_codes(result, pipefd[SCCERRNUM], status);
    sccroll_std(result, pipefd);
    sccroll_files(result);
    return result;
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
        status = read(pipefd[PIPEREAD], va_arg(args, char*), SCCMAX);
        if (status >= 0) sccroll_pipes(PIPECLOSE, name, pipefd, PIPEREAD);
        break;
    case PIPEWRTE:
        status = write(pipefd[PIPEWRTE], va_arg(args, char*), SCCMAX);
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

static void sccroll_codes(SccrollEffects* restrict result, int pipefd[2], int status)
{
    char buffer[SCCMAX] = { 0 };
    sccroll_pipes(PIPEREAD, result->name, pipefd, buffer);
    result->codes[SCCERRNUM] = strtol(buffer, NULL, 10);
    result->codes[SCCSTATUS] = WEXITSTATUS(status);
    result->codes[SCCSIGNAL] = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
}

static void sccroll_std(SccrollEffects* restrict result, int pipefd[SCCMAXSTD][2])
{
    char buffer[SCCMAX] = { 0 };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) {
        sccroll_pipes(PIPEREAD, result->name, pipefd[i], buffer);
        result->std[i] = strdup(buffer);
        memset(buffer, 0, strlen(buffer));
    }
}

static void sccroll_files(SccrollEffects* restrict result)
{
    int i;
    char buffer[SCCMAX] = { 0 };
    char error[SCCMAX]  = { 0 };
    FILE* file;

    for (i = 0; i < SCCMAX && result->files[i].path && !error[0]; ++i, memset(buffer, 0, strlen(buffer))) {
        file  = fopen(result->files[i].path, "w+b");
        if (!file)
            sprintf(error, "%s: %s", result->name, "open");
        else if (!fread(buffer, sizeof(char), SCCMAX, file) && ferror(file))
            sprintf(error, "%s: %s", result->name, "read");
        result->files[i].content = strdup(buffer);
        fclose(file);
    }
    sccroll_err(error[0], error, result->files[i - 1].path);
}

static const SccrollEffects* sccroll_nofork(SccrollEffects* restrict result)
{
    int origfd[SCCMAXSTD] = { 0 };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
        sccroll_err((origfd[i] = dup(i)) < 0, "dup save of standard", result->name);

    int pipefd[SCCMAXSTD][2] = { 0 };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) {
        sccroll_pipes(PIPEOPEN, result->name, pipefd[i]);
        sccroll_pipes(PIPEDUP, result->name, pipefd[i], i);
    }

    errno = 0;
    result->wrapper();
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
        if (dup2(origfd[i], i) < 0) {
            printf("dup2 standard back to original failed for %s", result->name);
            exit(EXIT_FAILURE);
        }

    result->codes[SCCERRNUM] = errno;
    sccroll_std(result, pipefd);
    sccroll_files(result);
    return result;
}

static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    bool diff                    = false;
    int flags                    = expected->flags;
    const char* name             = expected->name;
    const char* expectedstr      = NULL;
    const char* resultstr        = NULL;
    const char* const descs[][3] = {
        {"errno", "signal", "status"},
        {   NULL, "stdout", "stderr"}
    };

    for (int i = SCCERRNUM; i < SCCMAXSIG; ++i)
        if (i < SCCMAXSIG && expected->codes[i] != result->codes[i]) {
            diff = true;
            sccroll_pdiff(
                flags, EXITFMT, CYAN, "DIFF", name,
                descs[0][i], expected->codes[i], result->codes[i]);
        }

    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
    {
        expectedstr = sccroll_strip(flags, expected->std[i]);
        resultstr   = sccroll_strip(flags, result->std[i]);
        sccroll_strcmp(expectedstr, resultstr, OUTPUTFMT, descs[1][i], resultstr);
        free((void*)expectedstr);
        free((void*)resultstr);
    }

    for (int i = 0; i < SCCMAX && (bool)expected->files[i].path; ++i)
        sccroll_strcmp(
            expected->files[i].content ? expected->files[i].content : "",
            result->files[i].content,
            FILESFMT,
            expected->files[i].path,
            expected->files[i].content,
            result->files[i].content);

    return diff;
}

static const char* sccroll_strip(int flags, const char* restrict string)
{
    char* stripped = strdup(string ? string : "");
    if (!sccroll_flag(flags, NOSTRP)) {
        while(isspace(*stripped)) ++stripped;
        char* end = stripped+strlen(stripped)-1;
        while(end > stripped && isspace(*end)) {
            *end = 0;
            --end;
        }
    }

    return stripped;
}

static void sccroll_review(int report[REPORTMAX])
{
    int passed    = report[REPORTTOTAL] - report[REPORTFAIL];
    float percent = 100.0 * passed / report[REPORTTOTAL];

    char separator[MAXLINE + 1] = { 0 };
    memset(separator, '-', MAXLINE);
    fprintf(stderr, REPORTFMT,
        separator,
        report[REPORTFAIL] ? RED : GREEN,
        report[REPORTFAIL] ? "FAIL" : "PASS",
        "success rate", percent, passed, report[REPORTTOTAL]);
}

static void sccroll_free(const SccrollEffects* restrict effects)
{
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) free(effects->std[i]);
    for (int i = 0; i < SCCMAX && effects->files[i].path; ++i) free(effects->files[i].content);
    free((void*)effects);
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

/** @} */
