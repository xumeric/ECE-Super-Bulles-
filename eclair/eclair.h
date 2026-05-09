//
// Created by xumer on 27/04/2026.
//

#ifndef SPACEGAME_ECLAIR_H
#define SPACEGAME_ECLAIR_H


#include <allegro.h>
#include "../entites.h"

int creer_eclair(Eclair eclairs[], int max, float x_bulle, float y_bulle);

void update_eclairs(Eclair eclairs[], int max, int ecran_y);

void afficher_eclairs(BITMAP *buffer, Eclair eclairs[], int max);

int collision_eclair_joueur(Eclair *e, int joueur_x, int joueur_y,
                             int joueur_largeur, int joueur_hauteur);

#endif

