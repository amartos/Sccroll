/**
 * @file        lists.c
 * @version     0.1.0
 * @brief       Lists unit tests source code.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2023 Alexandre Martos <contact@amartos.fr>
 * @license     MIT License
 */

#include "sccroll.h"

// clang-format off

/*******************************************************************************
 * Preparation
 *******************************************************************************/
// clang-format on

static const char* foobar      = "foobar";
static const char* bizbuz      = "bizbuz";
static const char* aliceandbob = "aliceandbob";

int lptrcmp(const void* a, const void* b)
{
    if (a == b)     return 0;
    else if (!a)    return -2;
    else if (!b)    return 2;
    else if (a < b) return -1;
    return 1;
}

int lstrcmp(const void* a, const void* b)
{
    int ptrs = lptrcmp(a,b);
    if (abs(ptrs) != 1) return ptrs;
    return strcmp((const char*)a, (const char*)b);
}

bool lnull(const void* data) { return !data; }

bool lfoobar(const void* data)
{
    if (!data) return false;
    return data == foobar;
}

// clang-format off

/*******************************************************************************
 * Tests
 *******************************************************************************/
// clang-format on

void tests_link(void)
{
    Node a = {0};
    Node b = {0};
    Node c = {0};

    assert(!llink(NULL, NULL));
    assert(llink(&a, NULL) == &a);
    assert(a.prev == NULL && a.next == NULL);
    assert(llink(NULL, &a) == &a);
    assert(a.prev == NULL && a.next == NULL);

    assert(!lunlink(NULL, NULL));
    assert(lunlink(&a, NULL) == &a);
    assert(a.prev == NULL && a.next == NULL);
    assert(lunlink(NULL, &a) == &a);
    assert(a.prev == NULL && a.next == NULL);

    // #a
    assert(llink(&a, &a) == &a);
    assert(a.prev == &a && a.next == &a);

    // #a->b
    assert(llink(&a, &b) == &a);
    assert(a.prev == &a && a.next == &b);
    assert(b.prev == &a && b.next == NULL);

    // #a->b
    assert(llink(&b, &a) == &b);
    assert(a.prev == &b && a.next == &b);
    assert(b.prev == &a && b.next == &a);

    // a b
    assert(lunlink(&a, &b) == &a);
    assert(a.prev == NULL && a.next == NULL);
    assert(b.prev == NULL && b.next == NULL);

    // a->b
    assert(llink(&a, &b) == &a);
    assert(a.prev == NULL && a.next == &b);
    assert(b.prev == &a && b.next == NULL);

    // a->b->c
    assert(llink(&b, &c) == &b);
    assert(a.prev == NULL && a.next == &b);
    assert(b.prev == &a && b.next == &c);
    assert(c.prev == &b && c.next == NULL);

    // a->b->c
    assert(lunlink(&a, &c) == &a);
    assert(a.prev == NULL && a.next == &b);
    assert(b.prev == &a && b.next == &c);
    assert(c.prev == &b && c.next == NULL);

    // a->b->c
    assert(lunlink(&c, &a) == &c);
    assert(a.prev == NULL && a.next == &b);
    assert(b.prev == &a && b.next == &c);
    assert(c.prev == &b && c.next == NULL);

    //  a-> c
    //  ^-b-^
    assert(llink(&a, &c) == &a);
    assert(a.prev == NULL && a.next == &c);
    assert(b.prev == &a && b.next == &c);
    assert(c.prev == &a && c.next == NULL);

    // a->c
    // ^b
    assert(lunlink(&c, &b) == &c);
    assert(a.prev == NULL && a.next == &c);
    assert(b.prev == &a && b.next == NULL);
    assert(c.prev == &a && c.next == NULL);
}

void tests_pushappend(void)
{
    List* list = lpush((void*)foobar, NULL);
    assert(list->len == 1);
    assert(list->head == list->tail);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);

    assert(list == lpush(NULL, list));
    assert(list->len == 2);
    assert(list->head->data == NULL);
    assert(list->tail->data == foobar);
    assert(list->tail->prev == list->head);
    assert(list->tail->next == NULL);
    assert(list->head->next == list->tail);
    assert(list->head->prev == NULL);

    Node* head = list->head;
    Node* tail = list->tail;
    assert(list == lappend((void*)bizbuz, list));
    assert(list->len == 3);
    assert(list->head == head);
    assert(list->tail != tail);

    assert(list->tail->data == bizbuz);
    assert(list->tail->prev == tail);
    assert(list->tail->next == NULL);
    assert(list->head->next == tail);
    assert(list->head->prev == NULL);

    lfree(list);
    lfree(NULL); // test de la capacité
}

