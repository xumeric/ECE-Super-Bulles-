//
// Created by xumer on 23/04/2026.
//
#include <stdlib.h>
#include "../boss/boss.h"

void init_boss(Boss *boss, int ecran_x)
{
    boss->largeur = 160;
    boss->hauteur = 160;
    boss->x = ecran_x / 2 - boss->largeur / 2;
    boss->y = 80;
    boss->hp = 50;
    boss->hp_max = 50;
    boss->actif = 1;
    boss->timer_teleport = 30;
    boss->timer_tir = 90;
    boss->timer_bulle = 300;
    boss->vx = 5.0;                // vitesse de glissement
    boss->mode = 0;
    boss->timer_mode = 240;
    boss->phase = 1;
    boss->timer_transition = 0;
    boss->timer_laser = 0;
}

// Teleporte le boss a une position X aleatoire
void teleporter_boss(Boss *boss, int ecran_x)
{
    boss->x = 50 + rand() % (ecran_x - 100 - boss->largeur);
    boss->y = 50 + rand() % 100;
}

void update_boss(Boss *boss, int ecran_x)
{

    if (boss->phase == 1 && boss->hp <= 35)
    {
        boss->phase = 2;
        boss->timer_transition = 60;   // 1 seconde de flash
    }
    else if (boss->phase == 2 && boss->hp <= 15)
    {
        boss->phase = 3;
        boss->timer_transition = 60;
    }

    // Décrémenter les timers
    if (boss->timer_transition > 0) boss->timer_transition--;
    if (boss->timer_laser > 0)      boss->timer_laser--;

    if (!boss->actif) return;

    if (boss->timer_mode > 0)
    {
        boss->timer_mode--;
    }
    else
    {
        // Changement de mode
        if (boss->mode == 0)
        {
            // Passe en mode teleportation
            boss->mode = 1;
            boss->timer_mode = 180;
            boss->timer_teleport = 0;
        }
        else
        {
            // Repasse en mode deplacement
            boss->mode = 0;
            boss->timer_mode = 240;
            boss->vx = (rand() % 2 == 0) ? 3.0 : -3.0;
        }
    }

    if (boss->mode == 0)
    {
        boss->x += boss->vx;

        // Rebond sur les bords
        if (boss->x < 20)
        {
            boss->x = 20;
            boss->vx = -boss->vx;
        }
        if (boss->x + boss->largeur > ecran_x - 20)
        {
            boss->x = ecran_x - 20 - boss->largeur;
            boss->vx = -boss->vx;
        }
    }
    else
    {
        if (boss->timer_teleport > 0)
        {
            boss->timer_teleport--;
        }
        else
        {
            teleporter_boss(boss, ecran_x);
            boss->timer_teleport = 30;   // teleportation toutes les 0.5 secondes
        }
    }

    // Timer de tir
    if (boss->timer_tir > 0)
    {
        boss->timer_tir--;
    }

    // Timer de lachage de bulle
    if (boss->timer_bulle > 0)
    {
        boss->timer_bulle--;
    }
}

// Collision entre un tir et le boss
int collision_tir_boss(Tir *t, Boss *boss)
{
    return (t->x >= boss->x &&
            t->x <= boss->x + boss->largeur &&
            t->y >= boss->y &&
            t->y <= boss->y + boss->hauteur);
}
// Le boss tire un projectile vers le bas
int boss_tirer(Boss *boss, Tir projectiles[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!projectiles[i].actif)
        {
            projectiles[i].actif = 1;
            projectiles[i].x = boss->x + boss->largeur / 2;
            projectiles[i].y = boss->y + boss->hauteur;
            projectiles[i].vx = 0;
            return 1;
        }
    }
    return 0;
}

// Le boss lache une bulle a sa position
int boss_lacher_bulle(Boss *boss, Bulle bulles[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!bulles[i].actif)
        {
            bulles[i].actif = 1;
            bulles[i].x = boss->x + boss->largeur / 2;
            bulles[i].y = boss->y + boss->hauteur;
            bulles[i].vx = (rand() % 2 == 0) ? -2.0 : 2.0;
            bulles[i].vy = 0;
            bulles[i].taille = 1;   // petite bulle
            bulles[i].rayon = 22;
            return 1;
        }
    }
    return 0;
}