/*
 * Thread.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Karim Sobh
 */

#ifndef THREAD_H
#define THREAD_H

#include "includes.h"


class Thread    // Thread Class
{
	protected:
                pthread_attr_t pthread_attr; // pthread attribute data member
                long cpu_count;     // Number of CPUs data member
		pthread_t pthread;  // pthread_t identifier data member
		char identifier[128]; // A printable thread identifier represented by the time th thread created
		bool started;   // A flag indicating if a thread is started or not
		bool running;   // A flag indicating if a thread is running or not
		pthread_mutex_t mutex;  // A mutex that controls the execution of the thread
		bool termination_request;   // A flag indicating that a termination request is initiated
		void *(*threadRoutine   ) (void *); // A pointer to the start routine of the thread execution
		void setRunning (bool _running);    // Sets the running flag data member of the thread
                static void  cleanup(void * target_thread); // A static method that performs house keeping after the thread terminates
	public:
		Thread(void *(*_threadRoutine) (void *) =NULL); // Constructor 
		bool isRunning ();      // Check if thread is running
		pthread_t * getThreadHandler(); // Returns a pointer to the thread identifier
		void start ();                  // A jacket wrapper method that fork the thread execution
                virtual void * threadMainBody (void * arg) = 0; // A pure virtual method whose implementation is the thread main function
		static void * run (void * arg);     // A static method that is passed to pthread_create and invokes threadMainBody from within
		void waitForRunToFinish (); // Blocks until the running thread finishes execution
		char * getThreadIdentifier ();  // Return the thread identifier string
                bool isAlive ();    // Checks if the thread start is initiated
		virtual ~Thread();  // Virtual Thread Destructor
};

#endif // THREAD_H