void tests_dup(void)
{
    assert(!ldup(NULL));

    // NULL->aliceandbob->bizbuz->foobar
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    lpush(NULL, test);

    List* copy = ldup(test);
    assert(test != copy);

    Node *curr = test->head, *cpcurr = copy->head;
    while(curr && cpcurr)
    {
        assert(curr != cpcurr);
        assert(curr->data == cpcurr->data);
        curr   = curr->next;
        cpcurr = cpcurr->next;
    }

    lfree(test);
    lfree(copy);
}

void tests_insert(void)
{
    // aliceandbob->bizbuz->foobar
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    assert(test->len == 3);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == bizbuz);
    assert(test->head->next->next->data == foobar);
    assert(test->head->next->next == test->tail);

    // NULL->aliceandbob->bizbuz->foobar
    assert(lins(NULL, 0, test) == test);
    assert(test->len == 4);
    assert(test->head->data == NULL);

    // NULL->foobar->aliceandbob->bizbuz->foobar
    assert(lins((void*)foobar, 1, test) == test);
    assert(test->len == 5);
    assert(test->head->data == NULL);
    assert(test->head->next->data == foobar);
    assert(test->head->next != test->tail);
    assert(test->head->next->data == test->tail->data);

    // NULL->foobar->foobar->aliceandbob->bizbuz->foobar
    assert(lins((void*)foobar, 2, test) == test);
    assert(test->len == 6);
    assert(test->head->prev == NULL);
    assert(test->head->data == NULL);
    assert(test->head->next->data == foobar);
    assert(test->head->next->next->data == foobar);
    assert(test->head->next->next->next->data == aliceandbob);
    assert(test->head->next->next->next->next->data == bizbuz);
    assert(test->head->next->next->next->next->next->data == foobar);
    assert(test->head->next->next->next->next->next->next == NULL);

    // NULL->foobar->foobar->aliceandbob->bizbuz->foobar->bizbuz
    assert(lins((void*)bizbuz, -1, test) == test);
    assert(test->len == 7);
    assert(test->head->data == NULL);
    assert(test->tail->data == bizbuz);

    // NULL->foobar->foobar->aliceandbob->bizbuz->foobar->bizbuz->bizbuz
    Node* tail = test->tail;
    assert(lins((void*)bizbuz, -2, test) == test);
    assert(test->len == 8);
    assert(test->head->data == NULL);
    assert(test->tail->data == bizbuz);
    assert(test->tail->prev->data == bizbuz);
    assert(test->tail == tail);
    assert(test->tail->prev != tail);

    // NULL->foobar->foobar->aliceandbob->bizbuz->foobar->bizbuz->bizbuz->NULL
    assert(lins(NULL, test->len, test) == test);
    assert(test->len == 9);
    assert(test->head->data == NULL);
    assert(test->tail->data == NULL);
    assert(test->head != test->tail);

    // aliceandbob->NULL->foobar->foobar->aliceandbob->bizbuz->foobar->bizbuz->bizbuz->NULL
    assert(lins((void*)aliceandbob, -(test->len+1), test) == test);
    assert(test->len == 10);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == NULL);

    // aliceandbob->NULL->foobar->foobar->aliceandbob->bizbuz->foobar->bizbuz->bizbuz->NULL
    // ->NULL*5->foobar
    tail = test->tail;
    int out = 16;
    assert(test->len < out);
    int diff = out - test->len;
    assert(lins((void*)foobar, out-1, test) == test);
    assert(test->len == out);
    assert(test->tail && test->tail != tail);
    assert(test->tail->data == foobar);
    assert(test->tail->prev != tail);
    assert(test->tail->prev->data == NULL);
    Node* curr = test->tail;
    int i      = 0;
    do { ++i, curr = curr->prev; } while(curr && curr != tail);
    assert(diff == i && i < test->len);

    lfree(test);
    test = NULL;

    // NULL...*6->NULL
    assert((test = lins(NULL, 6, test)));
    assert(test->len == 7);
    lfree(test);
}

