/*
 * timedTaskQueue.h
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

#ifndef TIMEDTASKQUEUE_H_
#define TIMEDTASKQUEUE_H_

#include "taskQueue.h"
#include "timestruct.h"
#include <stdlib.h>
#include <inttypes.h>

struct timedtask_S {
	Task *T;
	Time t;
	struct timedtask_S *next;
};
typedef struct timedtask_S TimedTask;

int8_t initTimedTaskQueue (void (*setTimerFunction) (Time *t), Time *(*getTimerFunction) (void), void (*notification) (void));
TimedTask *getEmptyTimedTask (void);
void returnTimedTask (TimedTask *TT);
int8_t scheduleTimedTask (TimedTask *T);
uint8_t giveTimeTick (Time *time_spent);
uint8_t isTimedScheduled (uint16_t id);
void purgeTimedTasks (void *comp);

void getTimedTaskState ();

#endif /* TIMEDTASKQUEUE_H_ */
