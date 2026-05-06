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


    for (i = 0; i < 6; i++)
    {
        sprintf(chemin, "ressources/fond_niveau%d.bmp", i + 1);
        a.fonds[i] = charger_un_sprite(chemin);
    }



    a.asteroides[0] = charger_un_sprite("ressources/asteroide_taille4.bmp");
    a.asteroides[2] = charger_un_sprite("ressources/asteroide_taille3.bmp");
    a.asteroides[1] = charger_un_sprite("ressources/asteroide_taille2.bmp");
    a.asteroides[3] = charger_un_sprite("ressources/asteroide_taille1.bmp");



        sprintf(chemin,"ressources/menu_frame0.bmp");
        a.menu_fond = charger_un_sprite(chemin);


    // BOSS (7 frames d'animation UFO rouge)
    for (i = 0; i < 7; i++)
    {
        sprintf(chemin, "ressources/boss_frame%d.bmp", i);
        a.boss_frames[i] = charger_un_sprite(chemin);
    }

    // frame explosions
    const char *suffixes[5] = {"mini", "petit", "moyen", "gros", "boss"};
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            sprintf(chemin, "ressources/explosion_%s_frame%d.bmp", suffixes[i], j);
            a.explosions_frames[i][j] = charger_un_sprite(chemin);
        }
    }

    // tir joueur
    a.sprite_tir = charger_un_sprite("ressources/tir_simple.bmp");

    // Projetil boss
    for (i = 0; i < 4; i++)
    {
        sprintf(chemin, "ressources/projectile_boss_frame%d.bmp", i);
        a.projectile_boss_frames[i] = charger_un_sprite(chemin);
    }

    //joueur
    a.joueur_idle = charger_un_sprite("ressources/joueur_idle.bmp");
    for (i = 0; i < 4; i++)
    {
        sprintf(chemin, "ressources/joueur_run%d.bmp", i);
        a.joueur_run[i] = charger_un_sprite(chemin);
    }

    a.musique_menu = load_sample("ressources/musique_menu.wav");

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

    for (i = 0; i < 10; i++) destroy_bitmap(a->menu_fond);

    destroy_sample(a->musique_menu);
}