

#ifndef SPACEGAME_POWERUP_H
#define SPACEGAME_POWERUP_H


#include "../entites.h"
#include <allegro.h>

#define MAX_POWERUPS 5

void init_powerups(Powerup powerups[], int max);


int creer_powerup(Powerup powerups[], int max, float x, float y);

void update_powerups(Powerup powerups[], int max, int ecran_y);

void afficher_powerups(BITMAP *buffer, Powerup powerups[], int max);

int ramasser_powerup(Powerup powerups[], int max,
                     int joueur_x, int joueur_y,
                     int joueur_largeur, int joueur_hauteur);

#endif

