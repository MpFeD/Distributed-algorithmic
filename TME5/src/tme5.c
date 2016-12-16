/*
 * David
 * TOTY
 * 3000755
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define nb 3

int nb_ack;
MPI_Status status;
int file[100];
int h;
int etat;
/*
 * 0 not requesting
 * 1 requestiong
 * 2 SC
 */

int date_r;
int message;
/*
 * 0 REQ
 * 1 ACK
 */

int rang;
void reception_msg(int mj, int hj,int j);

void init(){
  int i;
  for(i=0;i<100;i++)
    file[i]=-1;
  nb_ack=0;
  h=0;
  etat=0;
  date_r=0;
}

void demande_SC(){
  h++;
  etat=1;
  date_r=h;
  nb_ack=0;
  int i;
  int rang_r;
  int h_r;
  int message_r;
  for(i=0;i<nb;i++){
    if(i==rang)
      continue;
    message=0;
    MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    MPI_Send(&h, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
    MPI_Send(&rang, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
  }
  
  i=0;
  while(1){
    
    MPI_Recv(&message_r, 1, MPI_INT, MPI_ANY_SOURCE, 0,MPI_COMM_WORLD,&status);
    MPI_Recv(&h_r, 1, MPI_INT, MPI_ANY_SOURCE, 1,MPI_COMM_WORLD,&status);
    MPI_Recv(&rang_r, 1, MPI_INT, MPI_ANY_SOURCE, 2,MPI_COMM_WORLD,&status);
    reception_msg(message_r,h_r,rang_r);
    i++;
    if(etat==2)
      break;
  }
}

void sortie_SC(){
  h++;
  int i=0;
  message=2;
  while(file[i]!=-1){
    MPI_Send(&message, 1, MPI_INT, file[i], 0, MPI_COMM_WORLD);
    MPI_Send(&h, 1, MPI_INT, file[i], 1, MPI_COMM_WORLD);
    MPI_Send(&rang, 1, MPI_INT, file[i], 2, MPI_COMM_WORLD);
    i++;
  }
  for(i=0;i<nb;i++)
    file[i]=-1;
}


void reception_msg(int mj, int hj,int j){
  int i=0;
  if(h<hj)
    h=hj;
  h++;
  if (mj==0){
     if( ((etat==0)||(etat==1)) && ( (hj < date_r) || ( (hj == date_r) && (j<rang)))){
      message=1;
      MPI_Send(&message, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
      MPI_Send(&h, 1, MPI_INT, j, 1, MPI_COMM_WORLD);
      MPI_Send(&rang, 1, MPI_INT, j, 2, MPI_COMM_WORLD);
    }else{
      while(file[i]!=-1)
	i++;
      file[i]=j;
    }
  }else{
    nb_ack++;
    if (nb_ack==nb-1){
      etat=2;
    }
  }
}
  

void go(){
  init();
  demande_SC();
  printf("%d accede a SC\n",rang);
  sleep(2);
  sortie_SC();
}


int main (int argc, char* argv[]) {
  int nb_proc;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  if(nb_proc!=3){
    printf("il faut utiliser 3 comme argument\n");
    MPI_Finalize();
    return -1;
  }
  go();

  MPI_Finalize();
  return 0;
}
