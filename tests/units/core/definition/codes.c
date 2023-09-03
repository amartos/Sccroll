/**
 * @file        codes.c
 * @version     0.1.0
 * @brief       Core module unit tests for errno/status/signal codes.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 */

#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

static int code = 0;
static int type = 0;

// Signals tested.
// SIGINT is not tested because some CI/CD workers (eg. Jenkins)
// capture it and set the build as a failure, even if it is a fork
// child that raises it and the parent is ignoring it.
static const int sigs[] = {
    SIGTERM, SIGKILL, SIGABRT, 0,
};

enum {
    MAX = 256, // max value for codes
};

bool issig(void) { return type == SCCSIGNAL; }
void set_code(int i) { code = issig() ? sigs[i] : i; }

// Simulate a unit test raising an error code.
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

void test_nofork(void)
{
    SccrollEffects test_success = {
        .wrapper = test_signals,
        .name = "signal nofork",
        .code ={.type = type, .value = code},
        .flags = NOFORK,
    };
    sccroll_register(&test_success);
    if (sccroll_run()) exit(1);
}

// Test all values for a given type of code.
void test_errstat(int t, const char* name)
{
    int i;
    type = t;
    char buffer[BUFSIZ] = { 0 };
    sprintf(buffer, "%s fail", name);
    SccrollEffects test_success = { .wrapper = test_signals, .name = name, .code.type = t };
    SccrollEffects test_fail = test_success;
    test_fail.name = strdup(buffer);

    for (i = 0, set_code(i); (issig() && code) || (!issig() && i < MAX); set_code(++i))
    {
        test_success.code.value = code;
        test_fail.code.value = !code;
        sccroll_register(&test_success);
        sccroll_register(&test_fail);
        assert(sccroll_run() == 1);
    }
    free((void*)test_fail.name);
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

int main(void)
{
    test_errstat(SCCERRNUM, "test errno");
    test_errstat(SCCSTATUS, "test status");
    test_errstat(SCCSIGNAL, "test signal");

    type = SCCERRNUM;
    int status = sccroll_simplefork("test errno nofork", test_nofork);
    assert(!status);
    type = SCCSTATUS;
    status = sccroll_simplefork("test status nofork", test_nofork);
    assert(!status);
    type = SCCSIGNAL;
    status = sccroll_simplefork("test signal nofork", test_nofork);
    assert(!status);
    return EXIT_SUCCESS;
}
