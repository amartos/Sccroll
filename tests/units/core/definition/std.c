/**
 * @file        std.c
 * @version     0.1.0
 * @brief       Core module unit tests for standard IO.
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

// Strings used in the tests
#define teststr "what is actually printed"
#define errstr "what is wrongly expected"

// Print on a given standard output, or fetch from stdin and check.
void test_std(FILE* stream, const char* restrict string)
{
    char buffer[BUFSIZ] = { 0 };
    if (stream != stdin) fprintf(stream, "%s\n", string);
    else {
        if (fread(buffer, sizeof(char), BUFSIZ, stream) < strlen(string) && ferror(stream))
            err(EXIT_FAILURE, "could not read stdin");
        assert(!strcmp(string, buffer));
    }
}

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

void test_stdin(void)  { test_std(stdin,  teststr); }
void test_stdout(void) { test_std(stdout, teststr); }
void test_stderr(void) { test_std(stderr, teststr); }
void test_integration(void)
{
    char buffer[BUFSIZ] = { 0 };
    fprintf(stdout, "%s\n", teststr);
    fprintf(stderr, "%s\n", teststr);
    fread(buffer, sizeof(char), BUFSIZ, stdin);
    assert(!strcmp(teststr, buffer));
}

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

// macro facilitating the tests registration
#define test_run(io)                            \
    test.wrapper = test_ ## io;                 \
    test.name = "test " #io " success";         \
    test.std[fileno(io)].content.blob = teststr;\
    sccroll_register(&test);                    \
    test.name = "test " #io " fail";            \
    test.std[fileno(io)].content.blob = errstr; \
    sccroll_register(&test);                    \
    test = zero

int main(void)
{
    const SccrollEffects zero = { 0 };
    SccrollEffects test = { 0 };

    // individually
    test_run(stdin);
    test_run(stdout);
    test_run(stderr);

    // multiple IO
    test.wrapper = test_integration;

    test.name = "test all success";
    test.std[STDIN_FILENO].content.blob = teststr;
    test.std[STDOUT_FILENO].content.blob = teststr;
    test.std[STDERR_FILENO].content.blob = teststr;
    sccroll_register(&test);

    test.name = "test all fail (stdin)";
    test.std[STDIN_FILENO].content.blob = errstr;
    test.std[STDOUT_FILENO].content.blob = teststr;
    test.std[STDERR_FILENO].content.blob = teststr;
    sccroll_register(&test);

    test.name = "test all fail (stdout)";
    test.std[STDIN_FILENO].content.blob = teststr;
    test.std[STDOUT_FILENO].content.blob = errstr;
    test.std[STDERR_FILENO].content.blob = teststr;
    sccroll_register(&test);

    test.name = "test all fail (stderr)";
    test.std[STDIN_FILENO].content.blob = teststr;
    test.std[STDOUT_FILENO].content.blob = teststr;
    test.std[STDERR_FILENO].content.blob = errstr;
    sccroll_register(&test);

    test.name = "test all fail (stdin + stdout)";
    test.std[STDIN_FILENO].content.blob = errstr;
    test.std[STDOUT_FILENO].content.blob = errstr;
    test.std[STDERR_FILENO].content.blob = teststr;
    sccroll_register(&test);

    test.name = "test all fail (stdin + stderr)";
    test.std[STDIN_FILENO].content.blob = errstr;
    test.std[STDOUT_FILENO].content.blob = teststr;
    test.std[STDERR_FILENO].content.blob = errstr;
    sccroll_register(&test);

    test.name = "test all fail (stdout + stderr)";
    test.std[STDIN_FILENO].content.blob = teststr;
    test.std[STDOUT_FILENO].content.blob = errstr;
    test.std[STDERR_FILENO].content.blob = errstr;
    sccroll_register(&test);

    test.name = "test all fail";
    test.std[STDIN_FILENO].content.blob = errstr;
    test.std[STDOUT_FILENO].content.blob = errstr;
    test.std[STDERR_FILENO].content.blob = errstr;
    sccroll_register(&test);

    assert(sccroll_run() == 10);

    return EXIT_SUCCESS;
}
