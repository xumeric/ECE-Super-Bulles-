#include <allegro.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "etat_bonus.h"
#include "../constantes.h"
#include "physique.h"
#include "jeu.h"
#include "sauvegarde.h"

static void init_bonus(Partie *p, Entites *e)
{
    int i;

    // Vider tout
    for (i = 0; i < p->taille_bulles_actuelle; i++) e->bulles[i].actif = 0;
    for (i = 0; i < MAX_TIRS; i++) e->tirs[i].actif = 0;
    for (i = 0; i < MAX_ECLAIRS; i++) e->eclairs[i].actif = 0;
    for (i = 0; i < MAX_PROJ_BOSS; i++) e->projectiles_boss[i].actif = 0;

    // Créer 8 bulles au sol qui montent
    for (i = 0; i < 8; i++)
    {
        e->bulles[i].actif = 1;
        e->bulles[i].x = 100 + (i * (SCREEN_W - 200)) / 8;
        e->bulles[i].y = SCREEN_H - 50;
        e->bulles[i].vx = (i % 2 == 0) ? 2.5 : -2.5;
        e->bulles[i].vy = -12.0;
        e->bulles[i].taille = 2;
        e->bulles[i].rayon = rayon_selon_taille(2);
    }
    p->temps_restant = 30 * 60;

    // Repositionner le joueur au centre
    e->joueur.x = SCREEN_W / 2 - e->joueur.largeur / 2;


    p->decompte_initial = 180;
}



