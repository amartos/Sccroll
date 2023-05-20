/**
 * @file      lists.h
 * @version   0.1.0
 * @brief     Librairie pour la gestion de listes chaînées.
 * @year      2023
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright MIT License
 *
 * @addtogroup ListAPI
 * @{
 */

#ifndef LISTS_H_
#define LISTS_H_

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _SCCUNITTESTS
// Permet de tester aisément les gestions d'erreurs.
#include "sccroll/mocks.h"
#endif

// clang-format off

/*******************************************************************************
 * @name Construction de listes
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @struct Node
 * @since 0.1.0
 * @brief Structure d'un noeud de liste chaînée ou d'arbre.
 */
typedef struct Node {
    void* data;        /**< L'élément du noeud. */
    struct Node* prev; /**< Le noeud précédent. */
    struct Node* next; /**< Le noeud suivant. */
} Node;

/**
 * @struct List
 * @since 0.1.0
 * @brief Structure d'une liste chaînée.
 * @note List::curr est utilisée en interne et devrait être considéré
 * comme opaque. Elle est régulièrement assignée avec @c NULL lors des
 * appels de fonctions de la librairie, et perd donc toute valeur qui
 * y aurait été stockée.
 */
typedef struct List {
    Node* head; /**< Le noeud de tête de liste. */
    Node* tail; /**< Le noeud de queue de liste. */
    int len;    /**< La longueur de la liste. */
} List;


/**
 * @since 0.1.0
 * @brief Ajoute un élément comme noeud de tête de la liste.
 * @param data L'élément du noeud.
 * @param list La liste.
 * @return @p list si non-@c NULL, sinon un pointeur vers la liste.
 */
List* lpush(void* data, List* restrict list);

/**
 * @since 0.1.0
 * @brief Ajoute un élément comme noeud de queue de la liste.
 * @param data L'élément du noeud.
 * @param list La liste.
 * @return @p list si non-@c NULL, sinon un pointeur vers la liste.
 */
List* lappend(void* data, List* restrict list);

/**
 * @since 0.1.0
 * @brief Créé une copie de la liste.
 * @alert Le pointeur Node::data n'est **pas** dupliqué dans
 * l'opération, il est commun aux deux listes.
 * @param list La liste à dupliquer.
 * @return Une copie de @p list, ou une liste vide si @p list est
 * @c NULL.
 */
List* ldup(List* restrict list);

/**
 * @since 0.1.0
 * @brief Libère la liste et ses noeuds.
 * @alert Ne libère pas Node::data.
 * @param list La liste à libérer.
 */
