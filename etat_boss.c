//
// Created by xumer on 29/04/2026.
//
//
// etat_boss.c - Gestion de l'etat ETAT_BOSS (combat de boss)
//

//
// etat_boss.c - Gestion de l'etat ETAT_BOSS (combat de boss)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <allegro.h>
#include "etat_boss.h"
#include "constantes.h"
#include "physique.h"
#include "jeu.h"
#include "boss.h"
#include "powerup.h"
#include "eclair.h"
#include "sauvegarde.h"

int gerer_etat_boss(BITMAP *buffer, Assets *assets,
                    Partie *p, Entites *e, Animations *a,
                    int couleur, float gravite, int *fin)
{
    int nouvel_etat = ETAT_BOSS;

    // Avancer les animations (e->boss et ses projectiles)
    a->anim_boss_compteur++;
    if (a->anim_boss_compteur >= DUREE_FRAME_BOSS)
    {
        a->anim_boss_compteur = 0;
        a->anim_boss_frame = (a->anim_boss_frame + 1) % 7;
    }

    a->anim_proj_boss_compteur++;
    if (a->anim_proj_boss_compteur >= DUREE_FRAME_PROJ_BOSS)
    {
        a->anim_proj_boss_compteur = 0;
        a->anim_proj_boss_frame = (a->anim_proj_boss_frame + 1) % 4;
    }

    // Afficher le fond du e->boss
    blit(assets->fonds[5], buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    // Deplacement du e->joueur
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

        // Affichage du e->boss
        if (e->boss.actif)
        {
            draw_sprite(buffer, assets->boss_frames[a->anim_boss_frame], e->boss.x, e->boss.y);
        }

        // Calculer quel chiffre afficher
        int chiffre = (p->decompte_initial / 60) ;   // 240->4, 180->3, 120->2, 60->1
        char texte_chiffre[10];

        if (chiffre >= 4)
            strcpy(texte_chiffre, "3");
        else if (chiffre == 3)
            strcpy(texte_chiffre, "2");
        else if (chiffre == 2)
            strcpy(texte_chiffre, "1");
        else
            strcpy(texte_chiffre, "GO !");

        // Afficher le chiffre en GROS au centre
        textout_centre_ex(buffer, font, texte_chiffre,
                          SCREEN_W / 2, SCREEN_H / 2,
                          makecol(255, 255, 0), -1);

        // Afficher "BOSS INCOMING !"
        textout_centre_ex(buffer, font, "BOSS INCOMING !",
                          SCREEN_W / 2, 100,
                          makecol(255, 50, 50), -1);

        p->decompte_initial--;

        return nouvel_etat;   // saute le reste de la boucle while
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
            p->score=1000;   // ajoute 1000
            if (e->boss.actif) e->boss.hp -= 5;
        }
    }


    // Update e->boss
    update_boss(&e->boss, SCREEN_W);

    // Le e->boss tire si son timer est a 0
    if (e->boss.actif && e->boss.timer_tir == 0)
    {
        boss_tirer(&e->boss, e->projectiles_boss, MAX_PROJ_BOSS);
        e->boss.timer_tir = 15;
    }

    // Le e->boss lache une bulle si son timer est a 0
    if (e->boss.actif && e->boss.timer_bulle == 0)
    {
        boss_lacher_bulle(&e->boss, e->bulles, p->taille_bulles_actuelle);
        e->boss.timer_bulle = 15;
    }

    // Physique et affichage des e->bulles
    int i;
    for (i = 0; i < p->taille_bulles_actuelle; i++)
    {
        if (e->bulles[i].actif)
        {
            deplacer_bulle(&e->bulles[i], gravite);
            rebonds_bulle(&e->bulles[i], SCREEN_W, SCREEN_H);

            // Collision avec e->joueur
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
    // === MISE A JOUR ET AFFICHAGE DES EXPLOSIONS (mode e->boss) ===
    int ex_b;
    for (ex_b = 0; ex_b < MAX_EXPLOSIONS; ex_b++)
    {
        if (e->explosions[ex_b].actif)
        {
            BITMAP *frame_sprite = assets->explosions_frames[e->explosions[ex_b].taille][e->explosions[ex_b].frame];
            draw_sprite(buffer, frame_sprite,
                        e->explosions[ex_b].x - frame_sprite->w / 2,
                        e->explosions[ex_b].y - frame_sprite->h / 2);

            e->explosions[ex_b].compteur++;
            if (e->explosions[ex_b].compteur >= DUREE_FRAME_EXPLOSION)
            {
                e->explosions[ex_b].compteur = 0;
                e->explosions[ex_b].frame++;
                if (e->explosions[ex_b].frame >= 5)
                {
                    e->explosions[ex_b].actif = 0;
                }
            }
        }
    }

    // Collision tir/bulle
    for (i = 0; i < MAX_TIRS; i++)
    {
        if (e->tirs[i].actif)
        {
            int j;
            for (j = 0; j < p->taille_bulles_actuelle; j++)
            {
                if (e->bulles[j].actif)
                {
                    if (collision_tir_bulle(&e->tirs[i], &e->bulles[j]))
                    {
                        e->tirs[i].actif = 0;
                        declencher_explosion(e->explosions, MAX_EXPLOSIONS,
                        e->bulles[j].x, e->bulles[j].y, e->bulles[j].taille);
                        e->bulles[j].actif = 0;   // pas de division en mode e->boss
                        p->score += 100;
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

    // Deplacement des projectiles du e->boss et collision avec e->joueur
    for (i = 0; i < MAX_PROJ_BOSS; i++)
    {
        if (e->projectiles_boss[i].actif)
        {
            e->projectiles_boss[i].y += 5;

            if (e->projectiles_boss[i].y > SCREEN_H)
            {
                e->projectiles_boss[i].actif = 0;
            }
            else
            {
                BITMAP *frame_pb = assets->projectile_boss_frames[a->anim_proj_boss_frame];
                draw_sprite(buffer, frame_pb,
                            e->projectiles_boss[i].x - frame_pb->w / 2,
                            e->projectiles_boss[i].y - frame_pb->h / 2);
            }

            // Collision avec le e->joueur
            if (e->projectiles_boss[i].x + 8 > e->joueur.x &&
                e->projectiles_boss[i].x - 8 < e->joueur.x + e->joueur.largeur &&
                e->projectiles_boss[i].y + 8 > e->joueur.y &&
                e->projectiles_boss[i].y - 8 < e->joueur.y + e->joueur.hauteur)
            {
                if (p->bouclier_actif)
                {
                    p->bouclier_actif = 0;
                    e->projectiles_boss[i].actif = 0;
                }
                else
                {
                    nouvel_etat = ETAT_GAME_OVER;
                }
            }
        }
    }

    // Cooldown du e->joueur
    if (p->tir_cooldown > 0) p->tir_cooldown--;
    if (p->tir_double_timer > 0) p->tir_double_timer--;

    // Tir du e->joueur
    if (key[KEY_SPACE] && p->tir_cooldown == 0)
    {
        if (p->tir_double_timer > 0)
        {
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

    // Deplacement et collision des e->tirs (avec le e->boss)
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

            // Collision tir/e->boss
            if (e->boss.actif && collision_tir_boss(&e->tirs[i], &e->boss))
            {
                e->tirs[i].actif = 0;
                e->boss.hp--;

                // Si le e->boss est mort
                if (e->boss.hp <= 0)
                {
                    e->boss.actif = 0;
                    declencher_explosion(e->explosions, MAX_EXPLOSIONS,
                        e->boss.x + e->boss.largeur/2, e->boss.y + e->boss.hauteur/2, 4);
                    p->score += 5000;

                    // Nettoyer projectiles et e->bulles
                    int k;
                    for (k = 0; k < MAX_PROJ_BOSS; k++) e->projectiles_boss[k].actif = 0;
                    for (k = 0; k < p->taille_bulles_actuelle; k++) e->bulles[k].actif = 0;

                    sauvegarder_partie(p->pseudo, p->score, p->niveau);
                    nouvel_etat = ETAT_VICTOIRE;
                }
            }
        }
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

    // Affichage e->boss
    if (e->boss.actif)
    {
        int couleur_boss;

        draw_sprite(buffer, assets->boss_frames[a->anim_boss_frame], e->boss.x, e->boss.y);

        // Barre de vie
        int largeur_barre = 200;
        int x_barre = SCREEN_W / 2 - largeur_barre / 2;
        int y_barre = 30;

        rectfill(buffer, x_barre, y_barre,
                 x_barre + largeur_barre, y_barre + 15,
                 makecol(50, 50, 50));

        int largeur_vie = (e->boss.hp * largeur_barre) / e->boss.hp_max;
        rectfill(buffer, x_barre, y_barre,
                 x_barre + largeur_vie, y_barre + 15,
                 makecol(200, 50, 50));

        rect(buffer, x_barre, y_barre,
             x_barre + largeur_barre, y_barre + 15,
             makecol(255, 255, 255));

        textout_centre_ex(buffer, font, "BOSS",
                          SCREEN_W / 2, 10, makecol(255, 255, 0), -1);
    }

    // HUD des effets actifs
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

    return nouvel_etat;
}