void tests_pop(void)
{
    // lpop == lpopidx(0)
    // lpoplast == lpopidx(-1)
    List tmp = {0};
    assert(!lpopidx(0, NULL));
    assert(!lpopidx(10, NULL));
    assert(!lpopidx(-1, NULL));
    assert(!lpopidx(-42, NULL));
    assert(!lpopidx(0, &tmp));
    assert(!lpopidx(10, &tmp));
    assert(!lpopidx(-1, &tmp));
    assert(!lpopidx(-42, &tmp));

    // aliceandbob->bizbuz->foobar
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);

    // aliceandbob->foobar
    void* data = lpopidx(1, test);
    assert(data == bizbuz);
    assert(test->len == 2);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == foobar);
    assert(test->head->next == test->tail);

    // foobar
    data = lpopidx(-2, test);
    assert(data == aliceandbob);
    assert(test->len == 1);
    assert(test->head->data == foobar);
    assert(test->head == test->tail);
    lfree(test);

    // aliceandbob->bizbuz->foobar
    test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);

    // aliceandbob->bizbuz
    data = lpopidx(-1, test);
    assert(data == foobar);
    assert(test->len == 2);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == bizbuz);
    assert(test->head->next == test->tail);

    // bizbuz
    data = lpopidx(0, test);
    assert(data == aliceandbob);
    assert(test->len == 1);
    assert(test->head->data == bizbuz);
    assert(test->head == test->tail);
    lfree(test);
}

void tests_rev(void)
{
    assert(!lrev(NULL));

    // aliceandbob->bizbuz->foobar->NULL
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    lpush(NULL, test);

    List* saved = ldup(test);
    assert(lrev(test) == test);

    Node *curr = test->head, *rcurr = saved->tail;
    while(curr && rcurr)
    {
        assert(curr != rcurr);
        assert(curr->data == rcurr->data);
        curr  = curr->next;
        rcurr = rcurr->prev;
    }

    lfree(test);
    lfree(saved);
}

void tests_filter(void)
{
    assert(!lfilter(NULL, NULL));
    assert(!lfilter(lnull, NULL));

    List* test = lpush(NULL, NULL);
    lpush((void*)foobar, test);
    lpush((void*)bizbuz, test);
    lpush(NULL, test);
    lpush((void*)aliceandbob, test);
    lpush(NULL, test);
    assert(test->len == 6);

    List* save = ldup(test);
    lfilter(NULL, test);
    assert(lequal(lstrcmp, test, save));
    lfilter(lnull, test);
    assert(test->len == 3);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == bizbuz);
    assert(test->head->next->next->data == foobar);

    lfilter(lfoobar, test);
    assert(test->len == 2);
    assert(test->head->data == aliceandbob);
    assert(test->head->next->data == bizbuz);
    assert(test->head->next->next == NULL);

    lfree(test);
    lfree(save);
}

void tests_print(void)
{
    FILE* tmp = tmpfile();
    if (!tmp) err(EXIT_FAILURE, "could not open tmpfile");

    const char* testsep = "ZZZXXXYYY";
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    lpush(NULL, test);

    fprintf(stderr, "NOLIST >>>\n");
    lprint(NULL, NULL, NULL, NULL);
    lprint(stderr, NULL, NULL, NULL);
    lprint(NULL, lstrprt, NULL, NULL);
    lprint(NULL, NULL, testsep, NULL);
    lprint(NULL, NULL, NULL, test);
    lprint(stderr, lstrprt, NULL, NULL);
    lprint(stderr, NULL, testsep, NULL);
    lprint(stderr, lstrprt, testsep, NULL);
    fprintf(stderr, "       <<<\n");

    fprintf(stderr, "DEFAULT >>>\n");
    char expected[BUFSIZ] = {'(',};
    char perdata[256]     = {0};
    char buffer[BUFSIZ]   = {0};
    Node* curr            = test->head;
    int i = 0;
    while (curr) {
        sprintf(perdata, "[Node %i: %p]", i, curr->data);
        strcat(expected, perdata);
        curr = curr->next;
        ++i;
        if (curr) strcat(expected, ", ");
    }
    strcat(expected, ")\n");
    lprint(tmp, NULL, NULL, test);
    rewind(tmp);
    int s = fread(buffer, sizeof(char), BUFSIZ, tmp);
    if ((s < 0 || (size_t)s < strlen(expected)) && ferror(tmp))
        err(EXIT_FAILURE, "could not read tmpfile");
    assert(!strcmp(buffer, expected));
    fprintf(stderr, "        <<<\n");

    fprintf(stderr, "FUNC >>>\n");
    lprint(stderr, lstrprt, NULL, test);
    fprintf(stderr, "     <<<\n");

    fprintf(stderr, "BOTH >>>\n");
    lprint(stderr, lstrprt, testsep, test);
    fprintf(stderr, "     <<<\n");

    fprintf(stderr, "PRETTY >>>\n");
    lprint(stderr, lstrprt, ",\n ", test);
    fprintf(stderr, "     <<<\n");

    lfree(test);
}

