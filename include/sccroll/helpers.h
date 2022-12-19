/**
 * @file        helpers.h
 * @version     0.1.0
 * @brief       Macros d'aides à la programmation.
 * @date        2022
 * @author      Alexandre Martos
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
 ******************************************************************************/
// clang-format on

#endif // SCCROLL_HELPERS_H_
/** @} @} */
