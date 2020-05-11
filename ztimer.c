/* ------------------------------------------- * 
 * ztimer.c
 * ---------
 * A timing library for C.
 *
 * Usage
 * -----
 *
 *
 * LICENSE
 * -------
 * Copyright 2020 Tubular Modular Inc. dba Collins Design
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * TODO
 * ----
 * 
 * ------------------------------------------- */
#include "ztimer.h"

static const unsigned long CV_1T = 1000;
static const unsigned long CV_1M = 1000000;
static const unsigned long CV_1B = 1000000000;

//Initiailize a timer
void __timer_init (Timer *t, LiteTimetype type) {
	memset(t, 0, sizeof(Timer));	
	t->type = type;
}


//Set the name of a timer
void __timer_set_name (Timer *t, const char *label) {
	t->label = label;
}


//Start a currently running timer
#ifndef DEBUG_H 
void __timer_start (Timer *t) {
#else
void __timer_start (Timer *t, const char *file, int line) {
	t->file = file;
	t->linestart = line;
#endif 
	t->clockid = CLOCK_REALTIME;
	clock_gettime( t->clockid, &t->start );
}


//Stop a currently running timer
#ifndef DEBUG_H 
void __timer_end (Timer *t) {
#else
void __timer_end (Timer *t, const char *file, int line) {
	t->file = file;
	t->lineend = line;
#endif 
	clock_gettime( t->clockid, &t->end );
}


//Return the current time (in a platform agnostic way)
char *timer_now( Timer *t ) {
#if 1
	time_t tm = time( NULL ); 
	snprintf( t->ts_string, sizeof( t->ts_string ), "%s", ctime( &tm )); 
	if ( t->ts_string[ strlen( t->ts_string ) - 1 ] == '\n' ) {
		t->ts_string[ strlen( t->ts_string ) - 1 ] = '\0';
	}
#else
	t->clockid = CLOCK_REALTIME;
	clock_gettime( t->clockid, &t->start );
	struct tm tm;
	//tm.tm_sec = t->start.tv_sec % 60;
	fprintf( stderr, "seconds\n" );
	nlprintf( t->start.tv_sec % 60 ); 
	fprintf( stderr, "min\n" );
	nlprintf( ( t->start.tv_sec / 60 ) % 60 ); 
	fprintf( stderr, "hour\n" ); //This seems wrong, but we are 5 hours ahead
	nlprintf( ( ( t->start.tv_sec / 60 ) / 60 ) % 12 ); 
	fprintf( stderr, "day\n" );
	//60 secs * 60 min * 24 = 86400 secs per day, crude math would give me this...
	//add all leap years since 1970, then the remaining
	nlprintf(( t->start.tv_sec / 86400 ) % 365 );
	//nlprintf( ( ( ( t->start.tv_sec / 60 ) / 60 ) / 24 ) % 365 ); 
	//nlprintf( t->start.tv_sec % 60 ); 
	//strftime( t->ts_string, sizeof( t->ts_string ), "%a %b %d %I:%M:%S %Y", t-> );
#endif
	return t->ts_string;
}


//Returns difference of start and end time
int __timer_elap (Timer *t) {
	return t->end.tv_sec - t->start.tv_sec;
}


//Pretty prints difference in requested format of start and end time
void __timer_eprint (Timer *t) {
	//Define some stuff...
	unsigned long nsdiff = 0;
	time_t secs = 0;
	double mod = 0;
	char ch[64] = { 0 };
	const char *ts ;
	const char *time   = "ns\0ms\0us\0s";
	const char *label  = (t->label) ? t->label :
	 #ifdef CV_VERBOSE_TIMER
		(t->file) ? t->file : "anonymous"
	 #else
		"anonymous"
	 #endif
	;
	const char *fmt    =
   #ifdef CV_VERBOSE_TIMER
	  //"routine @[ %-20s %d - %d ] completed in %11ld %s\n"
	  "routine @[ %-20s %d - %d ] completed in %s %s\n"
   #else
	  "routine [ %-20s ] completed in %s %s\n"
   #endif
	;

	//Get the raw elapsed seconds and nanoseconds
	if ((secs = t->end.tv_sec - t->start.tv_sec) == 0)
		nsdiff = t->end.tv_nsec - t->start.tv_nsec;
	else if (secs < 0) {
		fprintf(stderr, "Timer error occurred!\n");
		return; /*Some kind of error occurred*/
	}
	else if (secs > 0) {
		if ((nsdiff = (CV_1B - t->start.tv_nsec) + t->end.tv_nsec) > CV_1B) {
			nsdiff -= CV_1B;
			secs += 1;
		}
	}

	//Choose a modifier and make any final calculations for formatting
	switch (t->type) {
		case LITE_NSEC:   //0.000,000,001
			snprintf( ch, 64,  "%ld", nsdiff );
			ts = &time[0];
			break;
		case LITE_USEC:   //0.000,001
			mod = ((float)secs * CV_1M) + (((float)nsdiff / (float)CV_1B) * (float)CV_1M);
			ts = &time[6];
			snprintf( ch, 64, "%.6f", mod);
			break;
		case LITE_MSEC:   //0.001
			mod = ((float)secs * CV_1T) + (((float)nsdiff / (float)CV_1B) * (float)CV_1T);
			ts = &time[3];
			snprintf( ch, 64, "%.6f", mod);
			break;
		case LITE_SEC:
			mod = ((float)secs + ((float)nsdiff / (float)CV_1B));
			ts = &time[9];
			snprintf( ch, 64, "%.6f", mod);
			break;
		default:
			return;
	}

	fprintf(stderr, fmt, label, ch, ts); 
}
