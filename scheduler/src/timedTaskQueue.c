/*
 * timedTaskQueue.c
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

#include "timedTaskQueue.h"
#ifdef MULTI_MONKS
#include <pthread.h>
pthread_mutex_t timed_task_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#include "myriaChapOut.h"

#define TIMEDTASKPOOL_SIZE 5

void (*setTimerFunctionPointer) (Time *t);
Time *(*getTimerFunctionPointer) (void);
TimedTask *timeQueue;

TimedTask timedTaskPool[TIMEDTASKPOOL_SIZE];
uint8_t emptyTimedTaskCount;

int8_t initTimedTaskQueue (void (*setTimerFunction) (Time *t), Time *(*getTimerFunction) (void), void (*notification) (void)) {
	uint8_t i;

	if(setTimerFunction == NULL || getTimerFunction == NULL) {
		Log(LOG_SCHEDULER, "[iTTQ] One of the timer functions is missing!\n");
		return -1;
	}
	else {
		setTimerFunctionPointer = setTimerFunction;
		getTimerFunctionPointer = getTimerFunction;
	}

	for(i=0; i<TIMEDTASKPOOL_SIZE; i++) {
		timedTaskPool[i].T 		= NULL;
		timedTaskPool[i].next 	= NULL;
		timedTaskPool[i].t.sec 	= 0;
		timedTaskPool[i].t.msec = 0;
	}
	emptyTimedTaskCount = TIMEDTASKPOOL_SIZE;

	timeQueue = NULL;

	Log(LOG_SCHEDULER, "[iTTQ] %u bytes\n", TIMEDTASKPOOL_SIZE * sizeof(TimedTask));

	initTaskQueue(notification);

	return 1;
}

TimedTask *getEmptyTimedTask (void) {
	uint8_t i;

	if(emptyTimedTaskCount == 0)
		return NULL;

	for(i=0; i<TIMEDTASKPOOL_SIZE; i++) {
		if(timedTaskPool[i].T == NULL && timedTaskPool[i].t.sec == 0 && timedTaskPool[i].t.msec == 0 && timedTaskPool[i].next == NULL)
			break;
	}

	if(i < TIMEDTASKPOOL_SIZE) {
		timedTaskPool[i].T = getEmptyTask();
		if(timedTaskPool[i].T == NULL)
			return NULL;
		emptyTimedTaskCount --;
//		Log(LOG_SCHEDULER, "[getEmptyTimedTask] Handing out TimedTask spot %u, %u left\n", i, emptyTimedTaskCount);
		return &(timedTaskPool[i]);
	}
	else {
		return NULL;
	}
}

void returnTimedTask (TimedTask *TT) {
	Log(LOG_SCHEDULER, "[rTT] \n");

	TT->T 		= NULL;
	TT->next 	= NULL;
	TT->t.sec 	= 0;
	TT->t.msec 	= 0;
	emptyTimedTaskCount ++;
}

int8_t scheduleTimedTask (TimedTask *TT) {
#if defined MULTI_MONKS || defined INTERRUPT_ON
	pthread_mutex_lock(&timed_task_mutex);
#endif

	TimedTask *ptr;
	Time *t;

	if(timeQueue == NULL) {
		timeQueue = TT;
		setTimerFunctionPointer(&(TT->t));
		Log(LOG_SCHEDULER, "[sTT] First t = %lu.%u, id = %x\n", TT->t.sec, TT->t.msec, TT->T->identifier);
	}
	else {
		ptr = timeQueue;
		t = getTimerFunctionPointer();
		Log(LOG_SCHEDULER, "[sTT] t = %lu.%u, cT: %lu.%u\n", TT->t.sec, TT->t.msec, t->sec, t->msec);
		if(sooner(&(TT->t), t)) {
			time_is(&(ptr->t), t);
			time_minus(&(ptr->t), &(TT->t));
			TT->next = ptr;
			timeQueue = TT;
			setTimerFunctionPointer(&(TT->t));
			Log(LOG_SCHEDULER, "[sTT] t = %lu.%u id = %x front\n", TT->t.sec, TT->t.msec, TT->T->identifier);
		}

		else {
			time_minus(&(TT->t), t);
			while(ptr->next != NULL) {
				if(sooner(&(TT->t), &(ptr->next->t)))
					break;
				else {
					ptr = ptr->next;
					time_minus(&(TT->t), &(ptr->t));
				}
			}
			TT->next = ptr->next;
			ptr->next = TT;
			if(TT->next != NULL) {
				time_minus(&(TT->next->t),&(TT->t));
			}
			Log(LOG_SCHEDULER, "[sTT] id = %x t = %lu.%u queue\n", TT->T->identifier, TT->t.sec, TT->t.msec);
		}
	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&timed_task_mutex);
#endif

	return 1;
}

uint8_t giveTimeTick (Time *time_spent) {
	Time t_spent;
	uint8_t returnValue = 0;

	t_spent.sec = time_spent->sec;
	t_spent.msec = time_spent->msec;

#if defined MULTI_MONKS || defined INTERRUPT_ON
	pthread_mutex_lock(&timed_task_mutex);
#endif

	if(timeQueue == NULL) {
		Log(LOG_SCHEDULER, "[gTT] empty\n");
	}
	else if(sooner(&t_spent, &(timeQueue->t))) {
		time_minus(&(timeQueue->t), &t_spent);
		setTimerFunctionPointer(&(timeQueue->t));
		Log(LOG_SCHEDULER, "[gTT] %lu.%u\n", timeQueue->t.sec, timeQueue->t.msec);
	}
	else {
		while(!sooner(&t_spent, &(timeQueue->t))) {
			TimedTask *toSchedule = timeQueue;

			time_minus(&t_spent, &(timeQueue->t));

			timeQueue = timeQueue->next;
			toSchedule->next = NULL;
			scheduleTask(toSchedule->T);
			returnTimedTask(toSchedule);

			returnValue ++;
			Log(LOG_SCHEDULER, "[gTT] nr %d sched\n", returnValue);

			if(timeQueue == NULL) {
				break;
			}
		}
		if(timeQueue != NULL) {
			Log(LOG_SCHEDULER, "[gTT] id %x, in %lu.%u\n", timeQueue->T->identifier, timeQueue->t.sec, timeQueue->t.msec);
			time_minus(&(timeQueue->t), &t_spent);
			setTimerFunctionPointer(&(timeQueue->t));
		}
 	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&timed_task_mutex);
#endif

	return returnValue;
}

uint8_t isTimedScheduled (uint16_t id) {
#if defined MULTI_MONKS || defined INTERRUPT_ON
	pthread_mutex_lock(&timed_task_mutex);
#endif

	uint8_t returnValue = 0;

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&timed_task_mutex);
#endif

	return returnValue;
}

void purgeTimedTasks (void *comp) {
#ifdef MULTI_MONKS
	pthread_mutex_lock(&timed_task_mutex);
#endif

	TimedTask *TT = timeQueue;
	TimedTask **temp = &timeQueue;
	Time interval;
	uint8_t i = 0;
	interval.sec = 0;
	interval.msec = 0;
	while(TT != NULL) {
		if(TT->T->schedulee == comp || TT->T->scheduler == comp) {
			Log(LOG_SCHEDULER, "[pTT] id %x purged!\n", TT->T->identifier);
			i++;
			time_plus(&interval, &TT->t);

			if(TT->next != NULL) {
				Log(LOG_SCHEDULER, "[pTT] Int+: %lu.%u.\n", interval.sec, interval.msec);
				time_plus(&(TT->next->t), &interval);
			}
			*temp = TT->next;
			returnMessage((Message *) TT->T->context);
			taskCompleted(TT->T);
			returnTimedTask(TT);
			TT = *temp;
		}
		else {
			interval.sec = TT->t.sec;
			interval.msec = TT->t.msec;
			temp = &(TT->next);
			TT = TT->next;
		}
	}

#ifdef MULTI_MONKS
	pthread_mutex_unlock(&timed_task_mutex);
#endif
}

void getTimedTaskState() {
	Log(LOG_STORAGE, "TimedTask: %d\n", emptyTimedTaskCount);
}

