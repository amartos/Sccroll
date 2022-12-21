/**
 * @file        sccroll_basics_tests.c
 * @version     0.1.0
 * @brief       Tests unitaires des propriétés de base.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -L build/libs -l scroll -Wl,--wrap,abort \
 *     tests/sccroll_basics_tests.c -o build/bin/sccroll_basics_tests
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparatifs des tests unitaires.
 ******************************************************************************/
// clang-format on

// Pipe destiné à stocker les noms des tests qui modifient cette
// variable (et sont donc exécutés).
static int called[2] = { 0 };

// Compteur incrémenté uniquement dans les tests avec fork, pour
// vérifier que ceux-ci sont bien exécuté dans un fork (ne devrait pas
// avoir d'effet dans le parent).
static int test_fork = 0;

// Constantes numériques des tests.
enum {
    CALLERS = 6, // Nombre de tests appellant is_called.
    FAILERS = 5, // Nombre de tests échouant.
    EMPTIES = 2, // Nombre de tests vide.
    REPEATS = 4, // Nombre de répétitions du test inscrit plusieurs fois.
    TOTAL   = 8, // Nombre de tests total.
};

// Inscrit le nom donné dans le pipe called et incrémente test_fork.
__attribute__((nonnull))
void is_called(const char* restrict name)
{
    if (!name[0]) err(EXIT_FAILURE, "test name must not be empty");

    char buffer[BUFSIZ] = { 0 };
    assert(strlen(name) < BUFSIZ && "name too long");
    sprintf(buffer, "%s ", name);
    if (write(called[1], buffer, strlen(buffer)) < 0)
        err(EXIT_FAILURE, "could not write test name in test pipe");
    if (close(called[1]) < 0)
        err(EXIT_FAILURE, "could not close write end of pipe");
    ++test_fork;
}

