/*
 * timestruct.h
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

#ifndef TIMESTRUCT_H_
#define TIMESTRUCT_H_

#include <stdlib.h>
#include <inttypes.h>

struct timestruct_S {
	uint32_t sec;
	uint16_t msec;
};
typedef struct timestruct_S Time;
//#warning something

int8_t sooner (Time *left, Time *right);
int8_t time_minus (Time *left, Time *right);
int8_t time_plus (Time *left, Time *right);
int8_t time_is (Time *left, Time *right);


#endif /* TIMESTRUCT_H_ */
