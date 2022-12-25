/**
 * @file        main.c
 * @version     0.1.0
 * @brief       Test unitaire de redéfinition du main (avec arguments).
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/execution/main.c -o build/objs/tests/units/core/execution/main.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/core/execution/main.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/execution/main
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

// Test simple, affiche un message sur stdout si le test est exécuté.
SCCROLL_TEST(test_print) { assert(false && "Test executed."); }

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

// Test de redéfinition du main() (version avec arguments).
int main(int argc, const char* argv[])
{
    --argc;
    ++argv;
    // L'affichage du test permet de montrer que c'est bien ce main
    // qui a été exécuté.
    printf("Main executed with %i arguments: [ ", argc);
    while(*argv) printf("%s ", *argv++);
    puts("]");
    assert(sccroll_run() == 1);
    return EXIT_SUCCESS;
}
