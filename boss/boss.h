//
// Created by xumer on 23/04/2026.
//

#ifndef SPACEGAME_BOSS_H
#define SPACEGAME_BOSS_H

#include "../entites.h"


void init_boss(Boss *boss, int ecran_x);


void teleporter_boss(Boss *boss, int ecran_x);

void update_boss(Boss *boss, int ecran_x);


int collision_tir_boss(Tir *t, Boss *boss);


int boss_tirer(Boss *boss, Tir projectiles[], int max);


int boss_lacher_bulle(Boss *boss, Bulle bulles[], int max);


#endif //SPACEGAME_BOSS_H