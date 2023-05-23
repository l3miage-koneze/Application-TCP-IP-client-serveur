#ifndef TRAITEMENT_CLIENT_H
#define TRAITEMENT_CLIENT_H

//#define nombre d'éléments caractérisant un livre 
#define NB_ELEMENT 6


struct sockaddr_in f_s(int NOPORT, struct hostent *h);

//fonction qui etablit la connexion avec le serveur
void f_connect(int socket_client, struct sockaddr_in s );

//utilisé pour la fonctionnalité 3 : renvoie true s'il y a au moins deux arguments passés en paramètre
bool compter_mot(char* argument);

//lit les informations renvoyées par le serveur (d'abord leur taille, puis elles-mêmes)
void lecture_informations(int socket_client);

//fonction qui permet au client d'intéragir avec l'utilisateur puis de lire les réponses de la requête (de la part du serveur)
void communication_serveur(int socket_client);


#endif
