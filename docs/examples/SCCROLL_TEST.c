/**
 * @file        docs/examples/SCCROLL_TEST.c
 * @version     0.1.0
 * @brief       #SCCROLL_TEST usage examples.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include "sccroll.h"

/******************************************************************************
 * Generic tests
 ******************************************************************************/

SCCROLL_TEST(test_simple)
{
    int a = 1, b = 2, c = 3, d = 3;
    assert(a < b);
    assert(c == d);
    // etc...
}

SCCROLL_TEST(test_output_exit,
    .std = { 0, "stdout!", "stderr..." }, // unused, stdout, stderr
    .codes = { 0, 0, 1 }                  // errno, signal, status
)
{
    fprintf(stdout, "stdout!");
    fprintf(stderr, "stderr...");
    // The expected errno is 0 because of the premature exit of the
    // test, which will prevent errno capture. The test will thus
    // **not** fail, even if it had to. Ensure you have correct
    // type and expected value for the error codes.
    errno = EFAULT;
    exit(1);
}

SCCROLL_TEST(test_output_nostrp,
    .flags = NOSTRP,
    .std = { 0, "stdout!\n", 0 }
)
{
    // With the NOSTRP option, the test would fail without the newline
    // character in the expected string.
    puts("stdout!");
}


SCCROLL_TEST(test_multiple_effects,
    .flags = NODIFF | NOSTRP | NOFORK,
    .codes = { EINVAL, 0, 0 },
    .std = { 0, "stdout...", "stderr!\n" },
    .files = {
        { .path = "my/path/file",       .content = "foo" },
        { .path = "my/other/path/file", .content = "bar" },
    }
)
{
    errno = EINVAL;
    printf("stdout...");
    fprintf(stderr, "stderr!\n");
    // No checking is done about the opened file to simplify the
    // example.
    FILE* f = fopen("my/path/file", "w");
    char* str = "foo";
    fwrite(str, sizeof(char), 3, f);
    fclose(f);
    f = fopen("my/other/path/file", "w");
    str = "bar";
    fwrite(str, sizeof(char), 3, f);
    fclose(f);
}

SCCROLL_TEST(test_abort, .codes = {0, SIGABRT, 0}) { abort(); }

/******************************************************************************
 * Tests functions registration
 ******************************************************************************/

static void my_func(char* str, int n)
{
    sccroll_unused(str);
    sccroll_unused(n);

    errno = EISNAM;
    printf("DONE");
}

SCCROLL_TEST(test_my_func,
    .codes = { EISNAM, 0, 0 },
    .std = {0, "DONE", 0 }
)
{
    my_func("foobar", 35);
}
