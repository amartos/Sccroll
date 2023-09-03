/**
 * @file        data.h
 * @version     0.1.0
 * @brief       Generate and handle typeless data.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 *
 * @addtogroup API
 * @{
 * @addtogroup DataAPI Typeless data generation and handling
 * @{
 */

#ifndef SCCROLL_DATA_H_
#define SCCROLL_DATA_H_

#include "sccroll/helpers.h"

#ifdef _SCCUNITTESTS
// Allows easier errors handling tests of the library.
// TODO: remove this dependency, implying an architecture redesign.
#include "sccroll/mocks.h"
#endif

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// clang-format off

/*******************************************************************************
 * @name Data handling
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @struct Data
 * @since 0.1.0
 * @brief Typelessly handle any data.
 * @note Data::type must be user-defined.
 */
typedef struct Data {
    void* blob;  /**< The data pointer. */
    size_t size; /**< The blob byte size. */
    int type;    /**< The user-defined data type. */
} Data;

/**
 * @since 0.1.0
 * @brief Allocate memory for a Data structure.
 * @attention Uses malloc, thus the returned struct needs freeing.
 * @param blob The data blob pointer.
 * @param size @p blob size in bytes.
 * @param type The user-defined type.
 * @return A Data struct pointer malloc'ed and storing the given
 * values. @c NULL is returned in case of errors.
 */
Data* mkdata(void* blob, size_t size, int type);

/**
 * @since 0.1.0
 * @brief Shallow copy a data struct.
 * @attention Uses malloc, thus the returned struct needs freeing.
 * @param data The Data struct to copy.
 * @return A new Data pointer malloc'ed and that is a shallow copy of
 * @p data. @c NULL is returned in case of errors.
 */
Data* datadup(const Data* restrict data);

// clang-format off

/******************************************************************************
 * @}
 * @name Random data generation
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Fills a blob with random data.
 * @alert This operation is destructive for @p blob content.
 * @param blob A memory space to overwrite with random bytes.
 * @param size The number of bytes to overwrite in @p blob.
 */
void sccroll_monkey(void* blob, size_t size) __attribute__((leaf, nothrow, nonnull (1)));

/**
 * @since 0.1.0
 * @brief Allocate a blob of memory initialized with random bytes.
 *
 * The function behavior is identical to malloc(), but fills the
 * allocated pointers with random bytes.
 *
 * @attention Uses malloc, thus the returned struct needs freeing.
 * @param nmemb The number of members.
 * @param size The byte size of each member.
 * @return A memory location of @p nmemb * @p size bytes initialized
 * with random bytes. @p NULL is returned in case of errors.
 */
void* sccroll_rndalloc(size_t nmemb, size_t size);

// clang-format off

/******************************************************************************
 * @}
 * @name Data copy.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Shallow copy a data blob.
 *
 * If @p blob is @c NULL, the function acts the same as calloc().
 *
 * @attention Uses malloc, thus the returned struct needs freeing.
 * @param blob The blob to copy, or @c NULL.
 * @param size The number of bytes to allocate and copy from a
 * non-@c NULL @p blob.
 * @return @c NULL if @p size is @c 0. Otherwise, a pointer to a
 * malloc'ed memory location of @p size bytes, initialized at @c 0 if
 * @p blob is @c NULL, or containing a copy of the first @p size of
 * @p blob if the latter is non-@c NULL (additional bytes are
 * initialized at @c 0).
 */
void* blobdup(const void* restrict blob, size_t size);

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_CREATE_H_
/** @} @} */
