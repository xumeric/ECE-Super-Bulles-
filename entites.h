//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_ENTITES_H
#define SPACEGAME_ENTITES_H


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


#endif //SPACEGAME_ENTITES_H