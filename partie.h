//
// Created by xumer on 29/04/2026.
//

//
// partie.h - Structures et helpers de l'etat du jeu
//

#ifndef SPACEGAME_PARTIE_H
#define SPACEGAME_PARTIE_H

#include "entites.h"

#include "constantes.h"

// === STRUCTURE EXPLOSION ===
typedef struct {
    int actif;
    float x, y;
    int taille;     // 0-3 pour bulle, 4 pour boss
    int frame;      // 0 a 4
    int compteur;
} Explosion;

// === STRUCTURE PARTIE : etat de progression ===
typedef struct {
    int score;
    int niveau;
    int niveau_gagner;
    int timer_niveau;
    int temps_restant;
    int decompte_initial;

    int bouclier_actif;
    int tir_double_timer;
    int tir_cooldown;

    char pseudo[50];

    int taille_bulles_actuelle;
} Partie;

// === STRUCTURE ENTITES : tous les objets en jeu ===
typedef struct {
    Joueur joueur;
    Boss boss;

    Bulle *bulles;
    Tir *tirs;
    Powerup *powerups;
    Eclair *eclairs;
    Tir *projectiles_boss;

    Explosion explosions[MAX_EXPLOSIONS];
} Entites;

// === STRUCTURE ANIMATIONS ===
typedef struct {
    int joueur_direction;
    int joueur_en_marche;
    int anim_joueur_compteur;
    int anim_joueur_frame;

    int anim_boss_compteur;
    int anim_boss_frame;

    int anim_proj_boss_compteur;
    int anim_proj_boss_frame;
} Animations;

// === FONCTIONS D'INITIALISATION ===
void init_partie(Partie *p);
void init_entites(Entites *e, int taille_bulles_max);
void liberer_entites(Entites *e);
void init_animations(Animations *a);

// === HELPERS DE GESTION ===

// Reallouer le tableau de bulles selon le niveau
// Retourne le nouveau pointeur (peut etre identique a l'ancien)
Bulle *reallouer_bulles(Bulle *bulles, int *taille_actuelle, int nouveau_niveau);

// Declenche une explosion a la position donnee, taille = niveau de l'asteroide (0-3) ou 4 pour boss
void declencher_explosion(Explosion explosions[], int max, float x, float y, int taille);

#endif