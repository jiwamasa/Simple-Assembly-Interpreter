#include "Thread.h"

// Modifier: Set thread running flag
void Thread::setRunning (bool _running)
{
	running = _running; // Set running
}
/* Static clean up method. It is defined as private as it need to be only invoked from within the object data members
Since it is static it received a void pointer that represent a pointer to the invoking object */
void Thread::cleanup(void * target_thread) 
{
    Thread * me = (Thread *) target_thread; // Cast target_thread to a Thread *
    me->setRunning(false);  // Set the thread as not running
    me->started = false;    // Set the thread as not started
    me->termination_request = true; // Set termination request to false
    pthread_mutex_unlock(&me->mutex);   // Unlock the execution management mutex
}
// Thread Constructor: Allows for overriding the pthread_create start routine parameters with another static method than "run" if needed
Thread::Thread(void *(*_threadRoutine) (void *))
{
    size_t stacksize = 1024*1024*4;  // Set stack to 4 MB   
    running = false;    // Set running to false
    started = false;    // Set started to false
    termination_request = false;    // Set termination request to false
    pthread_mutex_init (&mutex,NULL);// Initialize execution control mutex
    if ( _threadRoutine != NULL ) threadRoutine=_threadRoutine; // Set the pthread start routine to _threadRoutine if not NULL 
    else threadRoutine = &run;  // Else use the default static method run
    pthread_attr_init(&pthread_attr);   // Initialize pthread attributes
    pthread_attr_setdetachstate (&pthread_attr,PTHREAD_CREATE_DETACHED); // Set thread as dettached at creation time
    pthread_attr_setstacksize(&pthread_attr, stacksize ); // Set new stack size
    struct timeb thread_name;
    ftime( &thread_name );  // Get time snapshot in milli-seconds
    memset ( identifier,0,128); // Initialize identifier
    sprintf ( identifier,"%d.%d",(int) thread_name.millitm,(int) thread_name.time); // Set Identifier string
    cpu_count = sysconf( _SC_NPROCESSORS_ONLN ); // get the number of CPUs on the target running environment
}

// Selector: check if thread is running
bool Thread::isRunning ()
{
	return running;
}

// Selector: get the thread identifier
pthread_t * Thread::getThreadHandler()
{
	return & pthread;
}
// Start the execution of the thread
void Thread::start()
{
    pthread_mutex_lock(&mutex); // Acquire lock before forking the thread
    started=true; // Set started to true
    // Invoke pthread create and pass to the routine the current object as the start routine need to be static
    int created = pthread_create (&pthread,&pthread_attr,threadRoutine,this);
    if ( created != 0 ) // If the return in not zero then pthread_create failed
    {
        // Print an error message with the return integer value
        printf ("error creating thread: %d\n",created);
        // Check the error type and print an equivalent error message
        if (created == EAGAIN) printf ("EAGAIN generated\n");
        else if (created == EINVAL) printf ("EINVAL generated\n");
        else if (created == EPERM) printf ("EPERM generated\n");
        else if (created == ENOMEM) printf ("ENOMEM generated\n");
        started= false; // Set started to false
    }
    else setRunning(true); // Else thread is marked running
}

// Static method passed to pthread_create
void * Thread::run(void * arg)
{
	Thread * me = (Thread *) arg; // Cast the arg to Thread * which is the current thread
        // Push the cleanup static function to the cleanup functions stack to be invoked within pthread_exit
        // Notice the close curly bracket that substitute for the complementary pthread_cleanup_pop
//        pthread_cleanup_push(cleanup,arg); }  
//   	pthread_detach (me->pthread); 
	me->threadMainBody(arg); //Invoke the thread main function body
        me->cleanup(me); // For Linux
        pthread_exit(NULL); // Invoke pthread_exit to terminate and invoke the cleanup functions.
}

// Block until thread finishes execution
void Thread::waitForRunToFinish ()
{
	pthread_mutex_lock(&mutex); // Lock mutex which will block in case the thread is running
	pthread_mutex_unlock(&mutex);   // Unlock mutex as soon as it is acquired,which guarantees that the thread terminated
}
// Get Thread readable Identifier
char * Thread::getThreadIdentifier ()
{
	return identifier;  // return thread readable identifier
}
// Selector: Check if thread is started 
bool Thread::isAlive ()
{
    if ( !started ) return false;
    else return true;
}
// Destructor
Thread::~Thread()
{
    pthread_attr_destroy(&pthread_attr);    // Destroy pthread attributes    
    if ( started ) pthread_join (pthread,NULL); // Join on pthread if started
    pthread_mutex_destroy(&mutex);  //Destroy execution control mutex
}
