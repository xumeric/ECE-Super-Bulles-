
#include "../jeu/jeu.h"

// Retourne le rayon selon la taille
int rayon_selon_taille(int taille)
{
    if (taille == 3) return 65;
    if (taille == 2) return 40;
    if (taille == 1) return 30;
    return 20;   // taille 0
}

// Cree les bulles d'un niveau
void creer_bulles_niveau(Bulle bulles[], int max, int niveau, int ecran_x)
{
    int i;

    for (i = 0; i < max; i++)
    {
        bulles[i].actif = 0;
    }

    // Creer 'niveau' bulles, reparties horizontalement
    for (i = 0; i < niveau && i < max; i++)
    {
        bulles[i].actif = 1;
        bulles[i].x = (i + 1) * ecran_x / (niveau + 1);
        bulles[i].y = 50;
        bulles[i].vx = 1.5 + niveau * 0.5;
        bulles[i].vy = 0;
        bulles[i].taille = 3;
        bulles[i].rayon = rayon_selon_taille(3);
    }
}

void diviser_bulle(Bulle bulles[], int max, int index_bulle)
{
    if (bulles[index_bulle].taille <= 0) return;

    int nouvelle_taille = bulles[index_bulle].taille - 1;
    float x_parent = bulles[index_bulle].x;
    float y_parent = bulles[index_bulle].y;

    int crees = 0;
    int k;
    for (k = 0; k < max && crees < 2; k++)
    {
        if (!bulles[k].actif)
        {
            bulles[k].actif = 1;
            bulles[k].x = x_parent + (crees == 0 ? -15 : 15);
            bulles[k].y = y_parent;
            bulles[k].vx = (crees == 0) ? -2.5 : 2.5;
            bulles[k].vy = -3.5;
            bulles[k].taille = nouvelle_taille;
            bulles[k].rayon = rayon_selon_taille(nouvelle_taille);
            crees++;
        }
    }
}

int compter_bulles_actives(Bulle bulles[], int max)
{
    int nb = 0;
    int i;
    for (i = 0; i < max; i++)
    {
        if (bulles[i].actif) nb++;
    }
    return nb;
}