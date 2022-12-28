/**
 * @file        options.c
 * @version     0.1.0
 * @brief       Tests unitaires des options de tests.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -options=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/definition/options.c \
 *     -o build/objs/tests/units/core/definition/options.o
 * gcc -L build/libs -lsccroll \
 *     build/objs/tests/units/core/definition/options.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/definition/options
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

// Chaîne comparées par les tests.
#define strpstdout "This text is surrounded with spaces."
#define strpstderr "The left side is trimmed.        But not the center."
#define nostrpstdout " \n\n\t \t  " strpstdout "      \n\n\t"
#define nostrpstdoutnewline nostrpstdout "\n"
#define nostrpstderr "  " strpstderr
#define errstr "difference observed.\nother line\nanother line"

// Variable modifiée si pas de fork.
static int count = 0;

// Un test unitaire factice affichant sur stdout et stderr.
void test_dummy(void)
{
    ++count;
    puts(nostrpstdout);
    fprintf(stderr, "%s", nostrpstderr);
}

// Les effets attendus génériques pour le test unitaire factice.
static SccrollEffects test = {
    .wrapper = test_dummy,
    .std = {
        [STDOUT_FILENO].content = strpstdout,
        [STDERR_FILENO].content = strpstderr,
    },
};

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

// Tests individuels des options.

void test_nostrp(void)
{
    test.name = "test_nostrp";
    sccroll_register(&test);

    SccrollEffects test_opt = test;
    test_opt.flags = NOSTRP;
    test_opt.std[STDOUT_FILENO].content = nostrpstdoutnewline;
    test_opt.std[STDERR_FILENO].content = nostrpstderr;
    sccroll_register(&test_opt);
    assert(!sccroll_run());
}

void test_nofork(void)
{
    test.name = "test_nofork";
    sccroll_register(&test);

    SccrollEffects test_opt = test;
    test_opt.flags = NOFORK;
    // NOFORK ne devrait pas stopper le programme en cas de différence
    // observé/attendu.
    test_opt.std[STDOUT_FILENO].content = errstr;
    sccroll_register(&test_opt);
    assert(sccroll_run() == 1);
    assert(count == 1);
}

void test_nodiff(void)
{
    SccrollEffects test_opt = test;
    test_opt.name = "test_nodiff";
    test_opt.std[STDOUT_FILENO].content = errstr;
    test_opt.std[STDERR_FILENO].content = strpstdout;
    sccroll_register(&test_opt);
    test_opt.flags = NODIFF;
    sccroll_register(&test_opt);
    assert(sccroll_run() == 2);
}

// Test intégratif des options.

void test_integration(void)
{
    SccrollEffects test_opt = test;
    test_opt.name = "test_all";
    test_opt.flags = ~0;

    test_opt.std[STDOUT_FILENO].content = nostrpstdoutnewline;
    test_opt.std[STDERR_FILENO].content = errstr;

    sccroll_register(&test_opt);
    assert(sccroll_run() == 1);
}

// exécution des tests.

int main(void)
{

    test_nostrp();
    test_nofork();
    test_nodiff();
    test_integration();

    return EXIT_SUCCESS;
}
