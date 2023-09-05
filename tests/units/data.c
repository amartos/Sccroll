/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Data module unit tests.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

// Ensures to use the standard C lib assert function.
#include <assert.h>

#include "sccroll.h"

#include <math.h>

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

// Constants
enum {
    // sccroll_monkey number of iterations
    MAX = 10,
    // max-1 size of sccroll_monkey blobs.
    MAXSIZE = 255,
};

// Accepted margin of difference for random functions.
const float sigma = 0.05f;

// Expected bits ratio for random numbers.
const float expected = 0.50f;

// Data test blob.
static const char* foobar = "foobar";

// Calculate the bits ratio of a blob.
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

void crash_test(void)
{
    void* data;
    void* copy;
    for (int i = 0; i < 2; ++i) {
        data = sccroll_rndalloc(1, sizeof(int));
        assert(data);
        copy = blobdup(data, 1 * sizeof(int));
        assert(copy);
        free(data), data = NULL;
        free(copy), copy = NULL;
    }
}

void tests_datas(void)
{
    Data* data = NULL;
    Data* copy = NULL;
    assert((data = mkdata((void*)foobar, strlen(foobar)+1, 42)));
    assert(!strcmp((const char*)data->blob, foobar));
    assert(data->size == strlen(foobar)+1);
    assert(data->type == 42);
    assert((copy = datadup(data)) != data);
    assert(copy->blob == data->blob);
    assert(copy->type == data->type);
    assert(copy->size == data->size);
    free(data);
    free(copy);

    assert((data = mkdata(NULL, 0, 0)));
    assert(!data->blob);
    assert(!data->size);
    assert(!data->type);
    free(data);
}

// clang-format off

/******************************************************************************
 * Execution des tests.
 ******************************************************************************/
// clang-format on

int main(void)
{
    // Testing sccroll_rndalloc is equivalent to testing
    // sccroll_monkey.

    // Test the bit ratio obtained with ccroll_monkey. There are cases
    // where it *could* be higher than 50% +/- accepted margin, but
    // these are too rare to take account. In case of a fail, repeat
    // the test twice.
    void* data = NULL;
    void* copy = NULL;
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

    // blobdup test
    data = sccroll_rndalloc(MAXSIZE, sizeof(int));
    copy = blobdup(data, MAXSIZE * sizeof(int));
    char* a = (char*) data;
    char* b = (char*) copy;
    for (int i = 0; i < MAXSIZE; ++i, ++a, ++b) {
        assertMsg(a != b, "a and b are the same pointers");
        assertMsg(*a == *b, "different values for index %i: %c,%c", i, *a, *b);
    }
    assert(blobdup(data, 0) == NULL);
    free(data);
    data = NULL;
    assert((data = blobdup(NULL, 10)));
    free(data);

    // Errors handling
    sccroll_mockPredefined(crash_test);

    tests_datas();
    sccroll_mockPredefined(tests_datas);

    return EXIT_SUCCESS;
}
