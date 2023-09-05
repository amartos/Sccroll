/**
 * @file      lists.h
 * @version   0.1.0
 * @brief     Double linked list library.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2023 Alexandre Martos <contact@amartos.fr>
 * @license   MIT License
 *
 * @addtogroup API
 * @{
 * @addtogroup ListAPI Linked lists API.
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
// Allows easier errors handling tests of the library.
// TODO: remove this dependency, implying an architecture redesign.
#include "sccroll/mocks.h"
#endif

// clang-format off

/*******************************************************************************
 * @name Lists building
 *
 *
 * Memory allocation is done for any List created through these
 * functions. The lfree() function can be used to handle freeing such
 * lists.
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @struct Node
 * @since 0.1.0
 * @brief A list node.
 */
typedef struct Node {
    void* data;        /**< The node data. */
    struct Node* prev; /**< The next node. */
    struct Node* next; /**< The previous node. */
} Node;

/**
 * @struct List
 * @since 0.1.0
 * @brief Linked list structure.
 */
typedef struct List {
    Node* head; /**< The list head. */
    Node* tail; /**< The list tail. */
    int len;    /**< The list length. */
} List;

/**
 * @since 0.1.0
 * @brief Push a new node storing a data pointer at the list head.
 * @param data The data pointer to store in the node.
 * @param list The list to push to.
 * @return @p list if non-@c NULL, otherwise a pointer to a new List.
 */
List* lpush(void* data, List* restrict list);

/**
 * @since 0.1.0
 * @brief Append a new node storing a data pointer at the list tail.
 * @param data The data pointer to store in the node.
 * @param list The list to append to.
 * @return @p list if non-@c NULL, otherwise a pointer to a new List.
 */
List* lappend(void* data, List* restrict list);

/**
 * @since 0.1.0
 * @brief Shallow copy the given list.
 * @alert New List and Node pointers are generated, but the Node::data
 * pointer is **not** duplicated. Each Node::data pointer is thus
 * common to the original and copy lists.
 * @param list The list to shallow copy.
 * @return The new copy of @p list. If @p list is @c NULL, creates a
 * new empty List.
 */
List* ldup(List* restrict list);

/**
 * @since 0.1.0
 * @brief Frees the list and its nodes.
 * @alert Frees each Node **and the List structure** -- thus any non
 * malloc'ed List would raise an error. This function does **not**
 * free any Node::data.
 * @param The list to free.
 */
