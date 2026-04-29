//
// Created by xumer on 29/04/2026.
//

#ifndef SPACEGAME_ASSETS_H
#define SPACEGAME_ASSETS_H


// on a une struct Assets qui contient tous les sprites du jeu, et 2 fonctions

#include <allegro.h>

typedef struct {
    // Fonds de niveau (5 niveaux + boss = 6)
    BITMAP *fonds[6];

    // Asteroides statiques (4 tailles : mini, petit, moyen, gros)
    // asteroides[bulles[i].taille] donne le bon sprite
    BITMAP *asteroides[4];

    // Animation du boss UFO rouge (7 frames)
    BITMAP *boss_frames[7];

    // Frames d'explosion : [taille][frame]
    // taille : 0=mini, 1=petit, 2=moyen, 3=gros, 4=boss
    BITMAP *explosions_frames[5][5];

    // Tir du joueur (sprite simple, pas anime)
    BITMAP *sprite_tir;

    // Projectile du boss (4 frames d'animation orbe orange)
    BITMAP *projectile_boss_frames[4];

    // Sprites du joueur
    BITMAP *joueur_idle;       // pose statique
    BITMAP *joueur_run[4];     // 4 frames de course
} Assets;

// Charge tous les sprites depuis le dossier ressources/.
// En cas d'erreur, affiche un message via allegro_message et termine le programme.
Assets charger_assets(void);

// Libere toute la memoire allouee par charger_assets.

void liberer_assets(Assets *assets);



#endif //SPACEGAME_ASSETS_H