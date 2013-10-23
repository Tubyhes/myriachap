/*
 * schedulerInterface.c
 *
 *  Created on: Aug 31, 2010
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

#include "chapConfig.h"

#ifdef TINYOS

#include "schedulerInterface.h"
#include "boxStorage.h"
#include "myriaChapOut.h"

Time currentTimer;
Time FrameTime;

void setTimer (Time *t);
Time *getTimer ();

void initSchedulerInterface (uint32_t frame_time) {
	Log(LOG_SCHEDINTF, "[initSchedulerInterface] FrameTime: %lu, secs: %lu, usecs: %lu \n", frame_time, frame_time / 1000000, frame_time % 1000000);
	FrameTime.sec  = frame_time / 1000000;
	FrameTime.msec = (frame_time % 1000000) / 1000;
	Log(LOG_SCHEDINTF, "[initSchedulerInterface] FrameTime set to %lu.%u\n", FrameTime.sec, FrameTime.msec);
	initTimedTaskQueue(&setTimer, &getTimer, NULL);
}

/**
 * Send a message to a component
 * @param F  Pointer to a function to be executed, typically the inbox function of the receiving component
 * @param C1  Pointer to the component that should receive this message. Will be delivered as argument "schedulee" to function F.
 * @param C  Pointer to the message to be sent. Can be NULL. Will be delivered as argument "context" to function F.
 * @param C2  Pointer to the component that is sending this message. Can be NULL. Will be delivered as argument "scheduler" to function F.
 * @param p  Priority of this message. A higher priority will cause the message to take precedence over messages with a lower priority but the same due time.
 * @param t  Pointer to a Time struct denoting the delay with which this message must be delivered. Can be NULL, which means it should be delivered directly, and so the message will become a Task instead of a TimedTask.
 * @param id  The id of the message, that will allow components or the application to review whether a specific message is already scheduled or not.
 * @return  void
 */

void scheduleComponent (void (*F) (void *schedulee, void *context, void *scheduler), void *C1, void *C, void *C2, uint8_t p, Time *t, uint16_t id) {
	if(t != NULL) {
		TimedTask *TT = getEmptyTimedTask();
		TT->T->context = C;
		TT->T->function = F;
		TT->T->identifier = id;
		TT->T->priority = 0xFF - p;
		TT->T->schedulee = C1;
		TT->T->scheduler = C2;
		TT->t.sec = t->sec;
		TT->t.msec = t->msec;
		Log(LOG_SCHEDINTF, "[scheduleComponent] Scheduling Component as TimedTask with t %lu.%u\n", TT->t.sec, TT->t.msec);
		scheduleTimedTask(TT);
	}
	else {
		Task *T = getEmptyTask();
		Log(LOG_SCHEDINTF, "[scheduleComponent] Scheduling Component as Task\n");
		T->context = C;
		T->function = F;
		T->identifier = id;
		T->priority = 0xFF - p;
		T->schedulee = C1;
		T->scheduler = C2;
		scheduleTask(T);
	}
}

void setTimer (Time *t) {
	currentTimer.sec  = t->sec;
	currentTimer.msec = t->msec;

	// set OS-timer
}

Time *getTimer () {
	return &currentTimer;

	// return OS-timer
}

void pokeScheduler () {
	Log(LOG_SCHEDINTF, "[pokeScheduler] Start\n");

	giveTimeTick(&FrameTime);
}

void executeTasks () {
	Task *toExecute = retrieveTask();
	Log(LOG_SCHEDINTF, "[executeTasks] Start\n");
	while(toExecute != NULL) {

		Log(LOG_SCHEDINTF, "[executeTasks] Executing a task\n");

		toExecute->function(toExecute->schedulee, toExecute->context, toExecute->scheduler);
		returnMessage(toExecute->context);
		taskCompleted(toExecute);

		Log(LOG_SCHEDINTF, "[executeTasks] Task execution done\n");

		toExecute = retrieveTask();
	}

	Log(LOG_SCHEDINTF, "[executeTasks] End\n");

}

uint8_t idIsScheduled (uint8_t id) {
	return isScheduled(id);
}

uint8_t idIsTimedScheduled (uint8_t id) {
	return idIsTimedScheduled(id);
}

#endif /* TINYOS */
