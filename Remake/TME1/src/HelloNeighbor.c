#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 128


int main(int argc, char **argv){

  int rang;
  int size;
  int source;
  int dest;
  int tag=0;
  char message[SIZE];
  
  MPI_Status status;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);

  strcpy(message,"hello");
  dest=(rang+1)%size;
  source=(rang-1);
  if(source < 0)
    source+=size;

  printf ("Processus %d envoi message à %d\n",rang,dest);
  MPI_Send(message, strlen(message)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD);
  MPI_Recv(message,SIZE,MPI_CHAR,source, tag, MPI_COMM_WORLD, &status);
  printf ("Processus %d recoi message de %d\n",rang,source);
  MPI_Finalize();
  return 0;
}
		       
