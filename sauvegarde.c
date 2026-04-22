//
// Created by xumer on 21/04/2026.
//
#include <stdio.h>
#include <string.h>
#include "sauvegarde.h"

// Lit toutes les sauvegardes et les met dans le tableau
// Retourne le nombre de sauvegardes lues
int charger_toutes_sauvegardes(Sauvegarde tab[], int max)
{
    FILE *f = fopen("sauvegarde.txt", "r");
    if (f == NULL) return 0;

    int nb = 0;
    while (nb < max &&
           fscanf(f, "%s %d %d", tab[nb].pseudo, &tab[nb].score, &tab[nb].niveau) == 3)
    {
        nb++;
    }

    fclose(f);
    return nb;
}

// Sauvegarde en remplacant la ligne du meme pseudo
void sauvegarder_partie(const char *pseudo, int score, int niveau)
{
    Sauvegarde sauvegardes[MAX_SAUVEGARDES];
    int nb = charger_toutes_sauvegardes(sauvegardes, MAX_SAUVEGARDES);

    int trouve = -1;
    int i;
    for (i = 0; i < nb; i++)
    {
        if (strcmp(sauvegardes[i].pseudo, pseudo) == 0)
        {
            trouve = i;
            break;
        }
    }

    if (trouve != -1)
    {
        sauvegardes[trouve].score = score;
        sauvegardes[trouve].niveau = niveau;
    }
    else
    {
        if (nb < MAX_SAUVEGARDES)
        {
            strcpy(sauvegardes[nb].pseudo, pseudo);
            sauvegardes[nb].score = score;
            sauvegardes[nb].niveau = niveau;
            nb++;
        }
    }

    FILE *f = fopen("sauvegarde.txt", "w");
    if (f == NULL) return;

    for (i = 0; i < nb; i++)
    {
        fprintf(f, "%s %d %d\n",
                sauvegardes[i].pseudo,
                sauvegardes[i].score,
                sauvegardes[i].niveau);
    }

    fclose(f);
}

// Ancienne fonction de chargement simple (pour le pseudo uniquement)
int charger_partie(char *pseudo, int *score, int *niveau)
{
    FILE *f = fopen("sauvegarde.txt", "r");
    if (f == NULL) return 0;
    fscanf(f, "%s %d %d", pseudo, score, niveau);
    fclose(f);
    return 1;
}