/*
 * taskQueue.h
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

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <stdlib.h>
#include <inttypes.h>

#define MAX_IDENTIFIERS	 100

struct task_S {
	void *scheduler;
	void *schedulee;
	void *context;
	void (*function) (void *schedulee, void *context, void *scheduler);
	uint8_t priority;
	uint16_t identifier;
	struct task_S *next;
};
typedef struct task_S Task;

int8_t initTaskQueue (void (*notification) (void));
Task *getEmptyTask (void);
int8_t scheduleTask (Task *T);
Task *retrieveTask ();
void taskCompleted (Task *T);
uint8_t isScheduled (uint16_t id);
void purgeTasks (void *comp);

void getTaskState ();

#endif /* TASKQUEUE_H_ */
