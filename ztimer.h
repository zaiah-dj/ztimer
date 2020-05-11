//ztimer.h

#ifndef _WIN32
 #define _POSIX_C_SOURCE 200809L
#endif 

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#ifndef TIMER_H
#define TIMER_H

#define ZTIMER_ERRV_LENGTH 127

#define __CURRENT_TIME__ \
timer_now( &__thx__ )	

#define timer_init(a, b) \
__timer_init( a, b )

#define timer_use_us(a) \
__timer_init( a, LITE_USEC )

#define timer_use_ms(a) \
__timer_init( a, LITE_MSEC )

#define timer_use_secs(a) \
__timer_init( a, LITE_SEC )

#define timer_use_ns(a) \
__timer_init( a, LITE_NSEC )

#define timer_set_name(...) \
__timer_set_name( __VA_ARGS__ )

#define timer_elapsed(a) \
__timer_elap( a ) 

#define timer_selapsed() \
__timer_elap( &__thx__ )

#define timer_sprint( ) \
__timer_eprint( &__thx__ )

#define timer_sset_name( str ) \
__timer_set_name( &__thx__, str )

#define timer_print(a) \
__timer_eprint( a )

#ifdef DEBUG_H
#define timer_sstart( ) \
 __timer_start( &__thx__, __FILE__, __LINE__ )
#define timer_send( ) \
 __timer_end( &__thx__, __FILE__, __LINE__ )
#define timer_start( t ) \
 __timer_start( t, __FILE__, __LINE__ )
#define timer_end( t ) \
 __timer_end( t, __FILE__, __LINE__ )
#else
#define timer_sstart( ) \
 __timer_start( &__thx__ )
#define timer_send( ) \
 __timer_end( &__thx__ )
#define timer_start( t ) \
 __timer_start( t )
#define timer_end( t ) \
 __timer_end( t )
#endif

//Different time types
typedef enum {
	LITE_NSEC = 0,
	LITE_USEC,
	LITE_MSEC,
	LITE_SEC,
} LiteTimetype;


typedef struct { 	
	clockid_t   clockid;     
	struct 
  timespec    start,     
              end;      
	const char *label; 
	LiteTimetype  type;     
	char        ts_string[1024];
 #ifdef DEBUG_H 
  const char *file;        
	int         linestart,   
              lineend;	  
 #endif
 #ifndef ERR_H
  int error;
	#ifndef ERRV_H
	char  errmsg[ ZTIMER_ERRV_LENGTH ];
	#endif 
 #endif
} Timer;

void __timer_init (Timer *t, LiteTimetype type);
void __timer_set_name (Timer *t, const char *label);
int __timer_elap (Timer *t) ;
void __timer_eprint (Timer *t) ;
char *timer_now( Timer *t );
#ifndef TIMER_LOCAL_STRUCT_H
#define TIMER_LOCAL_STRUCT_H
static Timer __thx__ = { 0 };
#endif
#ifdef DEBUG_H
 void __timer_start (Timer *t, const char *file, int line);
 void __timer_end (Timer *t, const char *file, int line);
#else
 void __timer_start (Timer *t);
 void __timer_end (Timer *t);
#endif

#endif
