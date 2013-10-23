/*
 * time.h
 *
 *  Created on: Sep 23, 2010
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

#ifdef EMBEDDED
#include "time.h"
#include "myriaCore.h"

// returns time in seconds
time_t time (time_t *t) {
	uint64_t uptime = mcGetUptime();
	time_t time = uptime / 32768;
	if(uptime % 32768 > 16384)
		time ++;
	if(t != NULL)
		*t = time;
	return time;
}

#endif
