/**
 * @file      lists.c
 * @version   0.1.0
 * @brief     Librairie pour la gestion de listes chaînées.
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
 * @name Allocation mémoire
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Génère un noeud.
 * @alert Utilise malloc.
 * @param data La donnée du noeud.
 * @param previous Le noeud précédent.
 * @param next Le noeud suivant.
 * @return Un nouveau noeud initialisé avec les paramètres donnés.
 */
static Node* lnode(void* data, Node* previous, Node* next);

/**
 * @since 0.1.0
 * @brief Génère une liste.
 * @alert Utilise malloc.
 * @param node Le premier noeud de la liste.
 * @return Une nouvelle liste initialisée avec le premier noeud.
 */
static List* llist(Node* node);

/**
 * @since 0.1.0
 * @brief Duplique une liste (mais pas Node::data).
 * @param dir La direction de parcours.
 * @param curr Le noeud courant.
 * @param from La liste à copier.
 * @param to La liste de destination.
 * @return @p to, ou une nouvelle liste si @p to est @c NULL.
 */
static List* ldup_aux(int dir, Node* curr, List* restrict from, List* to);

/**
 * @since 0.1.0
 * @brief Libère les noeuds d'une liste (mais pas Node::data).
 * @param dir La direction de parcours.
 * @param node Le noeud courant.
 */
static void lfree_aux(int dir, Node* node);

// clang-format off

/*******************************************************************************
 * @}
 * @name Parcours de listes
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def ldir
 * @since 0.1.0
 * @brief Détermine une direction de parcours de la liste par défaut.
 * @param list La liste à parcourir.
 * @return @c 0 ou @c -1.
 */
#define ldir(list) list->head ? 0 : -1;

/**
 * @def lnextdir
 * @since 0.1.0
 * @brief Donne l'index du prochain noeud de la liste.
 * @param dir La direction de parcours.
 * @param index L'index du noeud courant.
 * @return L'index du prochain noeud.
 */
#define lnextidx(dir, index) dir < 0 ? index+1 : index-1

/**
 * @since 0.1.0
 * @brief Donne le prochain noeud de la liste.
 * @param dir La direction de parcours.
 * @param curr Le noeud courant.
 * @param list La liste parcourue (n'est utilisée que si @p curr est
 * @c NULL).
 * @return Le prochain noeud de la liste.
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
 * @brief Assigne les valeurs List::head et List::tail.
 * @param head La nouvelle tête de liste.
 * @param tail La nouvelle queue de liste.
 * @param list La liste à modifier.
 * @return @p list.
 */
static List* llset(Node* head, Node* tail, List* restrict list);

/**
 * @since 0.1.0
 * @brief Retire le noeud de la tête et/ou de la queue de liste.
 * @param node Le noeud à retirer.
 * @param list La liste à modifier.
 * @return @p list.
 */
static List* llunset(Node* node, List* restrict list);

/**
 * @since 0.1.0
 * @brief Ajoute des noeuds vides à une liste.
 * @param dir La direction d'ajout.
 * @param count Le nombre de noeuds vides (Node::data @c NULL) à
 * ajouter.
 * @param list La liste à modifier.
 * @return @p list, ou une nouvelle liste si @p list est @c NULL.
 */
static List* lfill(const int dir, int count, List* restrict list);

/**
 * @since 0.1.0
 * @brief Insère une donnée à un index précis dans la liste.
 * @param data La donnée à insérer.
 * @param index L'index où insérer la donnée.
 * @param list La liste à modifier.
 * @return @p list, ou une nouvelle liste si @p list est @c NULL.
 */
static List* lins_aux(void* data, int index, List* restrict list);

/**
 * @since 0.1.0
 * @brief Inverse le sens d'une liste.
 * @param dir La direction de parcours de la liste.
 * @param curr Le noeud courant.
 * @param list La liste à modifier.
 * @return @p list.
 */
