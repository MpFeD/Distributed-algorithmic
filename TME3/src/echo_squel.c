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
   int nb_voisins[NB_SITE+1] = {-1, 3, 3, 2, 3, 5, 2};
   int min_local[NB_SITE+1] = {-1, 12, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][5] = {{-1, -1, -1, -1, -1},
            {2, 5, 3, -1, -1}, {4, 1, 5, -1, -1}, 
            {1, 5, -1, -1, -1}, {6, 2, 5, -1, -1},
            {1, 2, 6, 4, 3}, {4, 5, -1, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
   }
}

void calcul_min(int rang){

int nb_voisins;
  int min_local;
  int min_tmp;
  int voisins[NB_SITE+1];
  int i;
  MPI_Status status;
  int pere=0;
  int decision=1;
  int recu[NB_SITE];
  int Spcond=1;

  
  MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(voisins, nb_voisins, MPI_INT,0 , TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&min_local, 1, MPI_INT,0, TAGINIT,MPI_COMM_WORLD,&status);


  /*
   * Initialistion
   */
  for(i=0;i<nb_voisins;i++){
    recu[voisins[i]]=0;
  }

  
  /*
   * Initiateur.
   * C'est ici celui de rang 1 mais cette valeure est arbitraire.
   */
  if(rang == 1){
    /*
     * Sp : Envoi a tous les voisins
     */
    for(i=0;i<nb_voisins;i++){
      MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
    }

    /*
     * Rp : Reception de message tant qu'on ne peut pas decider.
     */
    while(decision){
      MPI_Recv(&min_tmp, 1, MPI_INT,MPI_ANY_SOURCE, 1,MPI_COMM_WORLD,&status);
      if(min_tmp < min_local)
	min_local=min_tmp;
      
      recu[status.MPI_SOURCE]=1;

      decision=0;
      for(i=0;i<nb_voisins;i++)
	if(recu[voisins[i]]==0){
	  decision=1;
	  break;     
	}
    }
    /*
     * On a recu de tous nos voisins : Decision
     */
    printf("Le minimum est %d\n",min_local);
  }else{ /* Non initiateur */
    /*
     * Reception de message.
     * On s'arrete pas tant que l'on a
     * pas eu tous nos voisins.
     */
    while(Spcond){
      MPI_Recv(&min_tmp, 1, MPI_INT,MPI_ANY_SOURCE, 1,MPI_COMM_WORLD,&status);
      if (min_tmp<min_local)
	min_local=min_tmp;
      recu[status.MPI_SOURCE]=1;
      if(pere==0){
	/*
	 * Premier message recu : c'est mon pere
	 * J'envoi a tous mes voisins sauf mon pere
	 */
	pere=status.MPI_SOURCE;
	for(i=0;i<nb_voisins;i++)
	  if(voisins[i]!=pere){
	    MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
	  }
      }
      /*
       * Sp vaut 0 tant que je n'ai pas recu de tous mes voisins
       */
      Spcond=0;
      for(i=0;i<nb_voisins;i++)
	if(recu[voisins[i]]==0){
	  Spcond=1;
	  break;     
	} 
    }
    /*
     * Sp : J'envoi le min a mon pere
     */
    MPI_Send(&min_local, 1, MPI_INT, pere, 1, MPI_COMM_WORLD);
  }
}


/******************************************************************************/

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
