/**
 * @file        main.c
 * @version     0.1.0
 * @brief       Core unit tests for user-defined main.
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

// A simple test printing on stderr.
SCCROLL_TEST(test_print) { assert(false && "Test executed."); }

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

// user-defined main.
int main(int argc, const char* argv[])
{
    --argc;
    ++argv;
    // This ensures that it is the correct main used.
    // TODO: check if this is really necessary.
    printf("Main executed with %i arguments: [ ", argc);
    while(*argv) printf("%s ", *argv++);
    puts("]");
    assert(sccroll_run() == 1);
    return EXIT_SUCCESS;
}