// Affiche un message d'erreur sur stdout et quitte en retournant
// code. Cette macro sert quand stderr est redirigé avec dup*.
#define errout(code, fmt, ...)                      \
    {                                               \
        printf("%s:", strerror(errno));             \
        printf(fmt, ##__VA_ARGS__);                 \
        printf("\n");                               \
        exit(code);                                 \
    }

// Compte le nombre d'occurences d'une chaîne de caractères dans une
// autre.
int strcount(const char* haystack, const char* restrict needle)
{
    int count;
    for (count = 0; (haystack = strstr(haystack, needle)); ++count, ++haystack);
    return count;
}

// clang-format off

/******************************************************************************
 * Tests dummies
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(test_register_success) { is_called(__func__); }

SCCROLL_TEST(test_register_fail)
{
    is_called(__func__);
    assert(false);
}

void test_register_manually_fail(void)
{
    is_called(__func__);
    assert(false);
}

SCCROLL_TEST(test_register_empty) {}

void test_register_manually_empty(void) {}

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

// Test de redéfinition du main (même arguments que sccroll_run()).
int main(void)
{
    int i = 0;
    int failed = 0;
    int std = STDERR_FILENO;
    int pipefd[2] = { 0 };
    char buffer[BUFSIZ] = { 0 };
    char output[BUFSIZ] = { 0 };
    char names[BUFSIZ] = { 0 };
    char* strptr;
    const char* const repeated = "test_register_fail";
    // La liste des noms des (fonctions de) tests appelant is_called.
    const char* const expected[BUFSIZ] = {
        repeated,
        "test_register_success",
        "test_register_manually_fail",
        NULL
    };
    const char* const failers[BUFSIZ] = {
        "test_register_fail",
        "Custom name for manual register (fail)",
        "Alias for register_fail",
        "Another alias for the register_fail function",
        NULL,
    };

    // Préparation des tests.

    if (pipe(called) < 0)
        err(EXIT_FAILURE, "could not open pipe for exec names");

    // Enregistrement manuel.

    SccrollEffects manual = {
        .wrapper = test_register_manually_fail,
        .name = failers[1],
    };
    sccroll_register(&manual);

    manual.wrapper = test_register_manually_empty;
    manual.name = "My name of test (empty)";
    sccroll_register(&manual);

    // Test pour vérifier qu'un test définit avec SCCROLL_TEST peut
    // être enregistré plusieurs fois avec le même nom ou des noms
    // identiques ou différents.
    manual.wrapper = test_register_fail;
    manual.name = failers[0]; // même nom que l'original
    sccroll_register(&manual);
    manual.name = failers[2];
    sccroll_register(&manual);
    manual.name = failers[3];
    sccroll_register(&manual);

    // Exécution des tests.

    // On entoure les tests de lignes indicatrices (une à la fin
    // aussi) pour s'assurer que les tests ne sont pas exécutés avant
    // l'appel de sccroll_run; si les tests le sont, ceux qui échouent
    // afficheront leur message d'erreur avant cette ligne.
    fprintf(stderr, ">>>>>>> First line of tests.\n");

    // On capture le rapport affiché par l'exécution des tests pour
    // analyse ultérieure. On sauvegarde d'abord stderr pour le
    // rétablir ensuite.
    if ((std = dup(STDERR_FILENO)) < 0) err(EXIT_FAILURE, "could not save stderr fd");
    if (pipe(pipefd) < 0) err(EXIT_FAILURE, "could not pipe tests output");
    if (dup2(pipefd[1], STDERR_FILENO) < 0) err(EXIT_FAILURE, "could not dup2 tests output");

    failed = sccroll_run(); // Exécution des tests

    // On rétablit stderr. Si une erreur se produit ici, on ne saura
    // pas pourquoi si on tente d'afficher sur stderr (qui est dirigé
    // sur le pipe). Donc on affiche sur stdout.
    if (dup2(std, STDERR_FILENO) < 0) errout(EXIT_FAILURE, "could not dup2 back to stderr");
    if (close(std) < 0) err(EXIT_FAILURE, "could not close saved stderr descriptor");
    if (close(pipefd[1])) err(EXIT_FAILURE, "could not close pipe write of tests output");
    if (read(pipefd[0], output, BUFSIZ) < 0) err(EXIT_FAILURE, "Could not read pipe of tests output");
    if (close(pipefd[0])) err(EXIT_FAILURE, "could not close pipe read of tests output");

    fprintf(stderr, ">>>>>>> Last line of tests.\n");

    // Vérification de l'exécution des tests non vides.

    // Les fonctions exécutées doivent avoir inscrit leur nom dans
    // called.
    if (close(called[1]) < 0) err(EXIT_FAILURE, "could not close write end of test pipe");
    if (read(called[0], names, BUFSIZ) < 0) err(EXIT_FAILURE, "could not read names of tests called");
    if (close(called[0]) < 0) err(EXIT_FAILURE, "could not close read end of test pipe");

    // On s'assure que toutes les fonctions enregistrées et modifiant
    // called ont été appelées au moins une fois.
    // L'ordre des tests n'étant pas garanti, on ne peut chercher dans
    // l'ordre.
    for (i = 0; i<BUFSIZ && expected[i]; ++i) assert(strstr(names, expected[i]));

    // On vérifie que c'est bien la fonction enregistrée plusieurs
    // fois qui est appelée plusieurs fois.
    assert(strcount(names, repeated) == REPEATS);

    // On s'assure du nombre de fonctions appelées ayant modifié
    // called, et que les fonctions vides ne sont pas du lot. On
    // compte le nombre d'espaces car les noms des fonctions ayant
    // écrit dans called sont séparés par un espace; leur nombre
    // équivaut au nombre d'appels (la dernière fonction ajoutant
    // aussi l'espace).
    assert(strcount(names, " ") == CALLERS);
    assert(!strstr(names, "empty"));

    // Vérification du rapport et de l'exécution des tests vides.

    // Seuls 2 tests sont supposés échouer.
    assert(failed == FAILERS);

    // On ne peut pas prévoir la structure exacte du rapport (les
    // tests ne sont pas ordonnés). On ne peut donc que rechercher les
    // noms de tests prédictibles ou des mots clés.

    // On vérifie que tous les tests en échec ont bien émis un message d'erreur.
    assert(strcount(output, "FAIL") == FAILERS+1);

    // On vérifie quels tests apparaissent dans les rapports.
    // Multiplication *3 pour tenir compte des messages de [diff].
    memset(buffer, 0, strlen(buffer));
    sprintf(buffer, "] %s", failers[0]);
    assert(strcount(output, buffer) == 3*2);
    for (i = 1; i < BUFSIZ && failers[i]; ++i) {
        memset(buffer, 0, strlen(buffer));
        sprintf(buffer, "] %s", failers[i]);
        assert(strcount(output, buffer) == 3);
    }

     // on vérifie que le rapport final est bien émis et contient les
     // infos attendues.
    char sep[90] = { 0 };
    memset(sep, '-', 80);
    strptr = strstr(output, sep);
    assert(strptr);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer,
            "%s\n\n[ \e[0;1;31mFAIL\e[0m ] success rate: %.2f%% [%i/%i]\n",
            sep,
            100.0*((TOTAL-FAILERS)/(float) TOTAL),
            TOTAL-FAILERS, TOTAL);
    assert(strstr(strptr, buffer));

    // Pour vérifier que le rapport est bien sur la dernière ligne, on
    // vérifie le nombre de saut de lignes restant.
    assert(strlen(strptr+strlen(buffer)) == 0);

    // Vérification des effets secondaires sur le parent.

    // Les tests étant exécutés dans un fork, le test_fork du parent
    // ne devrait pas être affecté.
    assert(!test_fork);

    return EXIT_SUCCESS;
}