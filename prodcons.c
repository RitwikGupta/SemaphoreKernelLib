#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <linux/unistd.h>

struct ritwik_sem {
  int count;
  struct Node *first;
  struct Node *last;
};

void down(struct ritwik_sem *sem) {
  syscall(__NR_ritwik_down, sem);
}

void up(struct ritwik_sem *sem) {
  syscall(__NR_ritwik_up, sem);
}

int main(int argc, char *argv[]) {

  int num_prod, num_cons, buffer_size;

  if (argc != 4) {
    printf("You need to enter 3 parameters [NUM_PROD] [NUM_CONSUMER] [BUFFER_SIZE]\n");
    exit(-1);
  }

  num_prod = atoi(argv[1]);
  num_cons = atoi(argv[2]);
  buffer_size = atoi(argv[3]);

  if (num_prod == 0 || num_cons == 0) {
    printf("You have no producer or consumer, and this will result in a deadlock. Please reconsider your choice.\n");
    printf("Sleeping for 2 seconds\n");
    sleep(2);
  }

  void *ptr = mmap(NULL, sizeof(struct ritwik_sem) * 3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  int *buffer_ptr = (int *) mmap(NULL, sizeof(int) * buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  int *index_ptr = (int *) mmap(NULL, sizeof(int) * 2, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);

  struct ritwik_sem *empty;
  struct ritwik_sem *full;
  struct ritwik_sem *mutex;
  int status;

  empty = (struct ritwik_sem *) ptr;
  full = (struct ritwik_sem *) ptr + 1;
  mutex = (struct ritwik_sem *) ptr + 2;

  empty->count = buffer_size;
  full->count = 0;
  mutex->count = 1;

  empty->first = full->first = mutex->first = NULL;
  empty->last = full->last = mutex->last = NULL;

  int *front, *back;

  front = index_ptr;
  back = index_ptr + 1;

  *front = 0;
  *back = 0;

  srand(time(NULL));

  int i;
  for (i = 0; i < num_prod; i++) {
    if (fork() == 0) {
      int item;
      while (1) {
        down(empty);
        down(mutex);

        item = rand() % 10;
        buffer_ptr[*front] = item;
        printf("Producer %c produced %d\n", i + 65, item);
        *front = (*front + 1) % buffer_size;

        up(mutex);
        up(full);
      }
    }
  }

  int j;
  for (j = 0; j < num_cons; j++) {
    if (fork() == 0) {
      int item;
      while (1) {
        down(full);
        down(mutex);

        item = buffer_ptr[*back];
        printf("Consumer %c consumed %d\n", j + 65, item);
        *back = (*back + 1) % buffer_size;

        up(mutex);
        up(empty);
      }
    }
  }

  wait(&status); // threads will never end, so this will never happen

  return 0;

}
