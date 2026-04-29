//
// Created by xumer on 27/04/2026.
//

#ifndef SPACEGAME_ECLAIR_H
#define SPACEGAME_ECLAIR_H


#include <allegro.h>
#include "entites.h"

// Cree un eclair a partir d'une bulle
int creer_eclair(Eclair eclairs[], int max, float x_bulle, float y_bulle);

// Met a jour les eclairs (descente + duree)
void update_eclairs(Eclair eclairs[], int max, int ecran_y);

// Affiche les eclairs (zigzag jaune)
void afficher_eclairs(BITMAP *buffer, Eclair eclairs[], int max);

// Teste collision eclair/joueur
// Retourne 1 si collision, 0 sinon
int collision_eclair_joueur(Eclair *e, int joueur_x, int joueur_y,
                             int joueur_largeur, int joueur_hauteur);

#endif

