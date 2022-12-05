/**
 * @file        common.c
 * @version     0.1.0
 * @brief       Données communes pour tous les tests unitaires de Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -Iinclude \
 * tests/common.c -L build/libs -l scroll \
 * -o build/objs/common.o
 * @encode
 */

#include "sccroll.h"

/**
 * @name CodeCoverage
 * @brief Assure un calcul de couverture comprenant les données en cas
 * d'appel à abort().
 */
extern void __gcov_dump(void);
SCCROLL_MOCK(void, abort, void)
{
    __gcov_dump();
    __real_abort();
}
