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

#define DIAMETRE 5		/* !!!!! valeur a initialiser !!!!! */

void simulateur(void) {
  int i;

  /* nb_voisins_in[i] est le nombre de voisins entrants du site i */
  /* nb_voisins_out[i] est le nombre de voisins sortants du site i */
  int nb_voisins_in[NB_SITE+1] = {-1, 2, 1, 1, 2, 1, 1};
  int nb_voisins_out[NB_SITE+1] = {-1, 2, 1, 1, 1, 2, 1};

  int min_local[NB_SITE+1] = {-1, 4, 7, 1, 6, 2, 9};

  /* liste des voisins entrants */
  int voisins_in[NB_SITE+1][2] = {{-1, -1},
				  {4, 5}, {1, -1}, {1, -1},
				  {3, 5}, {6, -1}, {2, -1}};
                               
  /* liste des voisins sortants */
  int voisins_out[NB_SITE+1][2] = {{-1, -1},
				   {2, 3}, {6, -1}, {4, -1},
				   {1, -1}, {1, 4}, {5,-1}};

  for(i=1; i<=NB_SITE; i++){
    MPI_Send(&nb_voisins_in[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(&nb_voisins_out[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(voisins_in[i], nb_voisins_in[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(voisins_out[i], nb_voisins_out[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
  }
}

/******************************************************************************/

void calcul_min(int rang){
  int nb_voisins_in;
  int nb_voisins_out;
  int min_local;
  MPI_Status status;
  int i,j;
  int min_r;
  int decision=1;
  int SCount=0;
  int Spcond=0;

  
  
  MPI_Recv(&nb_voisins_in, 1, MPI_INT, 0, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&nb_voisins_out, 1, MPI_INT,0, TAGINIT,MPI_COMM_WORLD,&status);

  int voisins_in[nb_voisins_in];
  int voisins_out[nb_voisins_out];
  int RCount[NB_SITE];

  
  MPI_Recv(voisins_in, nb_voisins_in, MPI_INT, 0, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(voisins_out, nb_voisins_out, MPI_INT,0, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&min_local, 1, MPI_INT,0 , TAGINIT,MPI_COMM_WORLD,&status);
  
  /*
   * Initialisation.
   */
  for(i=0;i<nb_voisins_in;i++)
    RCount[voisins_in[i]]=0;
 
  while(decision){

    /*
     * La variable Spcond correspond a la condition Sp.
     * Si cette condition est vraie, on envoi le min
     * a tous les voisins sortants
     */
    Spcond=1;
    for(i=0;i<nb_voisins_in;i++){
      if(RCount[voisins_in[i]]<SCount || SCount >= DIAMETRE){
	Spcond=0;
	break;
      }
    }

    /*
     * Si condition Sp vraie
     */
    if(Spcond){
      for(i=0;i<nb_voisins_out;i++){
	MPI_Send(&min_local, 1, MPI_INT, voisins_out[i], TAGINIT, MPI_COMM_WORLD);
      }
      SCount++;
    }

    /*
     * Reception, incrementation de RCount
     */
    MPI_Recv(&min_r, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT,MPI_COMM_WORLD,&status);
    RCount[status.MPI_SOURCE]++;

    /*
     * Test et modification du min
     */
    if(min_r < min_local)
      min_local=min_r;


    /*
     * Test de decision
     * Si cette variable vaut 0 alors on fait la decision.
     * On sort de la boucle principale.
     */
    decision=0;
    for(i=0;i<nb_voisins_in;i++){
      if(RCount[voisins_in[i]]<DIAMETRE){
	decision=1;
	break;
      }
    }
  }

  /*
   * Decision
   */
  printf("Je suis %d, le min est %d\n",rang,min_local);
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
