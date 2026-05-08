#include <stdio.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "sauvegarde/sauvegarde.h"
#include "entites.h"
#include "graphique/graphique.h"
#include "physique/physique.h"
#include "jeu/jeu.h"
#include "boss/boss.h"
#include "powerup/powerup.h"
#include "eclair/eclair.h"
#include "menu/menu.h"
#include "assets/assets.h"
#include "partie/partie.h"
#include "constantes.h"
#include "etat_boss/etat_boss.h"
#include "etat_jeu/etat_jeu.h"


#define ERREUR(msg){\
    set_gfx_mode(GFX_TEXT,0,0,0,0);\
    allegro_message("err %s\nligne %d\nfile %s\n",msg,__LINE__,__FILE__);\
    allegro_exit();\
    return 1;\
}




int main(void)
{
    int fin = 0;
    int couleur;
    int etat = ETAT_MENU;
    int clic_presse = 0;
    float Gravite = 0.25;
    int anim_menu_compteur = 0;
    int anim_menu_frame = 0;

    Partie partie;
    Entites entites;
    Animations animations;

    init_partie(&partie);
    init_entites(&entites, partie.taille_bulles_actuelle);
    init_animations(&animations);

    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    show_mouse(screen);
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    srand(time(NULL));
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, Ecran_X, Ecran_Y, 0, 0) != 0)
        ERREUR(allegro_error);

    // Initialisation du joueur
    entites.joueur.x = 50;
    entites.joueur.y = 640;
    entites.joueur.largeur = 64;
    entites.joueur.hauteur = 96;
    entites.joueur.vitesse = 15;
    couleur = makecol(255, 100, 10);


    int i;

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    Assets assets = charger_assets();
    SAMPLE *musique_fond     =load_sample("ressources/musique_fond.wav");
    SAMPLE *musique_jeu      = load_sample("ressources/musique_jeu.wav");
    SAMPLE *musique_boss     = load_sample("ressources/musique_boss.wav");
    SAMPLE *musique_victoire = load_sample("ressources/musique_victoire.wav");

    if (!musique_fond || !musique_jeu || !musique_boss || !musique_victoire)
    {
        allegro_message("Probleme de chargement des musiques");
    }

    SAMPLE *musique_actuelle = NULL;



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
// bonjour
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

    // Boutons du menu pause
    Bouton bouton_pause_reprendre = {
        SCREEN_W / 2 - 100, SCREEN_H / 2 - 60,
        200, 50, "REPRENDRE", 0
    };
    Bouton bouton_pause_menu = {
        SCREEN_W / 2 - 100, SCREEN_H / 2 + 10,
        200, 50, "MENU PRINCIPAL", 0
    };
    Bouton bouton_pause_quitter = {
        SCREEN_W / 2 - 100, SCREEN_H / 2 + 80,
        200, 50, "QUITTER", 0
    };

