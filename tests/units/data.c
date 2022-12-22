/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Test unitaire de génération de données.
 * @date        2022
 * @author      Alexandre Martos
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

// clang-format off

/******************************************************************************
 * Préparation des tests.
 ******************************************************************************/
// clang-format on

// Variable utilisée pour provoquer des erreurs des mocks.
static unsigned errnum = SCCENONE;

// Prévu par l'API des mocks.
bool sccroll_mockTrigger(SccrollMockFlags mock)
{
    return sccroll_hasFlags(errnum, mock);
}

// clang-format off

/******************************************************************************
 * Exécution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    // On s'assure que sccroll_monkey fonctionne. On ne teste pas le
    // côté aléatoire, car la fonction utilisée (arc4random_buf) est
    // fournie par la librairie (et on présuppose qu'elle est déjà
    // testée).
    int zero = 0;
    sccroll_monkey(&zero, sizeof(int));

    // sccroll_monkey() remplit de manière aléatoire. Il y a donc une
    // (infime) chance que l'assertion échoue, et il n'est pas
    // possible de tester le nombre exact (seulement qu'il n'est plus
    // identique au précédent).
    assert(zero && "If this fails, retry at least once");

    int* data = sccroll_rndalloc(1, sizeof(int));
    assert(data);
    // Même remarque que pour le test de sccroll_monkey.
    assert(*data && "If this fails, retry at least once");
    free(data);
    // sccroll_rndalloc ne gère pas les erreurs.
    errnum = SCCEMALLOC;
    data = sccroll_rndalloc(1, sizeof(int));
    assert(data == NULL);

    // gcov uses malloc during exit.
    // TODO: fix malloc error triggering for exits.
    errnum = SCCENONE;
    return EXIT_SUCCESS;
}
