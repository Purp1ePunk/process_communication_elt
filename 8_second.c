
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main()
{
printf("--WAITING--");
int *array; /* Указатель на разделяемую память */
int shmid, semid;/* IPC дескриптор для области разделяемой памяти */
int new = 1; /* Флаг необходимости инициализации элементов массива */
char pathname[] = "8_first.c"; /* Имя файла, использующееся для генерации ключа. Файл с таким именем должен существовать в текущей директории */
key_t key; /* IPC ключ */
struct sembuf mybuf;
/* Генерируем IPC ключ из имени файла 06-1a.c в текущей директории и номера экземпляра области разделяемой памяти 0 */

if((key = ftok(pathname,0)) < 0){
printf("Can\'t generate key\n");
exit(-1);
}

if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
printf("Can\'t get semid\n");
exit(-1);
}

mybuf.sem_op = -1; //D(s1, 1)
mybuf.sem_flg = 0;
mybuf.sem_num = 0;

/* Пытаемся эксклюзивно создать разделяемую память для сгенерированного ключа, т.е. если для этого ключа она уже существует, то системный вызов вернет отрицательное значение. Размер памяти определяем как размер массива из 3-х целых переменных, права доступа 0666 - чтение и запись разрешены для всех */


if((shmid = shmget(key, 3*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0){


/* В случае возникновения ошибки пытаемся определить: возникла ли она из-за того, что сегмент разделяемой памяти уже существует, или по другой причине */

if(errno != EEXIST){

/* Если по другой причине - прекращаем работу */

printf("Can\'t create shared memory\n");
exit(-1);
} else {

/* Если из-за того, что разделяемая память уже существует, пытаемся получить ее IPC дескриптор и, в случае удачи, сбрасываем флаг необходимости инициализации элементов массива */

 if((shmid = shmget(key, 3*sizeof(int), 0)) < 0){
printf("Can\'t find shared memory\n");
exit(-1);
}
new = 0;
}
}


/* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса. Обратите внимание на то, что для правильного сравнения мы явно преобразовываем значение -1 к указателю на целое. */

if((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)){
printf("Can't attach shared memory\n");
exit(-1);
}


/* В зависимости от значения флага new либо инициализируем массив, либо увеличиваем соответствующие счетчики */


if(semop(semid, &mybuf, 1) < 0){ //D(s1, 1)
printf("Can\'t wait for condition\n");
exit(-1);
}

if(new){
array[0] = 0;
array[1] = 1;
array[2] = 1;
} else {
array[1] += 1;

array[2] += 1;
}


/* Печатаем новые значения счетчиков, удаляем разделяемую память из адресного пространства текущего процесса и завершаем работу */

printf("Program 1 was spawn %d times, program 2 - %d times, total - %d times\n",
array[0], array[1], array[2]);

if(shmdt(array) < 0){
printf("Can't detach shared memory\n");
exit(-1);
}
return 0;
}
