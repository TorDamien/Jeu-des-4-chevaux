/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#ifndef GESTION_PROCESSUS_H
#define GESTION_PROCESSUS_H

#include "joueur.h"

typedef struct Pipe
{
    int **joueurVersJoueur;
    int **pereVersJoueur;
    int *joueurVersPere; // A changé
    
} Pipe;

/** Toute première fonction appelé qui récupère le nombre de joueur depuis l'entrée standard,
 * initialise les structures Pipes, Joueurs et le Plateau en fonction du nombre de joueur et appelle la fonction
 * de gestion des processus. 
 * Quand la partie est fini, elle libère la mémoire alloué aux structures Pipe, Joueurs et Plateau.
 */
void exec_jeu();

/** Fonction principale exécutant la création des 4 processus joueurs issus du même père.
 * Chaque processus est identifié par l'indice i auquel il a été crée dans la boucle for.
 * Le joueur 1 sera donc toujours le joueur portant l'indice 0 et le dernier l'indice nbJoueur-1.
 * On ne sort de cette fonction que quand l'un des processus atteint la case gagante.
 * Tous les pipe sont fermés seulement à ce moment là. 
 */
void exec_processus(Pipe *p, Joueurs *j, char **plateau);

/* Récupère sur l'entrée standard un nombre entre 2 et 4 et le renvoie */
int nombre_de_joueurs();

/** Fonction principale des processus fils. L'entier i en paramètre est l'indice i qui identifie le numero du
 * joueur (appartient à l'intervalle [0, nbJoueurs-1])  
 * Récupère la valeur du joueur qui commence ainsi que la valeur du dès après avoir écrit et/ou lu dans les pipe.
 * Détermine l'action que le cavalier fera en fonction du nombre de cases parcourues et des cavaliers ou non devant lui.
 * Actualise les positions sur le plateau
 * Avant de sortir de cette fonction, si le nombre de case parcourue = 63 c'est que le joueur est sur la case finale
 * Cette fonction renvoie 0 si c'est le cas, 1 sinon
 */
int gestion_tour_fils(Pipe *p, Joueurs *j, char **plateau, int i);

/** Cette fonction est similaire à gestion_tour_fils sauf:
 *  - Prend en paramètre le premier joueur et l'envoie au processus fils (premier appartient à l'intervalle [1,nbJoueurs])
 *  - En fonction de l'action que fera le cavalier, cela provoquera un affichage sur la sortie standard 
 *  - Affichage du plateau actualisé à chaque tour
 */ 
int gestion_tour_pere(Pipe *p, Joueurs *j, char **plateau, int premier);

/** Fonction du processus père qui prend en paramètre le tableau des pipe reliant le père à
 * ses fils et écrit sur chacuns d'eux le numéro du joueur qui commence ce tour-ci. L'entier 
 * premier appartient à l'intervalle [1-nbJoueurs].
 */ 
void prevenir_joueur_commence(int **tabPipe, int nbJoueurs, int premier);

/** Fonction des processus fils qui leurs permet de récupérer le numéro du joueur qui commence
 * que le père envoie à chaque tour
 */
int premier_joueur(int *pipe);

/** Fonction du processus père qui récupère, sur le pipe unique reliant les fils à ce dernier, la 
 * valeur du dés. L'entier renvoyé appartient à l'intervalle [1-nbJoueurs].
 */ 
int pere_recupere_info(int *pipe);

/** Fonction des processus fils qui calcule et place les positions des joueurs durant un tour dans 
 * le tableau tabPosition. Les indices du tableau représentent la position dans le tour, et la valeur 
 * à cette indice le joueur associé. Par exemple tabPosition[0] = 3 indique que c'est le 3ème joueur
 * qui commence, tabPosition[3] = 1 indique que le joueur 1 est le dernier(cas à 4 joueurs).
 */
void placer_les_positions(int *tabPosition, int premier, int nbJoueurs);

/** Détermine la position des joueurs en fonction du joueur qui commence
 * et affecte le joueur courant à une action pendant ce tour en fonction de cette position
 * (joue en premier, au milieur ou en dernier)
 */ 
int tour_joueur(Pipe *p, Joueurs *j, int joueurCourant, int joueurQuiCommence);

/** Fonction utilisée par le processus qui commence durant un tour.
 * Lance le dés et envoie cette valeur au joueur 2
 */
int joue_en_premier(int **pipe, int joueur);

/** Cette fonction n'est pas utilisé dans le cas de 2 joueurs. Sinon elle gère la réception de la valeur
 * du dés envoyé par le joueur précédent et l'envoie au joueur suivant
 */ 
int joue_au_milieu(int **pipe, int joueur, int nbJoueurs);

/** Fonction utilisé par le dernier joueur du tour. Récupère la valeur du dés envoyé par le joueur précédent
 * et envoie de cette dernière au père
 */
int joue_en_dernier(int **pipe, int *informerPere, int joueur, int nbJoueurs);

/* Affiche le nom du gagnant et un simple message de fin de programme */
void message_de_fin(int premier);

/***************************INITIALISATION ET LIBERATION***************************/


Pipe *init_struct_pipe(int n);

int **ini_pipe(int n);

void free_all_malloc(Pipe *p, Joueurs *j, char **plateau);

void free_all_pipe(Pipe *p, int n);

void free_pipe(int **p, int n);

void ferme_tous_les_pipe(Pipe *p, int n);

#endif