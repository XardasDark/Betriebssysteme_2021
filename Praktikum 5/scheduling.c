#include "scheduling.h"

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

int compareJobPrio(JobNode *job1, JobNode *job2, void *userData)
{
    if (job1->priority < job2->priority)
    {
        return 1;
    }
    else if (job1->priority > job2->priority)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int compareJobProcessingTime(JobNode *job1, JobNode *job2, void *userData)
{
    if (job1->processingTime < job2->processingTime)
    {
        return -1;
    }
    else if (job1->processingTime > job2->processingTime)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static JobList list;
int currTime;
int residenceTime;
int listLength;
double averageResidenceTime;

void jobListIni()
{
    // Liste initialisieren
    List_init(&list);

    // Jobs erstellen
    JobNode *a = LIST_NEW_NODE(JobNode);
    a->name = "A";
    a->processingTime = 30;
    a->priority = 2;

    JobNode *b = LIST_NEW_NODE(JobNode);
    b->name = "B";
    b->processingTime = 20;
    b->priority = 1;

    JobNode *c = LIST_NEW_NODE(JobNode);
    c->name = "C";
    c->processingTime = 25;
    c->priority = 5;

    JobNode *d = LIST_NEW_NODE(JobNode);
    d->name = "D";
    d->processingTime = 28;
    d->priority = 4;

    JobNode *e = LIST_NEW_NODE(JobNode);
    e->name = "E";
    e->processingTime = 18;
    e->priority = 3;

    // Jobs zur Liste hinzufügen
    List_append(&list, a);
    List_append(&list, b);
    List_append(&list, c);
    List_append(&list, d);
    List_append(&list, e);
}

void firstComeFirstServed()
{
    currTime = 0;
    residenceTime = 0;
    listLength = List_count(&list);
    int i = 0;

    JobNode *current = list.head;
    printf("Start (Aktuelle Zeit: %d).\n", currTime);
    while (current != NULL)
    {
        currTime += current->processingTime;
        residenceTime += (listLength - i) * current->processingTime;
        printf("%s wurde abgearbeitet (Aktuelle Zeit: %d).\n", current->name, currTime);
        JobNode *prev = current;
        current = current->next;
        List_remove(&list, prev);
        i++;
    }

    averageResidenceTime = (double)residenceTime / listLength;
    printf("Mittlere Verweilzeit: %.2f\n\n", averageResidenceTime);
}

void shortestJobFirst()
{
    printf("\nShortest Job First:\n");
    List_sort(&list, (ListNodeCompareFunction)compareJobProcessingTime, NULL);
    firstComeFirstServed();
}

void prioScheduling()
{
    printf("\nPrioritätsgesteuertes Scheduling:\n");
    List_sort(&list, (ListNodeCompareFunction)compareJobPrio, NULL);
    firstComeFirstServed();
}

void roundRobin()
{
    currTime = 0;
    residenceTime = 0;
    listLength = List_count(&list);
    int parts = 0;
    printf("\nRound Robin:\n");
    while (!List_isEmpty(&list))
    {
        parts++;
        printf("Es wird an den Jobs zu folgenden Anteilen gearbeitet (Aktuelle Zeit: %d).\n", currTime);

        JobNode *current = list.head;
        while (current != NULL)
        {
            currTime++;
            current->processingTime--;
            printf("Es wurde %ds an %s gearbeitet.", parts, current->name);
            if (current->processingTime == 0)
            {
                residenceTime += currTime;
                printf(" | %s wurde abgearbeitet.\n", current->name);
                JobNode *prev = current;
                current = current->next;
                List_remove(&list, prev);
            }
            else
            {
                printf("\n");
                current = current->next;
            }
        }
        printf("\n");
    }
    printf("Alle Jobs abgearbeitet (Aktuelle Zeit: %d).\n\n", currTime);
    averageResidenceTime = (double)residenceTime / listLength;
    printf("Mittlere Verweilzeit: %.2f\n\n", averageResidenceTime);
}

float getSteps()
{
    JobNode *current = list.head;
    float steps = current->processingTime / current->priority;
    while (current != NULL)
    {
        float nodeSteps = current->processingTime / current->priority;
        if (nodeSteps < steps)
        {
            steps = nodeSteps;
        }
        current = (JobNode *)current->next;
    }
    return steps;
}

void roundRobinPrio()
{
    int amount = list.count;
    int currTime = 0;
    int parts = 0;
    float totalTime = 0;
    float averageTime = 0;
    float currentSteps = 0;
    float timeLast = 0;
    printf("\nRound Robin Prio:\n");
    JobNode *current = list.head;
    while (list.count != 0)
    {

        printf("Es wird an den Jobs zu folgenden Anteilen gearbeitet (Aktuelle Zeit: %d).\n", currTime);
        currentSteps = getSteps(&list);
        timeLast = 0;
        int termCount = 0;
        while (current != NULL)
        {
            currTime += current->processingTime;
            parts = currentSteps * current->priority;
            current->processingTime -= parts;
            timeLast += parts;
            printf("Es wurde %ds an %s gearbeitet", parts, current->name);
            if (current->processingTime == 0)
            {
                JobNode *prev = current;
                printf(" | %s wurde erfolgreich abgearbeitet\n", current->name);
                List_remove(&list, prev);
                termCount++;
            }
            else
            {
                printf("\n");
            }
            current = (JobNode *)current->next;
        }
        printf("\n");
        totalTime = timeLast * termCount + totalTime;
        averageTime += totalTime;
        current = list.head;
    }
    printf("\nAlle Jobs abgearbeitet\n\n");
    averageTime /= (float)amount;
    printf("Mittlere Verweilzeit:%.2fs\n", averageTime);
}

int main(void)
{

    jobListIni();
    firstComeFirstServed();

    jobListIni();
    shortestJobFirst();

    jobListIni();
    prioScheduling();

    jobListIni();
    roundRobin();

    jobListIni();
    roundRobinPrio();

    return 0;
}
