//
// Created by xumer on 21/04/2026.
//
#include <math.h>
#include "physique.h"

// Applique la gravite et met a jour la position
void deplacer_bulle(Bulle *b, float gravite)
{
    b->vy += gravite;
    b->y += b->vy;
    b->x += b->vx;
}

// Rebonds sur les murs et le sol
void rebonds_bulle(Bulle *b, int ecran_x, int ecran_y)
{
    // Rebond au sol
    if (b->y + b->rayon > ecran_y)
    {
        b->y = ecran_y - b->rayon;
        b->vy = -b->vy * 0.85;

        // Rebond minimum pour eviter que la bulle rampe au sol
        if (b->vy > -2.5 && b->vy < 0)
        {
            b->vy = -(b->rayon * 0.22 + 3.0);
        }
    }

    // Rebond mur gauche
    if (b->x - b->rayon < 0)
    {
        b->x = b->rayon;
        b->vx = -b->vx;
    }

    // Rebond mur droit
    if (b->x + b->rayon > ecran_x)
    {
        b->x = ecran_x - b->rayon;
        b->vx = -b->vx;
    }
}

// Collision entre une bulle (cercle) et le joueur (rectangle)
int collision_bulle_joueur(Bulle *b, int joueur_x, int joueur_y,
                           int joueur_largeur, int joueur_hauteur)
{
    return (b->x + b->rayon > joueur_x &&
            b->x - b->rayon < joueur_x + joueur_largeur &&
            b->y + b->rayon > joueur_y);
}

// Collision entre un tir et une bulle
int collision_tir_bulle(Tir *t, Bulle *b)
{
    float dx = t->x - b->x;
    float dy = t->y - b->y;
    float distance = sqrt(dx*dx + dy*dy);
    return (distance < b->rayon);
}