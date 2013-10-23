/*
 * boxStorage.c
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

#include "boxStorage.h"
#include "myriaChapOut.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef MEMORY_MANAGEMENT
#define MSGPOOL_SIZE	20
#define COMPOOL_SIZE	11
#define SUBPOOL_SIZE 	15
#define LITPOOL_SIZE	(2 * SUBPOOL_SIZE)

Message 	 msgPool[MSGPOOL_SIZE];
Component 	 comPool[COMPOOL_SIZE];
Subscription subPool[SUBPOOL_SIZE];
ListItem	 litPool[LITPOOL_SIZE];

uint8_t emptyMsgCount;
uint8_t emptyComCount;
uint8_t emptySubCount;
uint8_t emptyLitCount;
#endif

uint8_t initStorageSpace (uint8_t msgPoolS, uint8_t comPoolS, uint8_t subPoolS) {
#ifdef MEMORY_MANAGEMENT
	uint8_t i;

	emptyMsgCount = MSGPOOL_SIZE;
	emptyComCount = COMPOOL_SIZE;
	emptySubCount = SUBPOOL_SIZE;
	emptyLitCount = LITPOOL_SIZE;

	for(i=0; i<MSGPOOL_SIZE; i++) {
		msgPool[i].subject = 0;
		msgPool[i].content = NULL;
	}
	for(i=0; i<COMPOOL_SIZE; i++) {
		comPool[i].inheriter = NULL;
		comPool[i].inputs	 = NULL;
		comPool[i].outputs   = NULL;
		comPool[i].processMessage = NULL;
		comPool[i].settings.happiness = 0;
		comPool[i].settings.memoryLoad = 0;
		comPool[i].settings.memorySize = 0;
		comPool[i].settings.nrOfOutputs = 0;
		comPool[i].settings.run_interval.msec = 0;
		comPool[i].settings.run_interval.sec = 0;
	}
	for(i=0; i<SUBPOOL_SIZE; i++) {
		subPool[i].outputNumber = 0;
		subPool[i].subject = 0;
		subPool[i].subscriber = NULL;
	}

	for(i=0; i<LITPOOL_SIZE; i++) {
		litPool[i].next 	= NULL;
		litPool[i].object 	= NULL;
		litPool[i].flag		= 0;
	}

	Log(LOG_STORAGE, "[iSS] B: %u\n", MSGPOOL_SIZE * sizeof(Message) + COMPOOL_SIZE * sizeof(Component) + SUBPOOL_SIZE * sizeof(Subscription) + LITPOOL_SIZE * sizeof(ListItem));

#endif

	return 1;
}

/**
 * Getting an empty Message from the boxStorage must be done using a
 * normal function call, as it can't be done using a message: you would
 * need a message for that.
 */
Message *getEmptyMessage () {
#ifdef MEMORY_MANAGEMENT
	uint8_t i;

	if(emptyMsgCount == 0) {
		Log(LOG_STORAGE, "[eMsg] None\n");
		return NULL;
	}

	for(i=0; i<MSGPOOL_SIZE; i++) {
		if(msgPool[i].subject == EMPTY_SUBJECT) {
			msgPool[i].subject = DUMMY_SUBJECT;
			emptyMsgCount --;
//			Log(LOG_STORAGE, "[eMsg] %d left\n", emptyMsgCount);
			return &(msgPool[i]);
		}
	}

	return NULL;

#else

	Message *msg = malloc(sizeof(Message));
	if(msg != NULL)
		Log(LOG_STORAGE, "[getEmptyMessage] Alloced empty msg\n");
	else
		Log(LOG_STORAGE, "[getEmptyMessage] Cant alloc empty msg\n");
	return msg;

#endif
}

void returnMessage (Message *msg) {
#ifdef MEMORY_MANAGEMENT

	msg->subject = EMPTY_SUBJECT;
	msg->content = NULL;

	emptyMsgCount ++;
//	Log(LOG_STORAGE, "[rMsg] %d\n", emptyMsgCount);
	return;

#else

	free(msg);
	Log(LOG_STORAGE, "[returnMessage] message freed\n");
	return

#endif
}

Component *getEmptyComponent (uint8_t nrOfOutputs) {
#ifdef MEMORY_MANAGEMENT
	uint8_t i, j;

	if(emptyComCount == 0) {
		Log(LOG_STORAGE, "[eCom] None\n");
		return NULL;
	}

	for(i=0; i<COMPOOL_SIZE; i++) {
		if(comPool[i].processMessage == NULL) {
			comPool[i].processMessage = &ProcessMessageFunctionDummy;
			for(j=0; j<nrOfOutputs; j++) {
				ListItem *lit = getEmptyListItem();
				lit->next = comPool[i].outputs;
				comPool[i].outputs = lit;
			}
			comPool[i].settings.nrOfOutputs = nrOfOutputs;
			emptyComCount --;
//			Log(LOG_STORAGE, "[eCom] %d left\n", emptyComCount);
			return &(comPool[i]);
		}
	}

	return NULL;

#else

	Component *com = malloc(sizeof(Component));
	if(com != NULL) {
		Log(LOG_STORAGE, "[getEmptyComponent] Alloced empty component\n");
		if(nrOfOutputs > 0) {
			com->outputs = calloc(nrOfOutputs, sizeof(Subscription *));
		}
		com->settings.nrOfOutputs = nrOfOutputs;
	}
	else
		Log(LOG_STORAGE, "[getEmptyComponent] Can't alloc empty component\n");
	return com;

#endif
}

