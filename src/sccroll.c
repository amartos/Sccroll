/**
 * @file        sccroll.c
 * @version     0.1.0
 * @brief       Fichier source de la librairie Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -fpic -shared -Wl,--wrap,abort \
 *     -o build/libs/libsccroll.so
 * @endcode
 *
 * @addtogroup Internals Structures internes de Sccroll
 * @{
 */

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * @addtogroup Misc Aide à la programmation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @name Alias Macros générant des alias de fonctions
 * @{
 * @param name Nom de la fonction d'origine.
 * @param aliasname Nom de l'alias.
 * @param ... Attributs supplémentaires pour l'alias.
 */

/**
 * @def attr_alias
 * @since 0.1.0
 * @brief Génère un alias.
 */
#define attr_alias(name, aliasname, ...) \
    extern __typeof__(name) aliasname __attribute__((alias(#name), ##__VA_ARGS__))

/**
 * @def strong_alias
 * @since 0.1.0
 * @brief Génère un alias fort.
 */
#define strong_alias(name, aliasname) attr_alias(name, aliasname)

/**
 * @def weak_alias
 * @since 0.1.0
 * @brief Génère un alias faible.
 */
#define weak_alias(name, aliasname) attr_alias(name, aliasname, weak)
/** @} */

/**
 * @def sccroll_err
 * @since 0.1.0
 * @brief Lève une erreur si @p expr est vraie.
 *
 * Cette macro est utilisée pour les vérification de fonctions
 * modifiant @c errno en cas d'erreur.
 *
 * @param expr Une expression booléenne valant @c true pour une
 * erreur, sinon valant @c false.
 * @param op Description de l'opération effectuée ayant échoué.
 * @param name Un nom d'étape pour une description plus fine de
 * l'erreur.
 * @throw EXIT_FAILURE si @p expr est vraie.
 */
#define sccroll_err(expr, op, name)                             \
    if ((expr)) err(EXIT_FAILURE, "%s failed for %s", op, name);

/**
 * @enum SccrollConstant
 * @since 0.1.0
 * @brief Constantes numériques internes.
 */
typedef enum SccrollConstant {
    REPORTTOTAL = 0, /**< Index du nombre total de tests. */
    REPORTFAIL = 1,  /**< Index du nombre de tests échoués. */
    REPORTMAX  = 2,  /**< Index maximal de la table des rapports. */
    MAXLINE = 80,    /**< Longueur maximale des lignes d'un rapport. */
    PIPEREAD = 0, /**< Index du côté lecture d'un pipe. */
    PIPEWRTE = 1, /**< Index du côté écriture d'un pipe. */
    PIPEOPEN,     /**< Drapeau d'ouverture d'un pipe. */
    PIPECLOSE,    /**< Drapeau de fermeture d'un côté d'un pipe. */
    PIPEDUP,      /**< Drapeau de duplication d'un pipe. */
    PIPEMAX,      /**< Index maximal des drapeaux d'opérations de pipe. */
} SccrollConstant;

/**
 * @var PIPEDESC
 * @since 0.1.0
 * @brief Description d'opération des valeurs SccrollConstant::PIPE*.
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
 * @addtogroup Tests Tests unitaires
 * @{
 *
 * @addtogroup Preparation Préparation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fonction ne faisant absolument rien.
 *
 * Cette fonction est utilisée dans cette librairie comme base d'alias
 * faibles pour les fonctions dont la définition est laissée à
 * l'utilisateur.
 */
static void sccroll_void(void) __attribute__((unused));

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Registration Enregistrement
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @name Liste des tests
 * @{
 */

/**
 * @struct SccrollNode
 * @since 0.1.0
 * @brief Structure d'un noeud de la liste de tests.
 * @note SccrollNode::nth est utilisable pour déterminer le nombre
 * total de tests: le dernier test inscrit étant en tête de liste, son
 * numéro correspond au nombre total de tests.
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
 * @since 0.1.0
 * @var tests
 * @brief Liste des tests à exécuter.
 */
static SccrollList tests = NULL;
/** @} */

/**
 * @name Accès au données d'une SccrollList
 * @{
 * @param list la SccrollList dont on veut accéder aux données.
 */

/**
 * @def sccroll_car
 * @since 0.1.0
 * @brief Accède au test du noeud.
 */
#define sccroll_car(list) (list)->car

/**
 * @def sccroll_nth
 * @since 0.1.0
 * @brief Accède au numéro du noeud.
 */
#define sccroll_nth(list) (list)->nth

/**
 * @def sccroll_cdr
 * @since 0.1.0
 * @brief Accède au reste de la liste.
 */
#define sccroll_cdr(list) (list)->cdr
/** @} */

/**
 * @since 0.1.0
 * @brief Ajoute le test en tête de la liste #tests.
 * @param expected Le SccrollEffects attendu pour le test.
 */
static void sccroll_push(const SccrollEffects* restrict expected) __attribute__((nonnull));

/**
 * @name Générateurs de SccrollEffects
 * @attention Ces fonctions utilisent calloc.
 * @{
 */

/**
 * @since 0.1.0
 * @brief Génère une copie de @p effects.
 * @attention La structure est copiée, mais pas le contenu des
 * emplacements des pointeurs stockés dans la structure (ce sont les
 * mêmes pointeurs entre l'original et la copie).
 * @param effects Le SccrollEffects à copier.
 * @return Le pointeur d'une copie de @p effects.
 */
static SccrollEffects* sccroll_dup(const SccrollEffects* restrict effects) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Génère un SccrollEffects initialisé à 0.
 * @attention utilise calloc.
 * @return Le pointeur d'un SccrollEffects initialisé à 0.
 */
static SccrollEffects* sccroll_gen(void);
/** @} */

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Execution Exécution
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fonction main par défaut de la librairie.
 * @return EXIT_FAILURE si au moins un test a échoué, sinon
 * EXIT_SUCCESS.
 */
static int sccroll_main(void);

/**
 * @since 0.1.0
 * @brief Exécute le prochain test de la liste et détermine son
 * résultat.
 * @return 0 si le test réussit, 1 s'il échoue.
 */
static int sccroll_test(void);

/**
 * @var in_test
 * @since 0.1.0
 * @brief Indique si les tests sont en cours d'exécution.
 *
 * Cette variable sert à empêcher l'exécution de certaines fonctions
 * au cours des tests.
 */
static bool in_test = false;

/**
 * @since 0.1.0
 * @brief Retire le premier noeud de la liste de tests et renvoie le
 * pointeur du test qui y est stocké.
 * @return Le test du premier noeud de la liste.
 */
static const SccrollEffects* sccroll_pop(void);

/**
 * @name Test avec fork
 * @{
 */

/**
 * @since 0.1.0
 * @brief exécute la fonction du test dans un fork et enregistre les
 * effets dans la structure SccrollEffects donnée.
 * @see #NOFORK
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
 * @see Pipes
 * @attention Cette fonction ferme le pipe d'écriture après écriture,
 * et les deux côtés du pipe à la lecture.
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
 * @p type:
 * - #PIPEOPEN:  arguments supplémentaires ignorés;
 * - #PIPECLOSE: #PIPEREAD **ou** #PIPEWRTE selon le côté à fermer;
 * - #PIPEWRTE:  la chaîne de #SCCMAX caractères à écrire dans le pipe;
 * - #PIPEREAD:  une chaîne de #SCCMAX caractères comme destination de
 *               la lecture du pipe;
 * - #PIPEDUP:   le descripteur de fichier où dupliquer le pipe.
 */
static void sccroll_pipes(SccrollConstant type, const char* restrict name, int pipefd[2], ...) __attribute__((nonnull(2, 3)));
/** @} */

/**
 * @name Test sans fork
 * @{
 */

/**
 * @since 0.1.0
 * @brief exécute la fonction du test et enregistre les
 * effets dans la structure SccrollEffects donnée.
 * @see #NOFORK
 *
 * Les données enregistrées comprennent la valeur de errno après le
 * test (elle est remise à 0 avant le test), les affichages des
 * sorties standard stderr et stdout, ainsi que le contenu des
 * fichiers SccrollEffects::files après exécution du test.
 *
 * @attention Toute erreur qui terminerait le programme levée dans le test
 * @param result Le test à exécuter avec ses effets attendus.
 * @return Toujours result, mais modifié avec les effets obtenus.
 */
static const SccrollEffects* sccroll_nofork(SccrollEffects* restrict result) __attribute__((nonnull));
/** @} */

/**
 * @name Récolte des effets secondaires
 * @{
 */

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
 * @see #NOSTRP
 * @param result La structure SccrollEffects de destination.
 * @param pipestd Une table de pipes utilisés pour capter les sorties;
 * l'index des pipes correspond au descripteur de fichier des sorties
 * standard.
 */
static void sccroll_std(SccrollEffects* restrict result, int pipestd[SCCMAXSTD][2]) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Copie la chaîne, et retire les espaces en début et fin si
 * @p flags ne contient pas #NOSTRP.
 * @see #NOSTRP
 * @attention Utilise malloc.
 * @param flags Les drapeaux du test.
 * @param string La chaîne à traiter.
 * @return Une copie de la chaîne, avec les espaces amonts et avals
 * retirés si le drapeau #NOSTRP n'est pas donné.
 */
static char* sccroll_strip(int flags, const char* restrict string);

/**
 * @since 0.1.0
 * @brief Lis le contenu des fichiers de SccollEffects::files::path et
 * stocke les #SCCMAX premiers caractères dans
 * SccrollEffects::files::content.
 * @param result La structure SccrollEffects de destination.
 */
static void sccroll_files(SccrollEffects* restrict result) __attribute__((nonnull));
/** @} */

// clang-format off

/******************************************************************************
 * @}
 *
 * @addtogroup Report Affichage des rapports
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @name Formats des rapports
 * @{
 */

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
 * @def BASEFMT
 * @since 0.1.0
 * @brief Format de l'indicateur de status.
 * @param i Un chiffre SccrollColors.
 * @param s Status
 * @param s Nom du test.
 */
#define BASEFMT "[ \e[0;1;3%im%-4s\e[0m ] %s"

/**
 * @def REPORTFMT
 * @since 0.1.0
 * @brief Format du rapport final.
 * @param s Ligne de séparation
 * @param i Un chiffre SccrollColors.
 * @param s Status.
 * @param s Nom du test.
 * @param f Pourcentage de réussite des tests.
 * @param i Nombre de tests réussis.
 * @param i Nombre total de tests.
 */
#define REPORTFMT "\n%s\n\n" BASEFMT ": %.2f%% [%i/%i]\n"

/**
 * @def DIFFFMT
 * @since 0.1.0
 * @brief Format de l'affichage des différences attendu/obtenu.
 * @param i Un chiffre SccrollColors.
 * @param s Status
 * @param s Nom du test.
 * @param s Description de la différence.
 */
#define DIFFFMT BASEFMT " (%s): "

/**
 * @def OUTPUTFMT
 * @since 0.1.0
 * @brief Format d'affichage des différences des sorties standard.
 * @param i Un chiffre SccrollColors.
 * @param s Status.
 * @param s Nom du test.
 * @param s Description de la différence.
 * @param s Chaîne obtenue.
 */
#define OUTPUTFMT DIFFFMT "%s\n"

/**
 * @def FILESFMT
 * @since 0.1.0
 * @brief Format d'affichage de différences entre fichiers.
 * @param i Un chiffre SccrollColors.
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
 * @brief Format d'affichage d'erreurs concernant les codes d'erreur et
 * sortie attendus.
 * @param i Un chiffre SccrollColors.
 * @param s Status.
 * @param s Nom du test.
 * @param s Description du test.
 * @param i code attendu.
 * @param i code obtenu.
 */
#define EXITFMT DIFFFMT "expected %i, got %i\n"
/** @} */

/**
 * @name Comparaison des effets obtenus et attendus
 * @{
 */

/**
 * @since 0.1.0
 * @brief Compare deux SccrollEffects et indique leurs différences.
 *
 * La fonction détermine si @p expected et @p result diffèrent, et
 * renvoie @c true si c'est le cas. Si l'option #NODIFF est définie
 * pour le test, elle ne fait rien de plus. Dans le cas contraire,
 * elle affiche la différence entre les deux structures.
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
 * @brief Affiche un message d'erreur sur stderr uniquement si le
 * drapeau #NODIFF n'est pas donné pour le test.
 * @param flags Les drapeaux donnés pour le test.
 * @param fmt Format du message.
 * @param ... Les arguments de la chaîne de formatage.
 */
#define sccroll_pdiff(flags, fmt, ...)                                  \
    if (!sccroll_hasFlags(flags, NODIFF)) fprintf(stderr, fmt, ##__VA_ARGS__)

/**
 * @def sccroll_strcmp
 * @since 0.1.0
 * @brief Compare deux chaînes et affiche un message si elles sont
 * différences.
 * @attention Cette macro est spécifique à la fonction sccroll_diff().
 * @param exp La chaîne attendue.
 * @param res La chaîne obtenue.
 * @param fmt Format du message.
 * @param ... Les arguments de la chaîne de formatage.
 */
#define sccroll_strcmp(exp, res, fmt, ...)                              \
    if (strcmp(exp, res)) {                                             \
        sccroll_pdiff(flags, fmt, CYAN, "DIFF",                         \
                      expected->name, ##__VA_ARGS__);                   \
        diff = true;                                                    \
    }
/** @} */

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
 * @addtogroup Clean Nettoyage
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
 * - @p effects .
 * @param effects La structure à libérer.
 */
static void sccroll_free(const SccrollEffects* restrict effects) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 * @}
 *
 * @addtogroup Mocks Simulacres
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fonction renvoyant toujours #SCCENONE.
 * @note est utilisée comme alias faible de sccroll_mockTrigger().
 * @return #SCCENONE.
 */
static unsigned sccroll_enone(void);

/**
 * @since 0.1.0
 * @brief Fonction sauvegardant les données utilisées par gcov.
 */
extern void __gcov_dump(void);

// clang-format off

/******************************************************************************
 * @}
 * @}
 *
 * Implémentation des fonctions.
 *
 * Préparation, définition, enregistrement et exécution des tests.
 ******************************************************************************/
// clang-format on

weak_alias(sccroll_void, sccroll_init);
weak_alias(sccroll_void, sccroll_clean);
weak_alias(sccroll_void, sccroll_before);
weak_alias(sccroll_void, sccroll_after);
static void sccroll_void(void) {}

strong_alias(sccroll_push, sccroll_register);
static void sccroll_push(const SccrollEffects* restrict expected)
{
    assert(!in_test);

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

weak_alias(sccroll_main, main);
static int sccroll_main(void)
{
    return sccroll_run() ? EXIT_FAILURE : EXIT_SUCCESS;
}

int sccroll_run(void)
{
    assert(!in_test);
    if (!tests) return 0;

    in_test = true;
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

    in_test = false;
    return report[REPORTFAIL];
}

static int sccroll_test(void)
{
    const SccrollEffects* expected = sccroll_pop();
    const SccrollEffects* result   =
        sccroll_hasFlags(expected->flags, NOFORK)
        ? sccroll_nofork(sccroll_dup(expected))
        : sccroll_fork(sccroll_dup(expected));
    int failed = sccroll_diff(expected, result);
    if (failed) fprintf(stderr, BASEFMT "\n", RED, "FAIL", expected->name);
    free((void*)expected);
    sccroll_free(result);
    return failed;
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

static void sccroll_pipes(SccrollConstant type, const char* restrict name, int pipefd[2], ...)
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
        result->std[i] = sccroll_strip(result->flags, buffer);
        memset(buffer, 0, strlen(buffer));
    }
}

static char* sccroll_strip(int flags, const char* restrict string)
{
    char* stripped = strdup(string);
    if (!sccroll_hasFlags(flags, NOSTRP)) {
        while(isspace(*stripped)) ++stripped;
        char* end = stripped+strlen(stripped)-1;
        while(end > stripped && isspace(*end)) *end-- = 0;
    }

    return stripped;
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
        sccroll_strcmp(
            expected->std[i] ? expected->std[i] : "",
            result->std[i],
            OUTPUTFMT,
            descs[1][i],
            result->std[i]
        );

    for (int i = 0; i < SCCMAX && (bool)expected->files[i].path; ++i)
        sccroll_strcmp(
            expected->files[i].content ? expected->files[i].content : "",
            result->files[i].content,
            FILESFMT,
            expected->files[i].path,
            expected->files[i].content ? expected->files[i].content : "",
            result->files[i].content
        );

    return diff;
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
 * Assertions
 ******************************************************************************/
// clang-format on

__attribute__((format(printf,2,3)))
void sccroll_assert(int expr, const char* restrict fmt, ...)
{
    if (!expr) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
    }
}

// clang-format off

/******************************************************************************
 * Mocks
 ******************************************************************************/
// clang-format on

weak_alias(sccroll_enone, sccroll_mockTrigger);
static unsigned sccroll_enone(void) { return SCCENONE; }

SCCROLL_MOCK(void, abort, void)
{
    // On enregistre les fonctions avec atexit afin de permettre une
    // couverture de code complète: puisque les fonctions sont
    // appelées après exit, toutes les lignes de la fonction sont
    // utilisées. L'ordre est important, car les fonctions sont
    // appelée en ordre inverse de leur inscription.
    //
    // La fonction doit quitter. Mais une erreur possible pour elle
    // est de quitter de la mauvaise manière: au lieu de s'arrêter
    // avec un signal SIGABRT et un status EXIT_SUCCESS, la fonction
    // s'arrête avec exit et un status d'erreur. Ici c'est une
    // fonction qui ne fait rien qui est utilisée à la place de la
    // fonction abort originelle car ce simulacre quitte déjà avec un
    // exit et un code d'erreur (qui ne sera pas renvoyé si l'abort
    // originelle est utilisée).
    atexit(sccroll_hasFlags(sccroll_mockTrigger(), SCCEABORT) ? sccroll_void : __real_abort);
    atexit(__gcov_dump);
    exit(SIGABRT);
}
