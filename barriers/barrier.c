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

    printf("Thread %d performing Phase 1 work\n", thread_id);
    simulate_work();

    printf("Thread %d waiting at first barrier...\n", thread_id);
    pthread_barrier_wait(&barrier);

    printf("Thread %d performing Phase 2 work\n", thread_id);
    simulate_work();

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

  srand(time(NULL));

  pthread_barrier_init(&barrier, NULL, NUM_THREADS);

  for (int i = 0; i < NUM_THREADS; i++)
  {
    thread_ids[i] = i;
    pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  pthread_barrier_destroy(&barrier);

  printf("\nAll threads completed their work!\n");
  return 0;
}