void returnComponent (Component *com) {
#ifdef MEMORY_MANAGEMENT

	uint8_t i;
	for(i=0; i<com->settings.nrOfOutputs; i++) {
		ListItem *temp = com->outputs;
		com->outputs = temp->next;
		returnListItem(temp);
	}

	com->processMessage = NULL;
	com->inputs  = NULL;
	com->outputs = NULL;
	com->inheriter = NULL;
	com->settings.happiness = 0;
	com->settings.memoryLoad = 0;
	com->settings.memorySize = 0;
	com->settings.nrOfOutputs = 0;
	com->settings.run_interval.sec = 0;
	com->settings.run_interval.msec = 0;

	emptyComCount ++;
//	Log(LOG_STORAGE, "[rCom] %d\n", emptyComCount);
	return;

#else
	if(com->settings.nrOfOutputs > 0) {
		free(com->outputs);
	}

	free(com);
	Log(LOG_STORAGE, "[returnComponent] Component freed\n");
	return

#endif
}

Subscription *getEmptySubscription () {
#ifdef MEMORY_MANAGEMENT
	uint8_t i;

	if(emptySubCount == 0) {
		Log(LOG_STORAGE, "[eSub] None\n");
		return NULL;
	}

	for(i=0; i<SUBPOOL_SIZE; i++) {
		if(subPool[i].subject == EMPTY_SUBJECT) {
//			Log(LOG_STORAGE, "Sub %u\n", i);
			subPool[i].subject = DUMMY_SUBJECT;
			emptySubCount --;
//			Log(LOG_STORAGE, "[eSub] %d left\n", emptySubCount);
			return &(subPool[i]);
		}
	}

	return NULL;

#else

	Subscription *sub = malloc(sizeof(Subscription));
	if(sub != NULL)
		Log(LOG_STORAGE, "[getEmptySubscription] Alloced empty subscription\n");
	else
		Log(LOG_STORAGE, ("[getEmptySubscription] Can't alloc empty subscription\n");
	return sub;

#endif
}

void returnSubscription (Subscription *sub) {
#ifdef MEMORY_MANAGEMENT

	sub->subscriber = NULL;
	sub->subject	= EMPTY_SUBJECT;

	emptySubCount ++;
//	Log(LOG_STORAGE, "[rSub] %d\n", emptySubCount);
	return;

#else

	free(sub);
	Log(LOG_STORAGE, "[returnSubscription] Subscription freed\n");
	return;

#endif
}

ListItem *getEmptyListItem () {
#ifdef MEMORY_MANAGEMENT
	uint8_t i;

	if(emptyLitCount == 0) {
		Log(LOG_STORAGE, "[eLit] None\n");
		return NULL;
	}

	for(i=0; i<LITPOOL_SIZE; i++) {
		if(litPool[i].flag == 0) {
			litPool[i].flag = 1;
//			Log(LOG_STORAGE, "lit %u\n", i);
			emptyLitCount --;
//			Log(LOG_STORAGE, "[eLit] %d left\n", emptyLitCount);
			return &(litPool[i]);
		}

	}

	return NULL;

#else

	ListItem *lit = malloc(sizeof(ListItem));
	if(lit != NULL)
		Log(LOG_STORAGE, "[getEmptyListItem] Alloced empty ListItem\n");
	else
		Log(LOG_STORAGE, "[getEmptyListItem] Can't alloc empty ListItem\n");
	return sub;

#endif
}

void returnListItem (ListItem *lit) {
#ifdef MEMORY_MANAGEMENT

	lit->next 	= NULL;
	lit->object = NULL;
	lit->flag 	= 0;

	emptyLitCount ++;
//	Log(LOG_STORAGE, "[rLit] %u\n", emptyLitCount);
	return;

#else

	free(lit);
	Log(LOG_STORAGE, "[returnListItem] ListItem freed\n");
#endif
}

void getStorageState () {
#ifdef MEMORY_MANAGEMENT
	Log(LOG_STORAGE, "Sub: %d, Com: %d, Mes: %d, Lit: %d\n", emptySubCount, emptyComCount, emptyMsgCount, emptyLitCount);
#endif
}

void ProcessMessageFunctionDummy (void *schedulee, void *context, void *scheduler) {

}
