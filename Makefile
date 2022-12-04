# Ce fichier fait parti d'un projet réalisé dans le cadre de l'UE de système de la licence 3 MIAGE de l'UGA 
# (Octobre 2022)
# Les 2 auteurs de ce fichier sont Cormier levi et Tornambé Damien

CC=gcc 
CFLAGS= -Wall
EXEC=testJeu 

all: $(EXEC)

testJeu: main.o gestionProcessus.o joueur.o plateau.o
	$(CC) main.o gestionProcessus.o joueur.o plateau.o -o testJeu 

main.o: main.c  
	$(CC) -c main.c $(CFLAGS)

gestionProcessus.o: gestionProcessus.c 
	$(CC) -c gestionProcessus.c  $(CFLAGS)

joueur.o: joueur.c 
	$(CC) -c joueur.c  $(CFLAGS)

plateau.o: plateau.c
	$(CC) -c plateau.c  $(CFLAGS)

clean:
	@echo "Effacement des fichiers objets"
	rm -rf *.o

cleanAll:
	@echo "Effacement des fichiers objets et de l'exécutable"
	rm -rf $(EXEC) *.o