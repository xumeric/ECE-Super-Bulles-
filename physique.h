//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_PHYSIQUE_H
#define SPACEGAME_PHYSIQUE_H

#include "entites.h"

// Applique la gravite et met a jour la position d'une bulle
void deplacer_bulle(Bulle *b, float gravite);

// Gere les rebonds d'une bulle sur les murs et le sol
void rebonds_bulle(Bulle *b, int ecran_x, int ecran_y);

// Teste si une bulle touche le joueur (rectangle)
// Retourne 1 si collision, 0 sinon
int collision_bulle_joueur(Bulle *b, int joueur_x, int joueur_y,
                           int joueur_largeur, int joueur_hauteur);

// Teste si un tir touche une bulle
// Retourne 1 si collision, 0 sinon
int collision_tir_bulle(Tir *t, Bulle *b);

#endif //SPACEGAME_PHYSIQUE_H