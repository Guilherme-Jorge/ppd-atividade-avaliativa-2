#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct
{
  int value;
  int reader_count;
  int writer_waiting;
  int writer_active;
  pthread_mutex_t mutex;
  pthread_cond_t read_cond;
  pthread_cond_t write_cond;
} SharedData;

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

void cleanup_shared_data(SharedData *data)
{
  pthread_mutex_destroy(&data->mutex);
  pthread_cond_destroy(&data->read_cond);
  pthread_cond_destroy(&data->write_cond);
}

void *reader(void *arg)
{
  SharedData *data = (SharedData *)arg;

  while (1)
  {
    pthread_mutex_lock(&data->mutex);

    while (data->writer_active || data->writer_waiting > 0)
    {
      pthread_cond_wait(&data->read_cond, &data->mutex);
    }

    data->reader_count++;

    pthread_mutex_unlock(&data->mutex);

    printf("Reader %lu is reading value: %d\n",
           (unsigned long)pthread_self(), data->value);

    pthread_mutex_lock(&data->mutex);

    data->reader_count--;

    if (data->reader_count == 0)
    {
      pthread_cond_signal(&data->write_cond);
    }

    pthread_mutex_unlock(&data->mutex);

    usleep(rand() % 1000000);
  }
  return NULL;
}

void *writer(void *arg)
{
  SharedData *data = (SharedData *)arg;

  while (1)
  {
    pthread_mutex_lock(&data->mutex);

    data->writer_waiting++;

    while (data->reader_count > 0 || data->writer_active)
    {
      pthread_cond_wait(&data->write_cond, &data->mutex);
    }

    data->writer_waiting--;
    data->writer_active = 1;

    data->value++;
    printf("Writer %lu is writing value: %d\n",
           (unsigned long)pthread_self(), data->value);

    data->writer_active = 0;

    pthread_cond_broadcast(&data->read_cond);
    pthread_cond_signal(&data->write_cond);

    pthread_mutex_unlock(&data->mutex);

    usleep(rand() % 2000000);
  }
  return NULL;
}

int main()
{
  SharedData shared_data;
  init_shared_data(&shared_data);

  pthread_t readers[5], writers[2];
  int i;

  for (i = 0; i < 5; i++)
  {
    if (pthread_create(&readers[i], NULL, reader, &shared_data) != 0)
    {
      perror("Failed to create reader thread");
      exit(1);
    }
  }

  for (i = 0; i < 2; i++)
  {
    if (pthread_create(&writers[i], NULL, writer, &shared_data) != 0)
    {
      perror("Failed to create writer thread");
      exit(1);
    }
  }

  printf("Press Enter to exit...\n");
  getchar();

  cleanup_shared_data(&shared_data);
  return 0;
}