#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Shared data structure
typedef struct
{
  int value;                 // The shared resource
  int reader_count;          // Number of active readers
  int writer_waiting;        // Number of writers waiting
  int writer_active;         // Whether a writer is active
  pthread_mutex_t mutex;     // Main mutex for the structure
  pthread_cond_t read_cond;  // Condition for readers
  pthread_cond_t write_cond; // Condition for writers
} SharedData;

// Function prototypes
void *reader(void *arg);
void *writer(void *arg);
void init_shared_data(SharedData *data);
void cleanup_shared_data(SharedData *data);

// Initialize the shared data structure
void init_shared_data(SharedData *data)
{
  data->value = 0;
  data->reader_count = 0;
  data->writer_waiting = 0;
  data->writer_active = 0;

  pthread_mutex_init(&data->mutex, NULL);
  pthread_cond_init(&data->read_cond, NULL);
  pthread_cond_init(&data->write_cond, NULL);
}

// Clean up resources
void cleanup_shared_data(SharedData *data)
{
  pthread_mutex_destroy(&data->mutex);
  pthread_cond_destroy(&data->read_cond);
  pthread_cond_destroy(&data->write_cond);
}

// Reader process
void *reader(void *arg)
{
  SharedData *data = (SharedData *)arg;

  while (1)
  {
    pthread_mutex_lock(&data->mutex);

    // Wait while a writer is active or waiting
    while (data->writer_active || data->writer_waiting > 0)
    {
      pthread_cond_wait(&data->read_cond, &data->mutex);
    }

    // Enter critical section
    data->reader_count++;

    pthread_mutex_unlock(&data->mutex);

    // Reading section
    printf("Reader %lu is reading value: %d\n",
           (unsigned long)pthread_self(), data->value);

    pthread_mutex_lock(&data->mutex);

    // Exit critical section
    data->reader_count--;

    // If this is the last reader, signal a waiting writer
    if (data->reader_count == 0)
    {
      pthread_cond_signal(&data->write_cond);
    }

    pthread_mutex_unlock(&data->mutex);

    // Random delay before next read
    usleep(rand() % 1000000);
  }
  return NULL;
}

// Writer process
void *writer(void *arg)
{
  SharedData *data = (SharedData *)arg;

  while (1)
  {
    pthread_mutex_lock(&data->mutex);

    data->writer_waiting++;

    // Wait while there are readers or another writer is active
    while (data->reader_count > 0 || data->writer_active)
    {
      pthread_cond_wait(&data->write_cond, &data->mutex);
    }

    // Enter critical section
    data->writer_waiting--;
    data->writer_active = 1;

    // Writing section
    data->value++;
    printf("Writer %lu is writing value: %d\n",
           (unsigned long)pthread_self(), data->value);

    // Exit critical section
    data->writer_active = 0;

    // Signal all waiting readers
    pthread_cond_broadcast(&data->read_cond);
    // Signal next writer
    pthread_cond_signal(&data->write_cond);

    pthread_mutex_unlock(&data->mutex);

    // Random delay before next write
    usleep(rand() % 2000000);
  }
  return NULL;
}

int main()
{
  SharedData shared_data;
  init_shared_data(&shared_data);

  // Create reader and writer threads
  pthread_t readers[5], writers[2];
  int i;

  // Create 5 reader threads
  for (i = 0; i < 5; i++)
  {
    if (pthread_create(&readers[i], NULL, reader, &shared_data) != 0)
    {
      perror("Failed to create reader thread");
      exit(1);
    }
  }

  // Create 2 writer threads
  for (i = 0; i < 2; i++)
  {
    if (pthread_create(&writers[i], NULL, writer, &shared_data) != 0)
    {
      perror("Failed to create writer thread");
      exit(1);
    }
  }

  // Wait for user input to terminate
  printf("Press Enter to exit...\n");
  getchar();

  // Cleanup
  cleanup_shared_data(&shared_data);
  return 0;
}