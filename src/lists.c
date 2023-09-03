/**
 * @file      lists.c
 * @version   0.1.0
 * @brief     Double linked list library.
 * @year      2023
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright MIT License
 *
 * @addtogroup ListInternalAPI
 * @{
 */

#include "sccroll/lists.h"

// clang-format off

/*******************************************************************************
 * @name Memory allocation
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Generate a node.
 * @param data The node data.
 * @param previous The previous node.
 * @param next The next node.
 * @return A malloc'ed node initialised with the given parameters.
 */
static Node* lnode(void* data, Node* previous, Node* next);

/**
 * @since 0.1.0
 * @brief Generate a list.
 * @param node The first node of the list.
 * @return A malloc'ed list.
 */
static List* llist(Node* node);

/**
 * @since 0.1.0
 * @brief Shallow copy a list (Node pointers are duplicated, but not
 * Node::data). The copy is recursive.
 * @param dir The direction of the run.
 * @param curr The current node.
 * @param from The list to copy.
 * @param to The destination list.
 * @return @p to, or a malloc'ed copy List if @p to is @c NULL.
 */
static List* ldup_aux(int dir, Node* curr, List* restrict from, List* to);

/**
 * @since 0.1.0
 * @brief Frees the nodes of a list (but not Node::data).
 * @param dir The run direction.
 * @param node The current node.
 */
static void lfree_aux(int dir, Node* node);

// clang-format off

/*******************************************************************************
 * @}
 * @name Lists iterations
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def ldir
 * @since 0.1.0
 * @brief Determine an initial run direction.
 * @param list The list to iterate from.
 * @return @c 0 for forward, @c -1 for reverse.
 */
#define ldir(list) list->head ? 0 : -1;

/**
 * @def lnextdir
 * @since 0.1.0
 * @brief Determine the index of the next node of the list.
 * @param dir The run direction.
 * @param index The current node index.
 * @return The next node index.
 */
#define lnextidx(dir, index) dir < 0 ? index+1 : index-1

/**
 * @since 0.1.0
 * @brief Determine the next node of the list.
 * @param dir The run direction.
 * @param curr The current node.
 * @param list The list (used only if @p curr is @c NULL).
 * @return The next node of the list.
 */
