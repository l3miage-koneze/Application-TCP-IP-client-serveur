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



#define TAILLE_MAX 20 //nombre de livres maximum dans la bibliothèque
#define NB_ELEMENT 6 //nombre de catégories par livre


//renvoie un tableau adapté au fichier.txt : chaque ligne représente un livre, chaque colonne une catégorie (auteur, titre...)
char*** double_tableau_de_string(){
    char*** tab = (char***) malloc(sizeof(char**) * TAILLE_MAX);
    for (int k=0; k<TAILLE_MAX; k++){
        tab[k] = (char**) malloc(sizeof(char*) * NB_ELEMENT);
        for (int l = 0; l<NB_ELEMENT; l++){
            tab[k][l] = (char*) malloc(sizeof(char) * 30);
        }
    }
    return tab;
}


//renvoie un tableau pour contenir les informations d'un livre 
char** tableau_de_string(){
    char** tab = (char**) malloc(sizeof(char*) * 10);
    for(int k=0; k<10; k++){
        tab[k] = (char*) malloc(sizeof(char) * 30);
    }
    return tab;
}


//fractionne les informations du fichier.txt séparés par le caractère # et renvoie un tableau contenant toute la bibliothèque
void fractionner(char* fichier, char*** tab){

    char* ligne;
    size_t taille = 0;
    ssize_t lire;
    char * token;
    int i = 0;
    int j = 0;

    //le fichier est réouvert dans cette fonction : ce n'est pas un oubli ou une erreur de notre part
    //nous avons essayé de faire passer f dans les arguments mais lorsque nous lançons plusieurs fois un client, le fichier ne semble plus correct. 
    //nous n'avons pas compris la raison de cette erreur donc nous préferons ré-ouvrir le fichier (nous avons déjà vérifié dans le main qu'il existe bien)
    FILE* fp = fopen(fichier, "r");

    //lecture du fichier
    while((lire = getline(&ligne, &taille, fp)) != -1){
        token = strtok(ligne, "#");
        
        j = 0;
        while (token != NULL){
            strcpy(tab[i][j], token);
            token = strtok(NULL, "#");
            j++;
        }
        i++;
    }

}



// -1- fonction qui va rechercher le numéro de réference saisi envoyé par le client dans le fichier(tableau)
//renvoie toute la ligne concernée de cette référence 
char ** recherche_ref(char *** tab, char* ref){
    char** tabl = tableau_de_string();
    bool trouve = 0; //false

    for(int i=0;i<TAILLE_MAX; i++){
        if (strcmp(ref, tab[i][0]) == 0){
            tabl = tab[i];
            trouve = 1;
        }
    }
    if (!trouve){
        tabl = NULL;
    }
    return tabl;
}

//ces deux fonctions de comparaison utilisées pour le tri ont été trouvées sur internet ("work smarter, not harder")

//fonction de comparaison pour le tri selon les notations des livres (utilisé par la fonctionnalité 5)
int str_compare_note(const void * a, const void * b){
    const char *pa = ((const char ***)a)[0][5];
    const char *pb = ((const char ***)b)[0][5];
    return strcmp(pa, pb);
}

//fonction de comparaison pour le tri selon les auteurs (utilisé par la fonctionnalité 2)
int str_compare_auteur(const void * a, const void * b){
    const char *pa = ((const char ***)a)[0][1];
    const char *pb = ((const char ***)b)[0][1];
    return strcmp(pa, pb);
}



// -2- fonction recherchant les livres dont les titres contiennent les mots clés
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_mots_cles(char*** tab, char** mots_cles, int taille_tab_argument, int* nb_ligne){

    bool trouve = 0; //false

    //Tableau qui va contenir les lignes des livres dont le(s) mot(s) clé(s) est (sont) présents
    char*** tabl_resultat = double_tableau_de_string();

    //recherche d'un mot clé dans les titres
    for(int j=0; j<TAILLE_MAX; j++){
        for(int m=0; m<taille_tab_argument; m++){
            if(strstr(tab[j][2], mots_cles[m]) != NULL){
                tabl_resultat[*nb_ligne] = tab[j];
                (*nb_ligne)++;
                trouve = 1;
            }
        }     
    }
    //si le tableau n'est pas vide
    if(trouve){
        //tri
        qsort(tabl_resultat, *nb_ligne, sizeof(char **), str_compare_auteur);
    }
    else{
        tabl_resultat = NULL;
    }

    return tabl_resultat;

}


// -3- fonction recherchant un livre à partir de nom et genre donnés en argument au clavier
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_nom_genre(char*** tab, char** mots_cles, int* nb_ligne){
    //Allocation de la mémoire
    char*** tabl_resultat = double_tableau_de_string();

    bool trouve = 0; //false
    //recherche de l'auteur et du critère
    for(int j=0; j<TAILLE_MAX; j++){
        if((strstr(tab[j][1], mots_cles[0]) != NULL) && (strstr(tab[j][3], mots_cles[1]) != NULL)){
            tabl_resultat[*nb_ligne] = tab[j];
            trouve = 1;
            (*nb_ligne)++;
        }      
    }
    if (!trouve){
        tabl_resultat = NULL;
    }

    return tabl_resultat;

}



