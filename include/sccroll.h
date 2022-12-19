/**
 * @file        sccroll.h
 * @version     0.1.0
 * @brief       Ficher en-tête de la librairie Sccroll.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 *
 * Ce fichier est l'entête à intégrer dans les fichiers de
 * définition de tests. La définition des structures et fonctions se
 * trouve dans les fichiers en-tête situés dans `include/sccroll`.
 *
 * @compilation
 * @code{.sh}
 * gcc -xc -Wall -std=gnu99 -I include \
 *     -fpic -shared $(scripts/mocks.awk src) \
 *     src/sccroll/\*.c \
 *     -o build/libs/libsccroll.so
 * @endcode
 */

#ifndef SCCROLL_H_
#define SCCROLL_H_

#include "sccroll/core.h"
#include "sccroll/helpers.h"
#include "sccroll/data.h"
#include "sccroll/assert.h"
#include "sccroll/mocks.h"

#endif // SCCROLL_H_
