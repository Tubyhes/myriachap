/*
 * boxStorage.h
 *
 *  Created on: Mar 31, 2010
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

#ifndef BOXSTORAGE_H_
#define BOXSTORAGE_H_

#include "structures.h"
#include <inttypes.h>

#ifdef MEMORY_MANAGEMENT
#define EMPTY_SUBJECT 	0
#define DUMMY_SUBJECT 	UINT16_MAX
#define NOT_USED		0
#define USED			1

void ProcessMessageFunctionDummy (void *schedulee, void *context, void *scheduler);

#endif

uint8_t initStorageSpace (uint8_t msgPoolS, uint8_t comPoolS, uint8_t subPoolS);

Message *getEmptyMessage ();
void returnMessage (Message *msg);

Component *getEmptyComponent (uint8_t nrOfOutputs);
void returnComponent (Component *com);

Subscription *getEmptySubscription ();
void returnSubscription (Subscription *sub);

ListItem *getEmptyListItem();
void returnListItem (ListItem *L);

void getStorageState ();

#endif /* BOXSTORAGE_H_ */
