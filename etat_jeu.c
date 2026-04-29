//
// Created by xumer on 29/04/2026.
//
//
// etat_jeu.c - Gestion de l'etat ETAT_JEU (gameplay normal)
//

//
// etat_jeu.c - Gestion de l'etat ETAT_JEU (gameplay normal)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <allegro.h>
#include "etat_jeu.h"
#include "constantes.h"
#include "physique.h"
#include "jeu.h"
#include "boss.h"
#include "powerup.h"
#include "eclair.h"
#include "sauvegarde.h"

int gerer_etat_jeu(BITMAP *buffer, Assets *assets,
                   Partie *p, Entites *e, Animations *a,
                   int couleur, float gravite, int *fin)
{
    int nouvel_etat = ETAT_JEU;   // par defaut, on reste dans cet etat

    // Afficher le fond du p->niveau (recouvre le clear noir)
    blit(assets->fonds[p->niveau - 1], buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    // Mise a jour direction et etat de marche
    a->joueur_en_marche = 0;  // par defaut, immobile
    // Deplacement du e->joueur
    if (key[KEY_LEFT])
    {
        e->joueur.x -= e->joueur.vitesse;
        a->joueur_direction = -1;
        a->joueur_en_marche = 1;
    }
    if (key[KEY_RIGHT])
    {
        e->joueur.x += e->joueur.vitesse;
        a->joueur_direction = 1;
        a->joueur_en_marche = 1;
    }
    // Avancer l'animation de marche
    if (a->joueur_en_marche)
    {
        a->anim_joueur_compteur++;
        if (a->anim_joueur_compteur >= DUREE_FRAME_JOUEUR)
        {
            a->anim_joueur_compteur = 0;
            a->anim_joueur_frame = (a->anim_joueur_frame + 1) % 4;
        }
    }
    else
    {
        a->anim_joueur_frame = 0;  // reset quand on s'arrete
    }

    if (key[KEY_ESC])   *fin = 1;

    if (e->joueur.x < 0) e->joueur.x = 0;
    if (e->joueur.x > SCREEN_W - e->joueur.largeur) e->joueur.x = SCREEN_W - e->joueur.largeur;

    // Pendant le decompte initial : afficher le chiffre et sauter la logique
    if (p->decompte_initial > 0)
    {
        // Afficher le e->joueur (deja affiche au-dessus)
        // Choisir le bon sprite (idle ou frame de course)
        BITMAP *sprite_joueur = a->joueur_en_marche ? assets->joueur_run[a->anim_joueur_frame] : assets->joueur_idle;

        // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
        int sx = e->joueur.x + e->joueur.largeur / 2 - sprite_joueur->w / 2;
        int sy = e->joueur.y + e->joueur.hauteur - sprite_joueur->h;

        if (a->joueur_direction == 1)
            draw_sprite(buffer, sprite_joueur, sx, sy);
        else
            draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

        // Afficher les e->bulles SANS les bouger
        int b;
        for (b = 0; b < p->taille_bulles_actuelle; b++)
        {
            if (e->bulles[b].actif)
            {
                // e->bulles[b].taille va de 0 (plus gros) a 3 (plus petit)
                BITMAP *sprite = assets->asteroides[e->bulles[b].taille];
                draw_sprite(buffer, sprite,
                            e->bulles[b].x - sprite->w / 2,
                            e->bulles[b].y - sprite->h / 2);
            }
        }
        // Calculer quel chiffre afficher
        int chiffre = (p->decompte_initial / 60);   // 240->4, 180->3, 120->2, 60->1
        char texte_chiffre[10];

        if (chiffre == 3)
            strcpy(texte_chiffre, "3");
        else if (chiffre == 2)
            strcpy(texte_chiffre, "2");
        else if (chiffre == 1)
            strcpy(texte_chiffre, "1");
        else
            strcpy(texte_chiffre, "GO !");

        // Afficher le chiffre en GROS au centre
        textout_centre_ex(buffer, font, texte_chiffre,
                          SCREEN_W / 2, SCREEN_H / 2,
                          makecol(255, 255, 0), -1);

        p->decompte_initial--;

        return nouvel_etat;// saute le reste de la boucle while
    }


    // Affichage e->joueur (couleur normale ou rouge si tir double)
    int couleur_joueur = couleur;
    if (p->tir_double_timer > 0) couleur_joueur = makecol(255, 50, 50);

    // Choisir le bon sprite (idle ou frame de course)
    BITMAP *sprite_joueur = a->joueur_en_marche ? assets->joueur_run[a->anim_joueur_frame] : assets->joueur_idle;

    // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
    int sx = e->joueur.x + e->joueur.largeur / 2 - sprite_joueur->w / 2;
    int sy = e->joueur.y + e->joueur.hauteur - sprite_joueur->h;

    if (a->joueur_direction == 1)
        draw_sprite(buffer, sprite_joueur, sx, sy);
    else
        draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

    // Affichage bouclier autour du e->joueur
    if (p->bouclier_actif)
    {
        circle(buffer, e->joueur.x + e->joueur.largeur/2, e->joueur.y + e->joueur.hauteur/2,
               40, makecol(50, 150, 255));
        circle(buffer, e->joueur.x + e->joueur.largeur/2, e->joueur.y + e->joueur.hauteur/2,
               38, makecol(100, 180, 255));
    }

    // Bulles : physique + affichage + collision e->joueur
    int i;
    for (i = 0; i < p->taille_bulles_actuelle; i++)
    {
        if (e->bulles[i].actif)
        {
            deplacer_bulle(&e->bulles[i], gravite);
            rebonds_bulle(&e->bulles[i], SCREEN_W, SCREEN_H);

            if (collision_bulle_joueur(&e->bulles[i],
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
                    nouvel_etat = ETAT_GAME_OVER;
                }
            }
            // e->bulles[i].taille va de 0 (plus gros) a 3 (plus petit)
            BITMAP *sprite = assets->asteroides[e->bulles[i].taille];
            draw_sprite(buffer, sprite,
                        e->bulles[i].x - sprite->w / 2,
                        e->bulles[i].y - sprite->h / 2);
        }
    }

    // === MISE A JOUR ET AFFICHAGE DES EXPLOSIONS ===
    int ex;
    for (ex = 0; ex < MAX_EXPLOSIONS; ex++)
    {
        if (e->explosions[ex].actif)
        {
            // Dessiner la frame actuelle
            BITMAP *frame_sprite = assets->explosions_frames[e->explosions[ex].taille][e->explosions[ex].frame];
            draw_sprite(buffer, frame_sprite,
                        e->explosions[ex].x - frame_sprite->w / 2,
                        e->explosions[ex].y - frame_sprite->h / 2);

            // Avancer l'animation
            e->explosions[ex].compteur++;
            if (e->explosions[ex].compteur >= DUREE_FRAME_EXPLOSION)
            {
                e->explosions[ex].compteur = 0;
                e->explosions[ex].frame++;
                if (e->explosions[ex].frame >= 5)
                {
                    e->explosions[ex].actif = 0;  // animation terminee
                }
            }
        }
    }


    // Update et affichage des e->powerups
    update_powerups(e->powerups, MAX_POWERUPS, SCREEN_H);
    afficher_powerups(buffer, e->powerups, MAX_POWERUPS);

    // Ramassage des e->powerups
    int type_ramasse = ramasser_powerup(e->powerups, MAX_POWERUPS,
                                        e->joueur.x, e->joueur.y,
                                        e->joueur.largeur, e->joueur.hauteur);
    if (type_ramasse != -1)
    {
        if (type_ramasse == POWERUP_BOUCLIER)
        {
            p->bouclier_actif = 1;
        }
        else if (type_ramasse == POWERUP_TIR_DOUBLE)
        {
            p->tir_double_timer += 600;   // 10 secondes
        }
        else if (type_ramasse == POWERUP_BONUS_TEMPS)
        {
            p->temps_restant += 20 * 60;   // +20 secondes
        }
    }

    // Eclairs aleatoires des e->bulles a partir du p->niveau 3
    if (p->niveau >= 3)
    {
        for (i = 0; i < p->taille_bulles_actuelle; i++)
        {
            if (e->bulles[i].actif)
            {
                if (rand() % 600 == 0)
                {
                    creer_eclair(e->eclairs, MAX_ECLAIRS, e->bulles[i].x, e->bulles[i].y);
                }
            }
        }
    }

    // Update et affichage des e->eclairs
    update_eclairs(e->eclairs, MAX_ECLAIRS, SCREEN_H);
    afficher_eclairs(buffer, e->eclairs, MAX_ECLAIRS);

    // Collision eclair/e->joueur
    for (i = 0; i < MAX_ECLAIRS; i++)
    {
        if (e->eclairs[i].actif)
        {
            if (collision_eclair_joueur(&e->eclairs[i],
                                         e->joueur.x, e->joueur.y,
                                         e->joueur.largeur, e->joueur.hauteur))
            {
                if (p->bouclier_actif)
                {
                    p->bouclier_actif = 0;
                    e->eclairs[i].actif = 0;
                }
                else
                {
                    nouvel_etat = ETAT_GAME_OVER;
                }
            }
        }
    }

    // Cooldowns
    if (p->tir_cooldown > 0) p->tir_cooldown--;
    if (p->tir_double_timer > 0) p->tir_double_timer--;

    // Auto-tir
    if (key[KEY_SPACE] && p->tir_cooldown == 0)
    {
        if (p->tir_double_timer > 0)
        {
            // TIR DOUBLE : 2 balles espacees
            int balles_creees = 0;
            for (i = 0; i < MAX_TIRS && balles_creees < 2; i++)
            {
                if (!e->tirs[i].actif)
                {
                    e->tirs[i].actif = 1;
                    e->tirs[i].x = e->joueur.x + e->joueur.largeur / 2 + (balles_creees == 0 ? -10 : 10);
                    e->tirs[i].y = e->joueur.y;
                    balles_creees++;
                }
            }
            p->tir_cooldown = 5;
        }
        else
        {
            // TIR NORMAL : 1 balle
            for (i = 0; i < MAX_TIRS; i++)
            {
                if (!e->tirs[i].actif)
                {
                    e->tirs[i].actif = 1;
                    e->tirs[i].x = e->joueur.x + e->joueur.largeur / 2;
                    e->tirs[i].y = e->joueur.y;
                    p->tir_cooldown = 5;
                    break;
                }
            }
        }
    }

    // Tirs : deplacement + collision avec e->bulles
    for (i = 0; i < MAX_TIRS; i++)
    {
        if (e->tirs[i].actif)
        {
            e->tirs[i].y -= 7;
            if (e->tirs[i].y < 0)
            {
                e->tirs[i].actif = 0;
            }
            else
            {
                draw_sprite(buffer, assets->sprite_tir, e->tirs[i].x, e->tirs[i].y);
            }

            int j;
            for (j = 0; j < p->taille_bulles_actuelle; j++)
            {
                if (e->bulles[j].actif)
                {
                    if (collision_tir_bulle(&e->tirs[i], &e->bulles[j]))
                    {
                        e->tirs[i].actif = 0;
                        p->score += (4 - e->bulles[j].taille) * 100;
                        declencher_explosion(e->explosions, MAX_EXPLOSIONS,
                        e->bulles[j].x, e->bulles[j].y, e->bulles[j].taille);
                        diviser_bulle(e->bulles, p->taille_bulles_actuelle, j);
                        e->bulles[j].actif = 0;
                        // 20% de chance de creer un powerup
                        if (rand() % 100 < 20)
                        {
                            creer_powerup(e->powerups, MAX_POWERUPS, e->bulles[j].x, e->bulles[j].y);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Niveau gagne ?
    int bulles_restantes = compter_bulles_actives(e->bulles, p->taille_bulles_actuelle);

    if (bulles_restantes == 0 && !p->niveau_gagner)
    {
        p->niveau_gagner = 1;
        p->timer_niveau = 0;
    }

    if (p->niveau_gagner)
    {
        textout_centre_ex(buffer, font, "NIVEAU GAGNE !",
                          SCREEN_W / 2, SCREEN_H / 2,
                          makecol(0, 255, 0), -1);
        p->timer_niveau++;

        if (p->timer_niveau > 120)
        {
            p->niveau++;
            p->decompte_initial = 240;   // 4 secondes de decompte
            sauvegarder_partie(p->pseudo, p->score, p->niveau);

            for (i = 0; i < MAX_TIRS; i++) e->tirs[i].actif = 0;

            // Si on arrive au p->niveau 6, on passe au e->boss
            if (p->niveau == 6)
            {
                init_boss(&e->boss, SCREEN_W);
                for (i = 0; i < MAX_PROJ_BOSS; i++) e->projectiles_boss[i].actif = 0;
                for (i = 0; i < p->taille_bulles_actuelle; i++) e->bulles[i].actif = 0;
                e->joueur.x = SCREEN_W / 2 - e->joueur.largeur / 2;
                p->niveau_gagner = 0;
                p->timer_niveau = 0;
                nouvel_etat = ETAT_BOSS;
            }
            else
            {
                e->bulles = reallouer_bulles(e->bulles, &p->taille_bulles_actuelle, p->niveau);
                creer_bulles_niveau(e->bulles, p->taille_bulles_actuelle, p->niveau, SCREEN_W);
                e->joueur.x = SCREEN_W / 2 - e->joueur.largeur / 2;
                p->niveau_gagner = 0;
                p->timer_niveau = 0;
                p->temps_restant = (50 + p->niveau * 10) * 60;
            }
        }
    }

    // Decrementation du timer
    if (!p->niveau_gagner && p->temps_restant > 0)
    {
        p->temps_restant--;

        // Temps epuise = game over
        if (p->temps_restant == 0)
        {
            nouvel_etat = ETAT_GAME_OVER;
        }
    }

    // Affichage p->score, p->niveau, timer
    char texte_score[50];
    sprintf(texte_score, "Score : %d", p->score);
    textout_ex(buffer, font, texte_score, 10, 10, makecol(255, 255, 255), -1);

    char texte_niveau[30];
    sprintf(texte_niveau, "Niveau : %d", p->niveau);
    textout_ex(buffer, font, texte_niveau, SCREEN_W - 120, 10, makecol(255, 255, 255), -1);

    // Timer avec couleur changeante
    int secondes = p->temps_restant / 60;
    char texte_temps[30];
    sprintf(texte_temps, "Temps : %d", secondes);

    int couleur_temps;
    if (secondes > 10)      couleur_temps = makecol(255, 255, 255);   // blanc
    else if (secondes > 5)  couleur_temps = makecol(255, 200, 0);     // orange
    else                    couleur_temps = makecol(255, 50, 50);     // rouge

    textout_ex(buffer, font, texte_temps, SCREEN_W / 2 - 40, 10, couleur_temps, -1);

    // HUD des effets actifs (en bas a droite)
    int hud_y = SCREEN_H - 50;

    if (p->bouclier_actif)
    {
        int hud_x = SCREEN_W - 80;
        rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(50, 150, 255));
        rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
        textout_centre_ex(buffer, font, "B", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);
    }

    if (p->tir_double_timer > 0)
    {
        int hud_x = SCREEN_W - 40;
        rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 100, 100));
        rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
        textout_centre_ex(buffer, font, "D", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);

        char texte_temps_powerup[10];
        sprintf(texte_temps_powerup, "%d", p->tir_double_timer / 60);
        textout_centre_ex(buffer, font, texte_temps_powerup, hud_x + 15, hud_y + 35,
                          makecol(255, 255, 255), -1);
    }
    if (key[KEY_F1])
    {
        // Vider toutes les e->bulles pour declencher la victoire du p->niveau
        int k;
        for (k = 0; k < p->taille_bulles_actuelle; k++)
            e->bulles[k].actif = 0;
    }
    if (key[KEY_F2])
    {
        p->niveau = 5;
        int k;
        for (k = 0; k < p->taille_bulles_actuelle; k++)
            e->bulles[k].actif = 0;
    }

    return nouvel_etat;
}