void lfree(List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Search and compare List data
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @typedef lmatch
 * @since 0.1.0
 * @brief Filter functions prototype.
 * @param data The current Node::data pointer.
 * @return @c true if @p data is a match, @c false otherwise.
 */
typedef bool (*lmatch)(const void* data);

/**
 * @typedef lcomp
 * @since 0.1.0
 * @brief Comparison functions prototype.
 *
 * This prorotype follows the rules for the qsort() comparison
 * functions prototypes.
 *
 * @param a,b The nodes to compare.
 * @return
 * - @c 0 if `a == b`
 * - @c 1 si `a > b`
 * - @c -1 si `a < b`
 */
typedef int (*lcomp)(const void* a, const void* b);

// clang-format off

/*******************************************************************************
 * @}
 * @name Lists modifications
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Creates a link between two nodes.
 * @param prev The first node encountered in the
 * head->tail direction.
 * @param next The second node encountered in the head->tail
 * direction.
 * @return @p prev if non-@c NULL, otherwise @p next.
 */
Node* llink(Node* prev, Node* next);

/**
 * @since 0.1.0
 * @brief Unlink two linked nodes.
 *
 * The Node::prev or Node::next pointers are set to @c NULL, but
 * only if the corresponding pointer corresponds ot the other node to
 * unlink. Thus, if the two nodes do not have any reference to each
 * other, the function has no effect.
 *
 * @param a,b Two nodes to unlink.
 * @return @p a if non-@c NULL, otherwise @p b.
 */
Node* lunlink(Node* a, Node* b);

/**
 * @since 0.1.0
 * @brief Inserts a new node storing a data pointer at the given index
 * of the list.
 *
 * The index is defined in the head->tail direction; the List::head
 * node is thus of index @c 0, and the List::tail node has an index of
 * @code (List length) - 1 @endcode.
 *
 * If the index is negative, the considered direction is reversed, and
 * the index values change: the List::head Node has an index of
 * @code -(List length) @endcode and the List::tail Node has an index
 * of @c -1.
 *
 * An index equal to the list length would have the same effects as
 * pushing or appending to the list.
 *
 * In the case the index would be greater than to the list length,
 * empty nodes (i.e. with a @c NULL Node::data pointer) are
 * pushed/appended (depending on the index sign) until the specified
 * index is reached. For example, inserting at index @c 3 in a list
 * with @c 2 nodes would be the same as appending to the list first an
 * empty node and then the node storing @p data. The same case but
 * using an index of @c -3 would push the nodes, instead of append
 * them to the list.
 *
 * @param data The data to store in the new node.
 * @param index The index of the new node.
 * @param list The list to insert to.
 * @return @p list if non-NULL, otherwise a pointer to a newly created
 * List.
 */
List* lins(void* data, int index, List* restrict list);

/**
 * @def lpop
 * @since 0.1.0
 * @brief Removes the head node of the list.
 * @param list The list to pop from.
 * @return The Node::data pointer of the popped node.
 */
#define lpop(list) lpopidx(0, list)

/**
 * @def lpoplast
 * @since 0.1.0
 * @brief Removes the tail node of the list.
 * @param list The list to pop from.
 * @return The Node::data pointer of the popped node.
 */
#define lpoplast(list) lpopidx(-1, list)

/**
 * @since 0.1.0
 * @brief Removes the node of the list at the given index.
 *
 * The indexes follow the directions rules indicated in the lins()
 * documentation.
 *
 * @param list The list to pop from.
 * @return The Node::data pointer of the popped node, or @c NULL if
 * the index is greater or equal than the list length.
 */
void* lpopidx(int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Reverse the list.
 * @alert This function overwrites the Node::prev and Node::next
 * pointers, and is thus destructive.
 * @param list The list to reverse.
 * @return @p list.
 */
List* lrev(List* restrict list);

/**
 * @since 0.1.0
 * @brief Filters the list nodes.
 * @alert This function frees the removed nodes, but not the
 * Node::data pointers they contained.
 * @alert This function is destructive, i.e. @p list is modified by
 * it. Any Node::data pointer filtered is lost if not stored
 * elsewhere.
 * @param match Match function returning @c true to keep the node, and
 * @c false to remove it.
 * @param list The list to filter.
 * @return @p list.
 */
List* lfilter(lmatch match, List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Finding nodes and data
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def lfirst
 * @since 0.1.0
 * @brief Gives the List::head node.
 * @param list The list.
 * @return The List::head node.
 */
#define lfirst(list) lidx(0, list)

/**
 * @def lfirst
 * @since 0.1.0
 * @brief Gives the List::tail node.
 * @param list The list.
 * @return The List::tail node.
 */
#define llast(list) lidx(-1, list)

/**
 * @since 0.1.0
 * @brief Gives the Node pointer located at the given index of the
 * list.
 *
 * The index directions follow the rules described in the lins()
 * function documentation.
 *
 * @param list The list.
 * @return The Node pointer located at the given index in @p list, or
 * @c NULL if index is greater of equal than the list length.
 */
Node* lidx(int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Give the list length.
 * @param list The list.
 * @return The list length, i.e. the total number of nodes from
 * List::head to List::tail.
 */
int llen(List* restrict list);

/**
 * @since 0.1.0
 * @brief Count the number of nodes for which the given match function
 * returns @c true.
 * @param match the match function.
 * @param list The list.
 * @return The total number of nodes for which @c match returns
 * @c true.
 */
int lcount(lmatch match, List* restrict list);

/**
 * @since 0.1.0
 * @brief Gives the pointer of the first node for which @p match
 * returns @c true.
 * @param match The match function.
 * @param list The list.
 * @return The first Node pointer of @p list for which @c match
 * returns @c true, or @c NULL.
 */
Node* lmember(lmatch match, List* restrict list);

/**
 * @since 0.1.0
 * @brief Gives the pointer of the last node for which @p match
 * returns @c true.
 * @param match The match function.
 * @param list The list.
 * @return The last  Node pointer of @p list for which @c match
 * returns @c true, or @c NULL.
 */
Node* lrmember(lmatch match, List* restrict list);

/**
 * @def leql
 * @since 0.1.0
 * @brief Indicate if the two lists are strictly identical, i.e. have
 * the same Node pointers in the same order (the List pointers are not
 * considered).
 * @param a,b The lists to compare.
 * @return @c true if the lists are equal, @c false otherwise.
 */
#define leql(a,b) lequal(NULL, a, b)

/**
 * @since 0.1.0
 * @brief Indicate if the lists have the same Node::data in the same
 * order.
 * @param compare Node::data comparison function. A value of @c NULL
 * is the same as calling leql().
 * @param a,b The lists to compare.
 * @return @c true if @p compare returns @c 0 for all nodes of both
 * @p a and @p b, @c false otherwise.
 */
bool lequal(lcomp compare, List* restrict a, List* restrict b);

/**
 * @since 0.1.0
 * @brief Indicate if the list is a palindrome.
 *
 * A palindromic list is defined as a list for which the @p compare
 * function returns @c 0 for each Node::data comparison located at
 * opposite indexes of the @p list, i.e. the nodes couples at index
 * (0,-1), (1,-2), *etc*...
 *
 * @param compare Node::data comparison function.
 * @param list The list.
 * @return @c true if the list is a palindrome, @c false otherwise.
 */
bool lpalin(lcomp compare, List* restrict list);

/**
 * @since 0.1.0
 * @brief Indicate if the list is circular.
 *
 * A list is circular if any node it contains references a previous
 * node as its Node::next pointer.
 *
 * @param list The list.
 * @return The node located at the start of the circular portion of
 * the list, or @c NULL.
 */
Node* lcirc(List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Pretty printing
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @typedef lptr
 * @since 0.1.0
 * @brief Node::data pretty printing functions prototype.
 * @param index The index of the node in the list.
 * @param data The Node::data pointer of the node at index @p index in
 * the list.
 * @return A malloc'ed string describing @p data.
 */
typedef char* (*lprt)(int index, const void* data);

/**
 * @since 0.1.0
 * @brief Print a message describing the list.
 * @param stream The stream to print to.
 * @param printer The lprt() function for each node. If @c NULL, the
 * Node::data pointers addresses are printed instead.
 * @param sep The nodes description separator, defaults to ", ".
 * @param list The list to pretty print.
 */
void lprint(FILE* stream, lprt printer, const char* sep, List* restrict list);

/**
 * @since 0.1.0
 * @brief Pretty print a Node::data pointer as a string.
 *
 * The returned string is formatted as "index: 'data'"
 *
 * @param index The index of the node in the list.
 * @param strdata The string store in the node at index @p index in
 * the list.
 * @return A malloc'ed formatted string describing the Node::data
 * string.
 */
char* lstrprt(int index, const void* strdata);

#endif // LISTS

/** @}*/
