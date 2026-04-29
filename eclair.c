//
// Created by xumer on 27/04/2026.
//

#include <stdlib.h>
#include <allegro.h>
#include "eclair.h"

// Cree un eclair a la position d'une bulle
int creer_eclair(Eclair eclairs[], int max, float x_bulle, float y_bulle)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!eclairs[i].actif)
        {
            eclairs[i].actif = 1;
            eclairs[i].x = x_bulle;
            eclairs[i].y_haut = y_bulle;
            eclairs[i].y_bas = y_bulle;   // commence avec longueur 0
            eclairs[i].duree = 60;        // 1 seconde
            return 1;
        }
    }
    return 0;
}

// Met a jour les eclairs : ils s'allongent vers le bas puis disparaissent
void update_eclairs(Eclair eclairs[], int max, int ecran_y)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (eclairs[i].actif)
        {
            // Allongement vers le bas
            if (eclairs[i].y_bas < ecran_y)
            {
                eclairs[i].y_bas += 12;   // vitesse de descente
            }

            // Decompte de duree
            eclairs[i].duree--;
            if (eclairs[i].duree <= 0)
            {
                eclairs[i].actif = 0;
            }
        }
    }
}

// Affiche les eclairs (zigzag jaune-blanc effet electrique)
void afficher_eclairs(BITMAP *buffer, Eclair eclairs[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (eclairs[i].actif)
        {
            // Zigzag : on dessine plusieurs petits segments alternes
            float y = eclairs[i].y_haut;
            float x = eclairs[i].x;
            int cote = 0;   // cote actuel du zigzag

            while (y < eclairs[i].y_bas)
            {
                float prochain_y = y + 10;
                if (prochain_y > eclairs[i].y_bas) prochain_y = eclairs[i].y_bas;

                float prochain_x = eclairs[i].x + (cote == 0 ? 5 : -5);

                // Trait jaune epais
                line(buffer, x, y, prochain_x, prochain_y, makecol(255, 255, 0));
                line(buffer, x+1, y, prochain_x+1, prochain_y, makecol(255, 255, 100));
                line(buffer, x-1, y, prochain_x-1, prochain_y, makecol(255, 255, 100));

                x = prochain_x;
                y = prochain_y;
                cote = 1 - cote;   // alterner
            }
        }
    }
}

// Collision eclair/joueur (simple : zone verticale autour de x)
int collision_eclair_joueur(Eclair *e, int joueur_x, int joueur_y,
                             int joueur_largeur, int joueur_hauteur)
{
    // L'eclair occupe une zone X de +/- 8 pixels
    if (e->x + 8 > joueur_x &&
        e->x - 8 < joueur_x + joueur_largeur &&
        e->y_bas > joueur_y &&
        e->y_haut < joueur_y + joueur_hauteur)
    {
        return 1;
    }
    return 0;
}