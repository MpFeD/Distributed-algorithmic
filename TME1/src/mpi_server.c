#include "../include/mpi_server.h"

static server the_server;

int my_rank;
int nb_proc;

int main(int argc, char **argv){

  int source;
  int dest;
  int tag=0;
  int provided;
  int flag;
  MPI_Status status;

  void msg_pdn(int tag,int source){
    char message[128];
    if(tag==2){
      MPI_Recv(message,128,MPI_CHAR,source , tag, MPI_COMM_WORLD, &status);
      printf("x");
    }
    else{
      printf("y");
    }
  }

  MPI_Init_thread(&argc,&argv,MPI_THREAD_SERIALIZED,&provided);
  the_server.callback=msg_pdn;

  
  while(1){
    pthread_mutex_lock(&the_server.mutex);
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD,&flag, &status);
    pthread_mutex_unlock(&the_server.mutex);
    if(flag){
      //arg_server->callback...
	  printf("Il y a un message\n");
    }
    else
      printf("Pas de message\n");
  }
  
  return 0;
}

  
void start_server (void (*callback)(int tag, int source)){

  
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

}

void destroy_server(){

}

pthread_mutex_t* getMutex(){
  
  return &the_server.mutex;
  
}



