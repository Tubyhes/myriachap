/*
 * timestruct.c
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

#include "timestruct.h"

int8_t sooner (Time *left, Time *right) {
	if(left->sec < right->sec)
		return 1;
	else if(left->sec == right->sec && left->msec < right->msec)
		return 1;
	else
		return 0;
}

int8_t time_minus (Time *left, Time *right) {
	if(sooner(left, right))
		return 0;
	else {
		if(right->msec > left->msec) {
			left->sec --;
			left->msec += 1000;
		}
		left->sec -= right->sec;
		left->msec -= right->msec;
		return 1;
	}
}

int8_t time_plus (Time *left, Time *right) {
	left->msec += right->msec;
	if(left->msec > 1000) {
		left->sec ++;
		left->msec -= 1000;
	}
	left->sec += right->sec;

	return 1;
}

int8_t time_is (Time *left, Time *right) {
	left->sec = right->sec;
	left->msec = right->msec;
	return 1;
}