show_mouse(screen);
    while (!fin)
    {
        SAMPLE *musique_voulue = NULL;
        if (etat == ETAT_MENU || etat == ETAT_PSEUDO ||
            etat == ETAT_REPRENDRE || etat == ETAT_REGLES)
            musique_voulue = musique_fond;
        else if (etat == ETAT_JEU || etat == ETAT_PAUSE)
            musique_voulue = musique_jeu;
        else if (etat == ETAT_BOSS)
            musique_voulue = musique_boss;
        else if (etat == ETAT_VICTOIRE)
            musique_voulue = musique_victoire;
        // ETAT_GAME_OVER : pas de musique (silence)

        if (musique_voulue != musique_actuelle)
        {
            if (musique_actuelle != NULL) stop_sample(musique_actuelle);
            if (musique_voulue != NULL)   play_sample(musique_voulue, 150, 128, 1000, 1);
            musique_actuelle = musique_voulue;
        }
        if (etat != ETAT_PAUSE)
        {
            clear(buffer);
        }


        if (etat == ETAT_MENU)
        {
            // menu d'acceuil
            blit(assets.menu_fond, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

            anim_menu_compteur++;
            if (anim_menu_compteur >= DUREE_FRAME_MENU)
            {
                anim_menu_compteur = 0;
                anim_menu_frame = (anim_menu_frame + 1) % 10;
            }

            int choix = afficher_menu(buffer,
                                      &bouton_jouer, &bouton_reprendre,
                                      &bouton_regles, &bouton_quitter,
                                      &clic_presse, SCREEN_W);

            if (choix == MENU_JOUER)
            {
                etat = ETAT_PSEUDO;
                partie.pseudo[0] = '\0';
                partie.score = 0;
                partie.niveau = 1;
                partie.niveau_gagner = 0;
                partie.timer_niveau = 0;
                partie.bouclier_actif = 0;
                partie.tir_double_timer = 0;
                init_powerups(entites.powerups, MAX_POWERUPS);
                partie.decompte_initial = 240;   // 4 secondes de decompte

                for (i = 0; i < partie.taille_bulles_actuelle; i++) entites.bulles[i].actif = 0;
                for (i = 0; i < MAX_TIRS; i++) entites.tirs[i].actif = 0;
                for (i = 0; i < MAX_ECLAIRS; i++) entites.eclairs[i].actif = 0;

                entites.joueur.x = 50;
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
            int resultat = afficher_pseudo(buffer, partie.pseudo, SCREEN_W);

            if (resultat == 1)
            {
                etat = ETAT_JEU;
                sauvegarder_partie(partie.pseudo, 0, 1);
                entites.bulles = reallouer_bulles(entites.bulles, &partie.taille_bulles_actuelle, partie.niveau);
                creer_bulles_niveau(entites.bulles, partie.taille_bulles_actuelle, partie.niveau, SCREEN_W);
                partie.temps_restant = (50 + partie.niveau * 10) * 60;
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
                // Le entites.joueur a choisi la sauvegarde d'index 'resultat'
                strcpy(partie.pseudo, sauvegardes_reprise[resultat].pseudo);
                partie.score = sauvegardes_reprise[resultat].score;
                partie.niveau = sauvegardes_reprise[resultat].niveau;

                // Reset jeu
                partie.niveau_gagner = 0;
                partie.timer_niveau = 0;
                partie.bouclier_actif = 0;
                partie.tir_double_timer = 0;
                init_powerups(entites.powerups, MAX_POWERUPS);

                for (i = 0; i < MAX_TIRS; i++) entites.tirs[i].actif = 0;
                for (i = 0; i < MAX_ECLAIRS; i++) entites.eclairs[i].actif = 0;

                entites.bulles = reallouer_bulles(entites.bulles, &partie.taille_bulles_actuelle, partie.niveau);
                creer_bulles_niveau(entites.bulles, partie.taille_bulles_actuelle, partie.niveau, SCREEN_W);
                partie.temps_restant = (50 + partie.niveau * 10) * 60;

                entites.joueur.x = SCREEN_W / 2 - entites.joueur.largeur / 2;
                etat = ETAT_JEU;
                charge = 0;
                partie.decompte_initial = 240;   // 4 secondes de decompte
            }
        }
        else if (etat == ETAT_JEU)
        {
            etat = gerer_etat_jeu(buffer, &assets, &partie, &entites, &animations,
                                  couleur, Gravite, &fin);
        }
        else if (etat == ETAT_BOSS)
        {
            etat = gerer_etat_boss(buffer, &assets, &partie, &entites, &animations,
                                   couleur, Gravite, &fin);
        }
        else if (etat == ETAT_PAUSE)
        {
            int choix = afficher_pause(buffer,
                                        &bouton_pause_reprendre,
                                        &bouton_pause_menu,
                                        &bouton_pause_quitter,
                                        &clic_presse,
                                        SCREEN_W, SCREEN_H);

            if (choix == 1)         // REPRENDRE
            {
                etat = partie.etat_avant_pause;
            }
            else if (choix == 2)    // MENU PRINCIPAL
            {
                sauvegarder_partie(partie.pseudo, partie.score, partie.niveau);
                etat = ETAT_MENU;
            }
            else if (choix == 3)    // QUITTER
            {
                fin = 1;
            }
        }
        else if (etat == ETAT_GAME_OVER)
        {
            int resultat = afficher_game_over(buffer, partie.score, SCREEN_W);
            if (resultat == 1) etat = ETAT_MENU;
            if (resultat == -1) fin = 1;
        }
        else if (etat == ETAT_VICTOIRE)
        {
            int resultat = afficher_victoire(buffer, partie.score, partie.pseudo, SCREEN_W, SCREEN_H);
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
    liberer_entites(&entites);
    liberer_assets(&assets);
    destroy_sample(musique_fond);
    destroy_sample(musique_jeu);
    destroy_sample(musique_boss);
    destroy_sample(musique_victoire);





    exit(EXIT_SUCCESS);
}
END_OF_MAIN();