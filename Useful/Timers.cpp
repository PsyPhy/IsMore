/*****************************************************************************/
/*                                                                           */
/*                               DexTimers.cpp                               */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Timers.h"

// 
// Scale factor to produce slow motion.
// Set it to 1.0 to produce normal speed movements.
// Note that all the timers share this common factor.
//

static double dex_slow_motion = 1.0;

/*****************************************************************************/

void TimerSetSlowmotion( float factor ) {
	dex_slow_motion = factor;
}


/*****************************************************************************/

/*
 * Implement a means of timing actions - version Win32.
 */

/****************************************************************************/

/*
 * Simply start the timer without setting an alarm.
 * Used to measure elapsed time rather than to wait for a timeout.
 */

void TimerStart ( Timer &timer ) {

	LARGE_INTEGER	li;

	QueryPerformanceFrequency( &li );
	timer.frequency = (double) li.QuadPart;
	QueryPerformanceCounter( &li );
	timer.mark = li.QuadPart;

	timer.split = timer.mark;

	timer.paused = false;
	timer.cumulative_elapsed_time = 0.0;

}
	
/*****************************************************************************/

double TimerElapsedTime ( Timer &timer ) {
	
	LARGE_INTEGER	li;
	__int64			current_time;
	double			duration;
	
	// If paused, then return how much time had elapsed when the pause was initiated.
	if ( timer.paused ) return( timer.cumulative_elapsed_time );

	/* Compute the true time interval since the timer was started. */
	QueryPerformanceCounter( &li );
	current_time = li.QuadPart;
	duration = (double) (current_time - timer.mark) / timer.frequency / dex_slow_motion;
	return( timer.cumulative_elapsed_time + duration );

}

double TimerRemainingTime( Timer &timer ) {
	return( timer.alarm - TimerElapsedTime( timer ) );
}

double TimerPause( Timer &timer ) {
	timer.cumulative_elapsed_time = TimerElapsedTime( timer );
	timer.paused = true;
	return( timer.cumulative_elapsed_time );
}

void TimerResume( Timer &timer ) {
	LARGE_INTEGER	li;
	__int64			current_time;

	// If not paused this should have no effect.
	if ( !timer.paused ) return;

	/* Compute the true time interval since the timer was started. */
	QueryPerformanceCounter( &li );
	current_time = li.QuadPart;
	timer.mark = current_time;
	timer.paused = false;
}

//
///****************************************************************************/
//
//double TimerSplitTime ( Timer &timer ) {
//	
//	LARGE_INTEGER	li;
//	__int64			current_time;
//	double			duration;
//	
//	/* Compute the true time interval since the last split was set. */
//	QueryPerformanceCounter( &li );
//	current_time = li.QuadPart;
//	duration = (double) (current_time - timer.split) / timer.frequency / dex_slow_motion;
//	timer.split = current_time;
//
//	return( duration );
//
//}

/****************************************************************************/

void TimerSet( Timer &timer, double seconds ) {

	timer.alarm = seconds;
	TimerStart( timer );

}

int	TimerTimeout( Timer &timer ) {
	return( TimerElapsedTime( timer ) >= timer.alarm );
}
