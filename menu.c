//
// Created by xumer on 27/04/2026.
//
#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "graphique.h"

int afficher_regles(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x)
{
    int retour_au_menu = 0;

    // Titre
    textout_centre_ex(buffer, font, "REGLES DU JEU",
                      ecran_x/2, 80, makecol(255,255,0), -1);

    // Les regles, ligne par ligne
    textout_centre_ex(buffer, font, "- Detruisez toutes les bulles pour passer au niveau suivant",
                      ecran_x/2, 150, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Les grosses bulles se divisent en 2 petites",
                      ecran_x/2, 180, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Si une bulle vous touche, c'est perdu !",
                      ecran_x/2, 210, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Utilisez les fleches pour vous deplacer",
                      ecran_x/2, 260, makecol(200,200,255), -1);
    textout_centre_ex(buffer, font, "- Maintenez ESPACE pour tirer",
                      ecran_x/2, 290, makecol(200,200,255), -1);
    textout_centre_ex(buffer, font, "- Les petites bulles rapportent plus de points !",
                      ecran_x/2, 320, makecol(200,200,255), -1);

    // Gerer le bouton RETOUR
    bouton_retour->survol = souris_dans_bouton(bouton_retour);
    dessiner_bouton(buffer, bouton_retour);

    // Clic sur retour
    if ((mouse_b & 1) && !(*clic_presse))
    {
        *clic_presse = 1;
        if (bouton_retour->survol)
        {
            retour_au_menu = 1;
        }
    }
    if (!(mouse_b & 1)) *clic_presse = 0;

    return retour_au_menu;
}

int afficher_lord(BITMAP *buffer, Bouton *bouton_retour,
                    int *clic_presse, int ecran_x )
{
    int retour_au_menu = 0;

    // Titre
    textout_centre_ex(buffer, font, "HISTOIRE DU JEU",
                      ecran_x/2, 80, makecol(255,255,0), -1);

    // Les regles, ligne par ligne
    textout_centre_ex(buffer, font, "- Detruisez toutes les bulles pour passer au niveau suivant",
                      ecran_x/2, 150, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Les grosses bulles se divisent en 2 petites",
                      ecran_x/2, 180, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Si une bulle vous touche, c'est perdu !",
                      ecran_x/2, 210, makecol(255,255,255), -1);
    textout_centre_ex(buffer, font, "- Utilisez les fleches pour vous deplacer",
                      ecran_x/2, 260, makecol(200,200,255), -1);
    textout_centre_ex(buffer, font, "- Maintenez ESPACE pour tirer",
                      ecran_x/2, 290, makecol(200,200,255), -1);
    textout_centre_ex(buffer, font, "- Les petites bulles rapportent plus de points !",
                      ecran_x/2, 320, makecol(200,200,255), -1);

    // Gerer le bouton RETOUR
    bouton_retour->survol = souris_dans_bouton(bouton_retour);
    dessiner_bouton(buffer, bouton_retour);

    // Clic sur retour
    if ((mouse_b & 1) && !(*clic_presse))
    {
        *clic_presse = 1;
        if (bouton_retour->survol)
        {
            retour_au_menu = 1;
        }
    }
    if (!(mouse_b & 1)) *clic_presse = 0;

    return retour_au_menu;
}


int afficher_game_over(BITMAP *buffer, int score, int ecran_x)
{
    textout_centre_ex(buffer, font, "GAME OVER",
                      ecran_x/2, 200, makecol(255,50,50), -1);

    char texte[50];
    sprintf(texte, "Score final : %d", score);
    textout_centre_ex(buffer, font, texte,
                      ecran_x/2, 250, makecol(255,255,255), -1);

    textout_centre_ex(buffer, font, "Appuyez sur ENTREE pour rejouer",
                      ecran_x/2, 350, makecol(255,255,255), -1);

    if (key[KEY_ENTER]) return 1;
    if (key[KEY_ESC]) return -1;
    return 0;
}

