/**
 * @file        data.c
 * @version     0.1.0
 * @brief       Ficher source de la génération de données pour
 *              tests unitaires.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 * @addtogroup Internals
 * @{
 * @addtogroup DataInternals Générateurs de données pour les tests unitaires
 * @{
 */

#include "sccroll/data.h"

/**
 * @since 0.1.0
 * @brief Initialise une nouvelle séquence de nombres
 * pseudo-aléatoires pour sccroll_arc4random_buf.
 */
static void sccroll_peanuts(void) __attribute__((constructor));

/**
 * @since 0.1.0
 * @brief Remplit l'espace mémoire donné de size octets aléatoires.
 * @param blob Un espace mémoire à remplir.
 * @param size Le nombre d'octets à remplir.
 *
 * @c arc4random_buf n'est définit dans la librairie C de GNU que dans
 * la version 2.36, très récente à l'écriture de ce code. il existe
 * une fonction a@c rc4random_buf dans la librairie BSD, mais rien ne
 * garantit sa présence non plus. Donc, la fonction de ce moduleest
 * redéfinie comme version faible. Si la version de la librairie C est
 * 2.36+, alors l'alias ne sera plus utilisé.
 *
 * @todo: supprimer le code correspondant une fois que la librairie C
 * version 2.36+ sera suffisamment répandue.
 */
void arc4random_buf(void* blob, size_t size) __attribute__((weak, nonnull(1)));

// clang-format off

/******************************************************************************
 * @} @}
 *
 * Implémentation
 ******************************************************************************/
// clang-format on

static void sccroll_peanuts(void) { srandom(time(NULL)); }

// arc4random_buf remplit déjà le rôle de sccroll_monkey.
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
