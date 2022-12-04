/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#ifndef PLATEAU_H
#define PLATEAU_H

#include "joueur.h"

#define LIGNE 31
#define COLONNE 46

#define MAX_ROLL 6 
#define MIN_ROLL 1

typedef enum { AVANCE, IMPOSSIBLE, COLLISION, DEPASSE } deplacement;

/**
 * Chaque case du plateau est de taille ligne * colonne = 3 * 4
 * Illustration : 	|--| 
 * 				  	|  |
 * 				  	|--|
 * 
 * Autrement dit, pour chaques cases, les informations que l'on peut y mettre (chiffres, chevaux joueurs,...)
 * se trouvent aux emplacement "ligne 2 colonne 2" ou/et "ligne 2 colonne 3"
 * Le plateau est construit de manière séquentielle afin de facilité la compréhension
 * 
 * Voici des représentations du plateau                          
 *  
 * |-----------------|--|--|--|-----------------|   |-----------------|--|--|--|-----------------|
 * |                 |  |C1|  |                 |   |                 |54|55|0 |                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |1 |  |                 |   |                 |53|  |1 |                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |2 |  |                 |   |                 |52|  |2 |                 |
 * |    Joueur 4     |--|--|--|    Joueur 1     |   |                 |--|--|--|                 |
 * |                 |  |3 |  |                 |   |                 |51|  |3 |                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |4 |  |                 |   |                 |50|  |4 |                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |5 |  |                 |   |                 |49|  |5 |                 |
 * |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
 * |  |  |  |  |  |  |  |6 |  |  |  |  |  |  |  |   |42|43|44|45|46|47|48|  |6 |7 |8 |9 |10|11|12|
 * |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
 * |C4|1 |2 |3 |4 |5 |6 |  |6 |5 |4 |3 |2 |1 |C2|   |41|  |  |  |  |  |  |  |  |  |  |  |  |  |13|
 * |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
 * |  |  |  |  |  |  |  |6 |  |  |  |  |  |  |  |   |40|39|38|37|36|35|34|  |20|19|18|17|16|15|14|
 * |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
 * |                 |  |5 |  |                 |   |                 |33|  |21|                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |4 |  |                 |   |                 |32|  |22|                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |3 |  |                 |   |                 |31|  |23|                 |
 * |    Joueur 3     |--|--|--|    Joueur 2     |   |                 |--|--|--|                 |
 * |                 |  |2 |  |                 |   |                 |30|  |24|                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |1 |  |                 |   |                 |29|  |25|                 |
 * |                 |--|--|--|                 |   |                 |--|--|--|                 |
 * |                 |  |C3|  |                 |   |                 |28|27|26|                 |
 * |-----------------|--|--|--|-----------------|   |-----------------|--|--|--|-----------------|
 * 
 * 
 * Le premier joueur à jouer sera toujours celui placé sur la case du cavalier C1. Dans le cas de 2 joueurs, 
 * le 2ème joueur sera à l'emplacement du cavalier C3. Dans le cas de 3 joueurs, la seule chose qui change et 
 * qu'il n'y aura pas de cavalier C4.
 * Dans la règle classique, un cavalier n'existe pas sur le plateau tant que le joueur n'a pas lancé au moins une 
 * fois les dés (il est placé sur le carré vide de son joueur). On décide ici que la position de départ d'un joueur 
 * est celle qu'il est sensé atteindre après un tour de plateau (même si sa position courante est indéfinie au départ)
 * 
 * Un cavalier doit parcourir, depuis la première case comprise, 56 cases avant de tenter l'ascension vers la victoire
 * Nous allons ici simplifier les déplacements des cavaliers en associant chaques cases, en partant de 0, à la cordonnée x,y 
 * du premier des 2 caractères que l'on peut placer dans une case (Cordonnée du 'C' de 'C1', ou des chiffres allant
 * de 1 à 6).
 * Les déplacements se feront donc par numéro, et une fonction de convertion s'occupera de lui associer le bon emplacement.
 * Dans le schéma, le cavalier C1 se trouve au numéro 55, la case à sa droite possède le numéro 0.
 * 
 * Pour l'ascension, on se sert du nombre de cases parcourues d'un cavalier. Si la valeur est égale à 56 c'est qu'il est 
 * au pied de son échelle. On s'en sert aussi pour savoir si le joueur à gagné (56 cases + 7 cases à gravir = 63)
 */ 

