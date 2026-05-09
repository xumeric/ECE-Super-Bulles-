//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_PHYSIQUE_H
#define SPACEGAME_PHYSIQUE_H

#include "../entites.h"

void deplacer_bulle(Bulle *b, float gravite);


void rebonds_bulle(Bulle *b, int ecran_x, int ecran_y);


int collision_bulle_joueur(Bulle *b, int joueur_x, int joueur_y,
                           int joueur_largeur, int joueur_hauteur);


int collision_tir_bulle(Tir *t, Bulle *b);

#endif //SPACEGAME_PHYSIQUE_H