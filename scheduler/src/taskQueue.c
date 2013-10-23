/*
 * taskQueue.c
 *
 *  Created on: Aug 30, 2010
 *      Author: freek
 */

/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "taskQueue.h"
#ifdef MULTI_MONKS
#include <pthread.h>
pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#include "myriaChapOut.h"

#define TASKPOOL_SIZE 15

void (*notifyFunctionPointer) (void);
Task *taskQueue;

Task taskPool[TASKPOOL_SIZE];
uint8_t emptyTaskCount;

void notifyFunctionDummy (void) {

}

int8_t initTaskQueue (void (*notification) (void)) {
	uint8_t i;

	for(i=0; i<TASKPOOL_SIZE; i++) {
		taskPool[i].context		= NULL;
		taskPool[i].function 	= NULL;
		taskPool[i].identifier 	= 0;
		taskPool[i].next 		= NULL;
		taskPool[i].priority 	= 0;
		taskPool[i].schedulee 	= NULL;
		taskPool[i].scheduler 	= NULL;
	}
	emptyTaskCount = TASKPOOL_SIZE;

	taskQueue = NULL;
	if(notification != NULL) {
		notifyFunctionPointer = notification;
	}
	else {
		notifyFunctionPointer = &notifyFunctionDummy;
	}
	Log(LOG_SCHEDULER, "[iTQ] %u bytes\n", TASKPOOL_SIZE * sizeof(Task));

	return 1;
}

Task *getEmptyTask (void) {
	uint8_t i;

	if(emptyTaskCount == 0) {
		return NULL;
	}

	for(i=0; i<TASKPOOL_SIZE; i++) {
		if(taskPool[i].context == NULL && taskPool[i].function == NULL  && taskPool[i].identifier == 0 && taskPool[i].next == NULL &&
		   taskPool[i].priority == 0   && taskPool[i].schedulee == NULL && taskPool[i].scheduler == NULL) {
			break;
		}
	}

	if(i < TASKPOOL_SIZE) {
		emptyTaskCount --;
//		Log(LOG_SCHEDULER, "[getEmptyTask] Handing out Task spot %u, %u spots left\n", i, emptyTaskCount);
		return &(taskPool[i]);
	}
	else
		return NULL;
}

int8_t scheduleTask (Task *T) {
#ifdef MULTI_MONKS
	pthread_mutex_lock(&task_mutex);
#endif
	uint8_t notify = 0;

	if(taskQueue == NULL) {
//		Log(LOG_SCHEDULER, "[sT] id %x first\n", T->identifier);
		taskQueue = T;
		notify = 1;
	}
	else if(T->priority < taskQueue->priority) {
//		Log(LOG_SCHEDULER, "[sT] id %x front\n", T->identifier);
		T->next = taskQueue;
		taskQueue = T;
	}
	else {
		Task *ptr = taskQueue;
		while(ptr->next != NULL) {
			if(T->priority < ptr->next->priority) {
				T->next = ptr->next;
				ptr->next = T;
//				Log(LOG_SCHEDULER, "[sT] id %x queue\n", T->identifier);
				break;
			}
			else
				ptr = ptr->next;
		}
		if(ptr->next == NULL) {
			ptr->next = T;
//			Log(LOG_SCHEDULER, "[sT] id %x rear\n", T->identifier);
		}
	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&task_mutex);
#endif

	if(notify)
		notifyFunctionPointer();

	return 1;
}

Task *retrieveTask () {
#ifdef MULTI_MONKS
	pthread_mutex_lock(&task_mutex);
#endif

	Task *T = taskQueue;
	if(T != NULL) {
		taskQueue = T->next;
		T->next = NULL;
		Log(LOG_SCHEDULER, "[rT] id %x\n", T->identifier);
	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&task_mutex);
#endif

	return T;
}

void taskCompleted (Task *T) {
	Log(LOG_SCHEDULER, "[tC] id %x\n", T->identifier);

	T->context		= NULL;
	T->function 	= NULL;
	T->identifier 	= 0;
	T->next 		= NULL;
	T->priority 	= 0;
	T->schedulee 	= NULL;
	T->scheduler 	= NULL;

	emptyTaskCount ++;
}

uint8_t isScheduled (uint16_t id) {
#ifdef MULTI_MONKS
	pthread_mutex_lock(&task_mutex);
#endif

	uint8_t returnValue = 0;

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&task_mutex);
#endif
	return returnValue;
}

/**
 * Purges Tasks where this component is scheduler or schedulee from the TaskQueue.
 */
void purgeTasks (void *comp) {
#ifdef MULTI_MONKS
	pthread_mutex_lock(&task_mutex);
#endif

	Task *T = taskQueue;
	Task **temp = &taskQueue;
	uint8_t i = 0;
	while(T != NULL) {
		if(T->schedulee == comp || T->scheduler == comp) {
			Log(LOG_SCHEDULER, "[pT] id %x\n", T->identifier);
			i ++;
			*temp = T->next;
			returnMessage((Message *) T->context);
			taskCompleted(T);
			T = *temp;
		}
		else {
			temp = &(T->next);
			T = T->next;
		}
	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&task_mutex);
#endif
}

void getTaskState () {
	Log(LOG_STORAGE, "Tasks: %d\n", emptyTaskCount);
}
