//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_ENTITES_H
#define SPACEGAME_ENTITES_H
#define POWERUP_BOUCLIER     0
#define POWERUP_TIR_DOUBLE   1
#define POWERUP_BONUS_TEMPS  2


typedef struct {
    float x, y;
    float vx, vy;
    int rayon;
    int actif;
    int taille;
} Bulle;

typedef struct {
    float x, y;
    int actif;
} Tir;

typedef struct {
    int x, y;
    int largeur, hauteur;
    int vitesse;
} Joueur;

typedef struct {
    int x, y;
    int largeur, hauteur;
    char texte[50];
    int survol;
} Bouton;

typedef struct {
    float x, y;
    int largeur, hauteur;
    int hp;                    // points de vie
    int hp_max;                // pour la barre de vie
    int actif;
    int timer_teleport;        // compteur avant prochaine teleportation
    int timer_tir;             // compteur avant prochain tir
    int timer_bulle;           // compteur avant lacher une bulle
    float vx;              // vitesse horizontale
    int mode;              // 0 = deplacement, 1 = teleportation
    int timer_mode;        // duree restante dans le mode actuel
} Boss;

typedef struct {
    float x, y;
    float vy;          // vitesse de chute
    int type;          // POWERUP_BOUCLIER, POWERUP_TIR_DOUBLE, POWERUP_BONUS_TEMPS
    int actif;
} Powerup;

typedef struct {
    float x;          // position X
    float y_haut;     // sommet de l'eclair (la bulle)
    float y_bas;      // bas de l'eclair (descend)
    int actif;
    int duree;        // duree avant disparition
} Eclair;

#endif //SPACEGAME_ENTITES_H