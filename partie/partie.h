

#ifndef SPACEGAME_PARTIE_H
#define SPACEGAME_PARTIE_H

#include "../entites.h"

#include "../constantes.h"


typedef struct {
    int actif;
    float x, y;
    int taille;
    int frame;
    int compteur;
} Explosion;


typedef struct {
    int score;
    int niveau;
    int niveau_gagner;
    int timer_niveau;
    int temps_restant;
    int decompte_initial;
    int etat_avant_pause;

    int bouclier_actif;
    int tir_double_timer;
    int tir_cooldown;

    char pseudo[50];

    int taille_bulles_actuelle;
} Partie;

typedef struct {
    Joueur joueur;
    Boss boss;

    Bulle *bulles;
    Tir *tirs;
    Powerup *powerups;
    Eclair *eclairs;
    Tir *projectiles_boss;
    Laser *lasers;

    Explosion explosions[MAX_EXPLOSIONS];
} Entites;


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


void init_partie(Partie *p);
void init_entites(Entites *e, int taille_bulles_max);
void liberer_entites(Entites *e);
void init_animations(Animations *a);


Bulle *reallouer_bulles(Bulle *bulles, int *taille_actuelle, int nouveau_niveau);


void declencher_explosion(Explosion explosions[], int max, float x, float y, int taille);

#endif