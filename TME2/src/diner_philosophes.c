#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <mpi.h>
#include <string.h>

#define SIZE 128

#define MASTER 0

#define REQ_BAG 0
#define ACK_BAG 1
#define FIN 2

int *listbag;
int priority;
int priorityd;
int priorityg;
int NB;
int tag=0;
int source;
int dest;
int i;
int msg=1;
int rang;
int size;
MPI_Status status;
char message[SIZE];

void prendrebaguettes(int a){
  /*
   * Modulo la taille du tableau
   */
  tag=0;
  
  int j,k;
  j=a+1;
  if (j==NB)
    j=1;
  k=a-1;
  if (k==0)
    k=NB-1;

  printf("%d %d\n",j,k);

  MPI_Send(&priority, 1,MPI_INT,k,tag,MPI_COMM_WORLD);
  MPI_Send(&priority, 1,MPI_INT,j,tag,MPI_COMM_WORLD);
  MPI_Recv(&priorityd,1,MPI_INT,k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  printf("XX");
  MPI_Recv(&priorityg,1,MPI_INT,j,MPI_ANY_TAG , MPI_COMM_WORLD, &status);

  if (priorityd > priority){
    MPI_Recv(&priorityd,1,MPI_INT,k,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    priority++;
  }
  if (priorityd < priority){
    MPI_Recv(&priorityd,1,MPI_INT,j,MPI_ANY_TAG , MPI_COMM_WORLD, &status);
    priority++;
  }

  while (listbag[a]==0 || listbag[j]==0){
    printf("baguettes dejà prise...\n");
    sleep(1);
  }
  
  listbag[a]=0;
  listbag[j]=0;
  
}

void poserbaguettes(int a){

  int j,k;
  j=a+1;
  if (j==NB)
    j=0;
  k=a-1;
  if (k==0)
    k=NB;

  if (listbag[a]==1 || listbag[j]==1){
    printf("baguette dejà poser => impossible\n");
  }
  
  listbag[j]=1;
  listbag[a]=1;

  MPI_Send(&priority,1,MPI_INT,k,tag,MPI_COMM_WORLD);
  MPI_Send(&priority,1,MPI_INT,j,tag,MPI_COMM_WORLD);
}

void go (int a, int C){
  int i;
  for(i=0;i<C;i++){
    printf("Le philosophe %d pense\n",a);
    sleep(1);
    prendrebaguettes(a);
    printf("Le philosophe %d mange\n",a);
    sleep(2);
    poserbaguettes(a);  
  }
}


int main (int argc, char** argv){
  int local_clock;
  /*
   * Question 6 :
   * MPI_send(&local_clock,1,MPI_INT,dest,tag,MPI_COMM_WORLD);
   */

  listbag=malloc(size * sizeof(int));

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  priority=rang;
  NB = size;
  int NB_MEALS=1; // nb cycles

  if(rang!=MASTER){
    for (i=0;i<NB;i++){
      listbag[i]=1;
    }
    go(rang,NB_MEALS);
    sprintf(message,"Le philosophe %d se termine",rang);
    dest=MASTER;
    MPI_Send(message, strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
  }

  for(i=0;i<size-1;i++){
     MPI_Recv(message,SIZE,MPI_CHAR,i+1, tag, MPI_COMM_WORLD, &status);
     printf("%s\n",message);
  }
  
  printf("Fin\n");
  free(listbag);
  MPI_Finalize();
  return EXIT_SUCCESS;
}