static List* lrev_aux(int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Élimine des noeuds de la liste.
 * @param match Fonction renvoyant @c true si le noeud courant doit
 * être conservé.
 * @param dir La direction de parcours de la liste.
 * @param curr Le noeud courant.
 * @param list La liste à modifier.
 * @return @p list.
 */
static List* lfilter_aux(lmatch match, int dir, Node* curr, List* restrict list);

// clang-format off

/*******************************************************************************
 * @}
 * @name Recherche de noeuds et données, et informations sur les listes
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Donne le noeud à l'index donné.
 * @param index L'index du noeud recherché.
 * @param curr Le noeud courant.
 * @param list La liste.
 * @return Le noeud de @p list situé à l'@p index.
 */
static Node* lidx_aux(int index, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Compte le nombre d'occurence d'une donnée.
 * @param match Fonction renvoyant @c true si l'occurrence du noeud
 * courant doit être comptabilisée. Par défaut (@p match vaut @c
 * NULL), compte tous les noeuds de la liste.
 * @param count Le compte total.
 * @param dir La direction de parcours de la liste.
 * @param curr Le noeud courant.
 * @param list La liste.
 * @return Le nombre total de noeuds pour lesquels @p match renvoie @c
 * true.
 */
static int lcount_aux(lmatch match, int count, int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Renvoie le premier noeud pour lequel @p match renvoie
 * @c true.
 * @param match Fonction de filtrage.
 * @param dir La direction de parcours de la liste.
 * @param curr Le noeud courant.
 * @param list La liste où chercher.
 * @return Le premier noeud pour lequel @p match donne @c true, ou
 * @c NULL si aucun noeud ne satisfait la condition.
 */
static Node* lmember_aux(lmatch match, int dir, Node* curr, List* restrict list);

/**
 * @since 0.1.0
 * @brief Indique si les deux listes sont équivalentes.
 * @param compare Fonction de comparaison des noeuds. @c NULL équivaut
 * à ne comparer que les pointeurs.
 * @param dir La direction de parcours de la liste.
 * @param a,b Les noeuds courants des listes à comparer.
 * @return @c true si tous les pointeurs de @p a et @p b sont
 * identiques ou si @p compare renvoie @c 0 pour tous les
 * noeuds. Sinon @c false.
 */
static bool leql_aux(lcomp compare, int dir, Node* a, Node* b);

/**
 * @since 0.1.0
 * @brief Indique si la liste est un palindrome.
 * @param compare Fonction de comparaison des noeuds. @c NULL équivaut
 * à ne comparer que les pointeurs.
 * @param curr Le noeud courant du parcours normal de la liste.
 * @param rcurr Le noeud courant du parcours à rebours de la liste.
 * @return @c true si la liste est un palindrome (@p compare renvoie
 * @c 0 pour les noeuds concernés, ou les pointeurs des noeuds si
 * @p compare est @c NULL). Sinon @c false.
 */
static bool lpalin_aux(lcomp compare, Node* curr, Node* rcurr);

/**
 * @since 0.1.0
 * @brief Indique si une liste est circulaire.
 * @param dir La direction de parcours.
 * @param hare Le noeud courant du parcours rapide.
 * @param turtle Le noeud courant du parcours lent.
 * @return Le noeud de tête de la portion circulaire de la liste,
 * sinon @c NULL si elle n'est pas circulaire.
 */
static Node* lcirc_aux(int dir, Node* hare, Node* turtle);

// clang-format off

/*******************************************************************************
 * @}
 * @name Conversion des listes pour affichage
 * @{
 *******************************************************************************/
// clang-format on

/**
 * @def LISTSFMT
 * @since 0.1.0
 * @brief Format d'indication des listes lors de leur impression.
 * @param s Le contenu de la liste.
 */
#define LISTSFMT "(%s)\n"

/**
 * @def LISTSSEP
 * @since 0.1.0
 * @brief Séparateur par défaut des éléments d'une liste à afficher.
 */
#define LISTSSEP ", "

/**
 * @since 0.1.0
 * @brief Renvoie une description d'un noeud.
 * @alert Utilise malloc.
 * @note Fonction utilisée par défaut si la fonction d'impression
 * donnée à lprt() est @c NULL.
 * @param index L'index du noeud courant.
 * @param data La donnée du noeud courant.
 * @return Une description de la donnée du noeud courant.
 */
static char* ldefaultprt(int index, const void* data);

// clang-format off

/*******************************************************************************
 * @}
 * Implémentation
 *
 * Allocation mémoire.
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
 * Construction de listes
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
    // On inverse l'entrée (push quand on traverse à rebourd) pour
    // conserver l'ordre de la liste.
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
 * Accès aux données
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
 * Modification de listes
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
 * Informations sur les listes.
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
