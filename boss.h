//
// Created by xumer on 23/04/2026.
//

#ifndef SPACEGAME_BOSS_H
#define SPACEGAME_BOSS_H

#include "entites.h"

// Initialise un boss au debut du combat
void init_boss(Boss *boss, int ecran_x);

// Teleporte le boss a une position aleatoire
void teleporter_boss(Boss *boss, int ecran_x);

// Met a jour le boss (timers, tirs, etc.)
void update_boss(Boss *boss, int ecran_x);

// Teste collision tir/boss
int collision_tir_boss(Tir *t, Boss *boss);

// Le boss tire un projectile vers le bas
// Retourne 1 si un projectile a ete cree, 0 sinon
int boss_tirer(Boss *boss, Tir projectiles[], int max);

// Le boss lache une bulle a sa position
// Retourne 1 si une bulle a ete creee, 0 sinon
int boss_lacher_bulle(Boss *boss, Bulle bulles[], int max);


#endif //SPACEGAME_BOSS_H