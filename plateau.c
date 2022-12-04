/** Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA (Octobre 2022)
 * Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien
 */

#include <stdio.h>
#include <stdlib.h>
#include "plateau.h"
#include "joueur.h"
#include <unistd.h>


void affiche_plateau(char **p) {
	for (int l = 0; l < LIGNE; l++) {
		for (int c = 0; c < COLONNE; c++) {
			printf("%c", p[l][c]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void affiche_statut_deplacement(deplacement s) {
	switch (s) {
		case IMPOSSIBLE:
			printf("Impossible\n");
			break;
		case COLLISION:
			printf("Collision\n");
			break;
		default:
			printf("Avancer\n");
	}
}


deplacement statut_deplacement(Joueurs *j, int numLanceurDes, int valeurDes, int *numAdv) {

	deplacement etat = AVANCE;
	int num = numLanceurDes-1; // On repasse sur l'intervalle [0-(nbJoueurs-1)] plutôt que [1-nbJoueurs] pour le modulo
	int lanceurPosFinale = j->tabJoueur[num].positionFinale;
	int posActuelle = j->tabJoueur[num].positionCourante;
	if (posActuelle == -1) { // Si le joueur n'est pas encore sur le plateau, on se base sur sa position finale
		posActuelle = j->tabJoueur[num].positionFinale;
	}

	int nouvellePosition = calcule_nouvelle_position(posActuelle, lanceurPosFinale, valeurDes);
	int tmp;	
	if ((tmp = chemin_est_bloque(j, num, posActuelle, nouvellePosition, valeurDes)) != -1) {
		*numAdv = tmp;
		etat = IMPOSSIBLE;
	} else if ((tmp = collision_avec_cavalier(j, num, nouvellePosition, valeurDes)) != -1) {
		*numAdv = tmp;
		etat = COLLISION;
	} else if ((tmp = depasse_case_finale(j->tabJoueur[num].nbCaseParcourue, valeurDes)) > 0) {
		*numAdv = tmp;
		etat = DEPASSE;
	}  

	return etat;

}

int calcule_nouvelle_position(int pActuelle, int pFinale, int valeurDes) {
	int nouvellePosition;

	if (pActuelle == -1) { // Si le joueur n'est pas encore sur le plateau
		nouvellePosition = (pFinale+valeurDes)%56;
	} else {
		nouvellePosition = (pActuelle+valeurDes)%56;
	} 
	return nouvellePosition;
}

int depasse_case_finale(int n, int valeurDes) {
	return (n+valeurDes) - 56;
}

int sur_case_finale(int n, int valeurDes) {
	return (n+valeurDes) == 56;
}

int chemin_est_bloque(Joueurs *j, int num, int posActuelle, int nouvellePosition, int valeurDes) {
	int estBloque = -1; 
	int i = 1;
	int posAdversaire;
	if (j->tabJoueur[num].positionCourante == -1) {
		if (j->tabJoueur[num].positionFinale != 55) {
			posActuelle = j->tabJoueur[num].positionFinale;
		}
	}

	if (depasse_case_finale(j->tabJoueur[num].nbCaseParcourue, valeurDes) <= 0) { 
		while ( i < j->nb && estBloque == -1) {
			posAdversaire = j->tabJoueur[(num+i)%j->nb].positionCourante;
			if (posAdversaire > posActuelle && posAdversaire < nouvellePosition) {
				estBloque = ((num+i)%j->nb)+1;	/* (num + i) % nbJoueurs ça permet de tester tous les autres joueurs */
			}
			i++;
		}
	}
	return estBloque;
}

int collision_avec_cavalier(Joueurs *j, int num, int nouvellePosition, int valeurDes) {
	int collision = -1; 
	int i = 1;
	int posAdversaire;

	if (depasse_case_finale(j->tabJoueur[num].nbCaseParcourue, valeurDes) <= 0) {
		while ( i < j->nb && collision == -1 ) {
			posAdversaire = j->tabJoueur[(num+i)%j->nb].positionCourante;
			
			if (posAdversaire == nouvellePosition) {
				collision = ((num+i)%j->nb)+1;
			}
			i++;
		}
	}
	return collision;
}

void retour_case_depart(Joueur *j) {
	j->x = -1;
	j->y = -1;
	j->nbCaseParcourue = 0;
	j->positionCourante = -1;
}

int valeur_attendue_du_des(int n) {
	switch (n) {
		case 56:
			return 1;
		case 57:
			return 2;
		case 58:
			return 3;
		case 59:
			return 4;
		case 60:
			return 5;
		case 61:
			return 6;
		default: // case 62
			return 6;

	}
}

void monter_echelle(char **plateau, Joueur *j) {

	plateau[j->x][j->y] = ' ';
	plateau[j->x][j->y+1] = ' ';

	switch (j->positionFinale) {
		case 13:
			j->y-=3;
			plateau[j->x][j->y] = j->nom[0];
			plateau[j->x][j->y+1] = j->nom[1];
			break;
		case 27:
			j->x-=2;
			plateau[j->x][j->y] = j->nom[0];
			plateau[j->x][j->y+1] = j->nom[1];
			break;
		case 41:
			j->y+=3;
			plateau[j->x][j->y] = j->nom[0];
			plateau[j->x][j->y+1] = j->nom[1];
			break;
		case 55: 
			j->x+=2;
			plateau[j->x][j->y] = j->nom[0];
			plateau[j->x][j->y+1] = j->nom[1];
		default:
			break;
	}
	j->nbCaseParcourue++; 
}


char **init_plateau() {
	char **p = (char **)malloc(LIGNE*sizeof(char*));
	for (int l = 0; l < LIGNE; l++) {
		p[l] = (char *)malloc(COLONNE*sizeof(char));
	}

	/**** Définition des bordures du plateau  ****/
	
	// Haut
	for (int l = 0; l < 1; l++) {	
		for (int c = 0; c < COLONNE; c++){
			p[l][c] = '-';
		}
	}

	// Bas
	for (int l = LIGNE-1; l < LIGNE; l++) {
		for (int c = 0; c < COLONNE; c++){
			p[l][c] = '-';
		}
	}

	// Gauche 
	for (int c = 0; c < 1; c++) {
		for (int l = 0; l < LIGNE; l++){
			p[l][c] = '|';
		}
	}

	// Droite
	for (int c = COLONNE-1; c < COLONNE; c++) {
		for (int l = 0; l < LIGNE; l++){
			p[l][c] = '|';
		}
	}


	/**** Remplissage des autres cases par des espaces avant les tracés du plateau intérieur
	 	  On accepte volontairement de parcourir presque 2 fois tout le tableau afin d'en simplifier sa conception ****/ 

	for (int l = 1; l < LIGNE-1; l++) {
		for (int c = 1; c < COLONNE-1; c++){
			p[l][c] = ' ';
		}
	}


	/**** Tracé des lignes verticales ****/

	// Tracé de haut en bas des 4 lignes verticales du milieu 
	for (int c = 18; c <= 27; c += 3) {
		for (int l = 0; l < LIGNE; l++){
			p[l][c] = '|';
		}
	}

	// Lignes verticales à gauche de celles du milieur
	for (int c = 3; c <= 15; c += 3) {
		for (int l = 12; l <= 18; l++){
			p[l][c] = '|';
		}
	}

	// Lignes verticales à droite de celles du milieur
	for (int c = 30; c <= COLONNE-3; c += 3) {
		for (int l = 12; l <= 18; l++){
			p[l][c] = '|';
		}
	}


	/**** Tracé des lignes horizontales ****/

	// Tracé de gauche à droite des 4 lignes horizontales du milieu
	for (int l = 12; l <= 18; l+=2) {	
		for (int c = 1; c < COLONNE-1; c+=3){
			p[l][c] = '-';
			p[l][c+1] = '-';
		}
	}

	// Lignes horizontales au dessus de celles du milieu
	for (int l = 2; l <= 10; l+=2) {	
		for (int c = 19; c <= 26; c+=3){
			p[l][c] = '-';
			p[l][c+1] = '-';
		}
	}

	// Lignes horizontales en dessous de celles du milieu
	for (int l = 20; l <= LIGNE-2; l+=2) {	
		for (int c = 19; c <= 26; c+=3){
			p[l][c] = '-';
			p[l][c+1] = '-';
		}
	}

	/**** Placement des chiffres de 1 à 6 ****/

	// Haut
	int chiffre = 1;
	for (int c=22; c<23; c++) {
		for (int l=3; l<=13; l+=2 ) {
			p[l][c] = (char)(chiffre+48);
			chiffre++; 
		}
	}

	// Bas
	chiffre = 6;
	for (int c=22; c<23; c++) {
		for (int l=17; l<=27; l+=2 ) {
			p[l][c] = (char)(chiffre+48);
			chiffre--; 
		}
	}

	// Gauche
	chiffre = 1;
	for (int l=15; l<16; l++) {
		for (int c=4; c<=20; c+=3 ) {
			p[l][c] = (char)(chiffre+48);
			chiffre++; 
		}
	}

	// Droite
	chiffre = 6;
	for (int l=15; l<16; l++) {
		for (int c=25; c<=40; c+=3 ) {
			p[l][c] = (char)(chiffre+48);
			chiffre--; 
		}
	}

	/* Placement d'un GG sur la case du milieu gagnante */

	p[15][22] = 'G';
	p[15][23] = 'G';

	return p;
}

void free_plateau(char **p) {
    for (int i = 0; i < LIGNE; i++) {
        free(p[i]);
    }
    free(p);
}

/* Incroyable comme fonction n'est-ce pas?  */
void deplacer_cavalier(char **plateau, Joueur *j, int nouvellePosition, int valeurDes) {

	char cara1 = j->nom[0];
	char cara2 = j->nom[1];

	/* On commence par supprimer le cavalier dans le tableau de sa position actuelle */
	if (j->x != -1) {
		plateau[j->x][j->y] = ' ';
		plateau[j->x][j->y+1] = ' ';
	}

	j->nbCaseParcourue += valeurDes; 
	j->positionCourante = nouvellePosition;

	switch (nouvellePosition) {
		case 0:
			plateau[1][25] = cara1;
			plateau[1][26] = cara2;
			j->x = 1;
			j->y = 25;
			break;
		case 1:
			plateau[3][25] = cara1;
			plateau[3][26] = cara2;
			j->x = 3;
			j->y = 25;
			break;
		case 2:
			plateau[5][25] = cara1;
			plateau[5][26] = cara2;
			j->x = 5;
			j->y = 25;
			break;
		case 3:
			plateau[7][25] = cara1;
			plateau[7][26] = cara2;
			j->x = 7;
			j->y = 25;
			break;
		case 4:
			plateau[9][25] = cara1;
			plateau[9][26] = cara2;
			j->x = 9;
			j->y = 25;
			break;
		case 5:
			plateau[11][25] = cara1;
			plateau[11][26] = cara2;
			j->x = 11;
			j->y = 25;
			break;

		case 6:
			plateau[13][25] = cara1;
			plateau[13][26] = cara2;
			j->x = 13;
			j->y = 25;
			break;

		case 7:
			plateau[13][28] = cara1;
			plateau[13][29] = cara2;
			j->x = 13;
			j->y = 28;
			break;
		case 8:
			plateau[13][31] = cara1;
			plateau[13][32] = cara2;
			j->x = 13;
			j->y = 31;
			break;
		case 9:
			plateau[13][34] = cara1;
			plateau[13][35] = cara2;
			j->x = 13;
			j->y = 34;
			break;
		case 10:
			plateau[13][37] = cara1;
			plateau[13][38] = cara2;
			j->x = 13;
			j->y = 37;
			break;		
		case 11:
			plateau[13][40] = cara1;
			plateau[13][41] = cara2;
			j->x = 13;
			j->y = 40;
			break;

		case 12:
			plateau[13][43] = cara1;
			plateau[13][44] = cara2;
			j->x = 13;
			j->y = 43;
			break;			
		case 13:
			plateau[15][43] = cara1;
			plateau[15][44] = cara2;
			j->x = 15;
			j->y = 43;
			break;
		case 14:
			plateau[17][43] = cara1;
			plateau[17][44] = cara2;
			j->x = 17;
			j->y = 43;
			break;

		case 15:
			plateau[17][40] = cara1;
			plateau[17][41] = cara2;
			j->x = 17;
			j->y = 40;
			break;
		case 16:
			plateau[17][37] = cara1;
			plateau[17][38] = cara2;
			j->x = 17;
			j->y = 37;
			break;
		case 17:
			plateau[17][34] = cara1;
			plateau[17][35] = cara2;
			j->x = 17;
			j->y = 34;
			break;
		case 18:
			plateau[17][31] = cara1;
			plateau[17][32] = cara2;
			j->x = 17;
			j->y = 31;
			break;
		case 19:
			plateau[17][28] = cara1;
			plateau[17][29] = cara2;
			j->x = 17;
			j->y = 28;
			break;

		case 20:
			plateau[17][25] = cara1;
			plateau[17][26] = cara2;
			j->x = 17;
			j->y = 25;
			break;

		case 21:
			plateau[19][25] = cara1;
			plateau[19][26] = cara2;
			j->x = 19;
			j->y = 25;
			break;		
		case 22:
			plateau[21][25] = cara1;
			plateau[21][26] = cara2;
			j->x = 21;
			j->y = 25;
			break;
		case 23:
			plateau[23][25] = cara1;
			plateau[23][26] = cara2;
			j->x = 23;
			j->y = 25;
			break;
		case 24:
			plateau[25][25] = cara1;
			plateau[25][26] = cara2;
			j->x = 25;
			j->y = 25;
			break;
		case 25:
			plateau[27][25] = cara1;
			plateau[27][26] = cara2;
			j->x = 27;
			j->y = 25;
			break;

		case 26:
			plateau[29][25] = cara1;
			plateau[29][26] = cara2;
			j->x = 29;
			j->y = 25;
			break;
		case 27:
			plateau[29][22] = cara1;
			plateau[29][23] = cara2;
			j->x = 29;
			j->y = 22;
			break;
		case 28:
			plateau[29][19] = cara1;
			plateau[29][20] = cara2;
			j->x = 29;
			j->y = 19;
			break;

		case 29:
			plateau[27][19] = cara1;
			plateau[27][20] = cara2;
			j->x = 27;
			j->y = 19;
			break;
		case 30:
			plateau[25][19] = cara1;
			plateau[25][20] = cara2;
			j->x = 25;
			j->y = 19;
			break;
		case 31:
			plateau[23][19] = cara1;
			plateau[23][20] = cara2;
			j->x = 23;
			j->y = 19;
			break;
		case 32:
			plateau[21][19] = cara1;
			plateau[21][20] = cara2;
			j->x = 21;
			j->y = 19;
			break;		
		case 33:
			plateau[19][19] = cara1;
			plateau[19][20] = cara2;
			j->x = 19;
			j->y = 19;
			break;

		case 34:
			plateau[17][19] = cara1;
			plateau[17][20] = cara2;
			j->x = 17;
			j->y = 19;
			break;

		case 35:
			plateau[17][16] = cara1;
			plateau[17][17] = cara2;
			j->x = 17;
			j->y = 16;
			break;
		case 36:
			plateau[17][13] = cara1;
			plateau[17][14] = cara2;
			j->x = 17;
			j->y = 13;
			break;
		case 37:
			plateau[17][10] = cara1;
			plateau[17][11] = cara2;
			j->x = 17;
			j->y = 10;
			break;
		case 38:
			plateau[17][7] = cara1;
			plateau[17][8] = cara2;
			j->x = 17;
			j->y = 7;
			break;
		case 39:
			plateau[17][4] = cara1;
			plateau[17][5] = cara2;
			j->x = 17;
			j->y = 4;
			break;

		case 40:
			plateau[17][1] = cara1;
			plateau[17][2] = cara2;
			j->x = 17;
			j->y = 1;
			break;
		case 41:
			plateau[15][1] = cara1;
			plateau[15][2] = cara2;
			j->x = 15;
			j->y = 1;
			break;
		case 42:
			plateau[13][1] = cara1;
			plateau[13][2] = cara2;
			j->x = 13;
			j->y = 1;
			break;

		case 43:
			plateau[13][4] = cara1;
			plateau[13][5] = cara2;
			j->x = 13;
			j->y = 4;
			break;		
		case 44:
			plateau[13][7] = cara1;
			plateau[13][8] = cara2;
			j->x = 13;
			j->y = 7;
			break;
		case 45:
			plateau[13][10] = cara1;
			plateau[13][11] = cara2;
			j->x = 13;
			j->y = 10;
			break;
		case 46:
			plateau[13][13] = cara1;
			plateau[13][14] = cara2;
			j->x = 13;
			j->y = 13;
			break;
		case 47:
			plateau[13][16] = cara1;
			plateau[13][17] = cara2;
			j->x = 13;
			j->y = 16;
			break;

		case 48:
			plateau[13][19] = cara1;
			plateau[13][20] = cara2;
			j->x = 13;
			j->y = 19;
			break;

		case 49:
			plateau[11][19] = cara1;
			plateau[11][20] = cara2;
			j->x = 11;
			j->y = 19;
			break;
		case 50:
			plateau[9][19] = cara1;
			plateau[9][20] = cara2;
			j->x = 9;
			j->y = 19;
			break;
		case 51:
			plateau[7][19] = cara1;
			plateau[7][20] = cara2;
			j->x = 7;
			j->y = 19;
			break;
		case 52:
			plateau[5][19] = cara1;
			plateau[5][20] = cara2;
			j->x = 5;
			j->y = 19;
			break;
		case 53:
			plateau[3][19] = cara1;
			plateau[3][20] = cara2;
			j->x = 3;
			j->y = 19;
			break;

		case 54:
			plateau[1][19] = cara1;
			plateau[1][20] = cara2;
			j->x = 1;
			j->y = 19;
			break;
		case 55:
			plateau[1][22] = cara1;
			plateau[1][23] = cara2;
			j->x = 1;
			j->y = 22;
			break;
	}

}





