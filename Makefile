#où trouver les fichiers d'en-tête
HEADERS= headers/
#où doivent se trouver les éxécutables
BIN= bin/
#où se trouve tous les fichiers sources
SRCDIR= sources/
CFLAGS= -Wall -I$(HEADERS)
#fichier source serveur.c
SRC1= $(SRCDIR)serveur.c
#fichier source client.c
SRC2= $(SRCDIR)client.c
#fichier qui contient les fonctionnalités de bases du serveur
SRC3= $(SRCDIR)traitement_serveur.c
#fichier qui contient les fontionnalités de bases du client
SRC4= $(SRCDIR)traitement_client.c

all : client serveur

serveur : serveur.o
	gcc serveur.o traitement_serveur.o -o $(BIN)serveur $(CFLAGS)
	
client : client.o 
	gcc client.o traitement_client.o -o $(BIN)client $(CFLAGS)
	
serveur.o : $(SRC1)	traitement_serveur.o 
	gcc -c $(SRC1) $(CFLAGS)
	
client.o : $(SRC2) traitement_client.o
	gcc -c $(SRC2) $(CFLAGS)
	
traitement_serveur.o : $(SRC3)
	gcc -c $(SRC3) $(CFLAGS)

traitement_client.o : $(SRC4)
	gcc -c $(SRC4) $(CFLAGS)
	
clean :
	@echo "On efface tous les fichiers objets"
	-rm *.o