int afficher_victoire(BITMAP *buffer, int score, const char *pseudo,
                       int ecran_x, int ecran_y)
{
    static int frame_animation = 0;
    frame_animation++;

    // Cercles colores qui pulsent
    int j;
    for (j = 0; j < 30; j++)
    {
        int taille = 3 + (frame_animation + j * 7) % 10;
        int px_etoile = (j * 137) % ecran_x;
        int py_etoile = (j * 89) % ecran_y;
        int couleur_etoile = makecol(
            100 + (j * 30) % 156,
            100 + (j * 50) % 156,
            100 + (j * 70) % 156
        );
        circlefill(buffer, px_etoile, py_etoile, taille, couleur_etoile);
    }

    // Titre VICTOIRE
    textout_centre_ex(buffer, font, "VICTOIRE !",
                      ecran_x/2, 150, makecol(255, 255, 0), -1);

    textout_centre_ex(buffer, font, "Vous avez vaincu le BOSS !",
                      ecran_x/2, 200, makecol(0, 255, 100), -1);

    char texte_score[100];
    sprintf(texte_score, "Score final : %d", score);
    textout_centre_ex(buffer, font, texte_score,
                      ecran_x/2, 280, makecol(255, 255, 255), -1);

    char texte_pseudo[100];
    sprintf(texte_pseudo, "Felicitations, %s !", pseudo);
    textout_centre_ex(buffer, font, texte_pseudo,
                      ecran_x/2, 320, makecol(255, 255, 255), -1);

    textout_centre_ex(buffer, font, "Appuyez sur ENTREE pour retourner au menu",
                      ecran_x/2, 450, makecol(200, 200, 200), -1);

    if (key[KEY_ENTER]) return 1;
    if (key[KEY_ESC]) return -1;
    return 0;
}

int afficher_pseudo(BITMAP *buffer, char pseudo[], int ecran_x)
{
    int retour = 0;

    // Titre
    textout_centre_ex(buffer, font, "ENTREZ VOTRE PSEUDO",
                      ecran_x/2, 200, makecol(255,255,0), -1);

    // Cadre pour la saisie
    rect(buffer, ecran_x/2 - 150, 280, ecran_x/2 + 150, 320,
         makecol(255,255,255));

    // Afficher le pseudo en cours de saisie
    textout_centre_ex(buffer, font, pseudo,
                      ecran_x/2, 295, makecol(255,255,255), -1);

    // Instructions
    textout_centre_ex(buffer, font, "Tapez votre nom puis appuyez sur ENTREE",
                      ecran_x/2, 380, makecol(200,200,200), -1);
    textout_centre_ex(buffer, font, "(Backspace pour effacer)",
                      ecran_x/2, 410, makecol(200,200,200), -1);

    // Lire le clavier
    if (keypressed())
    {
        int touche = readkey();
        int caractere = touche & 0xFF;
        int code = touche >> 8;

        if (code == KEY_ENTER)
        {
            if (strlen(pseudo) > 0)
            {
                retour = 1;
            }
        }
        else if (code == KEY_BACKSPACE)
        {
            int longueur = strlen(pseudo);
            if (longueur > 0)
            {
                pseudo[longueur - 1] = '\0';
            }
        }
        else if (code == KEY_ESC)
        {
            retour = -1;
        }
        else if (caractere >= 32 && caractere <= 126)
        {
            int longueur = strlen(pseudo);
            if (longueur < 20)
            {
                pseudo[longueur] = caractere;
                pseudo[longueur + 1] = '\0';
            }
        }
    }

    // Curseur clignotant
    if ((retrace_count / 30) % 2 == 0)
    {
        int largeur_pseudo = text_length(font, pseudo);
        line(buffer,
             ecran_x/2 + largeur_pseudo/2 + 2, 285,
             ecran_x/2 + largeur_pseudo/2 + 2, 315,
             makecol(255,255,255));
    }

    return retour;
}

