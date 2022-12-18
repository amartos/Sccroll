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

/**
 * @name Préparation du SccrollEffects des effets attendus
 * @{
 */

/**
 * @since 0.1.0
 * @brief Détermine si des options nécessitant un pré-traitement ont
 * été données, et les applique.
 * @attention Utilise malloc.
 * @param effects Une structure SccrollEffects à pré-traiter.
 * @return Une copie de @p effects avec les options de traitement appliquées.
 */
static SccrollEffects* sccroll_prepare(const SccrollEffects* restrict effects) __attribute__((nonnull));

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
static char* sccroll_strip(const char* string) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Détermine si @p path est effectivement un chemin de fichier,
 * et en récupère le contenu.
 *
 * @p path est un chemin si sa taille est `> 1` et si `dirname(path)`
 * renvoie autre chose que `"."`. Si @p path ne correspond pas à ces
 * critères, la fonction renvoie une simple copie de sa valeur.
 *
 * @see #EXPATH
 * @attention Utilise malloc.
 * @param path Une chaîne pouvant contenir un chemin de fichier.
 * @param name Le nom du test correspondant au fichier.
 * @return Si @p path est un chemin, le contenu du fichier
 * correspondant, sinon la valeur de @p path (les valeurs renvoyées
 * sont toujours des copies générées avec malloc).
 */
static char* sccroll_content(const char* restrict path, const char* restrict name);

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
 * @brief Lit le contenu du fichier décrit par @p path et stocke
 * #SCCMAX caractères dans @p buffer.
 * @param path Le chemin du fichier.
 * @param buffer La table où stocker les #SCCMAX premiers caractères
 * de @p path.
 * @param name Le nom du test.
 * @throw EXIT_FAILURE si un problème survient dans la lecture du
 * fichier.
 */
static void sccroll_fread(const char* restrict path, char buffer[SCCMAX+1], const char* restrict name) __attribute__((nonnull));
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
 * @since 0.1.0
 * @brief exécute la fonction du test et enregistre les effets dans la
 * structure SccrollEffects donnée.
 *
 * Si l'option #NOFORK est donnée, La fonction exécute directement la
 * fonction de test. Sinon, elle créé un fork avant de l'exécuter.
 *
 * Les données enregistrées comprennent la valeur de errno après le
 * test (elle est remise à 0 avant le test), les valeurs de signal et
 * status émises par l'éventuel fork (sinon 0), les affichages des
 * sorties standard stderr et stdout, ainsi que le contenu des
 * fichiers SccrollEffects::files après exécution du test.
 *
 * @see #NOFORK
 * @param result Le test à exécuter avec ses effets attendus.
 * @return Toujours result, mais modifié avec les effets obtenus.
 */
static const SccrollEffects* sccroll_exe(SccrollEffects* restrict result) __attribute__((nonnull));

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
 * @name Coloration avec codes ANSI.
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
 * @def COLSTRFMT
 * @since 0.1.0
 * @brief Ajoute les codes ANSI de coloration à une chaîne.
 * @param i Un code SccrollColors
 * @param s La chaîne à colorer
 */
#define COLSTRFMT "\e[0;1;3%im%s\e[0m"
/** @} */

/**
 * @name Formats des rapports
 * @{
 */

/**
 * @def BASEFMT
 * @since 0.1.0
 * @brief Format de l'indicateur de status.
 * @param i Un code SccrollColors.
 * @param s Status
 * @param s Nom du test.
 */
#define BASEFMT "[ " COLSTRFMT " ] %s"

/**
 * @def REPORTFMT
 * @since 0.1.0
 * @brief Format du rapport final.
 * @param s Ligne de séparation
 * @param i Un code SccrollColors.
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
 * @param name Nom du test
 * @param s Description de la différence.
 */
#define DIFFFMT BASEFMT ": %s\n"

/**
 * @def CODEFMT
 * @since 0.1.0
 * @brief Format d'affichage d'erreurs concernant les codes d'erreur et
 * sortie attendus.
 * @param i Un chiffre SccrollColors.
 * @param s Status.
 * @param s Nom du test.
 * @param s Description du test.
 * @param i Code attendu.
 * @param s Description du code attendu.
 * @param i Code obtenu.
 * @param s Description du code obtenu.
 */
#define CODEFMT BASEFMT ": %s: expected %i (%s), got %i (%s)\n"
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
 * @since 0.1.0
 * @brief Affiche un message d'erreur décrivant la différence entre
 * @p exp et @p res.
 * @param name Le nom du test.
 * @param code Le type de code (voir SccrollIndexes).
 * @param exp La valeur du code attendu.
 * @param res La valeur du code obtenu.
 */
