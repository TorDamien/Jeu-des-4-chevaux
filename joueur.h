/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#ifndef JOUEUR_H
#define JOUEUR_H

/** Les positions x,y et positionCourante sont initialisées à -1. Ce qui indique que le cavalier du 
 * joueur n'est pas encore sur le terrain
 */
typedef struct Joueur {
    char nom[2];
    int x;
    int y;
    int positionCourante;
    int positionFinale; 
    int nbCaseParcourue; 
} Joueur;

typedef struct Joueurs {
    Joueur *tabJoueur;
    int nb;
} Joueurs;

/* Retourne 0 si un joueur à gagné, 1 sinon */
int joueur_gagne(int nbCases);

Joueurs *init_struct_joueurs(int nbJoueurs);

void free_joueur(Joueurs *j);


#endif