/**
 * @file        sccroll_mocks_tests.c
 * @version     0.1.0
 * @brief       Test unitaire de définitions de mocks.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -L build/libs -l scroll \
 *     -Wl,--wrap,abort,--wrap,calloc,--wrap,free,\
 *     --wrap,sccroll_run,--wrap,sccroll_before,--wrap,printf \
 *     tests/sccroll_mocks_tests.c -o build/bin/sccroll_mocks_tests
 * @endcode
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

// clang-format off

/******************************************************************************
 * Préparation des tests unitaires.
 ******************************************************************************/
// clang-format on

// Variable utilisée comme drapeau pour certains mocks.
static int dummy_flag = 0;

// Test échouant à coup sûr.
SCCROLL_TEST(test_fail) { assert(false); }

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

// mock de calloc, mais n'interférant pas avec la fonction.
SCCROLL_MOCK(void*, calloc, size_t nmemb, size_t size)
{
    puts("calloc mocked.");
    // on s'assure de pouvoir utiliser les fonctions originales.
    return __real_calloc(nmemb, size);
}

// mock de free n'interférant pas avec la fonction.
SCCROLL_MOCK(void, free, void* ptr)
{
    puts("free mocked");
    __real_free(ptr);
}

// mock de sccroll_run qui annule les effets de la fonction si
// #dummy_flag est défini.
SCCROLL_MOCK(int, sccroll_run, void)
{
    if (dummy_flag) puts("sccroll_run mocked: flag seen.");
    else puts("sccroll_run mocked: nothing executed.");
    return 0;
}

// mock de sccroll_before, mais qui provoque une erreur en cas d'exécution.
SCCROLL_MOCK(void, sccroll_before, void)
{
    assert(false && "sccroll_before mocked, but should not be executed...");
}

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    // calloc est appelée avant la fonction main par SCCROLL_TEST.

    // Les fonctions mockées doivent afficher un message si le mock
    // est réussi. Sinon, sccroll_run() provoquera une erreur en
    // appelant sccroll_before(), et l'assert assure un second niveau
    // de vérification (puisque le seul test enregistré est en échec,
    // et non en réussite comme testé ici).
    assert(!sccroll_run());

    // On s'assure que les fonctions mockées peuvent être appelées
    // avec leur nom original.
    free(strdup("test"));

    // Un changement d'état du drapeau affichera un nouveau message.
    dummy_flag = 1;
    sccroll_run();

    return EXIT_SUCCESS;
}
