//
// Created by xumer on 29/04/2026.
//

//
// partie.c - Initialisation des structures de jeu
//

//
// partie.c - Initialisation des structures + helpers
//

//
// partie.c - Initialisation des structures + helpers
//

#include <stdlib.h>
#include <string.h>
#include "partie.h"
#include "powerup.h"

void init_partie(Partie *p)
{
    p->score = 0;
    p->niveau = 1;
    p->niveau_gagner = 0;
    p->timer_niveau = 0;
    p->temps_restant = 60 * 60;
    p->decompte_initial = 0;

    p->bouclier_actif = 0;
    p->tir_double_timer = 0;
    p->tir_cooldown = 0;

    p->pseudo[0] = '\0';

    p->taille_bulles_actuelle = 15;
}

void init_entites(Entites *e, int taille_bulles_max)
{
    int i;

    e->bulles           = malloc(taille_bulles_max * sizeof(Bulle));
    e->tirs             = malloc(MAX_TIRS * sizeof(Tir));
    e->powerups         = malloc(MAX_POWERUPS * sizeof(Powerup));
    e->eclairs          = malloc(MAX_ECLAIRS * sizeof(Eclair));
    e->projectiles_boss = malloc(MAX_PROJ_BOSS * sizeof(Tir));

    for (i = 0; i < taille_bulles_max; i++) e->bulles[i].actif = 0;
    for (i = 0; i < MAX_TIRS; i++)         e->tirs[i].actif = 0;
    for (i = 0; i < MAX_POWERUPS; i++)     e->powerups[i].actif = 0;
    for (i = 0; i < MAX_ECLAIRS; i++)      e->eclairs[i].actif = 0;
    for (i = 0; i < MAX_PROJ_BOSS; i++)    e->projectiles_boss[i].actif = 0;
    for (i = 0; i < MAX_EXPLOSIONS; i++)   e->explosions[i].actif = 0;

    e->boss.actif = 0;
}

void liberer_entites(Entites *e)
{
    free(e->bulles);
    free(e->tirs);
    free(e->powerups);
    free(e->eclairs);
    free(e->projectiles_boss);
}

void init_animations(Animations *a)
{
    a->joueur_direction = 1;
    a->joueur_en_marche = 0;
    a->anim_joueur_compteur = 0;
    a->anim_joueur_frame = 0;

    a->anim_boss_compteur = 0;
    a->anim_boss_frame = 0;

    a->anim_proj_boss_compteur = 0;
    a->anim_proj_boss_frame = 0;
}

// Reallouer le tableau de bulles selon le niveau
Bulle *reallouer_bulles(Bulle *bulles, int *taille_actuelle, int nouveau_niveau)
{
    int nouvelle_taille = nouveau_niveau * 15;

    Bulle *nouveau = realloc(bulles, nouvelle_taille * sizeof(Bulle));
    if (nouveau == NULL)
    {
        return bulles;   // echec : on garde l'ancien
    }

    int i;
    for (i = *taille_actuelle; i < nouvelle_taille; i++)
        nouveau[i].actif = 0;

    *taille_actuelle = nouvelle_taille;
    return nouveau;
}

// Declenche une explosion en cherchant un slot libre
void declencher_explosion(Explosion explosions[], int max, float x, float y, int taille)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!explosions[i].actif)
        {
            explosions[i].actif = 1;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].taille = taille;
            explosions[i].frame = 0;
            explosions[i].compteur = 0;
            return;
        }
    }
}