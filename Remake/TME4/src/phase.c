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

  MPI_Status status;
  
  int nb_voisins_in;  
  MPI_Recv(&nb_voisins_in,1,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  int nb_voisins_out;  
  MPI_Recv(&nb_voisins_out,1,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  int voisins_in[nb_voisins_in];
  MPI_Recv(voisins_in,nb_voisins_in,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  int voisins_out[nb_voisins_out];
  MPI_Recv(voisins_out,nb_voisins_out,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);
  int min_local;
  MPI_Recv(&min_local,1,MPI_INT,0, TAGINIT, MPI_COMM_WORLD, &status);

  int RCount[nb_voisins_out];
  int Scount=0;
  int min_recu;

  int i=0,j,k,l;
  
  for (i=0;i<nb_voisins_out;i++)
    RCount[i]=0;

 loop:
  
  for (i=0;i<nb_voisins_in;i++){
    if(RCount[i]>=Scount && Scount < DIAMETRE){
      for (j=0;j<nb_voisins_out;j++){
	printf("%d envoie message a %d\n",rang,voisins_out[j]);
	MPI_Send(&min_local, 1, MPI_INT, voisins_out[j], 1, MPI_COMM_WORLD);
      }
      Scount++;
      break;
    }
  }

  
  MPI_Recv(&min_recu,1,MPI_INT,MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
  //printf("%d recoi message de %d\n",rang,status.MPI_SOURCE);
  if(min_local>min_recu)
    min_local=min_recu;
  for(k=0;k<nb_voisins_in;k++){
    if(voisins_in[k]==status.MPI_SOURCE){
      RCount[k]++;
      break;
    }
  }
  
  for(l=0;l<nb_voisins_in;l++){
    if(RCount[l]<DIAMETRE)
      goto loop;
  }
  printf("%d decision le minimum est %d\n",rang,min_local);
  
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
