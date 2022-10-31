// C code for linked list merged sort
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "mergesort.h"

/* sorts the linked list by changing next pointers (not memory_usage) */
void MergeSort(struct struct_process** headRef)
{
    struct struct_process* head = *headRef;
    struct struct_process* a;
    struct struct_process* b;
 
    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }
 
    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b);
 
    /* Recursively sort the sublists */
    MergeSort(&a);
    MergeSort(&b);
 
    /* answer = merge the two sorted lists together */
    *headRef = SortedMerge(a, b);
}
 
/* See https:// www.geeksforgeeks.org/?p=3622 for details of this
function */
struct_process* SortedMerge(struct struct_process* a, struct struct_process* b)
{
    struct struct_process* result = NULL;
 
    /* Base cases */
    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);
 
    /* Pick either a or b, and recur */
    if (a->memory_usage <= b->memory_usage) {
        result = a;
        result->next = SortedMerge(a->next, b);
    }
    else {
        result = b;
        result->next = SortedMerge(a, b->next);
    }
    return (result);
}
 
/* UTILITY FUNCTIONS */
/* Split the struct_processs of the given list into front and back halves,
    and return the two lists using the reference parameters.
    If the length is odd, the extra struct_process should go in the front list.
    Uses the fast/slow pointer strategy. */
void FrontBackSplit(struct struct_process* source,
                    struct struct_process** frontRef, struct struct_process** backRef)
{
    struct struct_process* fast;
    struct struct_process* slow;
    slow = source;
    fast = source->next;
 
    /* Advance 'fast' two struct_processs, and advance 'slow' one struct_process */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}