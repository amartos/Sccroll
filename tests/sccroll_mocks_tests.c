/**
 * @file        sccroll_mocks_tests.c
 * @version     0.1.0
 * @brief       Test unitaire de définitions de mocks.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/sccroll_mocks_tests.c -L build/libs -l scroll \
 * -Wl,--wrapper,calloc,--wrapper,free,--wrapper,sccroll_run,\
 * --wrapper,sccroll_before,--wrapper,printf \
 * -o build/bin/sccroll_basics_tests
 * @encode
 *
 * @addtogroup Sccroll
 * @{
 * @addtogroup UnitTests
 * @{
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

/**
 * @var dummy_flag
 * @since 0.1.0
 *
 * Variable utilisée comme drapeau pour certains mocks.
 */
static int dummy_flag = 0;

/**
 * @since 0.1.0
 * @brief Test échouant à coup sûr.
 */
SCCROLL_TEST(test_fail) { assert(false); }

/**
 * @since 0.1.0
 * @brief mock de calloc, mais n'interférant pas avec la fonction.
 */
SCCROLL_MOCK(void*, calloc, size_t nmemb, size_t size)
{
    puts("calloc mocked.");
    // on s'assure de pouvoir utiliser les fonctions originales.
    return __real_calloc(nmemb, size);
}

/**
 * @since 0.1.0
 * @brief mock de free n'interférant pas avec la fonction.
 */
SCCROLL_MOCK(void, free, void* ptr)
{
    puts("free mocked");
    __real_free(ptr);
}

/**
 * @since 0.1.0
 * @brief mock de sccroll_run, annule les effets de la fonction.
 */
SCCROLL_MOCK(int, sccroll_run, void)
{
    if (dummy_flag) puts("sccroll_run mocked: flag seen.");
    else puts("sccroll_run mocked: nothing executed.");
    return 0;
}

/**
 * @since 0.1.0
 * @brief mock de sccroll_before, mais qui provoque une erreur en cas
 * d'exécution.
 */
SCCROLL_MOCK(void, sccroll_before, void)
{
    assert(false && "sccroll_before mocked, but should not be executed...");
}

/**
 * @since 0.1.0
 * @brief mock de printf malformé, qui ne devrait pas être possible
 * d'utiliser.
 */
SCCROLL_MOCK(int, printf, int a, int b)
{
    assert(false && "broken printf mock successfully replacing the original...");
    // to remove warnings about unused parameters.
    a = b;
    b = a;
    return b;
}

int main(void)
{
    // calloc est appelée avant la fonction main par SCCROLL_TEST.

    // Ces fonctions doivent afficher un message si le mock est
    // réussi. Sinon, sccroll_run provoquera une erreur en appelant
    // sccroll_before, et l'assert assure un second niveau de
    // vérification (puisque le seul test enregistré est en échec, et
    // non en réussite comme testé ici).
    assert(!sccroll_run());

    // On s'assure que les fonctions de la librairie mockées peuvent
    // être appelées avec leur nom original.
    free(strdup("test"));

    // Un changement d'état du drapeau affichera un nouveau message.
    dummy_flag = 1;
    sccroll_run();

    // Si le mock malformé de printf est exécuté, cet appel provoquera
    // une erreur.
    printf("printf not mocked: OK\n");

    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
