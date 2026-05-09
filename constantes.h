//
// Created by xumer on 29/04/2026.
//



#ifndef SPACEGAME_CONSTANTES_H
#define SPACEGAME_CONSTANTES_H


// ETATS DU JEU

#define ETAT_MENU       0
#define ETAT_JEU        1
#define ETAT_GAME_OVER  2
#define ETAT_REGLES     3
#define ETAT_PSEUDO     4
#define ETAT_REPRENDRE  5
#define ETAT_BOSS       6
#define ETAT_VICTOIRE   7
#define ETAT_PAUSE      8
#define ETAT_BONUS      9

// DIMENSIONS DE L'ECRAN

#define Ecran_X 1280
#define Ecran_Y 720

// LIMITES DES TABLEAUX

#define MAX_TIRS        1000   // tirs du joueur
#define MAX_ECLAIRS     100    // eclairs au sol
#define MAX_PROJ_BOSS   20     // projectiles du boss
#define MAX_BULLES      100    // taille max du tableau de bulles
#define MAX_EXPLOSIONS  32     // explosions affichees simultanement
#define MAX_LASERS  5    // lasers simultanés du boss


// DUREES D'ANIMATION

#define DUREE_FRAME_JOUEUR     5     // 12 fps pour le joueur
#define DUREE_FRAME_BOSS       6     // 10 fps pour le UFO
#define DUREE_FRAME_PROJ_BOSS  5     // 12 fps pour les projectiles boss
#define DUREE_FRAME_EXPLOSION  4     // 15 fps pour les explosions
#define DUREE_FRAME_MENU       2     // 10 fps pour le menu d'accueille


// GAMEPLAY (constantes physiques et de jeu)

#define GRAVITE        0.15f   // gravite appliquee aux bulles
#define VITESSE_TIR    7       // vitesse de monte des tirs

#endif