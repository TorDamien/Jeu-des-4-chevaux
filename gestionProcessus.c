/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "gestionProcessus.h"
#include "joueur.h"
#include "plateau.h"


void exec_jeu() {
    int nbJoueurs = nombre_de_joueurs();
    Joueurs *lesJoueurs = init_struct_joueurs(nbJoueurs);
    Pipe *lesPipes = init_struct_pipe(lesJoueurs->nb);
    char **plateau = init_plateau();
    exec_processus(lesPipes, lesJoueurs, plateau);
    free_all_malloc(lesPipes, lesJoueurs, plateau);
}

void exec_processus(Pipe *p, Joueurs *j, char **plateau) {
/** Toujours garder à l'esprit que en C nos joueurs sont stokés dans le tableau de 0 à nbJoueurs-1. 
 * Mais qu'on manipule une variable allant de 1 à nbJoueurs. C'est plus simple pour la manipulation 
 * mentale et l'application. Mais il faut y penser dans l'envoie des paramètres des fonctions qui utilisent
 * par exemple le modulo et ou là c'est mieux de travailler sur l'intervalle 0 à nbJoueurs-1 
 */
        for (int i = 0; i < j->nb; i++) {
            switch (fork()) {
                case -1:
                    perror("fork");
                    exit(-1);
                case 0: /* Partie processus fils */
                    srand(time(NULL)+getpid()); 
                    int continuer = 1;
                    do {

                        continuer = gestion_tour_fils(p, j, plateau, i);                  
                    
                    } while (continuer);
                    ferme_tous_les_pipe(p, j->nb);
                    exit(0);   
                default:
                    break;
            }
        }

        /* Partie processus père */
        int premier = 0;
        int continuer = 1;

        while (continuer) {
            //sleep(1);
            continuer = gestion_tour_pere(p, j, plateau, premier);

            if (continuer) {
                premier = (premier + 1) % j->nb;
            }
        }
        message_de_fin(premier+1);
        ferme_tous_les_pipe(p, j->nb);
        while (wait(NULL)!=-1);
} 

int nombre_de_joueurs() {
    int nb;
    char flush; 
    do {
        flush = ' ';
        printf("Entrer le nombre de joueurs souhaité(2, 3 ou 4): ");
        scanf("%d", &nb);
        while (flush != '\n') { // Permet de vider le buffer de l'entrée standard au cas ou on écrive n'importe quoi
            scanf("%c", &flush);
        }
        printf("%d\n", nb);
    } while (nb != 2 && nb != 3 && nb != 4);

    return nb;
}

int gestion_tour_fils(Pipe *p, Joueurs *j, char **plateau, int i) {
    int joueurQuiCommence = 0;
    deplacement statut;
    int valeurDes, newP, numAdv;

    joueurQuiCommence = premier_joueur(p->pereVersJoueur[i]);
    valeurDes = tour_joueur(p, j, i+1, joueurQuiCommence);
    
    if (j->tabJoueur[joueurQuiCommence-1].nbCaseParcourue >= 56) { // Si le joueur se trouve au pied de l'échelle ou plus loin
        int v = valeur_attendue_du_des(j->tabJoueur[joueurQuiCommence-1].nbCaseParcourue);
        
        if (v == valeurDes) {
            monter_echelle(plateau, &(j->tabJoueur[joueurQuiCommence-1]));
        } 

    } else {
        statut = statut_deplacement(j, joueurQuiCommence, valeurDes, &numAdv);
        newP = calcule_nouvelle_position(j->tabJoueur[joueurQuiCommence-1].positionCourante, j->tabJoueur[joueurQuiCommence-1].positionFinale, valeurDes);
        switch (statut) {
            case IMPOSSIBLE:
            case DEPASSE:
                break;
            case COLLISION:
                retour_case_depart(&(j->tabJoueur[numAdv-1]));
                deplacer_cavalier(plateau, &(j->tabJoueur[joueurQuiCommence-1]), newP, valeurDes);
                break;
            default: // Le joueur a le champ libre pour avancer
                deplacer_cavalier(plateau, &(j->tabJoueur[joueurQuiCommence-1]), newP, valeurDes);
        }
    }
    return joueur_gagne(j->tabJoueur[joueurQuiCommence-1].nbCaseParcourue);
}


