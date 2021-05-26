#include <stdlib.h>
#include <stdio.h>

#include "lists.h"
typedef struct Job
{
    LIST_NODE_HEADER(struct Job);
    char *name;
    int processingTime;
    int priority;
} JobNode;


typedef struct
{
    LIST_HEADER(JobNode);
} JobList;



static JobList list;

int main(void)
{
    // Liste initialisieren
    List_init(&list);

    // Studenten erstellen
    JobNode *a = LIST_NEW_NODE(JobNode);
    a->name =  "A";
    a->processingTime = 30;
    a->priority = 2;

    JobNode *b = LIST_NEW_NODE(JobNode);
    b->name =  "B";
    b->processingTime = 20;
    b->priority = 1;

    JobNode *c = LIST_NEW_NODE(JobNode);
    c->name =  "C";
    c->processingTime = 25;
    c->priority = 5;

    JobNode *d = LIST_NEW_NODE(JobNode);
    d->name =  "D";
    d->processingTime = 28;
    d->priority = 4;

    JobNode *e = LIST_NEW_NODE(JobNode);
    e->name =  "E";
    e->processingTime = 18;
    e->priority = 3;

    // Studenten zur Liste hinzufügen
    List_append(&list, a);
    List_append(&list, b);
    List_append(&list, c);
    List_append(&list, d);
    List_append(&list, e);

    
    List_remove(&list, b);
    printf("Anzahl der Studenten: %ld", List_count(&list));

    

    // Jedes Element der Liste ausgeben
    JobNode *current = list.head;
    while (current != NULL)
    {
        printf("%s - %d\n", current->name, current->processingTime);
        current = current->next;
    }
 
    // Alle Nodes löschen (reicht nur für Nodes, die keine Pointer 
    // auf allozierten Speicher besitzen)
    List_done(&list, NULL, NULL);
    return 0;
}



