//
// Created by xumer on 29/04/2026.
//

//
// etat_jeu.h - Gestion de l'etat ETAT_JEU (gameplay normal, niveaux 1-5)
//
//
// etat_jeu.h - Gestion de l'etat ETAT_JEU (gameplay normal, niveaux 1-5)
//

#ifndef SPACEGAME_ETAT_JEU_H
#define SPACEGAME_ETAT_JEU_H

#include <allegro.h>
#include "partie.h"
#include "assets.h"

// Gere une frame de l'etat ETAT_JEU.
// Retourne le nouvel etat (ETAT_JEU pour rester, ETAT_BOSS / ETAT_GAME_OVER pour transitionner)
// Si fin doit etre mis a 1 (touche ESC), il l'est via le pointeur.
int gerer_etat_jeu(BITMAP *buffer, Assets *assets,
                   Partie *p, Entites *e, Animations *a,
                   int couleur, float gravite, int *fin);

#endif