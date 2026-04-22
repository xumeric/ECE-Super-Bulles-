//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_JEU_H
#define SPACEGAME_JEU_H


#include "entites.h"

// Retourne le rayon d'une bulle selon sa taille
int rayon_selon_taille(int taille);

// Cree les bulles pour un niveau donne dans le tableau
void creer_bulles_niveau(Bulle bulles[], int max, int niveau, int ecran_x);

// Divise une bulle en 2 bulles plus petites
// index_bulle = indice de la bulle a diviser
void diviser_bulle(Bulle bulles[], int max, int index_bulle);

// Compte le nombre de bulles actives dans le tableau
int compter_bulles_actives(Bulle bulles[], int max);


#endif //SPACEGAME_JEU_H