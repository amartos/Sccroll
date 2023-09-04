/**
 * @file        sccroll.h
 * @version     0.1.0
 * @brief       Sccroll - A Units Tests library for C.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * You can include this header to use all features of the library.
 */

#ifndef SCCROLL_H_
#define SCCROLL_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "sccroll/core.h"
#include "sccroll/helpers.h"
#include "sccroll/lists.h"
#include "sccroll/data.h"

/**
 * The following are optional features. They do not impact the units
 * tests if none of their defined macros/functions are used.
 *
 * You can disable their inclusion by defining the macros:
 * - #SCC_NOASSERT for both (since the mocks module depends on the
 *   assert module; this will change in a future release to be able to
 *   use them independently)
 * - #SCC_NOMOCKS for the mocks module
 */
#ifndef SCC_NOASSERT
#include "sccroll/assert.h"

#ifndef SCC_NOMOCKS
#include "sccroll/mocks.h"

#endif // SCC_NOMOCKS
#endif // SCC_NOASSERT
#endif // SCCROLL_H_
