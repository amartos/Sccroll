/**
 * @file        core.c
 * @version     0.1.0
 * @brief       Fichier source de la gestions de tests.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup Internals Structures internes de Sccroll
 * @{
 * @addtogroup Core Création, exécution et analyse des tests unitaires
 * @{
 */

#include "sccroll/core.h"

// clang-format off

/******************************************************************************
 * @name Gestion basique de messages avec coloration
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollFonts
 * @since 0.1.0
 * @brief Styles des caractères pour les codes ANSI.
 */
typedef enum SccrollFonts {
    NORMAL     = 0,      /**< Style par défaut du terminal. */
    RESET      = NORMAL, /**< Style par défaut du terminal (alias). */
    BOLD       = 1,      /**< Caractères en gras. */
    DIM        = 2,      /**< Caractères assombris. */
    ITALIC     = 3,      /**< Caractères en italique. */
    UNDERSCORE = 4,      /**< Caractères soulignés. */
}SccrollFonts;

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
 * @def COLSTART
 * @since 0.1.0
 * @brief Les codes de changement de coloration et apparence.
 * @param i Un code SccrollFonts pour l'apparence du texte
 * @param i Un code SccrollColors pour la couleur du texte
 */
#define COLSTART "\e[0;%i;3%im"

/**
 * @def COLEND
 * @since 0.1.0
 * @brief Reset l'apparence du texte affiché au défaut.
 */
#define COLEND "\e[0m"

/**
 * @def COLSTRFMT
 * @since 0.1.0
 * @brief Ajoute les codes ANSI de coloration à une chaîne.
 * @param i Un code SccrollFonts pour l'apparence du texte
 * @param i Un code SccrollColors pour la couleur du texte
 * @param s La chaîne à colorer
 */
#define COLSTRFMT COLSTART "%s" COLEND

/**
 * @def HEXFMT
 * @since 0.1.0
 * @brief Format d'affichage des octets.
 * @param \* Le nombre de chiffres à afficher.
 * @param x La valeur de l'octet.
 */
#define HEXFMT "%0*x"

/**
 * @def COLHEXFMT
 * @since 0.1.0
 * @brief Ajoute les codes ANSI de coloration à une valeur d'octet.
 * @param i Un code SccrollFonts pour l'apparence du texte
 * @param \* Le nombre de chiffres à afficher.
 * @param x La valeur de l'octet.
 */
