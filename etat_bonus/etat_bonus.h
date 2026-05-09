#ifndef SPACEGAME_ETAT_BONUS_H
#define SPACEGAME_ETAT_BONUS_H

#include <allegro.h>
#include "partie.h"
#include "assets.h"

int gerer_etat_bonus(BITMAP *buffer, Assets *assets,
                     Partie *p, Entites *e, Animations *a,
                     int couleur, int *fin);

#endif