// -4- fonction recherchant un livre d'un auteur donné qui a une notation donnée
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_nom_critere(char*** tab, char* nom, int* nb_ligne){
    char*** tabl_resultat = double_tableau_de_string();

    bool trouve = 0; //false
    //recherche de l'auteur
    for(int j=0; j<TAILLE_MAX; j++){
        if(strcmp(tab[j][1], nom) == 0){
            tabl_resultat[*nb_ligne] = tab[j];
            (*nb_ligne)++;
            trouve = 1;
        }   
    }
    if (!trouve){
        tabl_resultat = NULL;
    }
    else{
        //tri selon les notes : les livres les mieux notés sont retournées en premier
        qsort(tabl_resultat, *nb_ligne, sizeof(char **), str_compare_note);
    }

    return tabl_resultat;
}

//fonction pour éviter les zombis
void end_child(int s){
    wait(NULL);
}

//deuxième fonction pour éviter les zombis
void f_sigaction(){
    struct sigaction ac;
    ac.sa_handler = end_child;
    ac.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &ac , NULL);
}

// crée et renvoie la socket d'écoute; si la création échous, on envoie un message d'erreur
int f_socket(){
    int socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_ecoute == -1){
        printf("la création de la socket a échoué :( !\n");
        exit(-1);
    }else{
         printf("la socket d'écoute a bien été créée :) ! \n");
    }
    return socket_ecoute;
}

//effectue le bind
void f_bind(int socket_ecoute, struct sockaddr_in s){
    int bind_ecoute = bind(socket_ecoute, (struct sockaddr *)&s, sizeof(s));
    if (bind_ecoute == -1){
        fprintf(stderr, "Erreur de bind\n");
        exit(-1);
    }
    else{
        printf("le bind a réussi :) \n");
    }
}

//effectue le listen
void f_listen(int socket_ecoute){
    int listen_ecoute = listen(socket_ecoute, 5);
    if (listen_ecoute == -1){
        fprintf(stderr, "Erreur de listen\n");
        exit(-1);
    }
    else{
        printf("le listen a marché ! \n");
    }
}

//renvoie s
struct sockaddr_in f_AF_INET(int NOPORT){
    struct sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = htonl(INADDR_ANY);
    s.sin_port = htons(NOPORT);
    return s;
}

//renvoie la socket de service par accept
int f_accept(int socket_ecoute){
    struct sockaddr_in s2;
    socklen_t tailleS2 = sizeof(s2);

    int socket_service = accept(socket_ecoute, (struct sockaddr *)&s2, &tailleS2);

    if (socket_service == -1){
        fprintf(stderr, "Erreur d'accept\n");
        exit(-1);
    }else{
        printf("la primitive accept a bien fonctionné ! \n");
    }
    return socket_service;
}


//lit le numéro de question demandé par l'utilisateur puis ses arguments
void reception_donnees(int socket_service, char* nQuestion, char* argument){
    //lecture du numéro de question 
    read(socket_service, nQuestion, sizeof(nQuestion));
    printf("Je suis le serveur et le client me demande le numéro de question n°%s. \n", nQuestion);
    
    //lecture des arguments
    int taille_argument;
    read(socket_service, &taille_argument, sizeof(int));

    read(socket_service, argument, taille_argument);
    printf("Le client m'a donné %s en argument. \n", argument);
}

//écrit ko si aucune information demandée par le client n'est trouvé par le serveur, ok sinon
bool verification_ok_ko(int socket_service, char** reponse){
    bool ok = 0;
    //si des résultats de la requête sont trouvés
    if (reponse != NULL){
        write(socket_service, "ok", strlen("ok")+1);
        ok = 1;
    }
    //s'il n'y a pas de livre associé à la demande
    else{
        write(socket_service, "ko", strlen("ok")+1);
    } 
    return ok;
}

//envoi des informations au client : titre, auteur, genre et nombre de pages 
void envoi_donnees(int socket_service, char** reponse){
    
    //préparation à l'envoi des String : récupération de chaque taille de leur valeur
    int tailleChaine[6];
    for(int i=0;i<6;i++){
        tailleChaine[i] = (int)strlen(reponse[i]) + 1;
    }
    //envoi des tailles au client (si on ne le fait pas, il ne saura pas combien d'octets il devra recevoir)
    for(int i=0;i<6;i++){
        int* p = tailleChaine + i;
        write(socket_service, p, sizeof(int));
    }

    //passage des informations au client : titre, auteur, genre, nombre de pages, note
    write(socket_service, reponse[2], strlen(reponse[2])+1);    //titre
    write(socket_service, reponse[1], strlen(reponse[1])+1);    //nom d'auteur
    write(socket_service, reponse[3], strlen(reponse[3])+1);    //genre littéraire
    write(socket_service, reponse[4], strlen(reponse[4])+1);    //nombre de pages
    write(socket_service, reponse[5], strlen(reponse[5])+1);    //note : appréciation
    
}

