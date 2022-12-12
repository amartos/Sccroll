/**
 * @file        SCCROLL_TEST.c
 * @version     0.1.0
 * @brief       Exemples d'utilisation de #SCCROLL_TEST.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 */

#include "sccroll.h"

/******************************************************************************
 * Tests standards
 ******************************************************************************/

SCCROLL_TEST(test_simples)
{
    int a = 1, b = 2, c = 3, d = 3;
    assert(a < b);
    assert(c == d);
    // etc...
}

SCCROLL_TEST(test_output_exit,
    .std = { 0, "stdout!", "stderr..." }, // unused, stdout, stderr
    .codes = { 0, 0, 1 }                  // errno, signal, status
)
{
    fprintf(stdout, "stdout!");
    fprintf(stderr, "stderr...");
    // errno ne pourra être capturé du fait de l'arrêt prématuré du
    // test, d'où le code 0 attendu.
    errno = EFAULT;
    // Le test n'échouera pas ici étant donné les effets attendus.
    exit(1);
}

SCCROLL_TEST(test_output_nostrp,
    .flags = NOSTRP,
    .std = { 0, "stdout!\n", 0 }
)
{
    // Avec NOSTRP, le test échouerait si le saut de ligne n'était
    // pas précisé dans les effets attendus.
    puts("stdout!");
}


SCCROLL_TEST(test_multiples_effets,
    .flags = NODIFF | NOSTRP | NOFORK,
    .codes = { EINVAL, 0, 0 },
    .std = { 0, "stdout...", "stderr!\n" },
    .files = {
        { .path = "my/path/file",       .content = "foo" },
        { .path = "my/other/path/file", .content = "bar" },
    }
)
{
    errno = EINVAL;
    printf("stdout...");
    fprintf(stderr, "stderr!\n");
    // aucune vérification n'est faite ici pour simplification de
    // l'exemple.
    FILE* f = fopen("my/path/file", "w");
    char* str = "foo";
    fwrite(str, sizeof(char), 3, f);
    fclose(f);
    f = fopen("my/other/path/file", "w");
    str = "bar";
    fwrite(str, sizeof(char), 3, f);
    fclose(f);
}

/******************************************************************************
 * Tests de fonctions
 ******************************************************************************/

SCCROLL_TEST(test_abort, .codes = {0, SIGABRT, 0}) { abort(); }

static void my_func(char* str, int n)
{
    sccroll_unused(str);
    sccroll_unused(n);

    errno = EISNAM;
    printf("DONE");
}

SCCROLL_TEST(test_my_func,
    .codes = { EISNAM, 0, 0 },
    .std = {0, "DONE", 0 }
)
{
    my_func("foobar", 35);
}
