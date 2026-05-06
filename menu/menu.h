//
// Created by xumer on 27/04/2026.
//

#ifndef SPACEGAME_MENU_H
#define SPACEGAME_MENU_H


#include <allegro.h>
#include "../entites.h"
#include "../sauvegarde/sauvegarde.h"

// Resultats possibles du menu principal
#define MENU_RIEN       0
#define MENU_JOUER      1
#define MENU_REPRENDRE  2
#define MENU_REGLES     3
#define MENU_QUITTER    4


// Resultats specifiques pour l'ecran reprendre
#define REPRENDRE_RIEN     -2
#define REPRENDRE_RETOUR   -1
// Toute valeur >= 0 = index de la sauvegarde choisie

// Affiche l'ecran des regles
// Retourne 1 si le joueur veut revenir au menu, 0 sinon
int afficher_regles(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x);

int afficher_lord(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x);

// Affiche l'ecran game over
int afficher_game_over(BITMAP *buffer, int score, int ecran_x);

// Affiche l'ecran de victoire
int afficher_victoire(BITMAP *buffer, int score, const char *pseudo, int ecran_x, int ecran_y);

// Affiche et gere l'ecran de saisie du pseudo
int afficher_pseudo(BITMAP *buffer, char pseudo[], int ecran_x);

// Affiche le menu principal et gere les clics
int afficher_menu(BITMAP *buffer,
                  Bouton *bouton_jouer,
                  Bouton *bouton_reprendre,
                  Bouton *bouton_regles,
                  Bouton *bouton_quitter,
                  int *clic_presse,
                  int ecran_x);

// Affiche l'ecran de reprise de partie avec liste cliquable
int afficher_reprendre(BITMAP *buffer,
                       Sauvegarde sauvegardes[],
                       int nb_sauvegardes,
                       Bouton *bouton_retour,
                       int *clic_presse,
                       int ecran_x);

// Affiche le menu pause par-dessu le jeu en cours
int afficher_pause(BITMAP *buffer, Bouton *bouton_reprendre,
                   Bouton *bouton_menu, Bouton *bouton_quitter,
                   int *clic_presse, int ecran_x, int ecran_y);
#endif //SPACEGAME_MENU_H