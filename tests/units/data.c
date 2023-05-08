/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Test unitaire de génération de données.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/data.c -o build/objs/tests/units/data.o
 * gcc -L build/libs -lsccroll build/objs/tests/units/data.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c)              \
 *     -o build/bin/tests/data
 * @endcode
 */

// On s'assure d'utiliser celui de la librairie.
#include <assert.h>

#include "sccroll.h"

#include <math.h>

// clang-format off

/******************************************************************************
 * Préparation des tests.
 ******************************************************************************/
// clang-format on

// Constantes des tests.
enum {
    // Nombre de tests de sccroll_monkey
    MAX = 10,
    // Taille max-1 de l'espace mémoire utilisé pour un test de
    // sccroll_monkey (en octets).
    MAXSIZE = 255,
};

// Marge d'erreur acceptée pour les fonctions aléatoires.
const float sigma = 0.05f;

// ratio de bits 0/1 attendu pour un nombre aléatoire.
const float expected = 0.50f;

// Cette fonction calcule le ratio de bits 1 et 0 de l'espace mémoire
// blob de size octets.
__attribute__((nonnull(1)))
static float bitratio(const void* blob, size_t size)
{
    float ones = 0.0f;
    if (!size) return ones;

    for (size_t byte = 0; byte < size; ++byte, ++blob)
        for (unsigned char mask = 1; mask <= 0x80; mask = mask >= 0x80 ? mask + 1 : mask << 1)
            ones += *((unsigned char*) blob) & mask ? 1.0f : 0.0f;

    return ones/(size*8);
}

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    // Tester la fonction sccroll_rndalloc revient à tester également
    // sccroll_monkey.

    // On teste le ratio de bits 0/1 obtenu avec la fonction
    // sccroll_monkey, du fait qu'elle génère des bits
    // aléatoires. Puisque chacun a 50% de chances d'apparaître pour
    // des espace mémoire assez grands, le ratio devrait être proche
    // de 0.5. Les rares cas où le ratio serait +/- élevé sont trop
    // rares pour êtres pris en compte (surtout que deux cas rares sur
    // deux tests d'affilées est exceptionnel, et donc indiquerait
    // plus un bug qu'une malchance).
    void* data = NULL;
    int i;
    size_t size;
    float ratio = 0.0f;
    for (i = 0, size = (random() & MAXSIZE)+1; i < MAX; ++i, size = (random() & MAXSIZE)+1) {
        data = sccroll_rndalloc(1, size);
        assert(data);
        ratio += bitratio(data, size);
        free(data);
    }
    ratio /= MAX;
    assert(fabs(ratio - expected) < sigma);

    // sccroll_rndalloc ne gère pas les erreurs.
    sccroll_mockTrigger(SCCEMALLOC, 0);
    data = sccroll_rndalloc(1, sizeof(int));
    sccroll_mockFlush();
    assert(data == NULL);

    return EXIT_SUCCESS;
}