int gestion_tour_pere(Pipe *p, Joueurs *j, char **plateau, int premier) {
    int valeurDes = 0;
    deplacement statut;
    int newP, numAdv;

    prevenir_joueur_commence(p->pereVersJoueur, j->nb, premier+1); // prevenir_joueur_commence
    valeurDes = pere_recupere_info(p->joueurVersPere);
    
    if (j->tabJoueur[premier].nbCaseParcourue >= 56) { // Si le joueur se trouve au pied de l'échelle ou plus loin
        int v = valeur_attendue_du_des(j->tabJoueur[premier].nbCaseParcourue);

        if (v == valeurDes) {
            monter_echelle(plateau, &(j->tabJoueur[premier]));
            printf("Le joueur %d lance le dés. Il obtient un %d.\n", premier+1, valeurDes);
            printf("Il grimpe d'une case.\n");
        } else {
            printf("Le joueur %d lance le dés. Il obtient un %d.\n", premier+1, valeurDes);
            printf("Il ne bouge pas ce tour-ci.\n");
        }
    } else {
        statut = statut_deplacement(j, premier+1, valeurDes, &numAdv);
        newP = calcule_nouvelle_position(j->tabJoueur[premier].positionCourante, j->tabJoueur[premier].positionFinale, valeurDes);
        switch (statut) {
            case IMPOSSIBLE:
                printf("Le joueur %d lance le dés. Il obtient un %d.\n", premier+1, valeurDes);
                printf("Il n'a pas pu avancer car le joueur %d lui bloque le passage.\n", numAdv);
                break;
            case DEPASSE:
                printf("Le joueur %d lance le dés. Il obtient un %d.\n", premier+1, valeurDes);
                printf("Il n'a pas pu avancer car il dépasserait l'échelle.\n");
                break;
            case COLLISION:
                retour_case_depart(&(j->tabJoueur[numAdv-1]));
                deplacer_cavalier(plateau, &(j->tabJoueur[premier]), newP, valeurDes);
                printf("Le joueur %d lance le dés. Il avance de %d cases.\n", premier+1, valeurDes);
                printf("Le joueur %d se trouve déjà sur cette case. Il est renvoyé à la case départ.\n", numAdv);
                break;
            default: // Le joueur a le champ libre pour avancer
                printf("Le joueur %d lance le dés. Il avance de %d cases.\n", premier+1, valeurDes);
                deplacer_cavalier(plateau, &(j->tabJoueur[premier]), newP, valeurDes);
        }
    }

    affiche_plateau(plateau);
    printf("\n---------------------------------------------------------\n\n");

    return joueur_gagne(j->tabJoueur[premier].nbCaseParcourue);
}

void prevenir_joueur_commence(int **tabPipe, int nbJoueurs, int premier) {
    for (int i = 0; i < nbJoueurs; i++) {
        write(tabPipe[i][1], &premier, sizeof(int));
    }
}

int premier_joueur(int *pipe) {
    int joueurQuiCommence;
    read(pipe[0], &joueurQuiCommence, sizeof(int));
    return joueurQuiCommence;
}

int pere_recupere_info(int *pipe) {
    int valeurDes;
    read(pipe[0], &valeurDes, sizeof(int));
    return valeurDes;
}

void placer_les_positions(int *tabPosition, int premier, int nbJoueurs) {

    for (int i = 0; i < nbJoueurs; i++) {
        
        if (premier > nbJoueurs) {
            premier = 1;
        }
        tabPosition[i] = premier;
        premier++;
    }
}

