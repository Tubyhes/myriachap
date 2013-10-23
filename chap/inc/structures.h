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


#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <timestruct.h>

struct Settings_Struct;
struct Component_Struct;
struct Subscription_Struct;
struct Message_Struct;
struct ListItem_Struct;

typedef struct Settings_Struct Settings;
typedef struct Component_Struct Component;
typedef struct Subscription_Struct Subscription;
typedef struct Message_Struct Message;
typedef struct ListItem_Struct ListItem;


/**
 * Settings for a component.
 */
struct Settings_Struct {
	Time	 run_interval;
	uint8_t  nrOfOutputs;
	uint16_t memorySize;
	uint16_t memoryLoad;
	int16_t  happiness;
};

/**
 * The component itself
 */
struct Component_Struct {
	void (*processMessage) (void *schedulee, void *context, void *scheduler);
	ListItem *inputs;
	ListItem *outputs;
	Settings settings;
	void *inheriter;
};

/**
 * Subscription of one component to another, indicating it is interested in at least part of the data produced
 * by the other component
 */
struct Subscription_Struct {
	uint8_t outputNumber;
	Component *subscriber;
	uint16_t subject;
};

/**
 * Message struct, the way for components to communicate with each other, and the means for transferring control
 */
struct Message_Struct {
	uint16_t 				subject;
	void *					content;
};

/**
 * Generic struct for putting anything in a list.
 */
struct ListItem_Struct {
	uint8_t					flag;
	void *					object;
	ListItem 				*next;
};



#endif /* COMPONENT_H */
