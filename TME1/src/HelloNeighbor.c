#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 128


int main(int argc, char **argv){

  int my_rank;
  int nb_proc;
  int source;
  int dest;
  int tag=0;
  char message[SIZE];
  
  MPI_Status status;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  sprintf(message, "%d",my_rank);
  
  if(my_rank != MASTER){
    MPI_Ssend(message, strlen(message)+1,MPI_CHAR,(my_rank+1)%nb_proc,tag,MPI_COMM_WORLD);
    MPI_Recv(message,SIZE,MPI_CHAR,(my_rank-1)%nb_proc , tag, MPI_COMM_WORLD, &status);
    printf("Je suis %d et je recois %s\n",my_rank,message);
  }
  else{
    MPI_Recv(message,SIZE,MPI_CHAR,(my_rank-1)%nb_proc , tag, MPI_COMM_WORLD, &status);
    printf("Je suis %d et je recois %s\n",my_rank,message);
    sprintf(message, "%d",my_rank);
    MPI_Ssend(message, strlen(message)+1,MPI_CHAR,(my_rank+1)%nb_proc,tag,MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  return 0;
}
		       
