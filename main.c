#include <stdio.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "sauvegarde.h"
#include "entites.h"
#include "graphique.h"
#include "physique.h"
#include "jeu.h"
#include "boss.h"
#include "powerup.h"
#include "eclair.h"
#include "menu.h"


#define ERREUR(msg){\
    set_gfx_mode(GFX_TEXT,0,0,0,0);\
    allegro_message("err %s\nligne %d\nfile %s\n",msg,__LINE__,__FILE__);\
    allegro_exit();\
    return 1;\
}

#define Ecran_X 1280
#define Ecran_Y 720
#define Max_Tire 1000
#define Max_Bulle 100
#define Max_Eclair 100
#define ETAT_MENU       0
#define ETAT_JEU        1
#define ETAT_GAME_OVER  2
#define ETAT_REGLES     3
#define ETAT_PSEUDO     4
#define ETAT_REPRENDRE  5
#define ETAT_BOSS       6
#define ETAT_VICTOIRE   7
#define Max_Projectile_Boss 20
#define DUREE_FRAME_BOSS 6        // UFO : ~10 fps
#define DUREE_FRAME_PROJ_BOSS 5   // projectile : ~12 fps
#define DUREE_FRAME_JOUEUR 5     // ~12 fps



// Reallouer le tableau de bulles selon le niveau
Bulle *reallouer_bulles(Bulle *bulles, int *taille_actuelle, int nouveau_niveau)
{
    int nouvelle_taille = nouveau_niveau * 15;   // 15 bulles max par grosse bulle initiale

    // Si la nouvelle taille est plus petite, ce n'est pas grave (realloc gere)
    Bulle *nouveau = realloc(bulles, nouvelle_taille * sizeof(Bulle));
    if (nouveau == NULL)
    {
        // Echec : on garde l'ancien et on continue
        return bulles;
    }

    // Initialiser les nouvelles cases (au cas ou la taille augmente)
    int i;
    for (i = *taille_actuelle; i < nouvelle_taille; i++)
        nouveau[i].actif = 0;

    *taille_actuelle = nouvelle_taille;
    return nouveau;
}

// === SYSTEME D'EXPLOSIONS ===
#define MAX_EXPLOSIONS 32
#define DUREE_FRAME_EXPLOSION 4   // chaque frame dure 4 ticks (~13 fps anim)

typedef struct {
    int actif;
    float x, y;
    int taille;       // 0-3 pour bulle, 4 pour boss
    int frame;        // 0 a 4
    int compteur;     // increments chaque tick, change de frame quand atteint DUREE_FRAME
} Explosion;

void declencher_explosion(Explosion explosions[], int max, float x, float y, int taille)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (!explosions[i].actif)
        {
            explosions[i].actif = 1;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].taille = taille;
            explosions[i].frame = 0;
            explosions[i].compteur = 0;
            return;
        }
    }
    // Si pas de slot libre, l'explosion est juste pas affichee (pas grave)
}