static Node* lnext(int dir, Node* curr, List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Modification de listes
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Set List::head and List::tail.
 * @param head The new list head.
 * @param tail The new list tail.
 * @param list The list to modify.
 * @return @p list.
 */
static List* llset(Node* head, Node* tail, List* restrict list);

/**
 * @since 0.1.0
 * @brief Removes the first node of the head or tail of the list.
 * @param node The node to remove.
 * @param list The list to modify.
 * @return @p list.
 */
static List* llunset(Node* node, List* restrict list);

/**
 * @since 0.1.0
 * @brief Add empty nodes to a list.
 * @param dir The direction to add to.
 * @param count The number of empty nodes to add.
 * @param list The list to modify.
 * @return @p list, or a new malloc'ed list if @p list is @c NULL.
 */
static List* lfill(const int dir, int count, List* restrict list);

/**
 * @since 0.1.0
 * @brief Inserts a given data at a given index in the list.
 * @param data The data to insert.
 * @param index The index to insert @p data at.
 * @param list The list to modify.
 * @return @p list, or a new malloc'ed list if @p list is @c NULL.
 */
static List* lins_aux(void* data, int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Reverse a list.
 * @param dir The run direction.
 * @param curr The current node.
 * @param list The list to modify.
 * @return @p list.
 */
static List* lrev_aux(int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Removes nodes from a list.
 * @param match Match function returning @c true for each node to
 * keep.
 * @param dir The run direction.
 * @param curr The current node.
 * @param list The list to modify.
 * @return @p list.
 */
static List* lfilter_aux(lmatch match, int dir, Node* curr, List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Search and infos
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Give the node at the given index.
 * @param index The index of the node to return.
 * @param curr The current node.
 * @param list The list.
 * @return The node at index @p index, or @c NULL if @p index is
 * greater or equal than the list length.
 */
static Node* lidx_aux(int index, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Count the number of nodes for which a match function returns
 * @c true.
 * @param match Match function returning @c true to be counted. The
 * default is to count all nodes of the list.
 * @param count The total count.
 * @param dir The run direction.
 * @param curr The current node.
 * @param list The list.
 * @return The total number of nodes for which @p match returns @c true.
 */
static int lcount_aux(lmatch match, int count, int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Give the first node for which @p match returns @c true.
 * @param match Match function returning @c true when the node is
 * found.
 * @param dir The run direction.
 * @param curr The current node.
 * @param list The list.
 * @return The first node in the given @p dir direction for which
 * @p match returns @c true, otherwise @c NULL.
 */
static Node* lmember_aux(lmatch match, int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Indicate if two lists are equivalent (the Node::data are the
 * same).
 * @param compare Data comparison function. @c NULL makes the function
 * compare the pointers.
 * @param dir The run direction.
 * @param a,b The nodes to compare.
 * @return @c true if all data pointers of the @p a and @p b lists are
 * identical and if @p compare is @c NULL, or if all the @p compare
 * calls return @c 0. Otherwise, @c false.
 */
static bool leql_aux(lcomp compare, int dir, Node* a, Node* b);

/**
 * @since 0.1.0
 * @brief Indicate if the list is a palindrome.
 * @param compare Fonction de comparaison des noeuds. @c NULL équivaut
 * à ne comparer que les pointeurs.
 * @param compare Data comparison function. @c NULL makes the function
 * compare the pointers.
 * @param curr The current node in the head->tail direction.
 * @param rcurr The current node in the tail->head direction.
 * @return @c true if the list is a palindrome (@p compare returns
 * @c 0 for all comparisons, or the pointers are the same). Otherwise,
 * @c false.
 */
static bool lpalin_aux(lcomp compare, Node* curr, Node* rcurr);

/**
 * @since 0.1.0
 * @brief Indicate if the list is circular.
 * @param dir The run direction.
 * @param hare The current node of the fast traveler.
 * @param turtle The current node of the slow traveler.
 * @return The node at the start of the loop, or @c NULL.
 */
static Node* lcirc_aux(int dir, Node* hare, Node* turtle);

// clang-format off

/*******************************************************************************
 * @}
 * @name Pretty printing
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def LISTSFMT
 * @since 0.1.0
 * @brief Lists delimiters format string.
 * @param s Le The list description.
 */
#define LISTSFMT "(%s)\n"

/**
 * @def LISTSSEP
 * @since 0.1.0
 * @brief Default separator of lists elements.
 */
#define LISTSSEP ", "

/**
 * @since 0.1.0
 * @brief Give a description of the given data as a string.
 * @note This is the default function used by lprt().
 * @param index The current node index.
 * @param data The data of the current node, considered as a string.
 * @return A malloc'ed string describing the @p data.
 */
static char* ldefaultprt(int index, const void* data);

// clang-format off

/*******************************************************************************
 * @}
 * Implementation
 *
 * Memory allocation.
 *******************************************************************************/
// clang-format on

static Node* lnode(void* data, Node* previous, Node* next)
{
    Node* new = calloc(1, sizeof(Node));
    if (!new) err(EXIT_FAILURE, "could not create Node");
    new->data = data;
    new->prev = previous;
    new->next = next;
    return new;
}

static List* llist(Node* node)
{
    List* new = calloc(1, sizeof(List));
    if (!new) err(EXIT_FAILURE, "could not create List");
    else if (!node) return new;
    new->head = node;
    new->tail = node;
    ++new->len;
    return new;
}

// clang-format off

/*******************************************************************************
 * Lists building
 *******************************************************************************/
// clang-format on

List* lpush(void* data, List* restrict list)
{
    Node* node = lnode(data, NULL, NULL);
    if (!list) return llist(node);

    llink(node, list->head);
    ++list->len;
    return llset(node, list->tail, list);
}

Node* llink(Node* prev, Node* next)
{
    if (prev) prev->next = next;
    if (next) next->prev = prev;
    return prev ? prev : next;
}

Node* lunlink(Node* a, Node* b)
{
    if (a) {
        if (a->prev == b) a->prev = NULL;
        if (a->next == b) a->next = NULL;
    }
    if (b) {
        if (b->prev == a) b->prev = NULL;
        if (b->next == a) b->next = NULL;
    }
    return a ? a : b;
}

static List* llset(Node* head, Node* tail, List* restrict list)
{
    if (!list) return NULL;

    list->head = head;
    list->tail = tail;
    return list;
}

List* lappend(void* data, List* restrict list)
{
    Node* node = lnode(data, NULL, NULL);
    if (!list) return llist(node);

    llink(list->tail, node);
    ++list->len;
    return llset(list->head, node, list);
}

List* ldup(List* restrict list)
{
    if (!list) return NULL;
    int dir = ldir(list);
    return ldup_aux(dir, lnext(dir, NULL, list), list, NULL);
}

static Node* lnext(int dir, Node* curr, List* restrict list)
{
    if (curr)
        return dir < 0 ? curr->prev : curr->next;
    else if (list)
        return dir < 0 ? list->tail : list->head;
    return NULL;
}

static List* ldup_aux(int dir, Node* curr, List* restrict from, List* to)
{
    if (!curr) return to;
    // We reverse the input (push when going reverse) to keep the list
    // order.
    to = dir < 0 ? lpush(curr->data, to) : lappend(curr->data, to);
    return ldup_aux(dir, lnext(dir, curr, from), from, to);
}

void lfree(List* restrict list)
{
    if (!list) return;
    int dir = ldir(list);
    lfree_aux(dir, lnext(dir, NULL, list));
    free(list);
}

static void lfree_aux(int dir, Node* node)
{
    if (!node) return;
    lfree_aux(dir, lnext(dir, node, NULL));
    free(node);
}

// clang-format off

/*******************************************************************************
 * Data access
 *******************************************************************************/
// clang-format on

Node* lidx(int index, List* restrict list)
{
    if (!list) return NULL;
    return lidx_aux(index < 0 ? index+1 : index, lnext(index, NULL, list), list);
}

static Node* lidx_aux(int index, Node* curr, List* restrict list)
{
    if (!curr || !index) return curr;
    return lidx_aux(lnextidx(index, index), lnext(index, curr, list), list);
}

// clang-format off

/*******************************************************************************
 * Lists modification
 *******************************************************************************/
// clang-format on

List* lins(void* data, int index, List* restrict list)
{
    if (list && (!index || index == -(list->len+1)))
        return lpush(data, list);
    else if (list && (index == -1 || index == list->len))
        return lappend(data, list);
    else if (!list || abs(index) > list->len) {
        int diff = abs(index > 0 ? index+1 : index) - (list ? list->len : 0) - 1;
        list = lfill(index, diff, list);
        return lins(data, index, list);
    }
    return lins_aux(data, index, list);
}

static List* lfill(const int dir, int count, List* restrict list)
{
    if (!count) return list;
    return
        dir < 0
        ? lpush(NULL, lfill(dir, count-1, list))
        : lappend(NULL, lfill(dir, count-1, list));
}

static List* lins_aux(void* data, int index, List* restrict list)
{
    Node* new  = lnode(data, NULL, NULL);
    Node* next = lidx(index, list);

    if (index < 0) {
        llink(new, next->next);
        llink(next, new);
    }
    else {
        llink(next->prev, new);
        llink(new, next);
    }

    ++list->len;
    return list;
}

void* lpopidx(int index, List* restrict list)
{
    Node* popped = lidx(index, list);
    if (!popped) return NULL;

    llunset(popped, list);

    llink(popped->prev, popped->next);

    void* data = popped->data;
    free(popped);
    --list->len;
    return data;
}

static List* llunset(Node* node, List* restrict list)
{
    list->head = list->head == node ? node->next : list->head;
    list->tail = list->tail == node ? node->prev : list->tail;
    return list;
}

List* lrev(List* restrict list)
{
    if (!list) return NULL;

    int dir = ldir(list);
    Node* curr = lnext(dir, NULL, list);
    lrev_aux(dir, curr, list);
    curr = list->head;
    list->head = list->tail;
    list->tail = curr;
    return list;
}

static List* lrev_aux(int dir, Node* curr, List* restrict list)
{
    if (!curr) return list;
    lrev_aux(dir, lnext(dir, curr, list), list);

    Node* prev = curr->prev;
    curr->prev = curr->next;
    curr->next = prev;
    return list;
}

List* lfilter(lmatch match, List* restrict list)
{
    if (!list) return NULL;

    int dir = ldir(list);
    return lfilter_aux(match, dir, lnext(dir, NULL, list), list);
}

static List* lfilter_aux(lmatch match, int dir, Node* curr, List* restrict list)
{
    if (!curr) return list;

    Node* next = lnext(dir, curr, list);
    if (match && match(curr->data)) {
        llunset(curr, list);
        llink(curr->prev, curr->next);
        free(curr);
        --list->len;
    }
    return lfilter_aux(match, dir, next, list);
}

// clang-format off

/*******************************************************************************
 * Lists information
 *******************************************************************************/
// clang-format on

void lprint(FILE* stream, lprt printer, const char* sep, List* restrict list)
{
    if (!stream || !list) return;
    printer             = printer ? printer : ldefaultprt;
    sep                 = sep ? sep : LISTSSEP;
    char output[BUFSIZ] = { 0 };
    int dir             = ldir(list);
    Node* curr          = lnext(dir, NULL, list);
    char* data          = NULL;
    for (int i = dir < 0 ? list->len-1 : 0; curr; dir < 0 ? --i : ++i)
    {
        data = printer(i, curr->data);
        strcat(output, data);
        free(data);
        curr = lnext(dir, curr, NULL);
        if (curr) strcat(output, sep);
    }
    fprintf(stream, LISTSFMT, output);
}

static char* ldefaultprt(int index, const void* data)
{
    char sdata[256] = {0};
    sprintf(sdata, "[Node %i: %p]", index, data);
    return strdup(sdata);
}

char* lstrprt(int index, const void* data)
{
    char sdata[256] = {0};
    sprintf(sdata, "[Node %i: '%s']", index, data ? (const char*)data : "null");
    return strdup(sdata);
}

int llen(List* restrict list)
{
    if (!list) return 0;
    return list->len;
}

int lcount(lmatch match, List* restrict list)
{
    if (!list) return 0;
    int dir = ldir(list);
    return lcount_aux(match, 0, dir, lnext(dir, NULL, list), list);
}

static int lcount_aux(lmatch match, int count, int dir, Node* curr, List* restrict list)
{
    if (!curr) return count;
    return
        lcount_aux(
            match,
            count + (match ? match(curr->data) : 1),
            dir,
            lnext(dir, curr, list), list
        );
}

Node* lmember(lmatch match, List* restrict list)
{
    if (!list || !match) return NULL;
    return lmember_aux(match, 0, lnext(0, NULL, list), list);
}

static Node* lmember_aux(lmatch match, int dir, Node* curr, List* restrict list)
{
    if (!curr) return curr;
    return
        match(curr->data)
        ? curr
        : lmember_aux(match, dir, lnext(dir, curr, list), list);
}

Node* lrmember(lmatch match, List* restrict list)
{
    if (!list || !match) return NULL;
    return lmember_aux(match, -1, lnext(-1, NULL, list), list);
}

bool lequal(lcomp compare, List* restrict a, List* restrict b)
{
    if (a == b)
        return true;
    else if (!a || !b)
        return false;
    int dir = ldir(a);
    return leql_aux(compare, dir, lnext(dir, NULL, a), lnext(dir, NULL, b));
}

static bool leql_aux(lcomp compare, int dir, Node* a, Node* b)
{
    if (!a && !b)
        return true;
    else if (!a || !b)
        return false;
    else if (a == b || (compare && !compare(a->data, b->data)))
        return leql_aux(compare, dir, lnext(dir, a, NULL), lnext(dir, b, NULL));
    return false;
}

bool lpalin(lcomp compare, List* restrict list)
{
    if (!list)
        return false;
    return lpalin_aux(compare, lnext(0, NULL, list), lnext(-1, NULL, list));
}

static bool lpalin_aux(lcomp compare, Node* curr, Node* rcurr)
{
    if (!curr || !rcurr)
        return false;
    else if (
        curr->prev
        && rcurr->next
        && (curr->prev == rcurr->next || (curr->prev == rcurr && rcurr->next == curr))
        )
        return true;
    else if (
        (!compare && curr == rcurr)
        || (compare && !compare(curr->data, rcurr->data))
        )
        return lpalin_aux(compare, lnext(0, curr, NULL), lnext(-1, rcurr, NULL));
    return false;
}

Node* lcirc(List* restrict list)
{
    if (!list) return false;
    int dir     = ldir(list);
    Node* start = lnext(dir, NULL, list);
    return lcirc_aux(dir, lnext(dir, start, NULL), start);
}

static Node* lcirc_aux(int dir, Node* hare, Node* turtle)
{
    if (!hare || !hare->next) return NULL;
    else if (turtle == hare) return hare;

    hare = lnext(dir, hare, NULL);
    return lcirc_aux(dir, lnext(dir, hare, NULL), lnext(dir, turtle, NULL));
}
