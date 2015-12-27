/*
    threads_echo_caps.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define SIZE 100
#define MAX_THREADS 200
#define MAX_WAIT 1000000

#define DEFAULT_NUMBER_OF_THREADS 2
#define DEFAULT_WAIT 20

typedef struct
{
    char buff[SIZE];      // Buffer shared by all
    char end;             // Flag
    int numthr;           // # of "caps" threads
    int wtime;            // Time in microseconds
    sem_t semaphore_io;   // The I/O thread waits the others here
    sem_t semaphore_caps; // The "caps" threads wait I/O here
}
shared_data;

pthread_mutex_t mutexcaps;//mutex to avoid race condition in caps

int tsrandom (void);      // Like rand() but thread-safe

void * io (void *);       // Input/Output
void * caps (void *);     // Convert to capital letters

int main (int argc, char * argv[])
{
    pthread_t tids[MAX_THREADS];   // Thread identifiers
    int i, e;
    shared_data sd;   // Buffer, parameters and semaphores

    sd.end = 0;

    if (argc > 1)                      // Parse parameters
    {
        if (sscanf(argv[1],"%d",&sd.numthr) != 1 ||
            sd.numthr < 1 || sd.numthr > MAX_THREADS)
        {
            printf ("Wrong \"#_of_threads\" parameter.\n");
            sd.end = 1;
        }
    }
    else
        sd.numthr = DEFAULT_NUMBER_OF_THREADS;

    if (argc > 2)
    {
        if (sscanf(argv[2],"%d",&sd.wtime) != 1 ||
            sd.wtime < 0 || sd.wtime > MAX_WAIT)
        {
            printf ("Wrong \"microseconds\" parameter.\n");
            sd.end = 1;
        }
    }
    else
        sd.wtime = DEFAULT_WAIT;

    if (sd.end)     // If wrong params., print message and quit
    {
        printf ("Usage:\n\t%s [ #_of_threads "
                             "[ microseconds ] ]\n", argv[0]);
        printf ("Max. #_of_threads: %d\n", MAX_THREADS);
        printf ("Max. microseconds: %d\n", MAX_WAIT);
        return -2;
    }

    srand ((unsigned)getpid() ^ (unsigned)time(NULL));

    if (sem_init (&sd.semaphore_io, 0, 0) ||   // Semaphores
        sem_init (&sd.semaphore_caps, 0, 0))   // initially
    {                                          // in red (0)
        perror ("sem_init");
        return -1;
    }
                                          // Lauch "caps" threads
    for (i=0; i<sd.numthr; i++)
        if ((e=pthread_create (&tids[i], NULL, caps, &sd)) != 0)
        {
            fprintf (stderr, "Error %d while "
                             "creating thread # %d\n", e, i+1);
            return -1;
        }

                  // Run Input/Output in the main thread
    io (&sd);     // (though it might run in a new thread...)

                                 // Wait for end of "caps" thrds.
    for (i=0; i<sd.numthr; i++)
        if ((e=pthread_join (tids[i], NULL)) != 0)
        {
            fprintf (stderr, "Error %d waiting for "
                             "thread # %d\n", e, i+1);
            return -2;
        }

    sem_destroy (&sd.semaphore_io);
    sem_destroy (&sd.semaphore_caps);

    return 0;
}

void * io (void * pv)          // Input/Output
{
    shared_data * sd = (shared_data *) pv;
    int i;

    printf ("Type lines of text (Ctrl+D to finish):\n");

    for (;;)
    {
        if (!fgets (sd->buff, SIZE, stdin))   // Read string
            sd->end = 1;                      // from keyboard

        for (i=0; i<sd->numthr; i++)          // Give way to
            sem_post (&sd->semaphore_caps);   // "caps" threads

        if (sd->end)                // If EOF in fgets(), quit
            return NULL;
                                           // Wait until "caps"
        for (i=0; i<sd->numthr; i++)       // threads give way
            sem_wait (&sd->semaphore_io);  // to me
                                           
        fputs (sd->buff, stdout);   // Show resulting string
    }
}

void * caps (void * pv)        // Convert to capital letters
{
    shared_data * sd = (shared_data *) pv;
    int i;
    char c;

    for (;;)
    {
        sem_wait (&sd->semaphore_caps);    // Wait until I/O
                                           // gives way to me
        if (sd->end)
            return NULL;
                                           // Convert some
        for (i=0; sd->buff[i]; i++)        // letters to caps.
            if (tsrandom() > RAND_MAX/2)   // (with prob. 0.5)
            {
		pthread_mutex_lock (&mutexcaps);
		//lock to avoid more threads from accessing memory               
		c = sd->buff[i];             // Read character	

                if (c >= 'a' && c <= 'z')    // If small letter
                {
                    if (sd->wtime)               // Take a
                        usleep (sd->wtime);      // nap

		    sd->buff[i] -= ('a' - 'A');  // Convert
                }
		pthread_mutex_unlock (&mutexcaps);//unlock once it's done
            }

        sem_post (&sd->semaphore_io);       // Give way to I/O
    }
}

int tsrandom (void)     // Like rand() but thread-safe
{
    int i;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock (&mutex);     // The mutex guarantees that
                                     // the code between lock and
    i = rand ();                     // unlock will run in mutual
                                     // exclusion: There'll never
    pthread_mutex_unlock (&mutex);   // be more than one thread
                                     // executing i = rand ()
    return i;                        // at the same time.
}

