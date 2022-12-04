/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#include <stdio.h>
#include <stdlib.h>
#include "joueur.h"

Joueurs *init_struct_joueurs(int nbJoueurs) {
    Joueurs *j = (Joueurs *)malloc(sizeof(Joueurs));

    j->nb = nbJoueurs;
    j->tabJoueur = (Joueur *)malloc(j->nb*sizeof(Joueur));
         
    for (int i = 0; i < j->nb; i++) {

        j->tabJoueur[i].nom[0] = 'C';
        j->tabJoueur[i].nom[1] = (char)(i+49);
        j->tabJoueur[i].positionCourante = -1;
        j->tabJoueur[i].x = -1;
        j->tabJoueur[i].y = -1;
        j->tabJoueur[i].nbCaseParcourue = 0;

        if (j->nb == 2 && i == 1) { // Cas particulier ou on a que 2 joueurs. On place le 2ème joueur en face du premier
            j->tabJoueur[i].positionFinale = 27;
        } else {
            j->tabJoueur[i].positionFinale = (55 + (14*i)) % 56;  
        }
    }

    return j;
}

int joueur_gagne(int nbCases) {
    if (nbCases == 63) {
        return 0;
    }
    else{
        return 1;
    }
}

void free_joueur(Joueurs *j) {
    free(j->tabJoueur);
    free(j);
}




