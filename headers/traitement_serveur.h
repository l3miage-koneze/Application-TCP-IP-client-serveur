#ifndef TRAITEMENT_SERVEUR_H
#define TRAITEMENT_SERVEUR_H
#define TAILLE_MAX 20 //nombre de livres maximum dans la bibliothèque
#define NB_ELEMENT 6 //nombre de catégories par livre


struct sockaddr_in f_AF_INET(int NOPORT);

//fonction qui renvoie un tableau adapté au fichier.txt : chaque ligne représente un livre, chaque colonne une catégorie (auteur, titre...)
char*** double_tableau_de_string();

//renvoie un tableau pour contenir les informations d'un livre 
char** tableau_de_string();

//fractionne les informations du fichier.txt séparés par le caractère # et renvoie un tableau contenant toute la bibliothèque
void fractionner(char* fichier, char*** tab);


// -1- fonction qui va rechercher le numéro de réference saisi envoyé par le client dans le fichier(tableau)
//renvoie toute la ligne concernée de cette référence 
char** recherche_ref(char*** tab, char* ref);

//ces deux fonctions de comparaison utilisées pour le tri ont été trouvées sur internet ("work smarter, not harder")

//fonction de comparaison pour le tri selon les notations des livres (utilisé par la fonctionnalité 4) : trie en fonction de A B et C
int str_compare_note(const void *a, const void *b);


//fonction de comparaison pour le tri selon les auteurs (utilisé par la fonctionnalité 2) : trie par ordre alphabétique
int str_compare_auteur(const void *a, const void *b);


// -2- fonction recherchant les livres dont les titres contiennent les mots clés
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_mots_cles(char*** tab, char** mots_cles, int taille_tab_argument, int *nb_ligne);


// -3- fonction recherchant un livre à partir de nom et genre donnés en argument au clavier
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_nom_genre(char*** tab, char** mots_cles, int* nb_ligne);


// -4- fonction recherchant un livre d'un auteur donné qui a une notation donnée
//renvoie un tableau de livres qui correspondent à la requête
char*** recherche_nom_critere(char*** tab, char*nom, int* nb_ligne);

void end_child(int s);

void f_sigaction();

//crée la socket d'écoute
int f_socket();

void f_bind(int socket_ecoute, struct sockaddr_in s);

void f_listen(int socket_ecoute);

int f_accept(int socket_ecoute);

//lit le numéro de question demandé par l'utilisateur puis ses arguments
void reception_donnees(int socket_service, char* nQuestion, char* argument);

//écrit ko si aucune information demandée par le client n'est trouvé par le serveur, ok sinon
bool verification_ok_ko(int socket_service, char** reponse);

//envoi des informations au client : titre, auteur, genre et nombre de pages 
void envoi_donnees(int socket_service, char** reponse);

//sépare le string donné par ses espaces et retourne le nombre de mots trouvé
int fractionner_espace(char* argument, char** tab_argument);

//traitement de la requête 1
void fonctionnalite_1(char*** tab, char* argument, int socket_service);

//traitement de la requête 2
void fonctionnalite_2(char*** tab, char* argument, int socket_service);

//traitement de la requête 3
void fonctionnalite_3(char*** tab, char* argument, int socket_service);

//traitement de la requête 4
void fonctionnalite_4(char*** tab, char* argument, int socket_service);

//lance les fonctions de recherche selon la question du client
void communication_client(char*** tab, int socket_service, char* nQuestion, char* argument);

#endif
