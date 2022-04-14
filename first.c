#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) { //В качестве аргумента передаем строку

  int* shared_memory; //Указатель на выделенную общую память
  int shmid; //IPC-дескриптор

  int len = strlen(argv[1])+1;
  char str[len];
  for (int i = 0; i < len-1; i++) {str[i] = argv[1][i];}
  printf("LEN: %d", len-1);

  char memory_path[] = "first.c"; //Используется для создания ключа. Одинаковый во втором файле.
  key_t key = ftok(memory_path, 0); //IPC-ключ

  if (key < 0) {
    printf("Can\'t generate the key \n");
    exit(-1);
  }
  else {
    if ((shmid = shmget(key, len*sizeof(char), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
      if(errno != EEXIST) {
        printf("Can\'t create shared memory :( \n");
        exit(-1);
      }

      else {
      /* Если из-за того, что разделяемая память уже существует пытаемся получить ее IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости инициализации элементов массива */
        if ((shmid = shmget(key, len*sizeof(char), 0)) < 0) {
          printf("Can\'t find shared memory \n");

          exit(-1);
        }
      }
    }
  }

  //shared_memory = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))

  shared_memory = (int *)shmat(shmid, NULL, 0);

  if (shared_memory == (int *)(-1)) { //Если не получилось отобразить память.
    printf("Can't attach shared memory\n");
    exit(-1);
  }

  for (int i = 0; i < len-1; i++) {shared_memory[i] = str[i];}

  if(shmdt(shared_memory) < 0){
  printf("Can't detach shared memory\n");
  exit(-1);
  }

  return 0;
}
