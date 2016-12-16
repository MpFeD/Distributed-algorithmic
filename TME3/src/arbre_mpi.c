/*
 * David
 * TOTY
 * 3000755
 */


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
  int nb_voisins;
  int min_local;
  int min_tmp;
  int voisins[NB_SITE+1];
  int i;
  MPI_Status status;
  int last;
  int cpt;
  int q;
  int decision=1;
  int recu[NB_SITE];
  
  MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(voisins, nb_voisins, MPI_INT,0 , TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&min_local, 1, MPI_INT,0, TAGINIT,MPI_COMM_WORLD,&status);
  
  /*
   * Initialistion
   */
  for(i=0;i<nb_voisins;i++){
    recu[voisins[i]]=0;
  }

  
  int sent=0;
  while(decision){
    cpt=0;
    for(i=0;i<nb_voisins;i++){
      if(recu[voisins[i]]==1){
	cpt++;
      }
      else
	q=voisins[i];
    }
    
    /*
     * J'ai recu de tous mes voisins : je peux decider
     */
    if(cpt == nb_voisins){
      decision=0;
      break;
    }

    /*
     * Sp : J'ai recu de tous mes voisins sauf 1 (q)
     * et je n'ai pas encore envoye
     */
    if(sent == 0 && cpt == nb_voisins-1){
      MPI_Send(&min_local, 1, MPI_INT, q, 1, MPI_COMM_WORLD);
      sent=1;
    }

    /*
     * Reception et traitement du min.
     */
    MPI_Recv(&min_tmp, 1, MPI_INT, MPI_ANY_SOURCE, 1,MPI_COMM_WORLD,&status);
    recu[status.MPI_SOURCE]=1;
    last=status.MPI_SOURCE;

    if(min_tmp < min_local)
      min_local=min_tmp;
  }

  /*
   * Decision
   */
  printf("Je suis %d, le min est %d\n",rang,min_local);

  /*
   * Les 2 decideurs peuvent envoyes a leurs voisins le min.
   * Cela permet surtout de les debloquer.
   */
  for(i=0;i<nb_voisins;i++){
    if(voisins[i]!=last)
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
