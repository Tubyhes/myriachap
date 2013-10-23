/*
 * chapStdlib.c
 *
 *  Created on: Apr 2, 2010
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

#include "chapStdlib.h"
#include "boxStorage.h"
#include "interfaces.h"

#include "myriaChapOut.h"
#include <inttypes.h>

extern Time FrameTime;
//Time FrameTime;

void getRunInterval (Component *me, Component *him) {
	Message *msg = getEmptyMessage();
	msg->subject = GET_RUN_INTERVAL;
	msg->content = NULL;
	scheduleComponent(him->processMessage, him, msg, me, MEDIUM_PRIORITY, NULL, GET_RUN_INTERVAL);
}

void adjustRunIntervalUp (Component *Co, uint32_t sec, uint16_t msec) {
	Message *msg = getEmptyMessage();
	Time *t = malloc(sizeof(Time));
	t->sec = sec;
	t->msec = msec;
	msg->subject = ADJUST_RUN_INTERVAL_UP;
	msg->content = t;
	scheduleComponent(Co->processMessage, Co, msg, NULL, HIGHEST_PRIORITY, NULL, ADJUST_RUN_INTERVAL_UP);
}

void adjustRunIntervalDown (Component *Co, uint32_t sec, uint16_t msec) {
	Message *msg = getEmptyMessage();
	Time *t = malloc(sizeof(Time));
	t->sec = sec;
	t->msec = msec;
	msg->subject = ADJUST_RUN_INTERVAL_DOWN;
	msg->content = t;
	scheduleComponent(Co->processMessage, Co, msg, NULL, HIGHEST_PRIORITY, NULL, ADJUST_RUN_INTERVAL_DOWN);
}

void scheduleSelf (Component *Co, uint16_t subject, uint8_t priority) {
	Message *msg 	= getEmptyMessage();
	msg->subject 	= subject;
	msg->content	= NULL;
	scheduleComponent(Co->processMessage, Co, msg, Co, priority, &(Co->settings.run_interval), subject);
}

void scheduleSelfNow (Component *Co, uint16_t subject, uint8_t priority) {
	Message *msg 	= getEmptyMessage();
	msg->subject 	= subject;
	msg->content	= NULL;
	scheduleComponent(Co->processMessage, Co, msg, Co, priority, NULL, subject);
}

void scheduleSelfNextRound (Component *Co, uint16_t subject, uint8_t priority) {
	Message *msg	= getEmptyMessage();
	msg->subject 	= subject;
	msg->content	= NULL;
	Log(LOG_CHAP, "[scheduleSelfNextRound] Schedule component for %lu.%u\n", FrameTime.sec, FrameTime.msec);
	scheduleComponent(Co->processMessage, Co, msg, Co, priority, &FrameTime, subject);
}

void connectComponents (Component *inputSide, Component *outputSide, uint8_t outputNr, uint16_t subject) {
	Subscription *S = getEmptySubscription();
	Message *msg = getEmptyMessage();

	S->subscriber = inputSide;
	S->subject	  = subject;
	S->outputNumber = outputNr;

	msg->subject = ORDER_SUBSCRIBE;
	msg->content = S;

	scheduleComponent(inputSide->processMessage, inputSide, msg, outputSide, MEDIUM_PRIORITY, NULL, ORDER_SUBSCRIBE);
}

void copySettings (Settings *S, Component *to) {
	to->settings.run_interval 	= S->run_interval;
	to->settings.nrOfOutputs	= S->nrOfOutputs;
}

void freeComponent (Component *C) {
	uint8_t i;
	ListItem *lit;
	ListItem *L;
	ListItem *ite = C->outputs;

	// Return any subscriptions from other components and return them
	for(i=0; i<C->settings.nrOfOutputs; i++) {
		lit = (ListItem *) ite->object;
		while(lit != NULL) {
			Message *msg = getEmptyMessage();
			Subscription *S = (Subscription *) lit->object;
			Component *him = S->subscriber;
			msg->subject = UNSUBSCRIBED;
			msg->content = NULL;
			scheduleComponent(him->processMessage, him, msg, C, MEDIUM_PRIORITY, NULL, UNSUBSCRIBED);

			L = lit;
			lit = L->next;
			returnSubscription(S);
			returnListItem(L);
		}
		ite = ite->next;
	}

	// Unsubscribe from any subscribed components
	lit = C->inputs;
	while(lit != NULL) {
		Message *msg = getEmptyMessage();
		Component *him = (Component *) lit->object;
		msg->subject = UNSUBSCRIBE;
		msg->content = NULL;
		scheduleComponent(him->processMessage, him, msg, C, MEDIUM_PRIORITY, NULL, UNSUBSCRIBE);

		L = lit;
		lit = L->next;
		returnListItem(L);
	}

	// Send message to self to purge tasks and self-destruct
	scheduleSelfNow(C, SELF_DESTRUCT, LOWEST_PRIORITY);

}

void notifySubscribers (Component *Co, uint8_t outputNr, void *dataPtr) {
	uint8_t i = 0;
	ListItem *ite = Co->outputs;
	ListItem *Lit;

	while (i < outputNr) {
		ite = ite->next;
		i++;
	}

	Lit = (ListItem *) ite->object;
	while(Lit != NULL) {
		Subscription *Sub = (Subscription *) Lit->object;
		Message *msg;
		Log(LOG_CHAP, "nS %u %u\n", Sub->subject, Sub->outputNumber);
		if((msg = getEmptyMessage()) != NULL) {
			msg->subject		= Sub->subject;
			msg->content		= dataPtr;
			scheduleComponent(Sub->subscriber->processMessage, Sub->subscriber, msg, Co, MEDIUM_PRIORITY, NULL, Sub->subject);
		}
		Lit = Lit->next;
	}
}

void stdInbox (void *schedulee, void *context, void *scheduler) {
	Component *me  = (Component *) schedulee;
	Component *him = (Component *) scheduler;
	Message   *msg = (Message *) context;
	Message   *myMsg;
	Log(LOG_CHAP, "[stdInbox] Subject: %d ", msg->subject);
	switch(msg->subject) {
	case SET_SETTINGS:; {
		Log(LOG_CHAP, "SET_SETTINGS\n");
		copySettings((Settings *) msg->content, me);
		free(msg->content);
		break;
	}
	case ORDER_SUBSCRIBE:; {
		Log(LOG_CHAP, "ORDER_SUBSCRIBE\n");
		myMsg = getEmptyMessage();
		if(myMsg != NULL) {
			// add the other component to my inputs list
			ListItem *lit = getEmptyListItem();
			lit->object = him;
			lit->next 	= me->inputs;
			me->inputs 	= lit;

			// send the subscription
			myMsg->subject = SUBSCRIPTION;
			myMsg->content = msg->content;
			scheduleComponent(him->processMessage, him, myMsg, me, MEDIUM_PRIORITY, NULL, SUBSCRIPTION);
		}
		else {
			returnSubscription((Subscription *) msg->content);
		}
		break;
	}
	case SUBSCRIPTION:; {
		Subscription *new_thing = (Subscription *) msg->content;
		ListItem *lit = getEmptyListItem();
		ListItem *ite = me->outputs;
		uint8_t i = 0;
		Log(LOG_CHAP, "SUBSCRIPTION\n");
		if(new_thing->outputNumber >= me->settings.nrOfOutputs) {
			returnSubscription(new_thing);
			returnListItem (lit);
			break;
		}
		while(i < new_thing->outputNumber) {
			ite = ite->next;
			i++;
		}
		lit->object = new_thing;
		lit->next = (ListItem *) ite->object;
		ite->object = lit;

		break;
	}
	case UNSUBSCRIBE:; {
		uint8_t i;
		ListItem *ite = me->outputs;
		Log(LOG_CHAP, "UNSUBSCRIBE\n");
		for(i=0; i<me->settings.nrOfOutputs; i++) {
			ListItem **lit = (ListItem **) &(ite->object);
			while(*lit != NULL) {
				Subscription *Sub = (Subscription *) (*lit)->object;
				if(Sub->subscriber == him) {
					ListItem *L = *lit;
					*lit = L->next;
					returnSubscription(Sub);
					returnListItem(L);
				}
				else {
					lit = &((*lit)->next);
				}
			}
			ite = ite->next;
		}
		break;
	}
	case UNSUBSCRIBED:; {
		ListItem **ite = &(me->inputs);
		while(*ite != NULL) {
			Component *ext = (Component *) (*ite)->object;
			if(ext == him) {
				ListItem *L = *ite;
				*ite = L->next;
				returnListItem(L);
			}
			else {
				ite = &((*ite)->next);
			}
		}
		break;
	}
	case GET_RUN_INTERVAL:; {
		Log(LOG_CHAP, "GET_RUN_INTERVAL\n");
		myMsg = getEmptyMessage();
		myMsg->subject = RETURN_RUN_INTERVAL;
		myMsg->content = &(me->settings.run_interval);
		scheduleComponent(him->processMessage, him, myMsg, me, MEDIUM_PRIORITY, NULL, RETURN_RUN_INTERVAL);
		break;
	}
	case ADJUST_RUN_INTERVAL_UP:; {
		Time *up = (Time *) msg->content;
		Log(LOG_CHAP, "ADJUST_RUN_INTERVAL_UP\n");
		if(me->settings.run_interval.sec != 0 || me->settings.run_interval.msec != 0) {
			me->settings.run_interval.msec += up->msec;
			me->settings.run_interval.sec  += me->settings.run_interval.msec / 1000;
			me->settings.run_interval.msec %= 1000;
			me->settings.run_interval.sec  += up->sec;
		}
		break;
	}
	case ADJUST_RUN_INTERVAL_DOWN:; {
		Time *down = (Time *) msg->content;
		Log(LOG_CHAP, "ADJUST_RUN_INTERVAL_DOWN\n");
		if(!sooner(&(me->settings.run_interval), down)) {
			time_minus(&(me->settings.run_interval), down);
		}
		break;
	}
	case SELF_DESTRUCT:; {
		purgeTimedTasks((void *) me);
		purgeTasks((void *) me);
		returnComponent(me);
		break;
	}
	default:; {
		Log(LOG_CHAP, "UNKNOWN\n");
		break;
	}
	}

}