int afficher_menu(BITMAP *buffer,
                  Bouton *bouton_jouer,
                  Bouton *bouton_reprendre,
                  Bouton *bouton_regles,
                  Bouton *bouton_quitter,
                  int *clic_presse,
                  int ecran_x)
{
    int resultat = MENU_RIEN;

    // Survol
    bouton_jouer->survol = souris_dans_bouton(bouton_jouer);
    bouton_reprendre->survol = souris_dans_bouton(bouton_reprendre);
    bouton_regles->survol = souris_dans_bouton(bouton_regles);
    bouton_quitter->survol = souris_dans_bouton(bouton_quitter);


    // Titre
    textout_centre_ex(buffer, font, "SUPER BULLES",
                      ecran_x/2, 150, makecol(255,255,0), -1);

    // Dessin des boutons
    dessiner_bouton(buffer, bouton_jouer);
    dessiner_bouton(buffer, bouton_reprendre);
    dessiner_bouton(buffer, bouton_regles);
    dessiner_bouton(buffer, bouton_quitter);



    // Clics (avec protection anti-maintien)
    if ((mouse_b & 1) && !(*clic_presse))
    {
        *clic_presse = 1;
        if (bouton_jouer->survol)     resultat = MENU_JOUER;
        if (bouton_reprendre->survol) resultat = MENU_REPRENDRE;
        if (bouton_regles->survol)    resultat = MENU_REGLES;
        if (bouton_quitter->survol)   resultat = MENU_QUITTER;
    }
    if (!(mouse_b & 1)) *clic_presse = 0;

    if (key[KEY_ESC]) resultat = MENU_QUITTER;

    return resultat;
}
int afficher_reprendre(BITMAP *buffer,
                       Sauvegarde sauvegardes[],
                       int nb_sauvegardes,
                       Bouton *bouton_retour,
                       int *clic_presse,
                       int ecran_x)
{
    int resultat = REPRENDRE_RIEN;

    // Titre
    textout_centre_ex(buffer, font, "CHOISIR UNE PARTIE",
                      ecran_x/2, 80, makecol(255,255,0), -1);

    // Afficher chaque sauvegarde comme un bouton
    int j;
    for (j = 0; j < nb_sauvegardes; j++)
    {
        char texte[100];
        sprintf(texte, "%s - Score: %d - Niveau: %d",
                sauvegardes[j].pseudo,
                sauvegardes[j].score,
                sauvegardes[j].niveau);

        int by = 150 + j * 40;
        int bx = ecran_x/2 - 200;

        // Verifier le survol
        int survol = (mouse_x >= bx && mouse_x <= bx + 400 &&
                      mouse_y >= by && mouse_y <= by + 30);

        // Couleur selon survol
        int couleur_fond = survol ? makecol(80,80,180) : makecol(30,30,80);
        rectfill(buffer, bx, by, bx + 400, by + 30, couleur_fond);
        rect(buffer, bx, by, bx + 400, by + 30, makecol(255,255,255));

        textout_centre_ex(buffer, font, texte,
                          ecran_x/2, by + 10, makecol(255,255,255), -1);

        // Clic = on note l'index choisi
        if (survol && (mouse_b & 1) && !(*clic_presse))
        {
            *clic_presse = 1;
            resultat = j;
        }
    }

    // Si pas de sauvegarde
    if (nb_sauvegardes == 0)
    {
        textout_centre_ex(buffer, font, "Aucune sauvegarde trouvee",
                          ecran_x/2, 250, makecol(255,100,100), -1);
    }

    // Bouton retour
    bouton_retour->survol = souris_dans_bouton(bouton_retour);
    dessiner_bouton(buffer, bouton_retour);
    if (bouton_retour->survol && (mouse_b & 1) && !(*clic_presse))
    {
        *clic_presse = 1;
        resultat = REPRENDRE_RETOUR;
    }

    if (!(mouse_b & 1)) *clic_presse = 0;

    return resultat;
}