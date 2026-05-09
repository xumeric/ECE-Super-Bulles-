
#ifndef SPACEGAME_ETAT_JEU_H
#define SPACEGAME_ETAT_JEU_H

#include <allegro.h>
#include "../partie/partie.h"
#include "../assets/assets.h"



int gerer_etat_jeu(BITMAP *buffer, Assets *assets,
                   Partie *p, Entites *e, Animations *a,
                   int couleur, float gravite, int *fin);

#endif