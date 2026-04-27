//
// Created by xumer on 26/04/2026.
//
#include <stdlib.h>
#include <allegro.h>
#include "powerup.h"

// Initialise tous les powerups
void init_powerups(Powerup powerups[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        powerups[i].actif = 0;
    }
}

// Cree un powerup a la position donnee, type aleatoire
int creer_powerup(Powerup powerups[], int max, float x, float y)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!powerups[i].actif)
        {
            powerups[i].actif = 1;
            powerups[i].x = x;
            powerups[i].y = y;
            powerups[i].vy = 0;
            powerups[i].type = rand() % 3;   // 0, 1 ou 2
            return 1;
        }
    }
    return 0;
}

// Met a jour les powerups (gravite + chute)
void update_powerups(Powerup powerups[], int max, int ecran_y)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (powerups[i].actif)
        {
            powerups[i].vy += 0.15;   // meme gravite que les bulles
            powerups[i].y += powerups[i].vy;

            // Sortir de l'ecran = desactiver
            if (powerups[i].y > ecran_y)
            {
                powerups[i].actif = 0;
            }
        }
    }
}

// Affiche les powerups (carre colore avec lettre)
void afficher_powerups(BITMAP *buffer, Powerup powerups[], int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (powerups[i].actif)
        {
            int couleur;
            char lettre[2] = " ";

            // Couleur et lettre selon le type
            if (powerups[i].type == POWERUP_BOUCLIER)
            {
                couleur = makecol(50, 150, 255);    // bleu
                lettre[0] = 'B';
            }
            else if (powerups[i].type == POWERUP_TIR_DOUBLE)
            {
                couleur = makecol(255, 100, 100);   // rouge
                lettre[0] = 'D';
            }
            else
            {
                couleur = makecol(100, 255, 100);   // vert
                lettre[0] = 'T';
            }

            // Dessiner un carre
            int taille = 25;
            rectfill(buffer,
                     powerups[i].x - taille/2, powerups[i].y - taille/2,
                     powerups[i].x + taille/2, powerups[i].y + taille/2,
                     couleur);
            rect(buffer,
                 powerups[i].x - taille/2, powerups[i].y - taille/2,
                 powerups[i].x + taille/2, powerups[i].y + taille/2,
                 makecol(255, 255, 255));

            // Dessiner la lettre au centre
            textout_centre_ex(buffer, font, lettre,
                              powerups[i].x, powerups[i].y - 4,
                              makecol(255, 255, 255), -1);
        }
    }
}

// Le joueur ramasse-t-il un powerup ?
int ramasser_powerup(Powerup powerups[], int max,
                     int joueur_x, int joueur_y,
                     int joueur_largeur, int joueur_hauteur)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (powerups[i].actif)
        {
            int taille = 25;
            // Test collision rectangle/rectangle
            if (powerups[i].x + taille/2 > joueur_x &&
                powerups[i].x - taille/2 < joueur_x + joueur_largeur &&
                powerups[i].y + taille/2 > joueur_y &&
                powerups[i].y - taille/2 < joueur_y + joueur_hauteur)
            {
                int type = powerups[i].type;
                powerups[i].actif = 0;
                return type;
            }
        }
    }
    return -1;
}