int tour_joueur(Pipe *p, Joueurs *j, int joueurCourant, int joueurQuiCommence) {
    int tabPosition[j->nb];
    int valeurDes;

    placer_les_positions(tabPosition, joueurQuiCommence, j->nb); 

    if (joueurCourant == tabPosition[0]) { 
        valeurDes = joue_en_premier(p->joueurVersJoueur, joueurCourant);
    } else if (joueurCourant == tabPosition[j->nb-1]) { // A changé
        valeurDes = joue_en_dernier(p->joueurVersJoueur, p->joueurVersPere, joueurCourant, j->nb);
    } else {
        valeurDes = joue_au_milieu(p->joueurVersJoueur, joueurCourant, j->nb);
    }
    return valeurDes;
}

int joue_en_premier(int **pipe, int joueur) {
    int valeurDes = (rand() % (MAX_ROLL - MIN_ROLL + 1)) + MIN_ROLL;
    write(pipe[joueur-1][1], &valeurDes, sizeof(int));
    return valeurDes;
}

int joue_au_milieu(int **pipe, int joueur, int nbJoueurs) {
    int valeurDes;
    
    if (joueur == 1) { // cas du joueur 1 qui doit lire dans le pipe du joueur précédent étant le joueur 2, 3 ou 4
        read(pipe[nbJoueurs-1][0], &valeurDes, sizeof(int));
    } else {
        read(pipe[joueur-2][0], &valeurDes, sizeof(int));
    }
    write(pipe[joueur-1][1], &valeurDes, sizeof(int));
    return valeurDes;
}

int joue_en_dernier(int **pipe, int *pipeInformerPere, int joueur, int nbJoueurs) {
    int valeurDes;

    if (joueur == 1) { // cas du joueur 1 qui doit lire dans le pipe du joueur précédent étant le joueur 2, 3 ou 4
        read(pipe[nbJoueurs-1][0], &valeurDes, sizeof(int));
    } else {
        read(pipe[joueur-2][0], &valeurDes, sizeof(int));
    }
    write(pipeInformerPere[1], &valeurDes, sizeof(int));
    return valeurDes;
}

void message_de_fin(int premier) {
    printf("Le joueur %d a gagné la partie\n", premier+1);
    printf("Merci d'avoir joué\n");
    printf("\n----------------------Fermeture de l'application----------------------\n");
}

/***************************INITIALISATION ET LIBERATION***************************/


Pipe *init_struct_pipe(int n) {
    Pipe *p = (Pipe *)malloc(sizeof(Pipe));
    p->joueurVersJoueur = ini_pipe(n);
    p->pereVersJoueur = ini_pipe(n);
    p->joueurVersPere = (int *)malloc(2*sizeof(int));
    pipe(p->joueurVersPere);
    return p;
}


int **ini_pipe(int n) {
    
    int **p = (int **)malloc(n*sizeof(int *));
    for (int i = 0; i < n; i++) {
        p[i] = (int *)malloc(n*sizeof(int));
    } 

    for (int j=0; j<n; j++) {
        pipe(p[j]);
    }

    return p;
}


void free_all_malloc(Pipe *p, Joueurs *j, char **plateau) {
    free_all_pipe(p, j->nb);
    free_joueur(j);
    free_plateau(plateau);
}

void free_all_pipe(Pipe *p, int n) {
    free_pipe(p->joueurVersJoueur, n);
    free_pipe(p->pereVersJoueur, n);
    free(p->joueurVersPere);
}

void free_pipe(int **p, int n) {
    for (int i = 0; i < n; i++) {
        free(p[i]); 
    } 
    free(p);
}


void ferme_tous_les_pipe(Pipe *p, int n) {
    for (int i = 0; i < n; i++) {
        close(p->joueurVersJoueur[i][0]);
        close(p->joueurVersJoueur[i][1]);
        close(p->pereVersJoueur[i][0]);
        close(p->pereVersJoueur[i][1]);
    }
    close(p->joueurVersPere[0]);
    close(p->joueurVersPere[1]);
}
