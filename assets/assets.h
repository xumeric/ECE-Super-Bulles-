//
// Created by xumer on 29/04/2026.
//

#ifndef SPACEGAME_ASSETS_H
#define SPACEGAME_ASSETS_H

#include <allegro.h>

typedef struct {
    // Fonds de niveau
    BITMAP *fonds[6];

    // Asteroides statiques
    BITMAP *asteroides[4];

    // Animation du boss UFO rouge
    BITMAP *boss_frames[7];

    // Frames d'explosion
    BITMAP *explosions_frames[5][5];

    // Tir du joueur
    BITMAP *sprite_tir;

    // Projectile du boss
    BITMAP *projectile_boss_frames[4];

    // Sprites du joueur
    BITMAP *joueur_idle;
    BITMAP *joueur_run[4];

    BITMAP *menu_fond;

    SAMPLE *musique_menu;
    SAMPLE *musique_jeu;
    SAMPLE *musique_boss;
    SAMPLE *musique_victoire;

    BITMAP *sprite_laser;

} Assets;

Assets charger_assets(void);

void liberer_assets(Assets *assets);



#endif //SPACEGAME_ASSETS_H