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
#include <math.h>

/**
 * @since 0.1.0
 *
 * Constantes numériques du test unitaire.
 */
enum {
    BLOB_SIZE = 256, /**< Taille de la donnée aléatoire. */
    BAGS_OF_PEANUTS = 5000, /**< Nombre d'itérations des tests de
                             * sccroll_monkey(). */
};

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

/**
 * @since 0.1.0
 * @brief Compte le nombre de bits à 1 d'une donnée.
 *
 * @param blob Un espace mémoire.
 * @param size La taille de l'espace mémoire.
 * @return le nombre de bits à 1.
 */
__attribute__((nonnull(1)))
static unsigned popcount(const void* restrict blob, size_t size)
{
    unsigned count = 0;
    unsigned char* data = calloc(1, size);
    if (!data) err(EXIT_FAILURE, "could not alloc for popcount");
    unsigned char* start_data = data;

    memcpy(data, blob, size);
    for (size_t i = 0; i < size; ++i, ++data) {
        for (int b = 0; b < 8 && *data; ++b) {
            count += *data & 1;
            *data >>= 1;
        }
    }
    free(start_data);

    return count;
}

/**
 * @since 0.1.0
 * @brief Test vérifiant le ratio de bits 1/0 obtenu après appel de
 * sccroll_monkey().
 */
void test_monkeys(void)
{
    // On teste le ratio de bits 0/1 obtenu avec la fonction
    // sccroll_monkey, du fait qu'elle génère des bits
    // aléatoires. Puisque chacun a 50% de chances d'apparaître, le
    // ratio devrait être de 0.5. Les rares cas où le ratio serait +/-
    // élevé sont trop rares pour êtres pris en compte (surtout que
    // deux cas rares sur deux tests d'affilées est exceptionnel, et
    // donc indiquerait plus un bug qu'une malchance).
    void* blob = calloc(1, BLOB_SIZE);
    if (!blob) err(EXIT_FAILURE, "could not alloc for blob");

    unsigned ones = 0;
    float ratio = 0.0f;
    for (int i = 0; i < BAGS_OF_PEANUTS; ++i)
    {
        sccroll_monkey(blob, BLOB_SIZE);
        ones = popcount(blob, BLOB_SIZE);
        ratio += ((float) ones)/(BLOB_SIZE*8.0);
    }
    ratio /= (float)BAGS_OF_PEANUTS;

    fprintf(stderr, "Monkeys ratio is: %f\n", ratio);
    assert(fabsf(ratio - 0.50f) < 0.05f && \
           "Ratio if off by more than 5%. " && \
           "Test twice again to ensure repeatability of the error.");

    free(blob);
}

/**
 * @since 0.1.0
 * @brief Test vérifiant que la fonction sccroll_monkey n'affecte pas
 * un espace mémoire si la taille est à 0.
 */
void test_monkeys_zero_size(void)
{
    const char* text = "Throw peanuts, get monkeys";
    char* blob = strdup(text);

    sccroll_monkey(blob, 0);
    assert(!strcmp(blob, text));
    free(blob);
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

    // test de la randomness de sccroll_monkey.
    test_monkeys();
    test_monkeys_zero_size();

    return EXIT_SUCCESS;
}

/******************************************************************************
 * @} (UnitTests)
 * @} (Sccroll)
 ******************************************************************************/
