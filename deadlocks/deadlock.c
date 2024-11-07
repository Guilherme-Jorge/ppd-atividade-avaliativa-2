#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t resourceA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resourceB = PTHREAD_MUTEX_INITIALIZER;

void *thread1_function(void *arg)
{
  printf("Thread 1: Attempting to lock resource A\n");
  pthread_mutex_lock(&resourceA);
  printf("Thread 1: Acquired resource A\n");

  sleep(1);

  printf("Thread 1: Attempting to lock resource B\n");
  pthread_mutex_lock(&resourceB);
  printf("Thread 1: Acquired resource B\n");

  printf("Thread 1: Working in critical section\n");

  pthread_mutex_unlock(&resourceB);
  pthread_mutex_unlock(&resourceA);

  return NULL;
}

void *thread2_function(void *arg)
{
  printf("Thread 2: Attempting to lock resource B\n");
  pthread_mutex_lock(&resourceB);
  printf("Thread 2: Acquired resource B\n");

  sleep(1);

  printf("Thread 2: Attempting to lock resource A\n");
  pthread_mutex_lock(&resourceA);
  printf("Thread 2: Acquired resource A\n");

  printf("Thread 2: Working in critical section\n");

  pthread_mutex_unlock(&resourceA);
  pthread_mutex_unlock(&resourceB);

  return NULL;
}

int main()
{
  pthread_t thread1, thread2;

  printf("Creating threads...\n");

  pthread_create(&thread1, NULL, thread1_function, NULL);

  pthread_create(&thread2, NULL, thread2_function, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Program completed\n");
  return 0;
}