/*
 * myriaChapOut.h
 *
 *  Created on: Jan 18, 2010
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


#ifndef MYRIACHAPOUT_H_
#define MYRIACHAPOUT_H_

#include <inttypes.h>

#define LOG_LEVEL_NOLOG 	0

#define LOG_CHAP			(0x01)
#define LOG_SCHEDULER		(0x02)
#define LOG_SCHEDINTF		(0x04)
#define LOG_COMPONENT		(0x08)
#define LOG_APPLICATION		(0x10)
#define LOG_STORAGE			(0x20)
#define LOG_RESPONSE		(0x40)

#define LOG_CHAP_BM			(0x01)
#define LOG_SCHEDULER_BM 	(0x02)
#define LOG_SCHEDINTF_BM	(0x04)
#define LOG_COMPONENT_BM 	(0x08)
#define LOG_APPLICATION_BM	(0x10)
#define LOG_STORAGE_BM		(0x20)
#define LOG_RESPONSE_BM		(0x40)

void setLogLevel (uint8_t log_level_bm);
uint8_t getLogLevel ();
inline int Log(uint8_t level, const char *format, ...);


#endif /* MYRIACHAPOUT_H_ */
