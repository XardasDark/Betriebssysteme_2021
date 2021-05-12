/*
 *	File:		queue.c
 *	Description:	Abstrakter Datentyp Schlange (fuer int-Werte)
 *
 *	Elmar Ludwig,  Universitaet Osnabrueck
 *	Monday, 14. June 1999
 *
 *	-----
 *
 *	Angepasst von Ruben Grest und Ruben Hockemeyer (21.10.2014).
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>		/* malloc, free */

struct Queue			/* die interne Datenstruktur */
{
    struct Node *head;		/* erstes Element oder NULL */
    struct Node *tail;		/* letztes Element */
};

typedef struct Queue * Queue;

typedef void * element;

struct Node			/* Element der verketteten Liste */
{
    struct Node *next;		/* Zeiger auf naechstes Element */
    element value;		/* Wert dieses Elements */
};

Queue queue_create (void);
int queue_empty (Queue queue);
int queue_delete (Queue queue);
void queue_destroy (Queue queue);
int queue_insert (Queue queue, element value);
element queue_head (Queue queue);

#endif //QUEUE_H