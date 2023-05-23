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

//#define nombre d'éléments caractérisant un livre 
#define NB_ELEMENT 6


//renvoie s
struct sockaddr_in f_s(int NOPORT, struct hostent *h){
    struct sockaddr_in s;
    s.sin_family = AF_INET;
    memcpy(&s.sin_addr.s_addr, h->h_addr_list[0], sizeof(int));
    s.sin_port = htons(NOPORT);
    return s;
}


//etablit la connexion avec le serveur
void f_connect(int socket_client, struct sockaddr_in s ){
    int connect_serveur = connect(socket_client, (struct sockaddr *) &s, sizeof(s));
    if (connect_serveur == -1){
        fprintf(stderr, "Erreur de connect\n");
        exit(-1);
    }else{
        printf("Le client est connecté ! \n");
    }
}


//utilisé pour la fonctionnalité 3 : renvoie true s'il y a au moins deux arguments passés en paramètre
bool compter_mot(char* argument){
    int i = 0;
    int mot = 0;

    while(argument[i]!='\0'){
        if(argument[i]==' ' || argument[i]=='\n' || argument[i]=='\t'){
            mot++;
        }
        i++;
    }
    return(mot > 1);
}

//lit les informations renvoyées par le serveur (d'abord leur taille, puis elles-mêmes)
void lecture_informations(int socket_client){
    char* rep = (char*)malloc(sizeof(char)*30);

    //réception de la taille des futurs données
    int tailleChaine[6];
    for(int i=0;i<6;i++){
        int* p = tailleChaine + i;
        read(socket_client, p, sizeof(int));
        tailleChaine[i] = *p;
    }
    
    //le client lit les informations du livre renvoyées par le serveur 
    read(socket_client, rep, tailleChaine[2]);
    printf("Titre : %s\n", rep);
    read(socket_client, rep, tailleChaine[1]);
    printf("Auteur %s.\n", rep);
    read(socket_client, rep, tailleChaine[3]);
    printf("Genre : %s.\n", rep);
    read(socket_client, rep, tailleChaine[4]);

    //on vérifie le nombre de pages du livre
    if (atoi(rep) > 300){
        printf("Pages > 300 \n");
    }
    else{
        printf("Pages < 300 \n");
    }
    read(socket_client, rep, tailleChaine[5]);
    printf("Note : %s\n", rep);
}



//fonction qui permet au client d'intéragir avec l'utilisateur puis de lire les réponses de la requête (de la part du serveur)
void communication_serveur(int socket_client){

    //-------------------------------MENU-------------------------
    //demande à l'utilisateur la fonctionnalité souhaitée
    char* nQuestion = (char*) malloc (8 * sizeof (char));
    printf("Bonjour,\n");
    printf("Si vous voulez rechercher un livre à partir d'une référence, tapez 1\n");
    printf("Si vous voulez chercher un livre à partir de mots-clés, tapez 2\n");
    printf("Si vous voulez chercher un livre avec un auteur et un genre, tapez 3\n");
    printf("Si vous voulez chercher les livres les mieux notés d'un auteur, tapez 4 \n");
    scanf("%s", nQuestion);

    //vérification de l'existence de la question
    if ( (atoi(nQuestion) < 1 )|| (atoi(nQuestion) > 4) ){
        printf("Erreur, la question demandée n'existe pas.\n");
        exit(-1);
    }

    //écriture au serveur du numéro de question
    write(socket_client, nQuestion, sizeof(nQuestion));

    //demande à l'utilisateur les informations nécessaires pour répondre à sa requête
    char* argument = (char*) malloc (256 * sizeof (char));
    printf("Tapez l' (les) information(s) souhaitée(s).\n");
    scanf(" %[^\n]s", argument);

    //écriture au serveur de la taille des arguments qu'il a envoyé, puis des arguments
    int taille_argument = (int) strlen(argument) +1;
    write(socket_client, &taille_argument , sizeof(int));
    write(socket_client, argument, taille_argument);

    //lecture de la reponse du serveur : elle indique si la requête a pu être traitée correctement ou si elle ne renvoie rien
    char* valide = (char*)malloc(sizeof(char)*3);
    read(socket_client, valide, strlen("__")+1);

    int nb_ligne;

    //lecture de la réponse du serveur et gestion de l'affichage en fonction de la requête
    switch (atoi(nQuestion)){

        //obtenir un livre à partir d'une référence
        case 1:;

            //si la référence existe
            if (strcmp(valide, "ok") == 0){
                lecture_informations(socket_client);
            }
            else{
                printf("La référence n'existe pas ou est invalide !\n");
            }
            break;



        //obtenir des livres à partir de mots clés contenus dans leur titre
        case 2:;
                                
            //récupération du nombre de livres retournés
            read(socket_client, &nb_ligne, sizeof(int));

            //si la référence existe
            if (strcmp(valide, "ok") == 0){

                printf("Liste des livres qui contiennent les mots %s :\n", argument);
                printf("----------------------------------------------\n");

                for(int i=0; i<nb_ligne; i++){
                    lecture_informations(socket_client);
                    printf("----------------------------------------------\n");
                }
            }
            else{
                printf("Aucun livre ne correspond à ces mots-clés.\n");
            }
            break;

        //obtenir des livres à partir d'un nom d'auteur et d'un genre
        case 3:;
            //récupération du nombre de livres retournés
            read(socket_client, &nb_ligne, sizeof(int));

            //si la référence existe
            if (strcmp(valide, "ok") == 0){

                printf("Liste des livres de l'auteur et genre : %s :\n", argument);
                printf("----------------------------------------------\n");

                for(int i=0; i<nb_ligne; i++){
                    lecture_informations(socket_client);
                    printf("----------------------------------------------\n");
                }
            }
            else{
                if (!compter_mot(argument)){
                    printf("Vous devez entrer deux mots : un auteur et un genre.\n");
                }
                else{
                    printf("Aucun livre n'est de cet auteur et de ce genre.\n");
                }
            }
            break;
        
        //obtenir les meilleurs livres notés à partir d'un nom d'auteur
        case 4:;
            //si l'auteur existe
            //récupération du nombre de livres retournés
            read(socket_client, &nb_ligne, sizeof(int));

            //si l'auteur existe
            if (strcmp(valide, "ok") == 0){

                printf("Liste des livres de l'auteur %s :\n", argument);
                printf("----------------------------------------------\n");

                for(int i=0; i<nb_ligne; i++){
                    lecture_informations(socket_client);
                    printf("----------------------------------------------\n");
                }
            }
            else{
                printf("Il n'y a pas de livres qui correspondent à cet auteur.\n");
            }
            break;

    }
}

