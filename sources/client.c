#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include "traitement_client.h"


//Programme principal du client 

int main(int argc, char** argv){

    //vérification du nombre d'arguments
    if (argc != 3){
        printf("Erreur d'arguments : le premier doit être le numéro de port, le deuxième l'adresse du serveur. \n");
        exit(-1);
    }

    //déclaration d'une socket
    int socket_client = socket(AF_INET, SOCK_STREAM, 0);

    //récupération des informations concernant le serveur
    struct hostent * h;
    h = gethostbyname(argv[2]);

    //initialisation de s
    struct sockaddr_in s;
    s = f_s(atoi(argv[1]), h);

    //demande de connexion
    f_connect(socket_client, s);

    //échanges avec le serveur
    communication_serveur(socket_client);
    
    //fermeture de la socket
    close(socket_client);

}
