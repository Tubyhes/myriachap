/*
 * chapStdlib.h
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


#ifndef CHAPSTDLIB_H_
#define CHAPSTDLIB_H_

#include "structures.h"

/**
 * MyriaChap message subjects. Use the range 0x0100 - 0xFFFF for this, to make sure it is disjoint from the range of MyriaDiffuse item types,
 * which are in the range of 0x000 - 0x00FF. This way MyriaDiffuse item types can be used as MyriaChap msg type.
 */

#define DATA_AVAILABLE 				0x0100
#define RUN							0x0200
#define INITIALIZE					0x0300
#define ORDER_SUBSCRIBE 			0x0400
#define SET_SETTINGS				0x0500
#define SUBSCRIPTION				0x0600
#define PROCESS						0x0700
#define PREPARE						0x0800
#define GLOBAL_TIME					0x0900
#define NODE_ID						0x0A00
#define GET_RUN_INTERVAL 			0x0B00
#define SET_RUN_INTERVAL			0x0C00
#define ADJUST_RUN_INTERVAL_UP		0x0D00
#define ADJUST_RUN_INTERVAL_DOWN	0x0E00
#define RETURN_RUN_INTERVAL 		0x0F00

#define MEASURE						0x1000
#define UNSUBSCRIBE					0x1100
#define UNSUBSCRIBED				0x1200
#define SELF_DESTRUCT				0x1300
#define DATA_READY					0x1400
#define ARGUMENT1					0x1500
#define ARGUMENT2					0x1600
#define RESET						0x1700
#define BOOL_AVAILABLE				0x1800
#define QUERY_EXPIRED				0x1900


#define QUERY_NR					0x2000
//RESERVED							0x2200
//RESERVED							0x2300
//RESERVED							0x2400
//RESERVED							0x2500
//RESERVED							0x2600



#define HIGHEST_PRIORITY 0xFF
#define HIGH_PRIORITY	 0xC0
#define MEDIUM_PRIORITY  0x7F
#define LOW_PRIORITY	 0x40
#define LOWEST_PRIORITY  0x00

/**
 * Requests the run interval from another component.
 * @param me  	The requesting component
 * @param him  	The component whose run interval is requested
 */
void getRunInterval (Component *me, Component *him);

/**
 * Increases the run interval of a component, thereby decreasing the frequency with which it runs.
 * @param Co  	Component whose run interval to adjust
 * @param sec	Amount of seconds to add to the run interval
 * @param msec	Amount of milliseconds to add to the run interval
 */
void adjustRunIntervalUp (Component *Co, uint32_t sec, uint16_t msec);

/**
 * Decreases the run interval of a component, thereby increasing the frequency with which it runs.
 * @param Co	Compnent whose run interval to adjust
 * @param sec	Amount of seconds to remove from the run interval
 * @param msec	Amount of millisecond to remove from the run interval
 */
void adjustRunIntervalDown (Component *Co, uint32_t sec, uint16_t msec);

/**
 * Copy component settings to a component. Used to give a component new settings.
 * @param S		The settings to copy to the component.
 * @param to	The component to which to the copy the settings
 */
void copySettings (Settings *S, Component *to);

void freeComponent (Component *C);

/**
 * Schedule a component for execution according to the component's run interval.
 * @param Co		Component to schedule
 * @param subject	Subject of the message to send
 * @param priority	Priority of the message
 */
void scheduleSelf (Component *Co, uint16_t subject, uint8_t priority);

/**
 * Schedule a component for immediate execution, irrespective of its run interval.
 * @param Co		Component to schedule
 * @param subject	Subject of the message to send
 * @param priority	Priority of the message
 */
void scheduleSelfNow (Component *Co, uint16_t subject, uint8_t priority);

/**
 * Schedule a component for execution in the next round (MyriaNed only).
 * @param Co		Component to schedule
 * @param subject	Subject of the message to send
 * @param priority	Priority of the message
 */
void scheduleSelfNextRound (Component *Co, uint16_t subject, uint8_t priority);

/**
 * Connect two components.
 * @param inputSide		Component that is on the receiving end of the connection
 * @param outputSide 	Component that is on the sending end of the connection
 * @param outputNr		Nr of the output channel of the outputSide component that should be used
 * @param subject		Subject of the notification message that outputSide should send
 */
void connectComponents (Component *inputSide, Component *outputSide, uint8_t outputNr, uint16_t subject);

/**
 * Notifies all subscribed components that new data is available, and where to get it.
 * @param Co		Component that has new data available
 * @param outputNr 	Nr of the output channel for which new data is available
 * @param dataPtr	Pointer to the new data. Co is obliged to leave the data here until all subscribers have had a chance to retrieve it
 */
void notifySubscribers (Component *Co, uint8_t outputNr, void *dataPtr);

/**
 * The standard inbox function which takes care of standard messages.
 * @param schedulee		The scheduled component, or recepient of the message
 * @param context		The message itself
 * @param scheduler		The scheduling component, or the sender of the message
 */
void stdInbox (void *schedulee, void *context, void *scheduler);

#endif /* CHAPSTDLIB_H_ */
