#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "traitement_serveur.h"


//PROGRAMME PRINCIPAL DU SERVEUR

int main(int argc, char** argv){

    //vérification du nombre d'arguments
    if (argc != 3){
        printf("Mauvais nombre d'arguments ! \n");
        exit(-1);
    }

    //vérification de l'existence du fichier
    FILE* fp = fopen(argv[2], "r");
    if (fp == NULL){
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        exit(-1);
    }

    //déclaration de la socket d'ecoute
    int socket_ecoute = f_socket();

    //initialisation de s
    struct sockaddr_in s = f_AF_INET(atoi(argv[1]));
    

    //attachement de la socket d'ecoute
    f_bind(socket_ecoute, s);

    //ouverture de service sur la socket d'ecoute
    f_listen(socket_ecoute);

    //fractionnement des données de la bibliothèque dans un double tableau de string
    char*** tab = double_tableau_de_string();
    fractionner(argv[2], tab);

    //boucle infinie 
    while (1){

        char * nQuestion = (char*) malloc (8 * sizeof (char));
        char * argument = (char*) malloc (256 * sizeof (char));

        //création de la socket de service
        int socket_service = f_accept(socket_ecoute);

        //gestion des wait
        f_sigaction();

        //création des processus fils
        int f = fork();

        switch (f){
            case -1:
                perror("fork");
                exit(-1);

            case 0:
                communication_client(tab, socket_service, nQuestion, argument);

            default:
                //fermeture de la socket 
                close(socket_service);    
        }
    }
   
}