//sépare le string donné par ses espaces et retourne le nombre de mots trouvé
int fractionner_espace(char* argument, char** tab_argument){
    char* token;
    token = strtok(argument, " ");
    int nb_argument = 0;
    while (token != NULL){
        strcpy(tab_argument[nb_argument], token);
        token = strtok(NULL, " ");
        nb_argument++;
    }

    return nb_argument;
}

//traitement de la fonctionnalité 1
void fonctionnalite_1(char*** tab, char* argument, int socket_service){
    //création d'un tableau de string qui contient les informations reliées au livre
    char** reponse1 = tableau_de_string();
    reponse1 = recherche_ref(tab, argument);
    if(verification_ok_ko(socket_service, reponse1) == 1){
        envoi_donnees(socket_service, reponse1);
    }
}

//traitement de la fonctionnalité 2
void fonctionnalite_2(char*** tab, char* argument, int socket_service){
    //tableau qui contient les arguments lus au clavier
    char** tab_argument = tableau_de_string();

    //séparation des arguments en les fractionnant par espace
    int nb_argument = fractionner_espace(argument, tab_argument);

    //Allocation de la mémoire
    char*** reponse2 = double_tableau_de_string();

    //nombre de résultat que va recevoir le client
    int nb_ligne = 0;
    reponse2 = recherche_mots_cles(tab, tab_argument, nb_argument, &nb_ligne);
    if (verification_ok_ko(socket_service, reponse2[0]) == 1){

        //écriture du nombre de livres qu'on va renvoyer
        write(socket_service, &nb_ligne , sizeof(int));

        //BOUCLE : pour chaque ligne retournée
        for(int k=0; k<nb_ligne; k++){
            envoi_donnees(socket_service, reponse2[k]);
        }
    }
}

///traitement de la fonctionnalité 3
void fonctionnalite_3(char*** tab, char* argument, int socket_service){
    //tableau qui contient les arguments lus au clavier
    char** tabl_argument = tableau_de_string();

    //tableau des arguments séparé : on considère que tous les éléments hormis le dernier concernent le nom de l'auteur
    //exemple ["Vitor" "Hugo" "roman"] --> ["Victor Hugo" "roman"]
    char** tabl_argument_separes = tableau_de_string();

    //séparation des arguments en les fractionnant par espace
    int nb_argument = fractionner_espace(argument, tabl_argument);

    for (int i=1; i<nb_argument-1; i++){
        strcat(tabl_argument[0], " ");
        strcat(tabl_argument[0], tabl_argument[i]);
    }

    tabl_argument_separes[0] = tabl_argument[0];
    tabl_argument_separes[1] = tabl_argument[nb_argument-1];

    //Allocation de la mémoire
    char*** reponse3 = double_tableau_de_string();

    int nb_ligne = 0;
    reponse3 = recherche_nom_genre(tab, tabl_argument_separes, &nb_ligne);

    if (verification_ok_ko(socket_service, reponse3[0]) == 1){

        //écriture du nombre de livres qu'on va renvoyer
        write(socket_service, &nb_ligne , sizeof(int));

        //BOUCLE : pour chaque ligne retournée
        for(int k=0; k<nb_ligne; k++){
            envoi_donnees(socket_service, reponse3[k]);
        }
    }
}

//traitement de la fonctionnalité 4
void fonctionnalite_4(char*** tab, char* argument, int socket_service){
    //Allocation de la mémoire
    char*** reponse4 = double_tableau_de_string();

    int nb_ligne = 0;
    reponse4 = recherche_nom_critere(tab, argument, &nb_ligne);

    if (verification_ok_ko(socket_service, reponse4[0]) == 1){

        //écriture du nombre de livres qu'on va renvoyer
        write(socket_service, &nb_ligne , sizeof(int));

        //BOUCLE : pour chaque ligne retournée
        for(int k=0; k<nb_ligne; k++){
            envoi_donnees(socket_service, reponse4[k]);
        }
    }
}

//lance les fonctions de recherche selon la question du client
void communication_client(char ***tab, int socket_service, char* nQuestion, char* argument){

    //lecture des 2 envois du client : numéro de sa question, argument(s)
    reception_donnees(socket_service, nQuestion, argument);
    
    switch(atoi(nQuestion)){
        //DONNER UN LIVRE À PARTIR D'UNE RÉFÉRENCE
        case 1:;
            fonctionnalite_1(tab, argument, socket_service);
            break;

        //DONNER UN LIVRE À PARTIR DE MOTS-CLÉS DANS LE TITRE
        case 2:;
            fonctionnalite_2(tab, argument, socket_service);
            break;  

        case 3:;
            fonctionnalite_3(tab, argument, socket_service);
            break;
        
        case 4:;
            fonctionnalite_4(tab, argument, socket_service);
            break;

        default:;
            printf("La requête demandée n'existe pas\n");
    }

    printf("----------------------------------------------\n");
    close(socket_service);
    exit(0);
}