#define COLHEXFMT COLSTART HEXFMT COLEND

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Gestion d'erreurs
 * @{
 ******************************************************************************/
// clang-format on

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
    expr                                                        \
    ? err(EXIT_FAILURE, "%s failed for %s", op, name)           \
    : 0

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Préparation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute des préparatifs pour la librairie.
 */
static void sccroll_atrun(void) __attribute__((constructor));

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
 * @name Enregistrement des tests
 *
 * La gestion de l'enregistrement des tests passe par une queue
 * (LIFO). Bien que cette méthode suffise, elle peut être modifiée à
 * l'avenir; c'est la raison pour laquelle l'ordre des tests n'est pas
 * garanti.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @var tests
 * @brief Liste des tests à exécuter.
 */
static List* tests = NULL;

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Gestion de structures SccrollEffects et leurs membres
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Génère une copie de @p effects.
 * @attention La structure est copiée, mais pas le contenu des
 * emplacements des pointeurs stockés dans la structure (ce sont les
 * mêmes pointeurs entre l'original et la copie).
 * @attention utilise calloc.
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

/**
 * @since 0.1.0
 * @brief Duplique une structure SccrollBlob.
 * @param dest La structure SccrollBlob de destination.
 * @param src La structure SccrollBlob à dupliquer.
 */
static void sccroll_blobcpy(Data* restrict dest, const Data* restrict src)
    __attribute__((nonnull));

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
 * @brief Récupère le contenu du fichier donné.
 * @attention Utilise malloc.
 * @param file La structure contenant les informations sur le fichier.
 * @param name Le nom du test correspondant au fichier.
 */
static void sccroll_fread(SccrollFile* restrict file, const char* restrict name) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Exécution des tests
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

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Gestion de pipes
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef SccrollPipes
 * @since 0.1.0
 * @brief Constantes internes liées aux pipes.
 */
typedef enum SccrollPipes {
    PIPEREAD = 0, /**< Index du côté lecture d'un pipe. */
    PIPEWRTE = 1, /**< Index du côté écriture d'un pipe. */
    PIPEOPEN,     /**< Drapeau d'ouverture d'un pipe. */
    PIPECLOSE,    /**< Drapeau de fermeture d'un côté d'un pipe. */
    PIPEDUP,      /**< Drapeau de duplication d'un pipe. */
    PIPEMAX,      /**< Index maximal des drapeaux d'opérations de pipe. */
    PIPEERRN = SCCMAXSTD, /**< Index du pipe pour récolter errno. */
    PIPEMAXFD,            /**< Index maximal d'une table de pipes pour un test. */
} SccrollPipes;

/**
 * @var PIPEDESC
 * @since 0.1.0
 * @brief Description d'opération des valeurs SccrollPipes::PIPE*.
 */
const char* const PIPEDESC[PIPEMAX] = {
    "read pipe", "write pipe",
    "open pipe", "close pipe",
    "duplicate pipe"
};

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
 * - #PIPEREAD  pour la lecture et fermeture du côté lecture;
 * - #PIPEDUP   pour dupliquer le côté écriture du pipe sur un
 *              descripteur de fichier.
 * @param name Le nom du test courant.
 * @param pipefd Le pipe du test courant à modifier.
 * @param ... Arguments supplémentaires dépendant de la valeur de
 * @p type:
 * - #PIPEOPEN:  arguments supplémentaires ignorés;
 * - #PIPECLOSE: #PIPEREAD **ou** #PIPEWRTE selon le côté à fermer;
 * - #PIPEWRTE:  un pointeur vers les données à écrire dans
 *               @p pipefd[PIPEWRTE], et la taille en octets des
 *               données.
 * - #PIPEREAD:  un pointeur vers les données à lire de
 *               @p pipefd[PIPEREAD], et la taille maximale en octets
 *               des données.
 * - #PIPEREAD:  une chaîne de #SCCMAX caractères comme destination de
 *               la lecture du pipe;
 * - #PIPEDUP:   le côté du pipe à dupliquer (#PIPEREAD ou #PIPEWRTE),
 *               puis le descripteur de fichier où dupliquer le pipe.
 */
static void sccroll_pipes(SccrollPipes type, const char* restrict name, int pipefd[2], ...) __attribute__((nonnull(2, 3)));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Récolte des effets obtenus
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Lis le code d'erreur recherché obtenu au cours de
 * l'exécution d'un test et le stocke dans SccrollEffects::code.
 * @param result La structure SccrollEffects de destination.
 * @param pipefd Le pipe contenant la valeur de errno obtenu dans le
 * fork du test.
 * @param status Le wstatus obtenu avec la fonction wait().
 */
static void sccroll_codes(SccrollEffects* restrict result, int pipefd[2], int status) __attribute__((nonnull(1,2)));

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
 * @brief Lis le contenu des fichiers de SccollEffects::files::path et
 * stocke les #SCCMAX-1 premiers caractères dans
 * SccrollEffects::files::content.
 * @param result La structure SccrollEffects de destination.
 */
static void sccroll_files(SccrollEffects* restrict result) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 *
 * @name Analyse des effets obtenus et rapports
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum SccrollReport
 * @since 0.1.0
 * @brief Constantes numériques internes liées aux rapports.
 */
typedef enum SccrollReport {
    REPORTTOTAL = 0, /**< Index du nombre total de tests. */
    REPORTFAIL = 1,  /**< Index du nombre de tests échoués. */
    REPORTMAX  = 2,  /**< Index maximal de la table des rapports. */
    MAXLINE = 80,    /**< Longueur maximale des lignes d'un rapport. */
} SccrollReport;

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
 * @var SCCSEP
 * @since 0.1.0
 * @brief Ligne de séparation de texte.
 * @note N'est définie qu'une fois, à l'exécution.
 */
static const char* SCCSEP = NULL;

/**
 * @def REPORTFMT
 * @since 0.1.0
 * @brief Format du rapport final.
 * @param i Un code SccrollColors.
 * @param s Status.
 * @param s Nom du test.
 * @param f Pourcentage de réussite des tests.
 * @param i Nombre de tests réussis.
 * @param i Nombre total de tests.
 */
#define REPORTFMT "\n%s\n\n" BASEFMT ": %.2f%% [%i/%i]\n", SCCSEP, BOLD

/**
 * @def DIFFFMT
 * @since 0.1.0
 * @brief Format de l'affichage des différences attendu/obtenu.
 * @param name Nom du test
 * @param s Description de la différence.
 */
#define DIFFFMT BASEFMT ": %s\n", BOLD, CYAN, "DIFF"

/**
 * @def CODEFMT
 * @since 0.1.0
 * @brief Format d'affichage d'erreurs concernant les codes d'erreur et
 * sortie attendus.
 * @param s Nom du test.
 * @param s Description du test.
 * @param i Code attendu.
 * @param s Description du code attendu.
 * @param i Code obtenu.
 * @param s Description du code obtenu.
 */
#define CODEFMT BASEFMT ": %s: expected %i (%s), got %i (%s)\n", BOLD, CYAN, "DIFF"

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
 * @brief Compare deux SccrollEffects::codes et indique leurs
 * différences.
 *
 * La fonction détermine si les SccrollEffecst::codes de @p expected
 * et @p result diffèrent, et renvoie @c true si c'est le cas. Si
 * l'option #NODIFF est définie pour le test, elle ne fait rien de
 * plus. Dans le cas contraire, elle affiche la différence entre les
 * deux structures.
 *
 * @param expected,result Les structures à comparer.
 * @return true si les SccrollEffects::codes diffèrent.
 */
static bool sccroll_diffCodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Compare deux SccrollEffects::std et indique leurs
 * différences.
 *
 * La fonction détermine si les SccrollEffecst::std de @p expected
 * et @p result diffèrent, et renvoie @c true si c'est le cas. Si
 * l'option #NODIFF est définie pour le test, elle ne fait rien de
 * plus. Dans le cas contraire, elle affiche la différence entre les
 * deux structures.
 *
 * @param expected,result Les structures à comparer.
 * @return true si les SccrollEffects::std diffèrent.
 */
static bool sccroll_diffStd(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Compare deux SccrollEffects::files et indique leurs
 * différences.
 *
 * La fonction détermine si les SccrollEffecst::files de @p expected
 * et @p result diffèrent, et renvoie @c true si c'est le cas. Si
 * l'option #NODIFF est définie pour le test, elle ne fait rien de
 * plus. Dans le cas contraire, elle affiche la différence entre les
 * deux structures.
 *
 * @param expected,result Les structures à comparer.
 * @return true si au moins un des SccrollEffects::files diffère.
 */
static bool sccroll_diffFiles(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Affiche un message d'erreur décrivant la différence entre
 * @p exp et @p res.
 * @param expected Les effets attendus.
 * @param result Les effets obtenus.
 */
static void sccroll_pcodes(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
    __attribute__((nonnull));

/**
 * @typedef SccrollBlobDiff
 * @since 0.1.0
 * @brief Structure contenant les informations minimales pour
 * l'affichage de différences entre deux blobs d'un test.
 */
typedef struct SccrollBlobDiff {
    const Data* expected; /**< Le blob attendu. */
    const Data* result;   /**< Le blob obtenu. */
    const char* name;     /**< Le nom du test. */
    const char* desc;     /**< La description des blobs. */
} SccrollBlobDiff;

/**
 * @since 0.1.0
 * @brief Affiche les différences entre deux chaînes au niveau des
 * lignes.
 * @param infos La structure SccrollBlobDiff contenant les chaînes
 * SccrollBlobDiff::expected et SccrollBlobDiff::result différentes.
 */
static void sccroll_pdiff(const SccrollBlobDiff* restrict infos) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Affiche deux blobs de données différents.
 * @param infos La structure SccrollBlobDiff contenant les blobs
 * SccrollBlobDiff::expected et SccrollBlobDiff::result différents.
 */
static void sccroll_dump(const SccrollBlobDiff* restrict infos) __attribute__((nonnull));

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
 * @name Nettoyage post-tests
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

/**
 * @since 0.1.0
 * @brief Effectue des nettoyages pour la librairie.
 */
static void sccroll_atexit(void) __attribute__((destructor));

// clang-format off

/******************************************************************************
 * @}
 *
 * Implémentation
 *
 * Préparation
 ******************************************************************************/
// clang-format on

static void sccroll_atrun(void)
{
    // Ligne de séparation des rapports.
    char sep[MAXLINE+1] = { 0 };
    memset(sep, '-', MAXLINE);
    SCCSEP = strdup(sep);
}

// Pas de classe de stockage ici car ces fonctions font partie de
// l'API.
weak_alias(, sccroll_void, sccroll_init);
weak_alias(, sccroll_void, sccroll_clean);
weak_alias(, sccroll_void, sccroll_before);
weak_alias(, sccroll_void, sccroll_after);
static void sccroll_void(void) {}

// clang-format off

/******************************************************************************
 * Enregistrement
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
    // +char pour considérer les comparaisons de chaînes de
    // caractères. Puisque size n'est pas modifié, le dernier octet
    // est nul et "caché" à la comparaison.
    file->content.blob = blobdup(
        buffer,
        file->content.size < sizeof(buffer)
        ? file->content.size+sizeof(char)
        : sizeof(buffer)
    );
}

// clang-format off

/******************************************************************************
 * Exécution
 ******************************************************************************/
// clang-format on

// Pas de classe de stockage ici car main fait partie de l'API.
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
 * Récolte
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
    // La chaîne de charactères est identique (non dupliquée) avec
    // expected. Libérer celle de result engendrerait une erreur.
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
 * Analyse et rapports
 ******************************************************************************/
// clang-format on

static bool sccroll_diff(const SccrollEffects* restrict expected, const SccrollEffects* restrict result)
{
    // On ne renvoie pas un OU direct, car on veut *tout* comparer, un
    // OU direct s'arrêtant à la première différence.
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
 * Nettoyage
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
    // Ligne de séparation des rapports.
    free((void*)SCCSEP);
}

/** @} @} **/