/*
 * myriaChapOut.c
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

#include "chapConfig.h"
#include "myriaChapOut.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef MYRIANED
#include "mnUart.h"
#elif defined TINYOS
//#include <dbg.h>
#endif


uint8_t debug_level = LOG_LEVEL_NOLOG;

inline int Log(uint8_t level, const char *, ...) __attribute__ ((format (printf, 2, 3)));

void setLogLevel (uint8_t level) {
	debug_level = level;
}

uint8_t getLogLevel () {
	return debug_level;
}

#ifdef MYRIANED
inline int Log(uint8_t level, const char *format, ...) {
	if(debug_level & level) {
		char print_string[200];
		va_list ap;
		va_start(ap, format);
		vsprintf(print_string, format, ap);
		va_end(ap);
		mnUartPutStr(print_string);
	}
	return 0;
}
#endif

#ifdef TINYOS
inline int Log(uint8_t level, const char *format, ...) {
	if(debug_level & level) {
		char print_string[100];
		va_list ap;
		va_start(ap, format);
		vsprintf(print_string, format, ap);
		va_end(ap);
#ifdef TOSSIM
		dbg("MyriaChapC", print_string);
#else
		printf(print_string);
		printfflush();
#endif
	}
	return 0;
}
#endif
