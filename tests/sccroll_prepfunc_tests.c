/**
 * @file        sccroll_prepfunc_tests.c
 * @version     0.1.0
 * @brief       Tests unitaires des fonctions de préparation.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/sccroll_prepfunc_tests.c -L build/libs -l scroll \
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
 * @{
 * @since 0.1.0
 *
 * Variables permettant de tester les fonctions de préparation (en les
 * modifiant dans chacune d'elles).
 */
static int init   = 0;
static int clean  = 0;
static int before = 0;
static int after  = 0;
/** @} */

void sccroll_init(void) {  ++init; }
void sccroll_clean(void) { ++clean; }
void sccroll_before(void) { ++before; }
void sccroll_after(void) { ++after; }

/**
 * @since 0.1.0
 * @brief Test vérifiant les effets des fonctions de préparation.
 */
void test_prepfuncs(void)
{
    assert(init == 1);
    assert(before > 0 && after == before-1);
    assert(clean == 0);
}

int main(void)
{
    // 10 to 100 tests at random
    int repeats = 10 + random() % 90;
    for (int i=0; i<repeats; ++i)
        sccroll_register(test_prepfuncs, "test_prepfuncs");

    assert(!sccroll_run());
    assert(init == 1);
    assert(clean == 1);
    assert(before == after && before == repeats);
    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