static void sccroll_pcodes(const char* restrict name, int code, int exp, int res) __attribute__((nonnull(1)));

/**
 * @typedef SccrollStrDiff
 * @since 0.1.0
 * @brief Structure contenant les informations minimales pour
 * l'affichage de différences entre deux chaînes d'un test.
 */
typedef struct SccrollStrDiff {
    const char* expected; /**< La chaîne attendue. */
    const char* result;   /**< La chaîne obtenue. */
    const char* name;     /**< Le nom du test. */
    const char* desc;     /**< La description des chaînes. */
} SccrollStrDiff;

/**
 * @since 0.1.0
 * @brief Affiche les différences entre deux chaînes au niveau des
 * lignes.
 * @param infos La structure SccrollStrDiff contenant les chaînes
 * SccrollStrDiff::expected et SccrollStrDiff::result différentes.
 */
static void sccroll_pdiff(const SccrollStrDiff* restrict infos) __attribute__((nonnull));
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

    node->car = sccroll_prepare(expected);
    node->nth = !tests ? 1 : sccroll_nth(tests) + 1;
    node->cdr = tests;
    tests = node;
}

static SccrollEffects* sccroll_prepare(const SccrollEffects* restrict effects)
{
    SccrollEffects* prepared = sccroll_dup(effects);
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) {
        if (!prepared->std[i]) prepared->std[i] = strdup("");
        else {
            if (sccroll_hasFlags(prepared->flags, EXPATH))
                prepared->std[i] = sccroll_content(prepared->std[i], prepared->name);
            if (!sccroll_hasFlags(prepared->flags, NOSTRP))
                prepared->std[i] = sccroll_strip(prepared->std[i]);
        }
    }

    for (int i = 0; i < SCCMAX && prepared->files[i].path; ++i) {
        if (!prepared->files[i].content) prepared->files[i].content = strdup("");
        else if (sccroll_hasFlags(prepared->flags, EXPATH))
            prepared->files[i].content = sccroll_content(prepared->files[i].content, prepared->name);
        else
            sccroll_files(prepared);
    }

    return prepared;
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

static char* sccroll_content(const char* restrict path, const char* restrict name)
{
    char buffer[SCCMAX+1] = { 0 };
    char* content = strdup(path);
    if (strlen(content) > 1 && (bool)strcmp(".", dirname(content))) {
        sccroll_fread(content, buffer, name);
        free(content);
        content = strdup(buffer);
    }
    return content;
}

static void sccroll_files(SccrollEffects* restrict result)
{
    int i;
    char buffer[SCCMAX+1] = { 0 };
    for (i = 0; i < SCCMAX && result->files[i].path; ++i, memset(buffer, 0, strlen(buffer))) {
        sccroll_fread(result->files[i].path, buffer, result->name);
        result->files[i].content = strdup(buffer);
    }
}

static void sccroll_fread(const char* restrict path, char buffer[SCCMAX+1], const char* restrict name)
{
    FILE* file = fopen(path, "r");
    sccroll_err(!file, path, name);
    sccroll_err(!fread(buffer, sizeof(char), SCCMAX, file) && ferror(file), path, name);
    fclose(file);
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
    const SccrollEffects* result   = sccroll_exe(sccroll_dup(expected));
    int failed = sccroll_diff(expected, result);
    if (failed) fprintf(stderr, BASEFMT "\n", RED, "FAIL", expected->name);
    sccroll_free(expected);
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

static const SccrollEffects* sccroll_exe(SccrollEffects* restrict result)
{
    bool dofork              = !sccroll_hasFlags(result->flags, NOFORK);
    int status               = 0;
    char buffer[SCCMAX]      = { 0 };
    int origstd[SCCMAXSTD]   = { 0 };
    int pipefd[SCCMAXSTD][2] = { 0 };
    for (int i = 0; i < SCCMAXSTD; ++i)
        sccroll_pipes(PIPEOPEN, result->name, pipefd[i]);

    pid_t pid = dofork ? fork() : 0;
    sccroll_err(pid < 0, "fork", result->name);
    if (pid == 0) {
        for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) {
            if (!dofork) sccroll_err((origstd[i] = dup(i)) < 0, "dup save of standard", result->name);
            sccroll_pipes(PIPEDUP, result->name, pipefd[i], i);
        }

        errno = 0;
        result->wrapper();
        sprintf(buffer, "%i", errno);
        sccroll_pipes(PIPEWRTE, result->name, pipefd[0], buffer);

        for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
            sccroll_err(dup2(origstd[i], i) < 0, "original std fd restoration", result->name);
        if (dofork) exit(EXIT_SUCCESS);
    }

    if (dofork) wait(&status);
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

    result->codes[SCCSTATUS] = status;
    result->codes[SCCSIGNAL] = status;
    if (!sccroll_hasFlags(result->flags, NOFORK)) {
        result->codes[SCCSTATUS] = WEXITSTATUS(status);
        result->codes[SCCSIGNAL] = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    }
}