/************************************************************************************************************************/


/** Détermine si le déplacement est possible, s'il tombe sur la case ou se trouve déjà un cavalier,
 * si un joueur le bloque ou s'il dépasse la case au pied de l'échelle. Dans le cas ou un joueur le bloque ou 
 * qu'il tombe sur la case d'un joueur, renvoie le numéro de ce dernier en changeant la valeur donné en pointeur
 * que représente le paramètre numAdv.
 * Renvoie un élément de type énum deplacement permettant au processus de terminer l'action à suivre.
 */
deplacement statut_deplacement(Joueurs *j, int numLanceurDes, int valeurDes, int *numAdv);

/** Calcule le numéro de sa nouvelle position en fonction de:
 *              - la valeur du dés et sa position finale si le cavalier n'est pas encore sur le terrain
 *              - la valeur du dés et sa position actuelle sinon
 */
int calcule_nouvelle_position(int pActuelle, int pFinale, int valeurDes);

/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

/** Le paramètre n est le nombre de cases parcourues en comptant celui actuel + la valeur des dés. 
 * Renvoie la valeur maximale à parcourir avant d'atteindre l'échelle gagnante - la
 * valeur parcourue actuelement ajouté à la valeur du dés. 
 * Renvoie le recul à effectuer si positif ou un nombre <= 0 si ça ne dépasse pas.
 * Note: la notion de recul n'a pas eu le temps d'être exploité.
 */  
int depasse_case_finale(int n, int valeurDes);

/** Le paramètre n est le nombre de cases parcourues en comptant celui actuel + la valeur des dés. 
 *  Si elle est égal à 56, c'est qu'on est sur la position de l'echelle finale.
 */ 
int sur_case_finale(int n, int valeurDes);

/** Retourne le numéro du premier cavalier qui lui bloque (numero appartient à l'intervalle [1-nbJoueur]) le chemin 
 * s'il avance de sa position + la valeur du dés ou -1 si personne ne le lui bloque
 */
int chemin_est_bloque(Joueurs *j, int num, int posActuelle, int nouvellePosition, int valeurDes);

/** Retourne le numero du cavalier avec qui il collisionne (numero appartient à l'intervalle [1-nbJoueur]) s'il avance de 
 * sa position + la valeur du dés ou -1 si il ne rentre dans personne
 */
int collision_avec_cavalier(Joueurs *j, int num, int nouvellePosition, int valeurDes);

/* Réinitialise la position d'un cavalier */
void retour_case_depart(Joueur *j);

/** Fonction qui sert pour la monté de l'échelle. Prend en paramètre le nombre de case parcourue par un cavalier
 * qui est forcement >= 56 et lui associe la valeur de dés qu'il doit obtenir pour passer au palier suivant.
 */ 
int valeur_attendue_du_des(int n);

/** Actualise sa position sur le plateau(de la valeur du dés) et dans sa structure de Joueur dans le 
 * cas ou il peut avancer.
 */ 
void deplacer_cavalier(char **plateau, Joueur *j, int nouvellePosition, int valeurDes);

/** Actualise sa position sur le plateau et dans sa structure de Joueur dans le cadre ou il 
 * monte un palier de l'échelle finale.
 */ 
void monter_echelle(char **plateau, Joueur *j);

char **init_plateau();

void affiche_plateau(char **p);

void affiche_statut_deplacement(deplacement s);

void free_plateau(char **p);

#endif
