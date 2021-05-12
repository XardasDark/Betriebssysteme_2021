	#include "scheduler.h"

void firstComeFirstServed(listProcess *list) {
	printf("\nFirst Come First Served:\n");
	runJob(list);
}
void shortestJobFirst(listProcess *list){
	List_sort(list,(ListNodeCompareFunction)cmpTime, NULL);
	printf("\nShortest Job First:\n");
	runJob(list);
}
void prioScheduling(listProcess *list){
	List_sort(list,(ListNodeCompareFunction)cmpPrio, NULL);
	printf("\nPrio Scheduling:\n");
	runJob(list);
}
void appendJob(listProcess *list, char processname[50], int time, int priority) {
	job *process = LIST_NEW_NODE(job);
    strcpy(process->name, processname);
    process->time = time;
    process->priority = priority;
	List_append(list, process);
}
void roundRobinPrio(listProcess *list){
	int amount = list->count;
	float totalTime = 0;
	float averageTime = 0;
	float termTime = 0;
	float currentSteps = 0;
	float timeLast = 0;
	printf("\nRound Robin Prio:");
	job *node = list->head;
	while(list->count != 0){
		printf("\nEs wird an den Jobs zu folgenden Anteilen gearbeitet:\n");
//		termTime = getTime(list);
		currentSteps = getSteps(list);
		timeLast = 0;
		int termCount = 0;
		while(node != NULL){
			termTime = currentSteps * node->priority;
			node->time -= termTime;
			timeLast += termTime;
			printf("Es wurde %.2fs an %s gearbeitet\n",termTime, node->name);
			if(node->time == 0){
//				printf("%s wurde erfolgreich abgearbeitet\n",node->name);
				List_remove(list, node);
				termCount++;
			}
			node = (job*) node->next;
		}
		totalTime = timeLast * termCount + totalTime;
		averageTime += totalTime;
		node = list->head;
	}
	averageTime /= (float) amount;
	printf("Mittlere Verweilzeit:%.2f\n",averageTime);
}
float getTime(listProcess *list){
	job *node = list->head;
	float value = node->time;
	while(node != NULL){
		if(node->time < value){
			value = node->time;
		}
		node = (job*) node->next;
	}
	return value;
}
float getSteps(listProcess *list){
	job *node = list->head;
	float steps = node->time / node->priority;
	while(node != NULL){
		float nodeSteps = node->time / node->priority;
		if(nodeSteps < steps){
			steps = nodeSteps;
		}
		node = (job*) node->next;
	}
	return steps;
}

void roundRobin(listProcess *list){
	printf("\n\nRound Robin (keine Prioritaeten): Setze alle Prioritaeten auf 1...");
	job *node = list->head;
	while(node != NULL){
		node->priority = 1;
		node = (job*) node->next;
	}
	roundRobinPrio(list);
}

void runJob(listProcess *list){
	job *node = list->head;
	int amount = list->count;
	float currentTime = 0;
	float totalTime = 0;
	float averageTime = 0;

	while(node != NULL) {
		currentTime += node->time;
		totalTime += node->time * list->count;
		//Muss an sich nicht gemacht werden, symbolisiert aber, dass die Jobzeit nach dem Arbeiten auf 0 ist
		node->time -= node->time;
		printf("%s wurde abgearbeitet (Akutelle Zeit: %.2f).\n", node->name, currentTime);
		node = List_remove(list, node);
	}
	averageTime = totalTime / amount;
	printf("Mittlere Verweilzeit: %.2f\n", averageTime);
}
int cmpPrio(job *node1, job *node2, void *dummy){
	return node2->priority - node1->priority;
}

int cmpTime(job *node1, job *node2, void *dummy){
	return node1->time - node2->time;
}
void initJobs(listProcess *list){
	appendJob(list, "A", 30, 2);
	appendJob(list, "B", 20, 1);
	appendJob(list, "C", 25, 5);
	appendJob(list, "D", 28, 4);
	appendJob(list, "E", 18, 3);

//	appendJob(list, "A", 8, 2);
//	appendJob(list, "B", 4, 1);
//	appendJob(list, "C", 20, 5);
//	appendJob(list, "D", 16, 4);
//	appendJob(list, "E", 15, 3);

//	appendJob(list, "A", 10, 10);
//	appendJob(list, "B", 10, 1);
//	appendJob(list, "C", 10, 3);
//	appendJob(list, "D", 10, 6);
//	appendJob(list, "E", 10, 15);

//	appendJob(list, "A", 1, 2);
//	appendJob(list, "B", 1, 1);
//	appendJob(list, "C", 1, 5);
//	appendJob(list, "D", 1, 4);
//	appendJob(list, "E", 1, 3);



}
void printJob(listProcess *list){
	job *node = list->head;
	printf("\nJobs:\n");
	while(node != NULL){
		printf("Job:%s	Time:%.2f	Prio:%i\n",node->name, node->time, node->priority);
		node = (job*) node->next;
	}
}
int main() {
	listProcess list;
	List_init(&list);
	initJobs(&list);
	printJob(&list);
	firstComeFirstServed(&list);
	initJobs(&list);
	shortestJobFirst(&list);
	initJobs(&list);
	prioScheduling(&list);
	initJobs(&list);
	roundRobin(&list);
	initJobs(&list);
	roundRobinPrio(&list);
	return 0;
}
