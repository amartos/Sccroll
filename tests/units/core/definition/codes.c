/**
 * @file        codes.c
 * @version     0.1.0
 * @brief       Tests unitaires d'analyse des codes d'erreur.
 * @date        2022
 * @author      Alexandre Martos
 * @copyright   MIT License
 * @compilation
 * @see sccroll.h pour la compilation de libsccroll.so
 * @code{.c}
 * gcc -xc -Wall -Wextra -std=gnu99 -Iincludes -fpic -c \
 *     tests/units/core/definition/codes.c \
 *     -o build/objs/tests/units/core/definition/codes.o
 * gcc -L build/libs -lsccroll \
 *     build/objs/tests/units/core/definition/codes.o \
 *     $(scripts/mocks.awk src/sccroll/mocks.c) \
 *     -o build/bin/tests/core/definition/codes
 * @endcode
 */

// On s'assure d'utiliser l'assert original et non pas celui défini
// par la librairie.
#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Préparatifs des tests.
 ******************************************************************************/
// clang-format on

// Variable commune des tests.
static int code = 0;
static int type = 0;

// Signaux d'arrêt testés.
static const int sigs[] = {
    SIGTERM, SIGINT, SIGKILL, SIGABRT, 0,
};

// Constantes numériques des tests.
enum {
    MAX = 256,
};

// Indique si type contient SIG.
bool issig(void) { return type == SCCSIGNAL; }

// Si type contient sig, renvoie sigs[i], sinon renvoie i.
void set_code(int i) { code = issig() ? sigs[i] : i; }

// Fonction simulant un test unitaire.
void test_signals(void)
{
    switch(type)
    {
    case SCCERRNUM: errno = code; break;
    case SCCSIGNAL: kill(getpid(), code); break;
    case SCCSTATUS: exit(code); break;
    default: break;
    }
}


// Teste l'ensemble des valeurs définies pour le code de type t
// correspondant à index dans SccrollEffects::codes.
void test_errstat(int t, const char* name)
{
    int i;
    type = t;
    SccrollEffects test_success = { .wrapper = test_signals, .name = name, .code.type = t };
    SccrollEffects test_fail = test_success;

    for (i = 0, set_code(i); (issig() && code) || (!issig() && i < MAX); set_code(++i))
    {
        test_success.code.value = code;
        test_fail.code.value = !code;
        sccroll_register(&test_success);
        sccroll_register(&test_fail);
        assert(sccroll_run() == 1);
    }
}

// clang-format off

/******************************************************************************
 * Exécution des tests
 ******************************************************************************/
// clang-format on

int main(void)
{
    test_errstat(SCCERRNUM, "test errno");
    test_errstat(SCCSTATUS, "test status");
    test_errstat(SCCSIGNAL, "test signal");

    return EXIT_SUCCESS;
}