void tests_eql(void)
{
    List* test = lpush((void*)foobar, NULL);
    List* same = test;
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    List* copy     = ldup(test);
    List* falsecpy = ldup(test);
    falsecpy->head->next->data = NULL;

    assert(leql(NULL, NULL));
    assert(!leql(NULL, test));
    assert(!leql(test, NULL));
    // (test, test) is a problem with restrict
    assert(leql(test, same));
    assert(!leql(test, copy));
    assert(!leql(copy, falsecpy));
    assert(!leql(test, falsecpy));

    assert(lequal(lstrcmp, NULL, NULL));
    assert(!lequal(lstrcmp, NULL, test));
    assert(!lequal(lstrcmp, test, NULL));
    assert(lequal(lstrcmp, test, same));
    assert(lequal(lstrcmp, test, copy));
    assert(!lequal(lstrcmp, test, falsecpy));

    lfree(test);
    lfree(copy);
    lfree(falsecpy);
}

void tests_infos(void)
{
    Node* first    = NULL;
    Node* second   = NULL;
    Node* third    = NULL;
    Node* fourth   = NULL;
    Node* last     = NULL;
    Node* idxed    = NULL;
    Node* nulld    = NULL;
    Node* rnulld   = NULL;
    Node* foobard  = NULL;
    Node* rfoobard = NULL;

    List* test     = NULL;
    assert(llen(test) == 0);
    assert((first = lfirst(test))  == NULL);
    assert((last  = llast(test))   == NULL);
    assert((idxed = lidx(2, test)) == NULL);
    assert(lcount(lnull, test)     == 0);
    assert(!(nulld  = lmember(lnull, test)));
    assert(!(rnulld = lrmember(lnull, test)));
    assert(nulld == rnulld);
    assert(lcount(lfoobar, test) == 0);
    assert(!(foobard  = lmember(lfoobar, test)));
    assert(!(rfoobard = lrmember(lfoobar, test)));
    assert(foobard == rfoobard);

    test = lpush(NULL, test);
    assert(llen(test) == 1);
    assert((first = lfirst(test)) != NULL);
    assert(first->data == NULL);
    assert((last = llast(test)) == first);
    assert((idxed = lidx(2, test)) == NULL);
    assert(lcount(lnull, test) == 1);
    assert((nulld  = lmember(lnull, test)) == first);
    assert((rnulld = lrmember(lnull, test)) == first);
    assert(nulld == rnulld);
    assert(lcount(lfoobar, test) == 0);
    assert(!(foobard  = lmember(lfoobar, test)));
    assert(!(rfoobard = lrmember(lfoobar, test)));
    assert(foobard == rfoobard);

    test = lpush(NULL, test);
    assert(llen(test) == 2);
    assert((second = lfirst(test)) != first);
    assert(second->data == NULL);
    assert((last = llast(test)) == first);
    assert((idxed = lidx(2, test)) == NULL);
    assert(lcount(lnull, test) == 2);
    assert((nulld  = lmember(lnull, test)) == second);
    assert((rnulld = lrmember(lnull, test)) == first);
    assert(nulld != rnulld);
    assert(lcount(lfoobar, test) == 0);
    assert(!(foobard  = lmember(lfoobar, test)));
    assert(!(rfoobard = lrmember(lfoobar, test)));
    assert(foobard == rfoobard);

    test = lpush((void*)foobar, test);
    assert(llen(test) == 3);
    assert((third = lfirst(test)) != first);
    assert(third != second);
    assert(third->data == foobar);
    assert((last = llast(test)) == first);
    assert((idxed = lidx(2, test)) == last);
    assert(lcount(lnull, test) == 2);
    assert((nulld  = lmember(lnull, test)) == second);
    assert((rnulld = lrmember(lnull, test)) == first);
    assert(nulld != rnulld);
    assert(lcount(lfoobar, test) == 1);
    assert((foobard  = lmember(lfoobar, test)) == third);
    assert((rfoobard = lrmember(lfoobar, test)) == third);
    assert(foobard == rfoobard);

    test = lpush((void*)aliceandbob, test);
    assert(llen(test) == 4);
    assert((fourth = lfirst(test)) != first);
    assert(fourth != second);
    assert(fourth != third);
    assert(fourth->data == aliceandbob);
    assert((last = llast(test)) == first);
    assert((idxed = lidx(2, test)) == second);
    assert(lcount(lnull, test) == 2);
    assert((nulld  = lmember(lnull, test)) == second);
    assert((rnulld = lrmember(lnull, test)) == first);
    assert(nulld != rnulld);
    assert(lcount(lfoobar, test) == 1);
    assert((foobard  = lmember(lfoobar, test)) == third);
    assert((rfoobard = lrmember(lfoobar, test)) == third);
    assert(foobard == rfoobard);

    lfree(test);
}

