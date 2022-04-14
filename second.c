#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) { //В качестве аргумента передаем длинну строки, которую мы берем.

  int* shared_memory; //Указатель на выделенную общую память
  int shmid; //IPC-дескриптор
  char x; //Флаг-символ для удаления общей памяти.

  int len = atoi(argv[1])+1;

  char memory_path[] = "first.c"; //Используется для создания ключа. Одинаковый во втором файле.
  key_t key = ftok(memory_path, 0); //IPC-ключ

  if (key < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((shmid = shmget(key, len*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0){
    if(errno != EEXIST && errno != EINVAL) {
      if (errno == EEXIST) {
        printf("Can\'t create shared memory :( \n");
        exit(-1);
      }
      else {
        printf("There's no enough free space in segment");
      }
    }

    else {
    /* Если из-за того, что разделяемая память уже существует пытаемся получить ее IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости инициализации элементов массива */
      if ((shmid = shmget(key, len*sizeof(char), 0)) < 0) {
        printf("Can\'t find shared memory \n");
        exit(-1);
      }
    }
  }

  shared_memory = (int *)shmat(shmid, NULL, 0);

  if (shared_memory == (int *)(-1)) { //Если не получилось отобразить память.
    printf("Can't attach shared memory\n");
    exit(-1);
  }

  printf("THE STRING: ");
  for (int i = 0; i < len-1; i++) {printf("%c", shared_memory[i]);}
  printf("\n");


  if(shmdt(shared_memory) < 0){
    printf("Can't detach shared memory\n");
    exit(-1);
  }

  printf("Delete shared memory? y/n: "); 
  scanf("%c", &x);

  if (x == 'y') {shmctl(shmid, IPC_RMID, NULL); printf("done");}

  return 0;
}
