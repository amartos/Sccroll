/**
 * @file        sccroll_main_tests.c
 * @version     0.1.0
 * @brief       Test unitaire de redéfinition du main avec arguments.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/sccroll_main_tests.c -L build/libs -l scroll \
 * -o build/bin/sccroll_basics_tests
 * @encode
 *
 * @addtogroup Sccroll
 * @{
 * @addtogroup UnitTests
 * @{
 */

#include "sccroll.h"

/**
 * @since 0.1.0
 * @brief Affiche un message sur stdout si le test est exécuté.
 */
SCCROLL_TEST(test_print) { assert(false && "Test executed."); }

/**
 * @since 0.1.0
 * @brief Test de redéfinition du main() (version avec arguments).
 *
 * @param argc Le nombre d'arguments de la ligne de commande.
 * @param argv Les arguments de la ligne de commande.
 * @return EXIT_SUCESS.
 */
int main(int argc, const char* argv[])
{
    --argc;
    ++argv;
    printf("Main executed with %i arguments: [ ", argc);
    while(*argv) printf("%s ", *argv++);
    puts("]");
    assert(sccroll_run() == 1);
    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
