//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_SAUVEGARDE_H
#define SPACEGAME_SAUVEGARDE_H

#define MAX_SAUVEGARDES 10

// Structure d'une sauvegarde
typedef struct {
    char pseudo[50];
    int score;
    int niveau;
} Sauvegarde;

// Prototypes des fonctions
void sauvegarder_partie(const char *pseudo, int score, int niveau);
int charger_partie(char *pseudo, int *score, int *niveau);
int charger_toutes_sauvegardes(Sauvegarde tab[], int max);

#endif