static void sccroll_std(SccrollEffects* restrict result, int pipefd[SCCMAXSTD][2])
{
    char buffer[SCCMAX] = { 0 };
    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i) {
        sccroll_pipes(PIPEREAD, result->name, pipefd[i], buffer);
        result->std[i] = sccroll_hasFlags(result->flags, NOSTRP) ? strdup(buffer) : sccroll_strip(buffer);
        memset(buffer, 0, strlen(buffer));
    }
}

static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    bool diff            = false;
    SccrollStrDiff infos = { .name = expected->name };

    for (int i = SCCERRNUM; i < SCCMAXSIG; ++i)
        if (expected->codes[i] != result->codes[i]) {
            diff = true;
            if(!sccroll_hasFlags(expected->flags, NODIFF))
                sccroll_pcodes(expected->name, i, expected->codes[i], result->codes[i]);
        }

    for (int i = STDOUT_FILENO; i < SCCMAXSTD; ++i)
        if (strcmp(expected->std[i], result->std[i])) {
            diff = true;
            if (!sccroll_hasFlags(expected->flags, NODIFF)) {
                infos.expected = expected->std[i];
                infos.result = result->std[i];
                infos.desc = i == STDOUT_FILENO ? "stdout" : "stderr";
                sccroll_pdiff(&infos);
            }
        }

    for (int i = 0; i < SCCMAX && (bool)expected->files[i].path; ++i)
        if (strcmp(expected->files[i].content, result->files[i].content)) {
            diff = true;
            if (!sccroll_hasFlags(expected->flags, NODIFF)) {
                infos.expected = expected->files[i].content;
                infos.result = result->files[i].content;
                infos.desc = expected->files[i].path;
                sccroll_pdiff(&infos);
            }
        }
    return diff;
}

static void sccroll_pcodes(const char* restrict name, int code, int exp, int res)
{
    char expdesc[MAXLINE] = { 0 };
    char resdesc[MAXLINE] = { 0 };
    char* desc = NULL;

    switch(code)
    {
    case SCCERRNUM:
        desc = "errno";
        sprintf(expdesc, "%s", strerrorname_np(exp));
        sprintf(resdesc, "%s", strerrorname_np(res));
        break;
    case SCCSIGNAL:
        desc = "signal";
        exp
            ? sprintf(expdesc, "SIG%s", sigabbrev_np(exp))
            : sprintf(expdesc, "no signal");
        res
            ? sprintf(resdesc, "SIG%s", sigabbrev_np(res))
            : sprintf(resdesc, "no signal");
        break;
    case SCCSTATUS:
        desc = "status";
        sprintf(expdesc, exp ? "error" : "no error");
        sprintf(resdesc, res ? "error" : "no error");
        break;
    default: break;
    }
    fprintf(stderr, CODEFMT, CYAN, "DIFF", name, desc, exp, expdesc, res, resdesc);
}

static void sccroll_pdiff(const SccrollStrDiff* restrict infos)
{
    size_t expc, resc;
    char *expz = NULL, *resz = NULL;
    char *expn = NULL, *resn = NULL;

    sccroll_err(argz_create_sep(infos->expected, '\n', &expz, &expc), infos->desc, infos->name);
    sccroll_err(argz_create_sep(infos->result, '\n', &resz, &resc), infos->desc, infos->name);

    fprintf(stderr, DIFFFMT, CYAN, "DIFF", infos->name, infos->desc);
    for (
        expn = argz_next(expz, expc, expn), resn=argz_next(resz, resc, resn);
        expn || resn;
        expn = argz_next(expz, expc, expn), resn=argz_next(resz, resc, resn)
        )
        if (((bool)expn ^ (bool)resn) || (expn && strcmp(expn, resn)))
        {
            if (expn) fprintf(stderr, "- " COLSTRFMT "\n", RED, expn);
            if (resn) fprintf(stderr, "+ " COLSTRFMT "\n", GREEN, resn);
        }

    free(expz);
    free(resz);
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
