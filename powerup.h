//
// Created by xumer on 26/04/2026.
//

#ifndef SPACEGAME_POWERUP_H
#define SPACEGAME_POWERUP_H


#include "entites.h"
#include <allegro.h>

#define MAX_POWERUPS 5

// Initialise tous les powerups (les desactive)
void init_powerups(Powerup powerups[], int max);

// Tente de creer un powerup a une position donnee
// Le type est aleatoire parmi les 3 disponibles
// Retourne 1 si cree, 0 sinon
int creer_powerup(Powerup powerups[], int max, float x, float y);

// Met a jour tous les powerups (chute avec gravite)
void update_powerups(Powerup powerups[], int max, int ecran_y);

// Affiche tous les powerups sur le buffer
void afficher_powerups(BITMAP *buffer, Powerup powerups[], int max);

// Teste si le joueur ramasse un powerup
// Retourne le type ramasse (POWERUP_*) ou -1 si rien
int ramasser_powerup(Powerup powerups[], int max,
                     int joueur_x, int joueur_y,
                     int joueur_largeur, int joueur_hauteur);

#endif

