#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define NB_SITE 6

void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][3] = {{-1, -1, -1},
         {2, 3, -1}, {1, 4, 5}, 
         {1, 6, -1}, {2, -1, -1},
         {2, -1, -1}, {3, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
   }
}

/******************************************************************************/

void calcul_min(int rang){

  MPI_Status status;
  int nb_voisins;  
  MPI_Recv(&nb_voisins,1,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  
  int voisins[nb_voisins];
  MPI_Recv(voisins,nb_voisins,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  int min_local;
  MPI_Recv(&min_local,1,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);

  int recu[nb_voisins];
  int q=0;
  int sent=0;

  int i;
  int nb;
  int min_recu;

  
  for (i=0;i<nb_voisins;i++)
    recu[i]=0;

 loop:
  nb=nb_voisins;
  for(i=0;i<nb_voisins;i++){
    if(recu[i]==0){
      q=voisins[i];
      nb--;
    }
  }

  if(nb==nb_voisins){
     printf("decision du %d le min est %d\n",rang,min_local);
     goto end;
  }

  if(nb==nb_voisins-1 && sent==0){
    MPI_Send(&min_local, 1, MPI_INT, q, 1, MPI_COMM_WORLD);
    sent=1;
  }	   
  MPI_Recv(&min_recu,1,MPI_INT,MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
  if(min_local>min_recu)
    min_local=min_recu;
  for(i=0;i<nb_voisins;i++){
    if(voisins[i]==status.MPI_SOURCE){
      recu[i]=1;
      break;
    }
  }
  goto loop;

 end: //Pour debloquer
  for(i=0;i<nb_voisins;i++){
    MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
  }
}


int main (int argc, char* argv[]) {
   int nb_proc,rang;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

   if (nb_proc != NB_SITE+1) {
      printf("Nombre de processus incorrect !\n");
      MPI_Finalize();
      exit(2);
   }
  
   MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
   if (rang == 0) {
      simulateur();
   } else {
      calcul_min(rang);
   }
  
   MPI_Finalize();
   return 0;
}
