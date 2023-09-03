/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Source file of the Data module.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * @addtogroup Internals
 * @{
 * @addtogroup DataInternals Internals for data generation and handling
 * @{
 */

#include "sccroll/data.h"

/**
 * @since 0.1.0
 * @brief Initialise a new pseudo-random seed.
 * @see sccroll_monkeys()
 */
static void sccroll_peanuts(void) __attribute__((constructor));

/**
 * @since 0.1.0
 * @brief Fill the given @p blob with @p size bytes of random data.
 *
 * arc4random_buf() is defined in GNU C library at version 2.36,
 * which is very recent at the time this code is written. The BSD
 * library defines it already, but without garantees of support.
 * This function is thus defined as a weak alias until the function is
 * widely supported.
 *
 * @todo: delete the function code once it is widely supported in the
 * C standard library.
 *
 * @alert This function is destructive as it overwrite the data in
 * @p blob.
 * @param blob The blob to fill.
 * @param size The number of bytes to write.
 */
void arc4random_buf(void* blob, size_t size) __attribute__((weak, nonnull(1)));

// clang-format off

/******************************************************************************
 * @} @}
 *
 * Implementation
 ******************************************************************************/
// clang-format on

static void sccroll_peanuts(void) { srandom(time(NULL)); }

// arc4random_buf already fills the slot of sccroll_monkey.
weak_alias(,arc4random_buf, sccroll_monkey);

Data* mkdata(void* blob, size_t size, int type)
{
    Data* new = calloc(1, sizeof(Data));
    if (!new) err(EXIT_FAILURE, "could not create Data for blob");
    new->blob = blob;
    new->size = size;
    new->type = type;
    return new;
}

Data* datadup(const Data* restrict data)
{
    return mkdata(data->blob, data->size, data->type);
}

void arc4random_buf(void* blob, size_t size)
{
    for (size_t b = 0; b < size; ++b, ++blob)
        *((unsigned char*) blob) = (unsigned char)(random() & 0xFF);
}

void* sccroll_rndalloc(size_t nmemb, size_t size)
{
    size_t real_size = nmemb*size;
    void* randomized = malloc(real_size);
    if (!randomized)
        err(EXIT_FAILURE, "could not allocate for random blob");
    sccroll_monkey(randomized, real_size);
    return randomized;
}

void* blobdup(const void* restrict blob, size_t size)
{
    if (!size) return NULL;
    void* copy = calloc(1, size);
    if (!copy) err(EXIT_FAILURE, "could not copy blob");
    if (blob) memcpy(copy, blob, size);
    return copy;
}
