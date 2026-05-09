//
// Created by xumer on 27/04/2026.
//

#ifndef SPACEGAME_MENU_H
#define SPACEGAME_MENU_H


#include <allegro.h>
#include "../entites.h"
#include "../sauvegarde/sauvegarde.h"

#define MENU_RIEN       0
#define MENU_JOUER      1
#define MENU_REPRENDRE  2
#define MENU_REGLES     3
#define MENU_QUITTER    4



#define REPRENDRE_RIEN     -2
#define REPRENDRE_RETOUR   -1



int afficher_regles(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x);

int afficher_lord(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x);


int afficher_game_over(BITMAP *buffer, int score, int ecran_x);


int afficher_victoire(BITMAP *buffer, int score, const char *pseudo, int ecran_x, int ecran_y);


int afficher_pseudo(BITMAP *buffer, char pseudo[], int ecran_x);


int afficher_menu(BITMAP *buffer,
                  Bouton *bouton_jouer,
                  Bouton *bouton_reprendre,
                  Bouton *bouton_regles,
                  Bouton *bouton_quitter,
                  int *clic_presse,
                  int ecran_x);


int afficher_reprendre(BITMAP *buffer,
                       Sauvegarde sauvegardes[],
                       int nb_sauvegardes,
                       Bouton *bouton_retour,
                       int *clic_presse,
                       int ecran_x);


int afficher_pause(BITMAP *buffer, Bouton *bouton_reprendre,
                   Bouton *bouton_menu, Bouton *bouton_quitter,
                   int *clic_presse, int ecran_x, int ecran_y);

#endif //SPACEGAME_MENU_H