void lfree(List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Comparaison de Données
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @typedef lmatch
 * @since 0.1.0
 * @brief Type des fonctions de filtre de données.
 * @param data La donnée courante de la recherche.
 * @return true si la donnée match le filtre, sinon false.
 */
typedef bool (*lmatch)(const void* data);

/**
 * @typedef lcomp
 * @since 0.1.0
 * @brief Type des fonctions de comparaison de données.
 * @param a,b Les noeuds à comparer.
 * @return @c 0 si `a == b`, @c 1 si `a > b`, @c -1 si `a < b`.
 */
typedef int (*lcomp)(const void* a, const void* b);

// clang-format off

/*******************************************************************************
 * @}
 * @name Modification des listes
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Lie deux noeuds entre eux.
 * @param prev Le noeud précédent.
 * @param next Le noeud suivant.
 * @return @p prev si non-@c NULL, sinon @p next.
 */
Node* llink(Node* prev, Node* next);

/**
 * @since 0.1.0
 * @brief Délie deux noeuds s'ils sont liés.
 * @note Le lien est remplacé par @c NULL uniquement où pertinent.
 * @param a,b Deux noeuds à délier.
 * @return @p a si non-@c NULL, sinon @p b.
 */
Node* lunlink(Node* a, Node* b);

/**
 * @since 0.1.0
 * @brief Ajoute un élément comme noeud de la liste à l'index donné.
 * @param data L'élément du noeud.
 * @param index L'index du noeud à créer.
 * @param list La liste.
 * @return @p list si non-NULL, sinon un pointeur vers la liste.
 */
List* lins(void* data, int index, List* restrict list);

/**
 * @def lpop
 * @since 0.1.0
 * @brief Supprime le noeud de tête de la liste.
 * @param list La liste.
 * @return La donnée du noeud de tête.
 */
#define lpop(list) lpopidx(0, list)

/**
 * @def lpoplast
 * @since 0.1.0
 * @brief Supprime le noeud de queue de la liste.
 * @param list La liste.
 * @return La donnée du noeud de queue.
 */
#define lpoplast(list) lpopidx(-1, list)

/**
 * @since 0.1.0
 * @brief Supprime le noeud de la liste à l'index donné.
 * @param index L'index du noeud à supprimer.
 * @param list La liste.
 * @return La donnée du noeud à l'index donné.
 */
void* lpopidx(int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Inverse les noeuds de la liste.
 * @param list La liste à inverser.
 * @return @p list.
 */
List* lrev(List* restrict list);

/**
 * @since 0.1.0
 * @brief Filtre les éléments de la liste.
 * @alert Libère les noeuds éliminés, mais pas les éléments Node::data
 * qu'ils contiennent.
 * @param match Fonction renvoyant @c true si le noeud courant doit
 * être gardé, et @c false s'il doit être éliminé.
 * @param list La liste à filtrer.
 * @return @p list.
 */
List* lfilter(lmatch match, List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Recherche de noeuds et données
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def lfirst
 * @since 0.1.0
 * @brief Renvoie le noeud de tête de la liste.
 * @param list La liste.
 * @return Le noeud de tête de liste.
 */
#define lfirst(list) lidx(0, list)

/**
 * @def llast
 * @since 0.1.0
 * @brief Renvoie le noeud de queue de la liste.
 * @param list La liste.
 * @return Le noeud de queue de liste.
 */
#define llast(list) lidx(-1, list)

/**
 * @since 0.1.0
 * @brief Renvoie le noeud à l'index donné dans la liste.
 * @param list La liste.
 * @return Le noeud à l'index donné de la liste.
 */
Node* lidx(int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Calcule la taille de la liste.
 * @param list La liste.
 * @return La taille de la liste (le nombre de noeuds).
 */
int llen(List* restrict list);

/**
 * @since 0.1.0
 * @brief Compte le nombre d'éléments de la liste pour lesquels
 * @p match renvoie @c true.
 * @param match Fonction renvoyant @c true si l'élément du noeud
 * courant doit être compté.
 * @param list La liste.
 * @return Le nombre d'éléments pour lesquels @p match renvoie @c true.
 */
int lcount(lmatch match, List* restrict list);

/**
 * @brief Donne le premier l'élément de la liste pour lequel
 * @p match renvoie @c true.
 * @param match Fonction renvoyant @c true si l'élément du noeud
 * courant doit être renvoyé.
 * @param list La liste.
 * @return Le premier noeud pour lequel @p match renvoie @c true, ou
 * @c NULL si aucun noeud ne correspond.
 */
Node* lmember(lmatch match, List* restrict list);

/**
 * @brief Donne le dernier l'élément de la liste pour lequel
 * @p match renvoie @c true.
 * @param match Fonction renvoyant @c true si l'élément du noeud
 * courant doit être renvoyé.
 * @param list La liste.
 * @return Le dernier noeud pour lequel @p match renvoie @c true, ou
 * @c NULL si aucun noeud ne correspond.
 */
Node* lrmember(lmatch match, List* restrict list);

/**
 * @def leql
 * @since 0.1.0
 * @brief Indique si les deux listes sont strictements identiques.
 * @param a,b Les listes à comparer.
 * @return @c true si les pointeurs de chaque donnée et noeud des deux
 * listes sont identiques, sinon @c false.
 */
#define leql(a,b) lequal(NULL, a, b)

/**
 * @since 0.1.0
 * @brief Indique si les données des noeuds des deux listes sont
 * identiques.
 * @param compare Fonction de comparaison des données. @c NULL
 * équivaut à appeler leql().
 * @param a,b Les listes à comparer.
 * @return @c true si @p compare renvoie @c 0 pour tous les noeuds de
 * @p a et @p b, sinon @c false.
 */
bool lequal(lcomp compare, List* restrict a, List* restrict b);

/**
 * @since 0.1.0
 * @brief Indique si la liste est un palindrome.
 * @param compare Fonction de comparaison des données des noeuds.
 * @param list La liste.
 * @return @c true si la liste est un palindrome, sinon @c false.
 */
bool lpalin(lcomp compare, List* restrict list);

/**
 * @since 0.1.0
 * @brief Indique si la liste est circulaire.
 * @param list La liste.
 * @return Le noeud de début de la portion circulaire de la liste, ou
 * @c NULL si la liste n'est pas circulaire.
 */
Node* lcirc(List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Conversion des listes en chaînes de caractères
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @typedef lptr
 * @since 0.1.0
 * @brief Type des fonctions d'affichage de données.
 * @param index L'index de la donnée dans la liste.
 * @param data La donnée courante à afficher.
 * @return Une chaîne de caractère décrivant la donnée et allouée avec
 * malloc.
 */
typedef char* (*lprt)(int index, const void* data);

/**
 * @since 0.1.0
 * @brief Affiche un message décrivant la liste.
 * @param stream Le flux où afficher le message.
 * @param printer Fonction de conversion des données en chaîne de
 * caractères (le defaut affiche le pointeur et les autres
 * informations de la donnée).
 * @param sep Le séparateur entre les éléments de la liste (défaut ", ").
 * @param list La liste à afficher.
 */
void lprint(FILE* stream, lprt printer, const char* sep, List* restrict list);

/**
 * @since 0.1.0
 * @brief Considère la donnée comme une chaîne de caractères et
 * l'affiche avec le format "Node index: 'donnée'".
 * @alert Utilise malloc.
 * @param index L'index de la donnée dans la liste.
 * @param strdata La chaîne de caractères.
 * @return La chaîne de caractère décrivant la donnée dans la liste.
 */
char* lstrprt(int index, const void* strdata);

#endif // LISTS

/** @}*/
