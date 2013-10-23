/*
 * schedulerInterface.h
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

#ifdef MYRIANED

#ifndef SCHEDULERINTERFACE_H_
#define SCHEDULERINTERFACE_H_

#include "timedTaskQueue.h"
#include <inttypes.h>

void initSchedulerInterface (uint32_t frame_time);
void scheduleComponent (void (*F) (void *schedulee, void *context, void *scheduler), void *C1, void *C, void *C2, uint8_t p, Time *t , uint16_t id);
void pokeScheduler ();
void executeTasks ();
uint8_t idIsScheduled (uint8_t id);
uint8_t idIsTimedScheduled (uint8_t id);

#endif /* SCHEDULERINTERFACE_H_ */

#endif /* MYRIANED */