int gerer_etat_bonus(BITMAP *buffer, Assets *assets,
                     Partie *p, Entites *e, Animations *a,
                     int couleur, int *fin)
{
    // Décompte initial
    if (p->decompte_initial > 0)
    {
        int chiffre = p->decompte_initial / 60;
        char texte_chiffre[10];

        if (chiffre >= 2)      strcpy(texte_chiffre, "3");
        else if (chiffre == 1) strcpy(texte_chiffre, "2");
        else                   strcpy(texte_chiffre, "GO !");

        if (p->decompte_initial < 30) strcpy(texte_chiffre, "GO !");

        p->decompte_initial--;
    }

    static int initialise = 0;
    int nouvel_etat = ETAT_BONUS;
    int i;

    if (!initialise)
    {
        init_bonus(p, e);
        initialise = 1;
    }

    //fond du boss
    blit(assets->fonds[5], buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    //gravité inversé
    float gravite_inversee = -0.15f;

    //mouvement du joueur
    if (key[KEY_LEFT])
    {
        e->joueur.x -= e->joueur.vitesse;
        a->joueur_direction = -1;
        a->joueur_en_marche = 1;
    }
    else if (key[KEY_RIGHT])
    {
        e->joueur.x += e->joueur.vitesse;
        a->joueur_direction = 1;
        a->joueur_en_marche = 1;
    }
    else
    {
        a->joueur_en_marche = 0;
    }

    if (e->joueur.x < 0) e->joueur.x = 0;
    if (e->joueur.x > SCREEN_W - e->joueur.largeur)
        e->joueur.x = SCREEN_W - e->joueur.largeur;

    //animation du joueur
    if (a->joueur_en_marche)
    {
        a->anim_joueur_compteur++;
        if (a->anim_joueur_compteur >= DUREE_FRAME_JOUEUR)
        {
            a->anim_joueur_compteur = 0;
            a->anim_joueur_frame = (a->anim_joueur_frame + 1) % 4;
        }
    }

    //tirs du joueur
    if (p->tir_cooldown > 0) p->tir_cooldown--;
    if (key[KEY_SPACE] && p->tir_cooldown == 0)
    {
        for (i = 0; i < MAX_TIRS; i++)
        {
            if (!e->tirs[i].actif)
            {
                e->tirs[i].actif = 1;
                e->tirs[i].x = e->joueur.x + e->joueur.largeur / 2;
                e->tirs[i].y = e->joueur.y;
                e->tirs[i].vx = 0;
                p->tir_cooldown = 5;
                break;
            }
        }
    }

    // update des tires
    for (i = 0; i < MAX_TIRS; i++)
    {
        if (e->tirs[i].actif)
        {
            e->tirs[i].y -= VITESSE_TIR;
            e->tirs[i].x += e->tirs[i].vx;

            if (e->tirs[i].y < 0)
            {
                e->tirs[i].actif = 0;
                continue;
            }

            // Collision avec une bulle
            int j;
            for (j = 0; j < p->taille_bulles_actuelle; j++)
            {
                if (e->bulles[j].actif &&
     collision_tir_bulle(&e->tirs[i], &e->bulles[j]))
                {
                    e->tirs[i].actif = 0;
                    p->score += 100;
                    declencher_explosion(e->explosions, MAX_EXPLOSIONS,
                                         e->bulles[j].x, e->bulles[j].y,
                                         e->bulles[j].taille);
                    e->bulles[j].actif = 0;

                    // respawn immédiat d'une bulle en bas
                    int slot;
                    for (slot = 0; slot < p->taille_bulles_actuelle; slot++)
                    {
                        if (!e->bulles[slot].actif)
                        {
                            e->bulles[slot].actif = 1;
                            e->bulles[slot].x = 100 + (rand() % (SCREEN_W - 200));
                            e->bulles[slot].y = SCREEN_H - 50;
                            e->bulles[slot].vx = ((rand() % 2) == 0) ? 2.5 : -2.5;
                            e->bulles[slot].vy = -12.0;
                            e->bulles[slot].taille = 2;
                            e->bulles[slot].rayon = rayon_selon_taille(2);
                            break;
                        }
                    }

                    break;
                }
            }

            // Affichage du tir
            if (e->tirs[i].actif)
            {
                draw_sprite(buffer, assets->sprite_tir,
                            (int)e->tirs[i].x - assets->sprite_tir->w / 2,
                            (int)e->tirs[i].y);
            }
        }
    }

    // update des bulles avec la gravité inversée
    for (i = 0; i < p->taille_bulles_actuelle; i++)
    {
        if (e->bulles[i].actif)
        {
            // Gravité inversée
            e->bulles[i].vy += gravite_inversee;
            e->bulles[i].y += e->bulles[i].vy;
            e->bulles[i].x += e->bulles[i].vx;

            // Rebond sur le PLAFOND (en haut) au lieu du sol
            if (e->bulles[i].y - e->bulles[i].rayon < 0)
            {
                e->bulles[i].y = e->bulles[i].rayon;
                e->bulles[i].vy = -e->bulles[i].vy * 1;

                // Rebond minimum (pour pas qu'elle "rampe" au plafond)
                float rebond_min = 8.0 + e->bulles[i].rayon * 0.1;
                if (e->bulles[i].vy < rebond_min)
                {
                    e->bulles[i].vy = rebond_min;
                }
            }

            // Rebond gauche/droite
            if (e->bulles[i].x - e->bulles[i].rayon < 0)
            {
                e->bulles[i].x = e->bulles[i].rayon;
                e->bulles[i].vx = -e->bulles[i].vx;
            }
            if (e->bulles[i].x + e->bulles[i].rayon > SCREEN_W)
            {
                e->bulles[i].x = SCREEN_W - e->bulles[i].rayon;
                e->bulles[i].vx = -e->bulles[i].vx;
            }
            // Empêche la bulle de sortir par le bas
            if (e->bulles[i].y + e->bulles[i].rayon > SCREEN_H)
            {
                e->bulles[i].y = SCREEN_H - e->bulles[i].rayon;
                if (e->bulles[i].vy > 0)
                {
                    e->bulles[i].vy = 0;
                }
            }

            // Collision avec le joueur
            if (p->decompte_initial == 0  && collision_bulle_joueur(&e->bulles[i],
                              e->joueur.x, e->joueur.y,
                              e->joueur.largeur, e->joueur.hauteur))
            {
                if (p->bouclier_actif)
                {
                    p->bouclier_actif = 0;
                    e->bulles[i].actif = 0;
                }
                else
                {
                    initialise = 0;
                    nouvel_etat = ETAT_GAME_OVER;
                }
            }

            // Affichage
            BITMAP *spr = assets->asteroides[e->bulles[i].taille];
            draw_sprite(buffer, spr,
                        (int)e->bulles[i].x - spr->w / 2,
                        (int)e->bulles[i].y - spr->h / 2);
        }
    }

    // affichage du joueur
    BITMAP *spr_j = a->joueur_en_marche
                    ? assets->joueur_run[a->anim_joueur_frame]
                    : assets->joueur_idle;
    int sx = e->joueur.x + e->joueur.largeur / 2 - spr_j->w / 2;
    int sy = e->joueur.y + e->joueur.hauteur - spr_j->h;
    if (a->joueur_direction == 1)
        draw_sprite(buffer, spr_j, sx, sy);
    else
        draw_sprite_h_flip(buffer, spr_j, sx, sy);

    //frame explosions
    for (i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (e->explosions[i].actif)
        {
            e->explosions[i].compteur++;
            if (e->explosions[i].compteur >= DUREE_FRAME_EXPLOSION)
            {
                e->explosions[i].compteur = 0;
                e->explosions[i].frame++;
                if (e->explosions[i].frame >= 5)
                {
                    e->explosions[i].actif = 0;
                    continue;
                }
            }
            BITMAP *expl = assets->explosions_frames
                              [e->explosions[i].taille][e->explosions[i].frame];
            draw_sprite(buffer, expl,
                        (int)e->explosions[i].x - expl->w / 2,
                        (int)e->explosions[i].y - expl->h / 2);
        }
    }

    // timer decroissant
    if (p->decompte_initial == 0)
    {
        if (p->temps_restant > 0)
        {
            p->temps_restant--;
            if (p->temps_restant % 60 == 0)
            {
                p->score += 50;
            }
        }
        else
        {
            sauvegarder_partie(p->pseudo, p->score, p->niveau);
            initialise = 0;
            nouvel_etat = ETAT_VICTOIRE;
        }
    }

    char texte[100];
    int secondes = p->temps_restant / 60;

    textout_centre_ex(buffer, font, "NIVEAU BONUS - GRAVITE INVERSEE",
                      SCREEN_W / 2, 20,
                      makecol(255, 100, 255), -1);

    // Affichage du décompte initial
    if (p->decompte_initial > 0)
    {
        int chiffre = p->decompte_initial / 60;
        char txt_dc[20];

        if (chiffre >= 2)      strcpy(txt_dc, "3");
        else if (chiffre == 1) strcpy(txt_dc, "2");
        else                   strcpy(txt_dc, "GO !");

        if (p->decompte_initial < 30) strcpy(txt_dc, "GO !");

        textout_centre_ex(buffer, font, txt_dc,
                          SCREEN_W / 2, SCREEN_H / 2,
                          makecol(255, 255, 0), -1);
    }

    // Timer
    sprintf(texte, "Survie : %d s", secondes);
    int couleur_timer = (secondes <= 5) ? makecol(255, 50, 50) : makecol(255, 255, 0);
    textout_centre_ex(buffer, font, texte,
                      SCREEN_W / 2, 50, couleur_timer, -1);

    // Score
    sprintf(texte, "Score : %d", p->score);
    textout_ex(buffer, font, texte, 10, 10, makecol(255, 255, 255), -1);

    if (key[KEY_ESC] || key[KEY_P])
    {
        p->etat_avant_pause = ETAT_BONUS;
        nouvel_etat = ETAT_PAUSE;
    }

    return nouvel_etat;
}