void tests_palindrome(void)
{
    assert(!lpalin(NULL, NULL));

    // NULL->aliceandbob->bizbuz->foobar
    List* test = lpush((void*)foobar, NULL);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);
    lpush(NULL, test);
    assert(!lpalin(NULL, test));
    assert(!lpalin(lstrcmp, test));

    // NULL->aliceandbob->bizbuz->foobar
    // ->foobar->bizbuz->aliceandbob->NULL
    List* copy = ldup(test);
    lrev(copy);
    Node* joint = test->tail;
    llink(test->tail, copy->head);
    test->tail = copy->tail;
    free(copy);
    assert(lpalin(lstrcmp, test));

    Node pivot = { .prev = joint, .next = joint->next, };
    llink(joint, &pivot);
    llink(&pivot, pivot.next);
    // NULL->aliceandbob->bizbuz->foobar
    // ->pivot
    // ->foobar->bizbuz->aliceandbob->NULL
    assert(lpalin(lstrcmp, test));
    llink(joint, pivot.next);

    lfree(test);
}

void tests_circular(void)
{
    List* test = NULL;
    List tmp   = {0};
    assert(!lcirc(test));
    assert(!lcirc(&tmp));

    test = lpush((void*)foobar, test);
    lpush((void*)bizbuz, test);
    lpush((void*)aliceandbob, test);

    // aliceandbob->bizbuz->foobar
    assert(!lcirc(test));

    // aliceandbob->bizbuz->#foobar
    llink(test->tail, test->head);
    assert(lcirc(test) == test->tail);
    lunlink(test->head, test->tail);
    assert(!lcirc(test));

    Node* node = test->head->next;
    // #aliceandbob
    llink(test->head, test->head);
    assert(lcirc(test) == test->head);
    lunlink(test->head, test->head);
    llink(test->head, node);
    assert(!lcirc(test));

    // aliceandbob->#bizbuz
    node = test->head->next;
    llink(node, test->head);
    assert(lcirc(test) == node);
    llink(node, test->tail);
    assert(!lcirc(test));

    lfree(test);

    test = lins(NULL, 20, NULL);
    assert(test && test->len == 21);
    assert(!lcirc(test));
    node = llink(lidx(17, test), lidx(4, test));
    assert(lcirc(test));
    lunlink(lidx(17, test), lidx(4, test));

    lfree(test);
}

// clang-format off

/*******************************************************************************
 * Execution
 *******************************************************************************/
// clang-format on

void tests(void)
{
    tests_link();
    tests_pushappend();
    tests_dup();
    tests_insert();
    tests_pop();
    tests_rev();
    tests_filter();
    tests_print();
    tests_eql();
    tests_infos();
    tests_circular();
    tests_palindrome();
}


int main(void)
{
    tests();
    sccroll_mockPredefined(tests);
    return 0;
}
