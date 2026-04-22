//
// Created by xumer on 21/04/2026.
//
#include <allegro.h>
#include "graphique.h"

void dessiner_bouton(BITMAP *buffer, Bouton *b)
{
    int couleur = b->survol ? makecol(100,100,255) : makecol(50,50,150);
    rectfill(buffer, b->x, b->y, b->x + b->largeur, b->y + b->hauteur, couleur);
    rect(buffer, b->x, b->y, b->x + b->largeur, b->y + b->hauteur, makecol(255,255,255));
    textout_centre_ex(buffer, font, b->texte,
                      b->x + b->largeur/2, b->y + b->hauteur/2 - 4,
                      makecol(255,255,255), -1);
}

int souris_dans_bouton(Bouton *b)
{
    return (mouse_x >= b->x && mouse_x <= b->x + b->largeur &&
            mouse_y >= b->y && mouse_y <= b->y + b->hauteur);
}