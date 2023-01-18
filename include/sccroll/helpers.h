/**
 * @file        helpers.h
 * @version     0.1.0
 * @brief       Macros d'aides à la programmation.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Helpers Macros d'aides à la programmation
 * @{
 */

#ifndef SCCROLL_HELPERS_H_
#define SCCROLL_HELPERS_H_

#include <stdlib.h>
#include <errno.h>
#include <signal.h>

// clang-format off

/******************************************************************************
 * @name Gestion des drapeaux
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Détermine si les drapeaux @p flags contiennent les valeurs
 * @p values.
 * @param flags Les drapeaux combinés avec OR.
 * @param values Une liste de drapeaux regroupées par OR.
 * @return @p values si @p flags les contient, sinon 0.
 */
unsigned sccroll_hasFlags(unsigned flags, unsigned values);

// clang-format off

/******************************************************************************
 * @}
 * @name Alias Macros générant des alias de fonctions
 * @{
 * @param name Nom de la fonction d'origine.
 * @param aliasname Nom de l'alias.
 * @param ... Attributs supplémentaires pour l'alias.
 ******************************************************************************/
// clang-format on

/**
 * @def attr_alias
 * @since 0.1.0
 * @brief Génère un alias.
 */
#define attr_alias(name, aliasname, ...) \
    extern __typeof__(name) aliasname __attribute__((alias(#name), ##__VA_ARGS__))

/**
 * @def strong_alias
 * @since 0.1.0
 * @brief Génère un alias fort.
 */
#define strong_alias(name, aliasname) attr_alias(name, aliasname)

/**
 * @def weak_alias
 * @since 0.1.0
 * @brief Génère un alias faible.
 */
#define weak_alias(name, aliasname) attr_alias(name, aliasname, weak)

// clang-format off

/******************************************************************************
 * @}
 * @name CodeNames Fonctions traduisant les codes d'erreurs et signaux
 * en nom descriptif.
 *
 * Ces fonctions sont en réalité intégrée dans la librairie C de
 * GNU. Cependant, leur intégration est récente, et donc n'est pas
 * dans toutes les versions trouvables; de plus, d'autres librairies C
 * peuvent ne pas les intégrer (ou ne pas être installées).
 *
 * Elles ont donc été redéfinies ici afin de garantir leur
 * présence. Elles sont définies comme des versions faibles, pouvant
 * donc être suplantées si la librairie C utilisée les définit
 * elle-même.
 *
 * @todo supprimer ces fonctions lorsque le support global par la
 * librairie C sera assuré.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Renvoie le nom du code d'erreur errno correspondant.
 * @param errnum La valeur du code d'erreur errno.
 * @return Le nom du code d'erreur errno correspondant à la valeur @p
 * errnum ou @c NULL si la valeur ne correspond à aucun code.
 */
const char* strerrorname_np(int errnum) __attribute__((weak));

/**
 * @since 0.1.0
 * @brief Renvoie le nom abrégé du signal.
 * @param sig La valeur du signal.
 * @return Le nom abrégé (le 'XXX' de 'SIGXXX') du signal
 * correspondant à la valeur @p sig ou @c NULL si la valeur ne
 * correspond à aucun signal.
 */
const char* sigabbrev_np(int sig) __attribute__((weak));

// clang-format off

/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_HELPERS_H_
/** @} @} */