int main(void)
{
    int fin = 0;
    int couleur;
    int taille_bulles_actuelle = 15;   // Niveau 1 = 15 bulles max
    Bulle *bulles = malloc(taille_bulles_actuelle * sizeof(Bulle));
    if (bulles == NULL) ERREUR("malloc bulles failed");
    Tir *tirs = malloc(Max_Tire * sizeof(Tir));
    if (tirs == NULL) ERREUR("malloc tirs failed");
    Powerup *powerups = malloc(MAX_POWERUPS * sizeof(Powerup));
    if (powerups == NULL) ERREUR("malloc powerups failed");

    Eclair *eclairs = malloc(Max_Eclair * sizeof(Eclair));
    if (eclairs == NULL) ERREUR("malloc eclairs failed");

    Tir *projectiles_boss = malloc(Max_Projectile_Boss * sizeof(Tir));
    if (projectiles_boss == NULL) ERREUR("malloc projectiles_boss failed");
    Joueur joueur;                  // toutes les infos du joueur
    float Gravite = 0.15;
    int score = 0;
    int niveau = 1;
    int niveau_gagner = 0;
    int timer_niveau = 0;
    int etat = ETAT_MENU;
    int clic_presse = 0;
    char pseudo[50] = "";
    int temps_restant = 60 * 60;     // en "ticks", 60 ticks = 1 seconde
    Boss boss;
    int bouclier_actif = 0;          // 0 = pas de bouclier, 1 = bouclier
    int tir_double_timer = 0;        // > 0 = tir double actif, en frames
    int decompte_initial = 0;   // Frames restantes du decompte 3-2-1
    Explosion explosions[MAX_EXPLOSIONS];
    int e_idx;
    for (e_idx = 0; e_idx < MAX_EXPLOSIONS; e_idx++) explosions[e_idx].actif = 0;
    // Compteurs pour les animations
    int anim_boss_compteur = 0;
    int anim_boss_frame = 0;       // 0-6 (7 frames UFO)
    int anim_proj_boss_compteur = 0;
    int anim_proj_boss_frame = 0;  // 0-3 (4 frames orbe)
    // === ANIMATION DU JOUEUR ===
    int joueur_direction = 1;        // 1 = droite, -1 = gauche
    int joueur_en_marche = 0;        // 0 = idle, 1 = en train de courir
    int anim_joueur_compteur = 0;
    int anim_joueur_frame = 0;       // 0-3 pour la course


    allegro_init();
    install_keyboard();
    install_mouse();
    show_mouse(screen);
    set_color_depth(32);
    srand(time(NULL));
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, Ecran_X, Ecran_Y, 0, 0) != 0)
        ERREUR(allegro_error);

    // Initialisation du joueur
    joueur.x = 50;
    joueur.y = 640;
    joueur.largeur = 64;
    joueur.hauteur = 96;
    joueur.vitesse = 15;
    couleur = makecol(255, 100, 10);

    // Initialisation des tableaux
    init_powerups(powerups, MAX_POWERUPS);
    int i;
    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
    int tir_cooldown = 0;
    for (i = 0; i < taille_bulles_actuelle; i++) bulles[i].actif = 0;
    for (i = 0; i < Max_Projectile_Boss; i++) projectiles_boss[i].actif = 0;
    for (i = 0; i < Max_Eclair; i++) eclairs[i].actif = 0;

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fonds[6];   // 5 fonds de niveau + 1 fond boss

    // Charger les fonds d'ecran
    fonds[0] = load_bitmap("ressources/fond_niveau1.bmp", NULL);
    fonds[1] = load_bitmap("ressources/fond_niveau2.bmp", NULL);
    fonds[2] = load_bitmap("ressources/fond_niveau3.bmp", NULL);
    fonds[3] = load_bitmap("ressources/fond_niveau4.bmp", NULL);
    fonds[4] = load_bitmap("ressources/fond_niveau5.bmp", NULL);
    fonds[5] = load_bitmap("ressources/fond_niveau6.bmp", NULL);

    // Verifier que tous les fonds ont ete charges
    const char *noms_fonds[6] = {
        "fond_niveau1.bmp", "fond_niveau2.bmp", "fond_niveau3.bmp",
        "fond_niveau4.bmp", "fond_niveau5.bmp", "fond_boss.bmp"
    };
    int f;
    for (f = 0; f < 6; f++)
    {
        if (fonds[f] == NULL)
        {
            char message[100];
            sprintf(message, "Impossible de charger %s", noms_fonds[f]);
            ERREUR(message);
        }
    }

    // Charger les sprites d'asteroides (4 tailles + boss)
    // === SPRITES STATIQUES DES ASTEROIDES (4 tailles) ===
    // Convention : taille 0 = mini (le plus petit), taille 3 = gros (le plus gros)
    BITMAP *asteroides[4];
    asteroides[0] = load_bitmap("ressources/asteroide_taille4.bmp", NULL);  // 18x18 mini
    asteroides[1] = load_bitmap("ressources/asteroide_taille3.bmp", NULL);  // 30x30 petit
    asteroides[2] = load_bitmap("ressources/asteroide_taille2.bmp", NULL);  // 50x50 moyen
    asteroides[3] = load_bitmap("ressources/asteroide_taille1.bmp", NULL);  // 80x80 gros

    const char *noms_asteroides[4] = {
        "asteroide_taille4.bmp", "asteroide_taille3.bmp",
        "asteroide_taille2.bmp", "asteroide_taille1.bmp"
    };
    int a;
    for (a = 0; a < 4; a++)
    {
        if (asteroides[a] == NULL)
        {
            char message[100];
            sprintf(message, "Impossible de charger %s", noms_asteroides[a]);
            ERREUR(message);
        }
    }

    // === SPRITES ANIMES DU BOSS (7 frames) ===
    BITMAP *boss_frames[7];
    int bf;
    for (bf = 0; bf < 7; bf++)
    {
        char nom[100];
        sprintf(nom, "ressources/boss_frame%d.bmp", bf);
        boss_frames[bf] = load_bitmap(nom, NULL);
        if (boss_frames[bf] == NULL)
        {
            char msg[150];
            sprintf(msg, "Impossible de charger %s", nom);
            ERREUR(msg);
        }
    }

    // === SPRITES DU JOUEUR (1 idle + 4 run) ===
    BITMAP *joueur_idle = load_bitmap("ressources/joueur_idle.bmp", NULL);
    if (joueur_idle == NULL) ERREUR("Impossible de charger joueur_idle.bmp");

    BITMAP *joueur_run[4];
    int jr;
    for (jr = 0; jr < 4; jr++)
    {
        char nom[100];
        sprintf(nom, "ressources/joueur_run%d.bmp", jr);
        joueur_run[jr] = load_bitmap(nom, NULL);
        if (joueur_run[jr] == NULL)
        {
            char msg[150];
            sprintf(msg, "Impossible de charger %s", nom);
            ERREUR(msg);
        }
    }

    // === FRAMES D'EXPLOSION (4 tailles + boss, 5 frames chacune) ===
    // explosions_frames[taille][frame] où taille 0=mini, 1=petit, 2=moyen, 3=gros, 4=boss
    BITMAP *explosions_frames[5][5];
    const char *suffixes_taille[5] = {"mini", "petit", "moyen", "gros", "boss"};
    int t ;
    for (t = 0; t < 5; t++)
    {
        for (f = 0; f < 5; f++)
        {
            char nom_fichier[100];
            sprintf(nom_fichier, "ressources/explosion_%s_frame%d.bmp", suffixes_taille[t], f);
            explosions_frames[t][f] = load_bitmap(nom_fichier, NULL);
            if (explosions_frames[t][f] == NULL)
            {
                char message[150];
                sprintf(message, "Impossible de charger %s", nom_fichier);
                ERREUR(message);
            }
        }
    }

    // === SPRITE TIR DU JOUEUR ===
    BITMAP *sprite_tir = load_bitmap("ressources/tir_simple.bmp", NULL);
    if (sprite_tir == NULL) ERREUR("Impossible de charger tir_simple.bmp");

    // === SPRITES ANIMES DU PROJECTILE BOSS (4 frames) ===
    BITMAP *projectile_boss_frames[4];
    int pbf;
    for (pbf = 0; pbf < 4; pbf++)
    {
        char nom[100];
        sprintf(nom, "ressources/projectile_boss_frame%d.bmp", pbf);
        projectile_boss_frames[pbf] = load_bitmap(nom, NULL);
        if (projectile_boss_frames[pbf] == NULL)
        {
            char msg[150];
            sprintf(msg, "Impossible de charger %s", nom);
            ERREUR(msg);
        }
    }


    // Initialisation des boutons
    Bouton bouton_jouer;
    bouton_jouer.x = SCREEN_W/2 - 100;
    bouton_jouer.y = 300;
    bouton_jouer.largeur = 200;
    bouton_jouer.hauteur = 50;
    strcpy(bouton_jouer.texte, "JOUER");
    bouton_jouer.survol = 0;

    Bouton bouton_reprendre;
    bouton_reprendre.x = SCREEN_W/2 - 100;
    bouton_reprendre.y = 370;
    bouton_reprendre.largeur = 200;
    bouton_reprendre.hauteur = 50;
    strcpy(bouton_reprendre.texte, "REPRENDRE");
    bouton_reprendre.survol = 0;

    Bouton bouton_regles;
    bouton_regles.x = SCREEN_W/2 - 100;
    bouton_regles.y = 440;
    bouton_regles.largeur = 200;
    bouton_regles.hauteur = 50;
    strcpy(bouton_regles.texte, "REGLES");
    bouton_regles.survol = 0;

    Bouton bouton_quitter;
    bouton_quitter.x = SCREEN_W/2 - 100;
    bouton_quitter.y = 510;
    bouton_quitter.largeur = 200;
    bouton_quitter.hauteur = 50;
    strcpy(bouton_quitter.texte, "QUITTER");
    bouton_quitter.survol = 0;

    Bouton bouton_retour;
    bouton_retour.x = SCREEN_W/2 - 100;
    bouton_retour.y = 500;
    bouton_retour.largeur = 200;
    bouton_retour.hauteur = 50;
    strcpy(bouton_retour.texte, "RETOUR");
    bouton_retour.survol = 0;

    while (!fin)
    {
        clear(buffer);
        show_mouse(screen);

        if (etat == ETAT_MENU)
        {
            int choix = afficher_menu(buffer,
                                      &bouton_jouer, &bouton_reprendre,
                                      &bouton_regles, &bouton_quitter,
                                      &clic_presse, SCREEN_W);

            if (choix == MENU_JOUER)
            {
                etat = ETAT_PSEUDO;
                pseudo[0] = '\0';
                score = 0;
                niveau = 1;
                niveau_gagner = 0;
                timer_niveau = 0;
                bouclier_actif = 0;
                tir_double_timer = 0;
                init_powerups(powerups, MAX_POWERUPS);
                decompte_initial = 240;   // 4 secondes de decompte

                for (i = 0; i < taille_bulles_actuelle; i++) bulles[i].actif = 0;
                for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
                for (i = 0; i < Max_Eclair; i++) eclairs[i].actif = 0;

                joueur.x = 50;
            }
            else if (choix == MENU_REPRENDRE)
            {
                etat = ETAT_REPRENDRE;
            }
            else if (choix == MENU_REGLES)
            {
                etat = ETAT_REGLES;
            }
            else if (choix == MENU_QUITTER)
            {
                fin = 1;
            }
        }
        else if (etat == ETAT_PSEUDO)
        {
            int resultat = afficher_pseudo(buffer, pseudo, SCREEN_W);

            if (resultat == 1)
            {
                etat = ETAT_JEU;
                sauvegarder_partie(pseudo, 0, 1);
                bulles = reallouer_bulles(bulles, &taille_bulles_actuelle, niveau);
                creer_bulles_niveau(bulles, taille_bulles_actuelle, niveau, SCREEN_W);
                temps_restant = (50 + niveau * 10) * 60;
            }
            else if (resultat == -1)
            {
                etat = ETAT_MENU;
            }
        }
        else if (etat == ETAT_REPRENDRE)
        {
            static int charge = 0;
            static Sauvegarde sauvegardes_reprise[10];
            static int nb_sauvegardes_reprise = 0;

            if (!charge)
            {
                nb_sauvegardes_reprise = charger_toutes_sauvegardes(sauvegardes_reprise, 10);
                charge = 1;
            }

            int resultat = afficher_reprendre(buffer, sauvegardes_reprise,
                                               nb_sauvegardes_reprise,
                                               &bouton_retour, &clic_presse, SCREEN_W);

            if (resultat == REPRENDRE_RETOUR)
            {
                etat = ETAT_MENU;
                charge = 0;
            }
            else if (resultat >= 0)
            {
                // Le joueur a choisi la sauvegarde d'index 'resultat'
                strcpy(pseudo, sauvegardes_reprise[resultat].pseudo);
                score = sauvegardes_reprise[resultat].score;
                niveau = sauvegardes_reprise[resultat].niveau;

                // Reset jeu
                niveau_gagner = 0;
                timer_niveau = 0;
                bouclier_actif = 0;
                tir_double_timer = 0;
                init_powerups(powerups, MAX_POWERUPS);

                for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
                for (i = 0; i < Max_Eclair; i++) eclairs[i].actif = 0;

                bulles = reallouer_bulles(bulles, &taille_bulles_actuelle, niveau);
                creer_bulles_niveau(bulles, taille_bulles_actuelle, niveau, SCREEN_W);
                temps_restant = (50 + niveau * 10) * 60;

                joueur.x = SCREEN_W / 2 - joueur.largeur / 2;
                etat = ETAT_JEU;
                charge = 0;
                decompte_initial = 240;   // 4 secondes de decompte
            }
        }
        else if (etat == ETAT_JEU)
        {
            // Afficher le fond du niveau (recouvre le clear noir)
            blit(fonds[niveau - 1], buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

            // Mise a jour direction et etat de marche
            joueur_en_marche = 0;  // par defaut, immobile
            // Deplacement du joueur
            if (key[KEY_LEFT])
            {
                joueur.x -= joueur.vitesse;
                joueur_direction = -1;
                joueur_en_marche = 1;
            }
            if (key[KEY_RIGHT])
            {
                joueur.x += joueur.vitesse;
                joueur_direction = 1;
                joueur_en_marche = 1;
            }
            // Avancer l'animation de marche
            if (joueur_en_marche)
            {
                anim_joueur_compteur++;
                if (anim_joueur_compteur >= DUREE_FRAME_JOUEUR)
                {
                    anim_joueur_compteur = 0;
                    anim_joueur_frame = (anim_joueur_frame + 1) % 4;
                }
            }
            else
            {
                anim_joueur_frame = 0;  // reset quand on s'arrete
            }

            if (key[KEY_ESC])   fin = 1;

            if (joueur.x < 0) joueur.x = 0;
            if (joueur.x > SCREEN_W - joueur.largeur) joueur.x = SCREEN_W - joueur.largeur;

            // Pendant le decompte initial : afficher le chiffre et sauter la logique
            if (decompte_initial > 0)
            {
                // Afficher le joueur (deja affiche au-dessus)
                // Choisir le bon sprite (idle ou frame de course)
                BITMAP *sprite_joueur = joueur_en_marche ? joueur_run[anim_joueur_frame] : joueur_idle;

                // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
                int sx = joueur.x + joueur.largeur / 2 - sprite_joueur->w / 2;
                int sy = joueur.y + joueur.hauteur - sprite_joueur->h;

                if (joueur_direction == 1)
                    draw_sprite(buffer, sprite_joueur, sx, sy);
                else
                    draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

                // Afficher les bulles SANS les bouger
                int b;
                for (b = 0; b < taille_bulles_actuelle; b++)
                {
                    if (bulles[b].actif)
                    {
                        // bulles[b].taille va de 0 (plus gros) a 3 (plus petit)
                        BITMAP *sprite = asteroides[bulles[b].taille];
                        draw_sprite(buffer, sprite,
                                    bulles[b].x - sprite->w / 2,
                                    bulles[b].y - sprite->h / 2);
                    }
                }
                // Calculer quel chiffre afficher
                int chiffre = (decompte_initial / 60);   // 240->4, 180->3, 120->2, 60->1
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

                decompte_initial--;

                // Continuer la boucle (skip toute la logique de jeu)
                scare_mouse();
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                unscare_mouse();
                rest(16);
                continue;// saute le reste de la boucle while
            }


            // Affichage joueur (couleur normale ou rouge si tir double)
            int couleur_joueur = couleur;
            if (tir_double_timer > 0) couleur_joueur = makecol(255, 50, 50);

            // Choisir le bon sprite (idle ou frame de course)
            BITMAP *sprite_joueur = joueur_en_marche ? joueur_run[anim_joueur_frame] : joueur_idle;

            // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
            int sx = joueur.x + joueur.largeur / 2 - sprite_joueur->w / 2;
            int sy = joueur.y + joueur.hauteur - sprite_joueur->h;

            if (joueur_direction == 1)
                draw_sprite(buffer, sprite_joueur, sx, sy);
            else
                draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

            // Affichage bouclier autour du joueur
            if (bouclier_actif)
            {
                circle(buffer, joueur.x + joueur.largeur/2, joueur.y + joueur.hauteur/2,
                       40, makecol(50, 150, 255));
                circle(buffer, joueur.x + joueur.largeur/2, joueur.y + joueur.hauteur/2,
                       38, makecol(100, 180, 255));
            }

            // Bulles : physique + affichage + collision joueur
            for (i = 0; i < taille_bulles_actuelle; i++)
            {
                if (bulles[i].actif)
                {
                    deplacer_bulle(&bulles[i], Gravite);
                    rebonds_bulle(&bulles[i], SCREEN_W, SCREEN_H);

                    if (collision_bulle_joueur(&bulles[i],
                                               joueur.x, joueur.y,
                                               joueur.largeur, joueur.hauteur))
                    {
                        if (bouclier_actif)
                        {
                            bouclier_actif = 0;
                            bulles[i].actif = 0;
                        }
                        else
                        {
                            etat = ETAT_GAME_OVER;
                        }
                    }
                    // bulles[i].taille va de 0 (plus gros) a 3 (plus petit)
                    BITMAP *sprite = asteroides[bulles[i].taille];
                    draw_sprite(buffer, sprite,
                                bulles[i].x - sprite->w / 2,
                                bulles[i].y - sprite->h / 2);
                }
            }

            // === MISE A JOUR ET AFFICHAGE DES EXPLOSIONS ===
            int ex;
            for (ex = 0; ex < MAX_EXPLOSIONS; ex++)
            {
                if (explosions[ex].actif)
                {
                    // Dessiner la frame actuelle
                    BITMAP *frame_sprite = explosions_frames[explosions[ex].taille][explosions[ex].frame];
                    draw_sprite(buffer, frame_sprite,
                                explosions[ex].x - frame_sprite->w / 2,
                                explosions[ex].y - frame_sprite->h / 2);

                    // Avancer l'animation
                    explosions[ex].compteur++;
                    if (explosions[ex].compteur >= DUREE_FRAME_EXPLOSION)
                    {
                        explosions[ex].compteur = 0;
                        explosions[ex].frame++;
                        if (explosions[ex].frame >= 5)
                        {
                            explosions[ex].actif = 0;  // animation terminee
                        }
                    }
                }
            }


            // Update et affichage des powerups
            update_powerups(powerups, MAX_POWERUPS, SCREEN_H);
            afficher_powerups(buffer, powerups, MAX_POWERUPS);

            // Ramassage des powerups
            int type_ramasse = ramasser_powerup(powerups, MAX_POWERUPS,
                                                joueur.x, joueur.y,
                                                joueur.largeur, joueur.hauteur);
            if (type_ramasse != -1)
            {
                if (type_ramasse == POWERUP_BOUCLIER)
                {
                    bouclier_actif = 1;
                }
                else if (type_ramasse == POWERUP_TIR_DOUBLE)
                {
                    tir_double_timer += 600;   // 10 secondes
                }
                else if (type_ramasse == POWERUP_BONUS_TEMPS)
                {
                    temps_restant += 20 * 60;   // +20 secondes
                }
            }

            // Eclairs aleatoires des bulles a partir du niveau 3
            if (niveau >= 3)
            {
                for (i = 0; i < taille_bulles_actuelle; i++)
                {
                    if (bulles[i].actif)
                    {
                        if (rand() % 600 == 0)
                        {
                            creer_eclair(eclairs, Max_Eclair, bulles[i].x, bulles[i].y);
                        }
                    }
                }
            }

            // Update et affichage des eclairs
            update_eclairs(eclairs, Max_Eclair, SCREEN_H);
            afficher_eclairs(buffer, eclairs, Max_Eclair);

            // Collision eclair/joueur
            for (i = 0; i < Max_Eclair; i++)
            {
                if (eclairs[i].actif)
                {
                    if (collision_eclair_joueur(&eclairs[i],
                                                 joueur.x, joueur.y,
                                                 joueur.largeur, joueur.hauteur))
                    {
                        if (bouclier_actif)
                        {
                            bouclier_actif = 0;
                            eclairs[i].actif = 0;
                        }
                        else
                        {
                            etat = ETAT_GAME_OVER;
                        }
                    }
                }
            }

            // Cooldowns
            if (tir_cooldown > 0) tir_cooldown--;
            if (tir_double_timer > 0) tir_double_timer--;

            // Auto-tir
            if (key[KEY_SPACE] && tir_cooldown == 0)
            {
                if (tir_double_timer > 0)
                {
                    // TIR DOUBLE : 2 balles espacees
                    int balles_creees = 0;
                    for (i = 0; i < Max_Tire && balles_creees < 2; i++)
                    {
                        if (!tirs[i].actif)
                        {
                            tirs[i].actif = 1;
                            tirs[i].x = joueur.x + joueur.largeur / 2 + (balles_creees == 0 ? -10 : 10);
                            tirs[i].y = joueur.y;
                            balles_creees++;
                        }
                    }
                    tir_cooldown = 5;
                }
                else
                {
                    // TIR NORMAL : 1 balle
                    for (i = 0; i < Max_Tire; i++)
                    {
                        if (!tirs[i].actif)
                        {
                            tirs[i].actif = 1;
                            tirs[i].x = joueur.x + joueur.largeur / 2;
                            tirs[i].y = joueur.y;
                            tir_cooldown = 5;
                            break;
                        }
                    }
                }
            }

            // Tirs : deplacement + collision avec bulles
            for (i = 0; i < Max_Tire; i++)
            {
                if (tirs[i].actif)
                {
                    tirs[i].y -= 7;
                    if (tirs[i].y < 0)
                    {
                        tirs[i].actif = 0;
                    }
                    else
                    {
                        draw_sprite(buffer, sprite_tir, tirs[i].x, tirs[i].y);
                    }

                    int j;
                    for (j = 0; j < taille_bulles_actuelle; j++)
                    {
                        if (bulles[j].actif)
                        {
                            if (collision_tir_bulle(&tirs[i], &bulles[j]))
                            {
                                tirs[i].actif = 0;
                                score += (4 - bulles[j].taille) * 100;
                                declencher_explosion(explosions, MAX_EXPLOSIONS,
                                bulles[j].x, bulles[j].y, bulles[j].taille);
                                diviser_bulle(bulles, taille_bulles_actuelle, j);
                                bulles[j].actif = 0;
                                // 20% de chance de creer un powerup
                                if (rand() % 100 < 20)
                                {
                                    creer_powerup(powerups, MAX_POWERUPS, bulles[j].x, bulles[j].y);
                                }
                                break;
                            }
                        }
                    }
                }
            }

            // Niveau gagne ?
            int bulles_restantes = compter_bulles_actives(bulles, taille_bulles_actuelle);

            if (bulles_restantes == 0 && !niveau_gagner)
            {
                niveau_gagner = 1;
                timer_niveau = 0;
            }

            if (niveau_gagner)
            {
                textout_centre_ex(buffer, font, "NIVEAU GAGNE !",
                                  SCREEN_W / 2, SCREEN_H / 2,
                                  makecol(0, 255, 0), -1);
                timer_niveau++;

                if (timer_niveau > 120)
                {
                    niveau++;
                    decompte_initial = 240;   // 4 secondes de decompte
                    sauvegarder_partie(pseudo, score, niveau);

                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;

                    // Si on arrive au niveau 6, on passe au boss
                    if (niveau == 6)
                    {
                        init_boss(&boss, SCREEN_W);
                        for (i = 0; i < Max_Projectile_Boss; i++) projectiles_boss[i].actif = 0;
                        for (i = 0; i < taille_bulles_actuelle; i++) bulles[i].actif = 0;
                        joueur.x = SCREEN_W / 2 - joueur.largeur / 2;
                        niveau_gagner = 0;
                        timer_niveau = 0;
                        etat = ETAT_BOSS;
                    }
                    else
                    {
                        bulles = reallouer_bulles(bulles, &taille_bulles_actuelle, niveau);
                        creer_bulles_niveau(bulles, taille_bulles_actuelle, niveau, SCREEN_W);
                        joueur.x = SCREEN_W / 2 - joueur.largeur / 2;
                        niveau_gagner = 0;
                        timer_niveau = 0;
                        temps_restant = (50 + niveau * 10) * 60;
                    }
                }
            }

            // Decrementation du timer
            if (!niveau_gagner && temps_restant > 0)
            {
                temps_restant--;

                // Temps epuise = game over
                if (temps_restant == 0)
                {
                    etat = ETAT_GAME_OVER;
                }
            }

            // Affichage score, niveau, timer
            char texte_score[50];
            sprintf(texte_score, "Score : %d", score);
            textout_ex(buffer, font, texte_score, 10, 10, makecol(255, 255, 255), -1);

            char texte_niveau[30];
            sprintf(texte_niveau, "Niveau : %d", niveau);
            textout_ex(buffer, font, texte_niveau, SCREEN_W - 120, 10, makecol(255, 255, 255), -1);

            // Timer avec couleur changeante
            int secondes = temps_restant / 60;
            char texte_temps[30];
            sprintf(texte_temps, "Temps : %d", secondes);

            int couleur_temps;
            if (secondes > 10)      couleur_temps = makecol(255, 255, 255);   // blanc
            else if (secondes > 5)  couleur_temps = makecol(255, 200, 0);     // orange
            else                    couleur_temps = makecol(255, 50, 50);     // rouge

            textout_ex(buffer, font, texte_temps, SCREEN_W / 2 - 40, 10, couleur_temps, -1);

            // HUD des effets actifs (en bas a droite)
            int hud_y = SCREEN_H - 50;

            if (bouclier_actif)
            {
                int hud_x = SCREEN_W - 80;
                rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(50, 150, 255));
                rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
                textout_centre_ex(buffer, font, "B", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);
            }

            if (tir_double_timer > 0)
            {
                int hud_x = SCREEN_W - 40;
                rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 100, 100));
                rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
                textout_centre_ex(buffer, font, "D", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);

                char texte_temps_powerup[10];
                sprintf(texte_temps_powerup, "%d", tir_double_timer / 60);
                textout_centre_ex(buffer, font, texte_temps_powerup, hud_x + 15, hud_y + 35,
                                  makecol(255, 255, 255), -1);
            }
            if (key[KEY_F1])
            {
                // Vider toutes les bulles pour declencher la victoire du niveau
                int k;
                for (k = 0; k < taille_bulles_actuelle; k++)
                    bulles[k].actif = 0;
            }
            if (key[KEY_F2])
            {
                niveau = 5;
                int k;
                for (k = 0; k < taille_bulles_actuelle; k++)
                    bulles[k].actif = 0;
            }
        }
        else if (etat == ETAT_BOSS)
        {
            // Avancer les animations (boss et ses projectiles)
            anim_boss_compteur++;
            if (anim_boss_compteur >= DUREE_FRAME_BOSS)
            {
                anim_boss_compteur = 0;
                anim_boss_frame = (anim_boss_frame + 1) % 7;
            }

            anim_proj_boss_compteur++;
            if (anim_proj_boss_compteur >= DUREE_FRAME_PROJ_BOSS)
            {
                anim_proj_boss_compteur = 0;
                anim_proj_boss_frame = (anim_proj_boss_frame + 1) % 4;
            }

            // Afficher le fond du boss
            blit(fonds[5], buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

            // Deplacement du joueur
            // Mise a jour direction et etat de marche
            joueur_en_marche = 0;  // par defaut, immobile
            // Deplacement du joueur
            if (key[KEY_LEFT])
            {
                joueur.x -= joueur.vitesse;
                joueur_direction = -1;
                joueur_en_marche = 1;
            }
            if (key[KEY_RIGHT])
            {
                joueur.x += joueur.vitesse;
                joueur_direction = 1;
                joueur_en_marche = 1;
            }
            // Avancer l'animation de marche
            if (joueur_en_marche)
            {
                anim_joueur_compteur++;
                if (anim_joueur_compteur >= DUREE_FRAME_JOUEUR)
                {
                    anim_joueur_compteur = 0;
                    anim_joueur_frame = (anim_joueur_frame + 1) % 4;
                }
            }
            else
            {
                anim_joueur_frame = 0;  // reset quand on s'arrete
            }

            if (key[KEY_ESC])   fin = 1;
            if (joueur.x < 0) joueur.x = 0;
            if (joueur.x > SCREEN_W - joueur.largeur) joueur.x = SCREEN_W - joueur.largeur;

            // Pendant le decompte initial : afficher le chiffre et sauter la logique
            if (decompte_initial > 0)
            {
                // Afficher le joueur (deja affiche au-dessus)
                // Choisir le bon sprite (idle ou frame de course)
                BITMAP *sprite_joueur = joueur_en_marche ? joueur_run[anim_joueur_frame] : joueur_idle;

                // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
                int sx = joueur.x + joueur.largeur / 2 - sprite_joueur->w / 2;
                int sy = joueur.y + joueur.hauteur - sprite_joueur->h;

                if (joueur_direction == 1)
                    draw_sprite(buffer, sprite_joueur, sx, sy);
                else
                    draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

                // Affichage du boss
                if (boss.actif)
                {
                    draw_sprite(buffer, boss_frames[anim_boss_frame], boss.x, boss.y);
                }

                // Calculer quel chiffre afficher
                int chiffre = (decompte_initial / 60) ;   // 240->4, 180->3, 120->2, 60->1
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

                decompte_initial--;

                // Continuer la boucle (skip toute la logique de jeu)
                scare_mouse();
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                unscare_mouse();
                rest(16);
                continue;   // saute le reste de la boucle while
            }

            // Update boss
            update_boss(&boss, SCREEN_W);

            // Le boss tire si son timer est a 0
            if (boss.actif && boss.timer_tir == 0)
            {
                boss_tirer(&boss, projectiles_boss, Max_Projectile_Boss);
                boss.timer_tir = 15;
            }

            // Le boss lache une bulle si son timer est a 0
            if (boss.actif && boss.timer_bulle == 0)
            {
                boss_lacher_bulle(&boss, bulles, taille_bulles_actuelle);
                boss.timer_bulle = 15;
            }

            // Physique et affichage des bulles
            for (i = 0; i < taille_bulles_actuelle; i++)
            {
                if (bulles[i].actif)
                {
                    deplacer_bulle(&bulles[i], Gravite);
                    rebonds_bulle(&bulles[i], SCREEN_W, SCREEN_H);

                    // Collision avec joueur
                    if (collision_bulle_joueur(&bulles[i],
                                               joueur.x, joueur.y,
                                               joueur.largeur, joueur.hauteur))
                    {
                        if (bouclier_actif)
                        {
                            bouclier_actif = 0;
                            bulles[i].actif = 0;
                        }
                        else
                        {
                            etat = ETAT_GAME_OVER;
                        }
                    }

                    // bulles[i].taille va de 0 (plus gros) a 3 (plus petit)
                    BITMAP *sprite = asteroides[bulles[i].taille];
                    draw_sprite(buffer, sprite,
                                bulles[i].x - sprite->w / 2,
                                bulles[i].y - sprite->h / 2);
                }
            }
            // === MISE A JOUR ET AFFICHAGE DES EXPLOSIONS (mode boss) ===
            int ex_b;
            for (ex_b = 0; ex_b < MAX_EXPLOSIONS; ex_b++)
            {
                if (explosions[ex_b].actif)
                {
                    BITMAP *frame_sprite = explosions_frames[explosions[ex_b].taille][explosions[ex_b].frame];
                    draw_sprite(buffer, frame_sprite,
                                explosions[ex_b].x - frame_sprite->w / 2,
                                explosions[ex_b].y - frame_sprite->h / 2);

                    explosions[ex_b].compteur++;
                    if (explosions[ex_b].compteur >= DUREE_FRAME_EXPLOSION)
                    {
                        explosions[ex_b].compteur = 0;
                        explosions[ex_b].frame++;
                        if (explosions[ex_b].frame >= 5)
                        {
                            explosions[ex_b].actif = 0;
                        }
                    }
                }
            }

            // Collision tir/bulle
            for (i = 0; i < Max_Tire; i++)
            {
                if (tirs[i].actif)
                {
                    int j;
                    for (j = 0; j < taille_bulles_actuelle; j++)
                    {
                        if (bulles[j].actif)
                        {
                            if (collision_tir_bulle(&tirs[i], &bulles[j]))
                            {
                                tirs[i].actif = 0;
                                declencher_explosion(explosions, MAX_EXPLOSIONS,
                                bulles[j].x, bulles[j].y, bulles[j].taille);
                                bulles[j].actif = 0;   // pas de division en mode boss
                                score += 100;
                                break;
                            }
                        }
                    }
                }
            }

            // Deplacement des projectiles du boss et collision avec joueur
            for (i = 0; i < Max_Projectile_Boss; i++)
            {
                if (projectiles_boss[i].actif)
                {
                    projectiles_boss[i].y += 5;

                    if (projectiles_boss[i].y > SCREEN_H)
                    {
                        projectiles_boss[i].actif = 0;
                    }
                    else
                    {
                        BITMAP *frame_pb = projectile_boss_frames[anim_proj_boss_frame];
                        draw_sprite(buffer, frame_pb,
                                    projectiles_boss[i].x - frame_pb->w / 2,
                                    projectiles_boss[i].y - frame_pb->h / 2);
                    }

                    // Collision avec le joueur
                    if (projectiles_boss[i].x + 8 > joueur.x &&
                        projectiles_boss[i].x - 8 < joueur.x + joueur.largeur &&
                        projectiles_boss[i].y + 8 > joueur.y &&
                        projectiles_boss[i].y - 8 < joueur.y + joueur.hauteur)
                    {
                        if (bouclier_actif)
                        {
                            bouclier_actif = 0;
                            projectiles_boss[i].actif = 0;
                        }
                        else
                        {
                            etat = ETAT_GAME_OVER;
                        }
                    }
                }
            }

            // Cooldown du joueur
            if (tir_cooldown > 0) tir_cooldown--;

            // Tir du joueur
            if (key[KEY_SPACE] && tir_cooldown == 0)
            {
                if (tir_double_timer > 0)
                {
                    int balles_creees = 0;
                    for (i = 0; i < Max_Tire && balles_creees < 2; i++)
                    {
                        if (!tirs[i].actif)
                        {
                            tirs[i].actif = 1;
                            tirs[i].x = joueur.x + joueur.largeur / 2 + (balles_creees == 0 ? -10 : 10);
                            tirs[i].y = joueur.y;
                            balles_creees++;
                        }
                    }
                    tir_cooldown = 5;
                }
                else
                {
                    for (i = 0; i < Max_Tire; i++)
                    {
                        if (!tirs[i].actif)
                        {
                            tirs[i].actif = 1;
                            tirs[i].x = joueur.x + joueur.largeur / 2;
                            tirs[i].y = joueur.y;
                            tir_cooldown = 5;
                            break;
                        }
                    }
                }
            }

            // Deplacement et collision des tirs (avec le boss)
            for (i = 0; i < Max_Tire; i++)
            {
                if (tirs[i].actif)
                {
                    tirs[i].y -= 7;
                    if (tirs[i].y < 0)
                    {
                        tirs[i].actif = 0;
                    }
                    else
                    {
                        draw_sprite(buffer, sprite_tir, tirs[i].x, tirs[i].y);
                    }

                    // Collision tir/boss
                    if (boss.actif && collision_tir_boss(&tirs[i], &boss))
                    {
                        tirs[i].actif = 0;
                        boss.hp--;

                        // Si le boss est mort
                        if (boss.hp <= 0)
                        {
                            boss.actif = 0;
                            declencher_explosion(explosions, MAX_EXPLOSIONS,
                                boss.x + boss.largeur/2, boss.y + boss.hauteur/2, 4);
                            score += 5000;

                            // Nettoyer projectiles et bulles
                            int k;
                            for (k = 0; k < Max_Projectile_Boss; k++) projectiles_boss[k].actif = 0;
                            for (k = 0; k < taille_bulles_actuelle; k++) bulles[k].actif = 0;

                            sauvegarder_partie(pseudo, score, niveau);
                            etat = ETAT_VICTOIRE;
                        }
                    }
                }
            }

            // Affichage joueur (couleur normale ou rouge si tir double)
            int couleur_joueur = couleur;
            if (tir_double_timer > 0) couleur_joueur = makecol(255, 50, 50);

            // Choisir le bon sprite (idle ou frame de course)
            BITMAP *sprite_joueur = joueur_en_marche ? joueur_run[anim_joueur_frame] : joueur_idle;

            // Calcul position : centrer horizontalement, aligner par le bas avec la hitbox
            int sx = joueur.x + joueur.largeur / 2 - sprite_joueur->w / 2;
            int sy = joueur.y + joueur.hauteur - sprite_joueur->h;

            if (joueur_direction == 1)
                draw_sprite(buffer, sprite_joueur, sx, sy);
            else
                draw_sprite_h_flip(buffer, sprite_joueur, sx, sy);

            // Affichage bouclier autour du joueur
            if (bouclier_actif)
            {
                circle(buffer, joueur.x + joueur.largeur/2, joueur.y + joueur.hauteur/2,
                       40, makecol(50, 150, 255));
                circle(buffer, joueur.x + joueur.largeur/2, joueur.y + joueur.hauteur/2,
                       38, makecol(100, 180, 255));
            }

            // Affichage boss
            if (boss.actif)
            {
                int couleur_boss;

                draw_sprite(buffer, boss_frames[anim_boss_frame], boss.x, boss.y);

                // Barre de vie
                int largeur_barre = 200;
                int x_barre = SCREEN_W / 2 - largeur_barre / 2;
                int y_barre = 30;

                rectfill(buffer, x_barre, y_barre,
                         x_barre + largeur_barre, y_barre + 15,
                         makecol(50, 50, 50));

                int largeur_vie = (boss.hp * largeur_barre) / boss.hp_max;
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

            if (bouclier_actif)
            {
                int hud_x = SCREEN_W - 80;
                rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(50, 150, 255));
                rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
                textout_centre_ex(buffer, font, "B", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);
            }

            if (tir_double_timer > 0)
            {
                int hud_x = SCREEN_W - 40;
                rectfill(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 100, 100));
                rect(buffer, hud_x, hud_y, hud_x + 30, hud_y + 30, makecol(255, 255, 255));
                textout_centre_ex(buffer, font, "D", hud_x + 15, hud_y + 11, makecol(255, 255, 255), -1);

                char texte_temps_powerup[10];
                sprintf(texte_temps_powerup, "%d", tir_double_timer / 60);
                textout_centre_ex(buffer, font, texte_temps_powerup, hud_x + 15, hud_y + 35,
                                  makecol(255, 255, 255), -1);
            }
        }
        else if (etat == ETAT_GAME_OVER)
        {
            int resultat = afficher_game_over(buffer, score, SCREEN_W);
            if (resultat == 1) etat = ETAT_MENU;
            if (resultat == -1) fin = 1;
        }
        else if (etat == ETAT_VICTOIRE)
        {
            int resultat = afficher_victoire(buffer, score, pseudo, SCREEN_W, SCREEN_H);
            if (resultat == 1) etat = ETAT_MENU;
            if (resultat == -1) fin = 1;
        }
        else if (etat == ETAT_REGLES)
        {
            if (afficher_regles(buffer, &bouton_retour, &clic_presse, SCREEN_W))
            {
                etat = ETAT_MENU;
            }
        }

        scare_mouse();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        unscare_mouse();
        rest(16);
    }
    // Liberer la memoire allouee dynamiquement
    free(bulles);
    free(tirs);
    free(powerups);
    free(eclairs);
    free(projectiles_boss);

    // Liberer la memoire des fonds
    int b;
    for (b = 0; b < 6; b++)
        destroy_bitmap(fonds[b]);

    destroy_bitmap(buffer);

    for (a = 0; a < 4; a++) destroy_bitmap(asteroides[a]);

    for (bf = 0; bf < 7; bf++) destroy_bitmap(boss_frames[bf]);
    for (pbf = 0; pbf < 4; pbf++) destroy_bitmap(projectile_boss_frames[pbf]);
    destroy_bitmap(sprite_tir);

    for (t = 0; t < 5; t++)
        for (f = 0; f < 5; f++)
            destroy_bitmap(explosions_frames[t][f]);

    destroy_bitmap(joueur_idle);
    for (jr = 0; jr < 4; jr++) destroy_bitmap(joueur_run[jr]);






    exit(EXIT_SUCCESS);
}
END_OF_MAIN();