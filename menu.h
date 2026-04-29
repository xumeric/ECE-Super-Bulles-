//
// Created by xumer on 27/04/2026.
//

#ifndef SPACEGAME_MENU_H
#define SPACEGAME_MENU_H


#include <allegro.h>
#include "entites.h"
#include "sauvegarde.h"

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
// Retourne 1 si retour au menu, -1 si quitter, 0 sinon
int afficher_game_over(BITMAP *buffer, int score, int ecran_x);

// Affiche l'ecran de victoire
// Retourne 1 si retour au menu, -1 si quitter, 0 sinon
int afficher_victoire(BITMAP *buffer, int score, const char *pseudo, int ecran_x, int ecran_y);

// Affiche et gere l'ecran de saisie du pseudo
// Modifie pseudo[] selon les touches tapees
// Retourne 1 si pseudo valide (ENTREE), -1 si annule (ESC), 0 sinon
int afficher_pseudo(BITMAP *buffer, char pseudo[], int ecran_x);

// Affiche le menu principal et gere les clics
// Retourne MENU_JOUER, MENU_REPRENDRE, MENU_REGLES, MENU_QUITTER, ou MENU_RIEN
int afficher_menu(BITMAP *buffer,
                  Bouton *bouton_jouer,
                  Bouton *bouton_reprendre,
                  Bouton *bouton_regles,
                  Bouton *bouton_quitter,
                  int *clic_presse,
                  int ecran_x);

// Affiche l'ecran de reprise de partie avec liste cliquable
// sauvegardes contient les sauvegardes deja chargees
// nb_sauvegardes = nombre de sauvegardes valides
// Retourne l'index choisi (>=0), REPRENDRE_RETOUR, ou REPRENDRE_RIEN
int afficher_reprendre(BITMAP *buffer,
                       Sauvegarde sauvegardes[],
                       int nb_sauvegardes,
                       Bouton *bouton_retour,
                       int *clic_presse,
                       int ecran_x);
#endif //SPACEGAME_MENU_H