//
// Created by xumer on 29/04/2026.
//

//
// etat_boss.h - Gestion de l'etat ETAT_BOSS (combat de boss)
//

//
// etat_boss.h - Gestion de l'etat ETAT_BOSS (combat de boss)
//

#ifndef SPACEGAME_ETAT_BOSS_H
#define SPACEGAME_ETAT_BOSS_H

#include <allegro.h>
#include "partie.h"
#include "assets.h"

// Gere une frame de l'etat ETAT_BOSS.
// Retourne le nouvel etat (ETAT_BOSS pour rester, ETAT_VICTOIRE / ETAT_GAME_OVER pour transitionner)
int gerer_etat_boss(BITMAP *buffer, Assets *assets,
                    Partie *p, Entites *e, Animations *a,
                    int couleur, float gravite, int *fin);

#endif