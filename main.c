#include <stdio.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "sauvegarde.h"
#include "entites.h"
#include "graphique.h"
#include "physique.h"
#include "jeu.h"

#define ERREUR(msg){\
    set_gfx_mode(GFX_TEXT,0,0,0,0);\
    allegro_message("err %s\nligne %d\nfile %s\n",msg,__LINE__,__FILE__);\
    allegro_exit();\
    return 1;\
}

#define Ecran_X 800
#define Ecran_Y 600
#define Max_Tire 100
#define Max_Bulle 10
#define ETAT_MENU       0
#define ETAT_JEU        1
#define ETAT_GAME_OVER  2
#define ETAT_REGLES     3
#define ETAT_PSEUDO     4
#define ETAT_REPRENDRE  5


int main(void)
{
    int fin=0;
    int x,y;
    int px,py;
    int tx,ty;
    int couleur;
    Bulle bulles[Max_Bulle];
    Tir tirs[Max_Tire];
    Joueur joueur;
    float Gravite= 0.15;
    int score = 0;
    int niveau = 1;
    int niveau_gagner=0;
    int timer_niveau=0;
    int etat = ETAT_MENU;
    int clic_presse=0;
    char pseudo[50]="";
    int dernier_score, dernier_niveau;
    charger_partie(pseudo, &dernier_score, &dernier_niveau);
    int deja_sauvegarde=0;
    int temps_restant= 60*60;     // en "ticks", 60 ticks = 1 seconde
    int duree_niveau;      // duree totale du niveau

    allegro_init();
    install_keyboard();
    install_mouse();
    show_mouse(screen);
    srand(time(NULL));
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, Ecran_X,Ecran_Y, 0, 0)!=0)
        ERREUR(allegro_error);
    tx=50;
    x=50;
    px=10;
    ty=50;
    y=545;
    py=50;
    couleur=makecol(255,100,10);
    int i;
    for ( i = 0 ; i < Max_Tire ; i++ )
        tirs[i].actif=0;
    int tir_cooldown=0;
    for (i = 0; i < Max_Bulle; i++)
        bulles[i].actif = 0;

    BITMAP *buffer = create_bitmap(Ecran_X,Ecran_Y);

    Bouton bouton_jouer;
    bouton_jouer.x = Ecran_X/2 - 100;
    bouton_jouer.y = 300;
    bouton_jouer.largeur = 200;
    bouton_jouer.hauteur = 50;
    strcpy(bouton_jouer.texte, "JOUER");
    bouton_jouer.survol = 0;

    Bouton bouton_reprendre;
    bouton_reprendre.x = Ecran_X/2 - 100;
    bouton_reprendre.y = 370;
    bouton_reprendre.largeur = 200;
    bouton_reprendre.hauteur = 50;
    strcpy(bouton_reprendre.texte, "REPRENDRE");
    bouton_reprendre.survol = 0;

    Bouton bouton_regles;
    bouton_regles.x = Ecran_X/2 - 100;
    bouton_regles.y = 440;
    bouton_regles.largeur = 200;
    bouton_regles.hauteur = 50;
    strcpy(bouton_regles.texte, "REGLES");
    bouton_regles.survol = 0;

    Bouton bouton_quitter;
    bouton_quitter.x = Ecran_X/2 - 100;
    bouton_quitter.y = 510;
    bouton_quitter.largeur = 200;
    bouton_quitter.hauteur = 50;
    strcpy(bouton_quitter.texte, "QUITTER");
    bouton_quitter.survol = 0;

    Bouton bouton_retour;
    bouton_retour.x = Ecran_X/2 - 100;
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
            // Survol
            bouton_jouer.survol = souris_dans_bouton(&bouton_jouer);
            bouton_reprendre.survol = souris_dans_bouton(&bouton_reprendre);
            bouton_regles.survol = souris_dans_bouton(&bouton_regles);
            bouton_quitter.survol = souris_dans_bouton(&bouton_quitter);

            // Titre
            textout_centre_ex(buffer, font, "SUPER BULLES",
                              Ecran_X/2, 150, makecol(255,255,0), -1);

            // Dessin des boutons
            dessiner_bouton(buffer, &bouton_jouer);
            dessiner_bouton(buffer, &bouton_reprendre);
            dessiner_bouton(buffer, &bouton_regles);
            dessiner_bouton(buffer, &bouton_quitter);

            // Clics
            if ((mouse_b & 1) && !clic_presse)
            {
                clic_presse = 1;
                if (bouton_jouer.survol)
                {
                    etat = ETAT_PSEUDO;
                    pseudo[0] = '\0';
                    score = 0;
                    niveau = 1;
                    niveau_gagner = 0;
                    timer_niveau = 0;
                    deja_sauvegarde = 0;
                    for (i = 0; i < Max_Bulle; i++) bulles[i].actif = 0;
                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
                    x = 50;
                }
                if (bouton_quitter.survol) fin = 1;
                if (bouton_reprendre.survol) etat = ETAT_REPRENDRE;
                if (bouton_regles.survol) etat = ETAT_REGLES;
            }
            if (!(mouse_b & 1)) clic_presse = 0;
            if (key[KEY_ESC]) fin = 1;
        }
        else if (etat == ETAT_PSEUDO)
        {
            // Titre
            textout_centre_ex(buffer, font, "ENTREZ VOTRE PSEUDO",
                              Ecran_X/2, 200, makecol(255,255,0), -1);

            rect(buffer, Ecran_X/2 - 150, 280, Ecran_X/2 + 150, 320,
                 makecol(255,255,255));

            textout_centre_ex(buffer, font, pseudo,
                              Ecran_X/2, 295, makecol(255,255,255), -1);

            textout_centre_ex(buffer, font, "Tapez votre nom puis appuyez sur ENTREE",
                              Ecran_X/2, 380, makecol(200,200,200), -1);
            textout_centre_ex(buffer, font, "(Backspace pour effacer)",
                              Ecran_X/2, 410, makecol(200,200,200), -1);

            if (keypressed())
            {
                int touche = readkey();
                int caractere = touche & 0xFF;
                int code = touche >> 8;




                if (code == KEY_ENTER)
                {
                    if (strlen(pseudo) > 0)
                    {
                        etat = ETAT_JEU;
                        sauvegarder_partie(pseudo, 0, 1);
                        creer_bulles_niveau(bulles, Max_Bulle, niveau, Ecran_X);
                        temps_restant=(50 + niveau * 10) *60 ;
                    }
                }
                else if (code == KEY_BACKSPACE)
                {
                    int longueur = strlen(pseudo);
                    if (longueur > 0) pseudo[longueur - 1] = '\0';
                }
                else if (code == KEY_ESC)
                {
                    etat = ETAT_MENU;
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
            if ((retrace_count / 30) % 2 == 0)
            {
                int largeur_pseudo = text_length(font, pseudo);
                line(buffer,
                     Ecran_X/2 + largeur_pseudo/2 + 2, 285,
                     Ecran_X/2 + largeur_pseudo/2 + 2, 315,
                     makecol(255,255,255));
            }

        }
        else if (etat == ETAT_REPRENDRE)
        {
            static int charge = 0;
            static Sauvegarde sauvegardes[10];
            static int nb_sauvegardes = 0;

            if (!charge)
            {
                nb_sauvegardes = charger_toutes_sauvegardes(sauvegardes, 10);
                charge = 1;
            }

            textout_centre_ex(buffer, font, "CHOISIR UNE PARTIE",
                              Ecran_X/2, 80, makecol(255,255,0), -1);

            int j;
            for (j = 0; j < nb_sauvegardes; j++)
            {
                char texte[100];
                sprintf(texte, "%s - Score: %d - Niveau: %d",
                        sauvegardes[j].pseudo,
                        sauvegardes[j].score,
                        sauvegardes[j].niveau);

                int by = 150 + j * 40;
                int bx = Ecran_X/2 - 200;

                int survol = (mouse_x >= bx && mouse_x <= bx + 400 &&
                              mouse_y >= by && mouse_y <= by + 30);

                int couleur_fond = survol ? makecol(80,80,180) : makecol(30,30,80);
                rectfill(buffer, bx, by, bx + 400, by + 30, couleur_fond);
                rect(buffer, bx, by, bx + 400, by + 30, makecol(255,255,255));

                textout_centre_ex(buffer, font, texte,
                                  Ecran_X/2, by + 10, makecol(255,255,255), -1);

                if (survol && (mouse_b & 1) && !clic_presse)
                {
                    clic_presse = 1;
                    strcpy(pseudo, sauvegardes[j].pseudo);
                    score = sauvegardes[j].score;
                    niveau = sauvegardes[j].niveau;

                    creer_bulles_niveau(bulles, Max_Bulle, niveau, Ecran_X);
                    temps_restant = (50 + niveau * 10) *60 ;

                    niveau_gagner = 0;
                    timer_niveau = 0;
                    deja_sauvegarde = 0;
                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;

                    creer_bulles_niveau(bulles, Max_Bulle, niveau, Ecran_X);

                    x = Ecran_X / 2 - tx / 2;
                    etat = ETAT_JEU;
                    charge = 0;
                }
            }

            if (nb_sauvegardes == 0)
            {
                textout_centre_ex(buffer, font, "Aucune sauvegarde trouvee",
                                  Ecran_X/2, 250, makecol(255,100,100), -1);
            }

            bouton_retour.survol = souris_dans_bouton(&bouton_retour);
            dessiner_bouton(buffer, &bouton_retour);
            if (bouton_retour.survol && (mouse_b & 1) && !clic_presse)
            {
                clic_presse = 1;
                etat = ETAT_MENU;
                charge = 0;
            }

            if (!(mouse_b & 1)) clic_presse = 0;
        }
        else if (etat == ETAT_JEU)
        {
            if (key[KEY_LEFT])  x -= px;
            if (key[KEY_RIGHT]) x += px;
            if (key[KEY_ESC])   fin = 1;

            if (x < 0) x = 0;
            if (x > Ecran_X - tx) x = Ecran_X - tx;

            rectfill(buffer, x, y, x+tx, y+ty, couleur);

            // Bulles
            for (i = 0; i < Max_Bulle; i++)
            {
                if (bulles[i].actif)
                {
                    deplacer_bulle(&bulles[i], Gravite);
                    rebonds_bulle(&bulles[i], Ecran_X, Ecran_Y);

                    if (collision_bulle_joueur(&bulles[i], x, y, tx, ty))
                    {
                        etat = ETAT_GAME_OVER;
                    }

                    circlefill(buffer, bulles[i].x, bulles[i].y, bulles[i].rayon, couleur);
                }
            }

            // Cooldown
            if (tir_cooldown > 0) tir_cooldown--;

            // Auto-tir
            if (key[KEY_SPACE] && tir_cooldown == 0)
            {
                for (i = 0; i < Max_Tire; i++)
                {
                    if (!tirs[i].actif)
                    {
                        tirs[i].actif = 1;
                        tirs[i].x = x + tx / 2;
                        tirs[i].y = y;
                        tir_cooldown = 5;
                        break;
                    }
                }
            }

            // Tirs + collisions
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
                        rectfill(buffer, tirs[i].x, tirs[i].y,
                                 tirs[i].x + 2, tirs[i].y + 15,
                                 makecol(255, 255, 0));
                    }

                    int j;
                    for (j = 0; j < Max_Bulle; j++)
                    {
                        if (bulles[j].actif)
                        {
                            if (collision_tir_bulle(&tirs[i], &bulles[j]))
                            {
                                tirs[i].actif = 0;
                                score += (4 - bulles[j].taille) * 100;
                                diviser_bulle(bulles, Max_Bulle, j);
                                bulles[j].actif = 0;
                                break;
                            }
                        }
                    }
                }
            }

            // Niveau gagne ?
            int bulles_restantes = compter_bulles_actives(bulles, Max_Bulle);

            if (bulles_restantes == 0 && !niveau_gagner)
            {
                niveau_gagner = 1;
                timer_niveau = 0;
            }

            if (niveau_gagner)
            {
                textout_centre_ex(buffer, font, "NIVEAU GAGNE !",
                                  Ecran_X / 2, Ecran_Y / 2,
                                  makecol(0, 255, 0), -1);
                timer_niveau++;

                if (timer_niveau > 120)
                {
                    niveau++;
                    sauvegarder_partie(pseudo, score, niveau);

                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
                    creer_bulles_niveau(bulles, Max_Bulle, niveau, Ecran_X);

                    x = Ecran_X / 2 - tx / 2;
                    niveau_gagner = 0;
                    timer_niveau = 0;
                    temps_restant = (50 + niveau * 10) * 60;
                }
            }
            // Decrementation du timer (seulement si le niveau n'est pas deja gagne)
            if (!niveau_gagner && temps_restant > 0)
            {
                temps_restant--;

                // Temps epuise = game over
                if (temps_restant == 0)
                {
                    etat = ETAT_GAME_OVER;
                }
            }

            // Affichage score et niveau
            char texte_score[50];
            sprintf(texte_score, "Score : %d", score);
            textout_ex(buffer, font, texte_score, 10, 10, makecol(255, 255, 255), -1);

            char texte_niveau[30];
            sprintf(texte_niveau, "Niveau : %d", niveau);
            textout_ex(buffer, font, texte_niveau, Ecran_X - 120, 10, makecol(255, 255, 255), -1);
            // Affichage du timer
            int secondes = temps_restant / 60;   // convertir frames en secondes
            char texte_temps[30];
            sprintf(texte_temps, "Temps : %d", secondes);

            // Couleur qui change si le temps est court
            int couleur_temps;
            if (secondes > 10)
                couleur_temps = makecol(255, 255, 255);   // blanc
            else if (secondes > 5)
                couleur_temps = makecol(255, 200, 0);     // orange
            else
                couleur_temps = makecol(255, 50, 50);     // rouge (alerte)

            textout_ex(buffer, font, texte_temps, Ecran_X / 2 - 40, 10, couleur_temps, -1);
        }
        else if (etat == ETAT_GAME_OVER)
        {
            textout_centre_ex(buffer, font, "GAME OVER",
                              Ecran_X/2, 200, makecol(255,50,50), -1);
            char texte[50];
            sprintf(texte, "Score final : %d", score);
            textout_centre_ex(buffer, font, texte,
                              Ecran_X/2, 250, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "Appuyez sur ENTREE pour rejouer",
                              Ecran_X/2, 350, makecol(255,255,255), -1);

            if (key[KEY_ENTER])
            {
                etat = ETAT_MENU;
            }
            if (key[KEY_ESC]) fin = 1;
        }
        else if (etat == ETAT_REGLES)
        {
            textout_centre_ex(buffer, font, "REGLES DU JEU",
                              Ecran_X/2, 80, makecol(255,255,0), -1);

            textout_centre_ex(buffer, font, "- Detruisez toutes les bulles pour passer au niveau suivant",
                              Ecran_X/2, 150, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Les grosses bulles se divisent en 2 petites",
                              Ecran_X/2, 180, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Si une bulle vous touche, c'est perdu !",
                              Ecran_X/2, 210, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Utilisez les fleches pour vous deplacer",
                              Ecran_X/2, 260, makecol(200,200,255), -1);
            textout_centre_ex(buffer, font, "- Maintenez ESPACE pour tirer",
                              Ecran_X/2, 290, makecol(200,200,255), -1);
            textout_centre_ex(buffer, font, "- Les petites bulles rapportent plus de points !",
                              Ecran_X/2, 320, makecol(200,200,255), -1);

            bouton_retour.survol = souris_dans_bouton(&bouton_retour);
            dessiner_bouton(buffer, &bouton_retour);

            if ((mouse_b & 1) && !clic_presse)
            {
                clic_presse = 1;
                if (bouton_retour.survol) etat = ETAT_MENU;
            }
            if (!(mouse_b & 1)) clic_presse = 0;
        }

        scare_mouse();
        blit(buffer, screen, 0, 0, 0, 0, Ecran_X, Ecran_Y);
        unscare_mouse();
        rest(16);
    }

    destroy_bitmap(buffer);
    exit(EXIT_SUCCESS);
}
END_OF_MAIN();