#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


#define TAGINIT    0
#define TAGSUPR 1
#define TAG_RECH 2
#define TAG_ADD 3
#define TAG_SUPR 4
#define TAG_SITE_ADD 5
#define TAG_SITE_SUPR 6
#define NB_SITE 10
#define NB_DONNEE 200
#define m 8


int rang; //Rang du site
int successeur;
int identifiant_chord;
int identifiant_chord_successeur;
int identifiant_chord_premiere_donnee;
int premierSuccesseur;
int donnee[2][NB_DONNEE];
int mon_index=0;
int actif=1;

void sort_c(int array[], int size);
void retirer_site(int n);
void AfficherActif();
void trouverDonnee(int site,int id_donnee);
void ajouter_site(int n);

int puissance (int a, int n){
  if(n==0)
    return 1;
  return a * (puissance (a , n-1));
}

void simulateur(void) {
  int i,n,j;
  int identifiant_chord_site[NB_SITE]; /* Id Chord des sites */
  int numeroCorrect;  /* Controle des numero identifiant chord deja attribue */

  memset(identifiant_chord_site,'\0',NB_SITE);
  srand(getpid());

  /* 
   * Cette boucle permet d'attribuer
   * un identifiant chord pour chaque
   * site et de manière aléatoire 
   * tout en controlant l'unicité
   * de chaque clé 
   */
  
  for(i=0;i< NB_SITE;i++){
    do {
      numeroCorrect=1;
      n=rand() % ((puissance(2,m) +1 ));
      for(j=0;j<NB_SITE;j++)
	if (n==identifiant_chord_site[j]){
	  numeroCorrect=0;
	  break;
	}
    }
    while(!numeroCorrect);
    identifiant_chord_site[i]=n;
  }

  sort_c(identifiant_chord_site, NB_SITE);

  
  
  for(i=0;i<NB_SITE;i++)
    printf("%d\n",identifiant_chord_site[i]);

  printf ("******\n");

  int y;
  int PremierSite=1;
  int tmp;
  for(i=0; i<NB_SITE; i++){
    y=i+2;
    
    if(y<=NB_SITE)
      MPI_Send(&y, 1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    else
      MPI_Send(&PremierSite, 1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    
    MPI_Send(&identifiant_chord_site[i],1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    
    if(y<=NB_SITE)
      MPI_Send(&identifiant_chord_site[i+1],1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    else
      MPI_Send(&identifiant_chord_site[0],1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    
    if(i==0)
      tmp=identifiant_chord_site[9]+1;
    else
      tmp=identifiant_chord_site[i-1]+1;
    
    MPI_Send(&tmp,1, MPI_INT, i+1, TAGINIT, MPI_COMM_WORLD);
    
  }
}

/******************************************************************************/

void sort_c(int array[], int size){
  int i = 0;
  int j = 0;
  int tmp = 0;
  for(i=0; i<size ; i++)
    {
      for(j=i; j<size; j++)
        {
	  if(array[j]<array[i]){
	    tmp = array[i];
	    array[i] = array[j];
	    array[j] = tmp;
	  }
        }
    }
}

void init(){
  int i;
  for(i=0;i<NB_DONNEE;i++){
    donnee[0][i]=-1;
    donnee[1][i]=-1;
  }
}

void ajouterDonnee(int id_chord_donnee, int valeur){
  if(actif!=-1){
    int i,j;
    int k=0;
    MPI_Status status;
  
    if(id_chord_donnee >= identifiant_chord_premiere_donnee && id_chord_donnee <= identifiant_chord){
      for(i=0;i<mon_index;i++)
	if(donnee[0][i] == id_chord_donnee){
	  donnee[1][i]=valeur;
	  k=1;
	}
      if(k==0){
	donnee[0][mon_index]=id_chord_donnee;
	donnee[1][mon_index++]=valeur;
      }
    }

    k=0;
    int donneeRcv;
    int pasDeDonnee=-1;
    if(id_chord_donnee > identifiant_chord && identifiant_chord > identifiant_chord_successeur)
      MPI_Send(&id_chord_donnee,1, MPI_INT, successeur, TAG_ADD, MPI_COMM_WORLD);
    else
      MPI_Send(&pasDeDonnee,1, MPI_INT, successeur, TAG_ADD, MPI_COMM_WORLD);
  
    MPI_Recv(&donneeRcv, 1, MPI_INT, MPI_ANY_SOURCE, TAG_ADD,MPI_COMM_WORLD,&status);
  

    if(donneeRcv!=-1){
      for(i=0;i<mon_index;i++)
	if(donnee[0][i] == donneeRcv){
	  donnee[1][i]=valeur;
	  k=1;
	}
      if(k==0){
	donnee[0][mon_index]=donneeRcv;
	donnee[1][mon_index++]=valeur;
      }
    }
  }
}

void supprimerDonne(int id_chord_donnee){
  MPI_Status status;
  int i;
  if(id_chord_donnee >= identifiant_chord_premiere_donnee && id_chord_donnee <= identifiant_chord){
    for(i=0;i<mon_index;i++)
      if(donnee[0][i] == id_chord_donnee){
	donnee[0][i] =-1;
	donnee[1][i]=-1;
	mon_index--;
	break;
      }
  }

  int donneeRcv;
  int pasDeDonnee=-1;
  if(id_chord_donnee > identifiant_chord && identifiant_chord > identifiant_chord_successeur)
    MPI_Send(&id_chord_donnee,1, MPI_INT, successeur, TAG_SUPR, MPI_COMM_WORLD);
  else
    MPI_Send(&pasDeDonnee,1, MPI_INT, successeur, TAG_SUPR, MPI_COMM_WORLD);
  
  MPI_Recv(&donneeRcv, 1, MPI_INT, MPI_ANY_SOURCE, TAG_SUPR,MPI_COMM_WORLD,&status);
  

  if(donneeRcv!=-1){
    for(i=0;i<mon_index;i++){
      if(donnee[0][i] == donneeRcv){
	donnee[0][i] =-1;
	donnee[1][i]=-1;
	mon_index--;
	break;
      }
    }
  }

  
}

void AfficherDonnee(){
  int i;
  if(actif!=-1)
  for(i=0;i<mon_index;i++){
    printf ("rang = %d clé = %d valeur = %d\n",rang,donnee[0][i],donnee[1][i]);
  }
}


void affStatus(){
  if(actif!=-1)
  printf ("rang = %d id chord = %d mon successeur est %d son id chord %d id chord p donnee %d\n",rang,identifiant_chord,successeur,identifiant_chord_successeur,identifiant_chord_premiere_donnee);
}

void run(int n){
  MPI_Status status;
  int i,j;
  
  rang = n;
 
  MPI_Recv(&successeur, 1, MPI_INT,MPI_ANY_SOURCE, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&identifiant_chord, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&identifiant_chord_successeur, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT,MPI_COMM_WORLD,&status);
  MPI_Recv(&identifiant_chord_premiere_donnee, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT,MPI_COMM_WORLD,&status);

  premierSuccesseur = successeur;
  
  affStatus();
  init();
  ajouterDonnee(255,10);
  ajouterDonnee(150,15);
  ajouterDonnee(150,20);
  supprimerDonne(150);
  supprimerDonne(255);
  
  ajouterDonnee(70,10);
  ajouterDonnee(65,15);
  ajouterDonnee(60,20);
  ajouterDonnee(55,15);
  ajouterDonnee(50,20);
  
  ajouterDonnee(80,10);
  ajouterDonnee(85,15);
  ajouterDonnee(90,20);
  ajouterDonnee(95,15);
  ajouterDonnee(100,20);
  
  ajouterDonnee(45,10);
  ajouterDonnee(40,15);
  ajouterDonnee(35,20);
  ajouterDonnee(105,15);
  ajouterDonnee(110,20);

  
  sleep(1);
  AfficherDonnee();
  fflush(stdout);
  sleep(1);
  retirer_site(3);
  sleep(1);
  AfficherActif();
  fflush(stdout);
  sleep(1);
  AfficherDonnee();
  fflush(stdout);
  sleep(1);
  affStatus();
  fflush(stdout);
  sleep(1);
  trouverDonnee(8,50);  
  fflush(stdout);
  sleep(1);
  ajouter_site(3);
  usleep(500);    
  AfficherActif();
  fflush(stdout);
  usleep(500);
  AfficherDonnee();
  fflush(stdout);
  usleep(500);
  affStatus();
  usleep(500);
  fflush(stdout);
  
}


void retirer_site(int n){
  int i;
  int nb_nouvelle_donnee;
  int donnee1;
  int donnee2;
  int rang_recu;
  int successeur_recu;
  MPI_Status status;
  if (rang == n){
    printf("%d previens %d qu'il s'arrete il y a %d donnee a donner\n",rang,successeur,mon_index);
    MPI_Send(&rang,1, MPI_INT, successeur, TAG_SITE_SUPR, MPI_COMM_WORLD);
    MPI_Send(&successeur,1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
    MPI_Send(&mon_index,1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
    MPI_Send(&identifiant_chord_premiere_donnee,1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
    
    for(i=0;i<mon_index;i++){
       MPI_Send(&donnee[0][i],1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
       MPI_Send(&donnee[1][i],1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
       donnee[0][i]=-1;
       donnee[1][i]=-1;
    }
    mon_index=0;
    actif=-1;
  }

  else{
    if(actif!=-1){
      MPI_Recv(&rang_recu, 1, MPI_INT,MPI_ANY_SOURCE, TAG_SITE_SUPR,MPI_COMM_WORLD,&status);
      MPI_Recv(&successeur_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_SUPR ,MPI_COMM_WORLD,&status);
      printf("%d est informe de l'arret de %d\n",rang,rang_recu);

      if(successeur_recu==rang){
	MPI_Recv(&nb_nouvelle_donnee, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_SUPR ,MPI_COMM_WORLD,&status);
	printf("%d recois qu'il y a %d donnee a stocker\n",rang,nb_nouvelle_donnee);
	MPI_Recv(&identifiant_chord_premiere_donnee, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_SUPR ,MPI_COMM_WORLD,&status);
	for(i=0;i<nb_nouvelle_donnee;i++){
	  MPI_Recv(&donnee1, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_SUPR ,MPI_COMM_WORLD,&status);
	  MPI_Recv(&donnee2, 1, MPI_INT,MPI_ANY_SOURCE, TAG_SITE_SUPR,MPI_COMM_WORLD,&status);
	  donnee[0][mon_index]=donnee1;
	  donnee[1][mon_index++]=donnee2;
	}
      }
      if(rang_recu==successeur){
	successeur=successeur_recu;
	printf("%d change de successeur, nouveau sucesseur : %d\n",rang,successeur);
      }
      else{
	MPI_Send(&rang_recu,1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
	MPI_Send(&successeur_recu,1, MPI_INT, successeur,TAG_SITE_SUPR , MPI_COMM_WORLD);
      }
    }
  }
}


void trouverDonnee(int site,int id_donnee){
  if(actif!=-1){
    int i;
    int identifiant_donnee_recu;
    int valeur_recu=-1;
    int id_chord_responsable=-1;
    int trouver=0;

    MPI_Status status;

    if(site==rang){
      printf("Le site %d a besoin de la cle %d\n",site,id_donnee);
      MPI_Send(&id_donnee,1, MPI_INT, successeur,TAG_RECH, MPI_COMM_WORLD);
      MPI_Send(&valeur_recu,1, MPI_INT, successeur,TAG_RECH , MPI_COMM_WORLD);
      MPI_Send(&id_chord_responsable,1, MPI_INT, successeur,TAG_RECH , MPI_COMM_WORLD);
      
      MPI_Recv(&identifiant_donnee_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_RECH ,MPI_COMM_WORLD,&status);
      MPI_Recv(&valeur_recu, 1, MPI_INT,MPI_ANY_SOURCE, TAG_RECH,MPI_COMM_WORLD,&status);
      MPI_Recv(&id_chord_responsable, 1, MPI_INT,MPI_ANY_SOURCE, TAG_RECH,MPI_COMM_WORLD,&status);

      
      
      printf("Le site %d a bien recu la cle %d sa valeur est %d l'id chord du site responsable de cette cle est %d\n",site,id_donnee,valeur_recu,id_chord_responsable);
    
    }else{

      MPI_Recv(&identifiant_donnee_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_RECH ,MPI_COMM_WORLD,&status);
      MPI_Recv(&valeur_recu, 1, MPI_INT,MPI_ANY_SOURCE, TAG_RECH,MPI_COMM_WORLD,&status);
      MPI_Recv(&id_chord_responsable, 1, MPI_INT,MPI_ANY_SOURCE, TAG_RECH,MPI_COMM_WORLD,&status);
      
      if(valeur_recu==-1){
	for(i=0;i<mon_index;i++)
	  if(donnee[0][i]==identifiant_donnee_recu){
	    printf("Le site %d possede la donnee\n",rang);
	    id_chord_responsable=identifiant_chord;
	    valeur_recu=donnee[1][i];
	    break;
	  }
      }
      MPI_Send(&identifiant_donnee_recu,1, MPI_INT, successeur,TAG_RECH, MPI_COMM_WORLD);
      MPI_Send(&valeur_recu,1, MPI_INT, successeur,TAG_RECH, MPI_COMM_WORLD);
      MPI_Send(&id_chord_responsable,1, MPI_INT, successeur,TAG_RECH, MPI_COMM_WORLD);
    }
  }
}

void ajouter_site(int n){
  MPI_Status status;
  int i;
  int index_recu=0;
  int rang_recu;
  int identifiant_recu;
  int successeur_recu;
  int donnee_recu[2][NB_DONNEE];
  printf("%d : successeur recu = %d\n",rang,successeur);
  
  if(rang == n){
    MPI_Send(&rang,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
    MPI_Send(&successeur,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
    MPI_Send(&identifiant_chord,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
    MPI_Send(&index_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);

    MPI_Recv(&index_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);

    for(i=0;i<index_recu;i++){
      MPI_Recv(&donnee_recu[0][i], 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
      MPI_Recv(&donnee_recu[1][i], 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);

      printf("%d a recu %d : %d\n",rang,donnee_recu[0][i],donnee_recu[1][i]);

      donnee[0][i]=donnee_recu[0][i];
      donnee[1][i]=donnee_recu[1][i];
      mon_index++;
    }
    actif=1;
  }

  else{
    MPI_Recv(&rang_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
    MPI_Recv(&successeur_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
    MPI_Recv(&identifiant_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
    MPI_Recv(&index_recu, 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
    
    for(i=0;i<index_recu;i++){
      MPI_Recv(&donnee_recu[0][i], 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
      MPI_Recv(&donnee_recu[1][i], 1, MPI_INT,MPI_ANY_SOURCE,TAG_SITE_ADD ,MPI_COMM_WORLD,&status);
    }
    
    if(successeur_recu == rang){
      identifiant_chord_premiere_donnee=identifiant_recu+1;
      for(i=0;i<mon_index;i++){
	if (donnee[0][i] <= identifiant_recu && donnee[0][i]!=-1){
	  donnee_recu[0][index_recu]=donnee[0][i];
	  donnee_recu[1][index_recu++]=donnee[1][i];
	  
	  donnee[0][i]=donnee[0][mon_index];
	  donnee[1][i]=donnee[1][mon_index];
	  
	  donnee[0][mon_index]=-1;
	  donnee[1][mon_index]=-1;
	}
      }
      mon_index=mon_index-index_recu;
    }
    if(successeur==successeur_recu){
      successeur=rang_recu;
      identifiant_chord_successeur=identifiant_recu;
      MPI_Send(&index_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
      
    }else{
      MPI_Send(&rang_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
      MPI_Send(&successeur_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
      MPI_Send(&identifiant_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD); 
      MPI_Send(&index_recu,1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
    }
    for(i=0;i<index_recu;i++){
      printf("%d envoie %d ; %d\n",rang, donnee_recu[0][i], donnee_recu[1][i]);
      MPI_Send(&donnee_recu[0][i],1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
      MPI_Send(&donnee_recu[1][i],1, MPI_INT, successeur,TAG_SITE_ADD , MPI_COMM_WORLD);
    }
  }
}

       

void AfficherActif(){
  if(actif==1)
    printf("%d : Actif\n",rang);
}

int main (int argc, char* argv[]) {
  int nb_proc,rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

  if (nb_proc != NB_SITE+1) {
    printf("Nombre de processus incorrect !\n");
    MPI_Finalize();
    exit(2);
  }
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (rank == 0) {
    simulateur();
  } else {
    run(rank);
  }
  
  MPI_Finalize();
  return 0;
}
