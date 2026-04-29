//
// Created by xumer on 29/04/2026.
//
//
// assets.c - Implementation du chargement des sprites
//

#include <stdio.h>
#include <stdlib.h>
#include "assets.h"

static BITMAP *charger_un_sprite(const char *chemin)
{
    BITMAP *bmp = load_bitmap(chemin, NULL);
    if (bmp == NULL)
    {
        char message[200];
        sprintf(message, "Impossible de charger : %s", chemin);
        allegro_message(message);
        exit(1);
    }
    return bmp;
}

Assets charger_assets(void)
{
    Assets a;
    int i, j;
    char chemin[150];

    // === FONDS DES NIVEAUX (6 fichiers) ===
    for (i = 0; i < 6; i++)
    {
        sprintf(chemin, "ressources/fond_niveau%d.bmp", i + 1);
        a.fonds[i] = charger_un_sprite(chemin);
    }

    // === ASTEROIDES (4 tailles, ordre inverse pour matcher la convention) ===
    // taille 0 = mini, taille 3 = gros
    a.asteroides[0] = charger_un_sprite("ressources/asteroide_taille4.bmp");  // 18x18
    a.asteroides[1] = charger_un_sprite("ressources/asteroide_taille3.bmp");  // 30x30
    a.asteroides[2] = charger_un_sprite("ressources/asteroide_taille2.bmp");  // 50x50
    a.asteroides[3] = charger_un_sprite("ressources/asteroide_taille1.bmp");  // 80x80

    // === BOSS (7 frames d'animation UFO rouge) ===
    for (i = 0; i < 7; i++)
    {
        sprintf(chemin, "ressources/boss_frame%d.bmp", i);
        a.boss_frames[i] = charger_un_sprite(chemin);
    }

    // === FRAMES D'EXPLOSION (5 tailles x 5 frames = 25 fichiers) ===
    const char *suffixes[5] = {"mini", "petit", "moyen", "gros", "boss"};
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            sprintf(chemin, "ressources/explosion_%s_frame%d.bmp", suffixes[i], j);
            a.explosions_frames[i][j] = charger_un_sprite(chemin);
        }
    }

    // === TIR DU JOUEUR ===
    a.sprite_tir = charger_un_sprite("ressources/tir_simple.bmp");

    // === PROJECTILE BOSS (4 frames) ===
    for (i = 0; i < 4; i++)
    {
        sprintf(chemin, "ressources/projectile_boss_frame%d.bmp", i);
        a.projectile_boss_frames[i] = charger_un_sprite(chemin);
    }

    // === SPRITES JOUEUR ===
    a.joueur_idle = charger_un_sprite("ressources/joueur_idle.bmp");
    for (i = 0; i < 4; i++)
    {
        sprintf(chemin, "ressources/joueur_run%d.bmp", i);
        a.joueur_run[i] = charger_un_sprite(chemin);
    }

    return a;
}

void liberer_assets(Assets *a)
{
    int i, j;

    for (i = 0; i < 6; i++) destroy_bitmap(a->fonds[i]);
    for (i = 0; i < 4; i++) destroy_bitmap(a->asteroides[i]);
    for (i = 0; i < 7; i++) destroy_bitmap(a->boss_frames[i]);

    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            destroy_bitmap(a->explosions_frames[i][j]);

    destroy_bitmap(a->sprite_tir);
    for (i = 0; i < 4; i++) destroy_bitmap(a->projectile_boss_frames[i]);

    destroy_bitmap(a->joueur_idle);
    for (i = 0; i < 4; i++) destroy_bitmap(a->joueur_run[i]);
}