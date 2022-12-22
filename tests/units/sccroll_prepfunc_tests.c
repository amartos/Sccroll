/**
 * @file        sccroll_prepfunc_tests.c
 * @version     0.1.0
 * @brief       Tests unitaires des fonctions de préparation.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -L build/libs -l scroll -Wl,--wrap,abort \
 *     tests/sccroll_prepfunc_tests.c -o build/bin/sccroll_prepfunc_tests
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

// Constantes numériques des tests unitaires.
enum {
    MAX_PREP = 100, // Nombre maximum de tests exécutés.
};

// Variables comptant le nombre d'appel des fonctions de préparation
// correspondantes.
static int init   = 0;
static int clean  = 0;
static int before = 0;
static int after  = 0;

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

void sccroll_init(void) {  ++init; }
void sccroll_clean(void) { ++clean; }
void sccroll_before(void) { ++before; }
void sccroll_after(void) { ++after; }

// Test vérifiant les effets des fonctions de préparation au cours de
// l'exécution.
void test_prepfuncs(void)
{
    // sccroll_init() a été exécutée une fois avant tous les tests,
    // mais sccroll_clean() ne le sera qu'après le dernier test.
    assert(init == 1 && clean == init-1);

    // Pour le premier test, sccroll_before() a été exécuté, mais pas
    // encore sccroll_after(). sccroll_before() est donc exécutée une
    // fois de plus que sccroll_after() pour un test en cours.
    assert(before > 0 && after == before-1);
}

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    SccrollEffects test = {
        .name = "test_prepfuncs",
        .wrapper = test_prepfuncs,
    };

    // 10 à MAX_PREP tests (nombre aléatoire).
    int repeats = 10 + random() % MAX_PREP;
    for (int i=0; i<repeats; ++i) sccroll_register(&test);

    // On vérifie que les tests faits en cours d'exécution ont réussi.
    assert(!sccroll_run());

    // sccroll_init() et sccroll_clean() ne doivent être exécutées
    // qu'une seule fois.
    assert(init == clean && init == 1);

    // sccroll_before() et sccroll_after() doivent être exécutées
    // toutes deux une fois par test.
    assert(before == after && before == repeats);

    return EXIT_SUCCESS;
}
