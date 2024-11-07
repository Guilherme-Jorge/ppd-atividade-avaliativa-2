#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 3
#define NUM_ITERATIONS 2

pthread_barrier_t barrier;

void simulate_work()
{
  // Simulate random work by sleeping for 1-3 seconds
  int work_time = (rand() % 3) + 1;
  printf("Thread %lu doing work for %d seconds...\n",
         pthread_self(), work_time);
  sleep(work_time);
}

void *worker(void *arg)
{
  int thread_id = *((int *)arg);

  for (int i = 0; i < NUM_ITERATIONS; i++)
  {
    printf("\nThread %d starting iteration %d\n", thread_id, i + 1);

    // Phase 1: Each thread does some initial work
    printf("Thread %d performing Phase 1 work\n", thread_id);
    simulate_work();

    // Wait for all threads to complete Phase 1
    printf("Thread %d waiting at first barrier...\n", thread_id);
    pthread_barrier_wait(&barrier);

    // Phase 2: Each thread does some additional work
    printf("Thread %d performing Phase 2 work\n", thread_id);
    simulate_work();

    // Wait for all threads to complete Phase 2
    printf("Thread %d waiting at second barrier...\n", thread_id);
    pthread_barrier_wait(&barrier);

    printf("Thread %d completed iteration %d\n", thread_id, i + 1);
  }

  return NULL;
}

int main()
{
  pthread_t threads[NUM_THREADS];
  int thread_ids[NUM_THREADS];

  // Initialize random seed
  srand(time(NULL));

  // Initialize barrier
  if (pthread_barrier_init(&barrier, NULL, NUM_THREADS) != 0)
  {
    perror("Failed to initialize barrier");
    exit(1);
  }

  // Create threads
  for (int i = 0; i < NUM_THREADS; i++)
  {
    thread_ids[i] = i;
    if (pthread_create(&threads[i], NULL, worker, &thread_ids[i]) != 0)
    {
      perror("Failed to create thread");
      exit(1);
    }
  }

  // Wait for all threads to complete
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Destroy barrier
  pthread_barrier_destroy(&barrier);

  printf("\nAll threads completed their work!\n");
  return 0;
}