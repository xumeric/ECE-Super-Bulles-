#include <stdio.h>
#include <allegro.h>
#include <time.h>
#include <math.h>
#include <string.h>

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

typedef struct {
    float x, y;
    float vx, vy;
    int rayon;
    int actif;
    int taille;
} Bulle;

typedef struct {
    float x, y;
    int actif;
} Tir;

typedef struct {
    int x, y;
    int largeur, hauteur;
    int vitesse;
} Joueur;
typedef struct {
    int x, y;
    int largeur, hauteur;
    char texte[50];
    int survol;   // 1 si la souris est dessus
} Bouton;



// Sauvegarder dans un fichier texte
void sauvegarder_partie(const char *pseudo, int score, int niveau)
{
    FILE *f = fopen("sauvegarde.txt", "w");  // "w" = ecrire (ecrase l'ancien)
    if (f == NULL)
    {
        allegro_message("Impossible d'ecrire le fichier sauvegarde.txt");
        return;
    }
    fprintf(f, "%s %d %d\n", pseudo, score, niveau);
    fclose(f);
}

// Charger depuis le fichier texte
// Retourne 1 si reussi, 0 si le fichier n'existe pas
int charger_partie(char *pseudo, int *score, int *niveau)
{
    FILE *f = fopen("sauvegarde.txt", "r");  // "r" = lire
    if (f == NULL)
    {
        return 0;  // pas de fichier de sauvegarde
    }
    fscanf(f, "%s %d %d", pseudo, score, niveau);
    fclose(f);
    return 1;
}

void dessiner_bouton(BITMAP *buffer, Bouton *b)
{
    int couleur = b->survol ? makecol(100,100,255) : makecol(50,50,150);
    rectfill(buffer, b->x, b->y, b->x + b->largeur, b->y + b->hauteur, couleur);
    rect(buffer, b->x, b->y, b->x + b->largeur, b->y + b->hauteur, makecol(255,255,255));
    textout_centre_ex(buffer, font, b->texte,
                      b->x + b->largeur/2, b->y + b->hauteur/2 - 4,
                      makecol(255,255,255), -1);
}

int souris_dans_bouton(Bouton *b)
{
    return (mouse_x >= b->x && mouse_x <= b->x + b->largeur &&
            mouse_y >= b->y && mouse_y <= b->y + b->hauteur);
}

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
    char pseudo[50]="Koyoz";
    int deja_sauvegarde=0;


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

    // Creer la premiere bulle
    bulles[0].actif = 1;
    bulles[0].x = 400;
    bulles[0].y = 50;
    bulles[0].vx = 2;
    bulles[0].vy = 0;
    bulles[0].taille = 3;
    bulles[0].rayon = 50;

    //double buffer qui permet de réduire l'effet de scintillance
    BITMAP *buffer = create_bitmap(Ecran_X,Ecran_Y);


    Bouton bouton_jouer;
    bouton_jouer.x = Ecran_X/2 - 100;
    bouton_jouer.y = 300;
    bouton_jouer.largeur = 200;
    bouton_jouer.hauteur = 50;
    strcpy(bouton_jouer.texte, "JOUER");
    bouton_jouer.survol = 0;

    Bouton bouton_regles;
    bouton_regles.x = Ecran_X/2 - 100;
    bouton_regles.y = 370;
    bouton_regles.largeur = 200;
    bouton_regles.hauteur = 50;
    strcpy(bouton_regles.texte, "REGLES");
    bouton_regles.survol = 0;

    Bouton bouton_quitter;
    bouton_quitter.x = Ecran_X/2 - 100;
    bouton_quitter.y = 440;
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
        if (key[KEY_TAB])
        {
            etat = ETAT_JEU;
        }
        if (etat == ETAT_MENU)
        {
            // Survol
            bouton_jouer.survol = souris_dans_bouton(&bouton_jouer);
            bouton_regles.survol = souris_dans_bouton(&bouton_regles);
            bouton_quitter.survol = souris_dans_bouton(&bouton_quitter);

            // Titre
            textout_centre_ex(buffer, font, "SUPER BULLES",
                              Ecran_X/2, 150, makecol(255,255,0), -1);
            textout_centre_ex(buffer,font, "TAB pour jouer ou click pour choisir le menu",
                Ecran_X/2,130,makecol(200,100,50),-1);

            // Dessin des boutons
            dessiner_bouton(buffer, &bouton_jouer);
            dessiner_bouton(buffer, &bouton_regles);
            dessiner_bouton(buffer, &bouton_quitter);

            // Clics (avec protection anti-maintien)
            if ((mouse_b & 1) && !clic_presse)
            {
                clic_presse = 1;
                if (bouton_jouer.survol)
                {
                    etat = ETAT_JEU;
                    score = 0;
                    niveau = 1;
                    niveau_gagner = 0;
                    timer_niveau = 0;
                    deja_sauvegarde = 0;

                    // Nettoyer toutes les bulles et tirs
                    for (i = 0; i < Max_Bulle; i++)bulles[i].actif = 0;
                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;

                    // Creer la premiere bulle
                    bulles[0].actif = 1;
                    bulles[0].x = 400;
                    bulles[0].y = 50;
                    bulles[0].vx = 2;
                    bulles[0].vy = 0;
                    bulles[0].taille = 3;
                    bulles[0].rayon = 50;

                    // Replacer le joueur a gauche
                    x = 50;
                }
                if (bouton_quitter.survol) fin = 1;
                if (bouton_regles.survol)
                {
                    etat = ETAT_REGLES;
                }
            }
            if (!(mouse_b & 1)) clic_presse = 0;
        }
        else if (etat == ETAT_JEU)
        {
            if (key[KEY_LEFT])  x -= px;
            if (key[KEY_RIGHT]) x += px;
            if (key[KEY_ESC])   fin = 1;

            // controle des bords
            if (x < 0) x = 0;
            if (x > Ecran_X - tx) x = Ecran_X - tx;

            rectfill(buffer, x, y, x+tx, y+ty, couleur);
            for (i=0; i<Max_Bulle; i++)
            {
                if (bulles[i].actif)
                {
                    bulles[i].vy += Gravite;
                    bulles[i].y += bulles[i].vy;
                    bulles[i].x += bulles[i].vx;
                    if (bulles[i].y + bulles[i].rayon > Ecran_Y)  // 540 = position du sol
                    {
                        bulles[i].y= Ecran_Y - bulles[i].rayon;        // recoller au sol
                        bulles[i].vy = -bulles[i].vy * 0.85;        // inverser + perte d'energie

                        if (bulles[i].vy > -2.5 && bulles[i].vy < 0)
                        {
                            bulles[i].vy = -(bulles[i].rayon * 0.22 + 3.0);
                        }
                    }
                    if (bulles[i].x + bulles[i].rayon > x &&        // bulle pas trop a gauche
                       bulles[i].x - bulles[i].rayon < x + tx &&    // bulle pas trop a droite
                        bulles[i].y+ bulles[i].rayon> y)           // bulle assez basse
                    {
                        sauvegarder_partie(pseudo, score, niveau);
                        etat=ETAT_GAME_OVER;  // game over
                    }
                    if (bulles[i].x - bulles[i].rayon< 0)
                    {
                        bulles[i].x= bulles[i].rayon;
                        bulles[i].vx = -bulles[i].vx;   // inverser direction
                    }
                    if (bulles[i].x + bulles[i].rayon > Ecran_X)
                    {
                        bulles[i].x = Ecran_X - bulles[i].rayon;
                        bulles[i].vx = -bulles[i].vx;

                    }
                    circlefill(buffer,bulles[i].x,bulles[i].y,bulles[i].rayon,couleur);
                }


            }

            // Cooldown diminue a chaque frame
            if (tir_cooldown > 0) tir_cooldown--;

            // Auto-tir : tant que espace est enfonce, tirer
            if (key[KEY_SPACE] && tir_cooldown == 0)
            {
                // Chercher un slot libre
                for (i = 0; i < Max_Tire; i++)
                {
                    if (!tirs[i].actif)
                    {
                        tirs[i].actif = 1;
                        tirs[i].x = x + tx  / 2;
                        tirs[i].y = y ;
                        tir_cooldown = 5;  // attendre 10 frames avant le prochain tir
                        break;  // on a trouve un slot, on arrete de chercher
                    }
                }
            }
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

                    // Collision tir/bulle
                    int j;
                    for (j=0; j < Max_Bulle; j++)
                    {

                        if (bulles[j].actif)
                        {
                            float dx = tirs[i].x - bulles[j].x;
                            float dy = tirs[i].y - bulles[j].y;
                            float distance = sqrt(dx*dx + dy*dy);
                            if (distance < bulles[j].rayon)
                            {
                                tirs[i].actif = 0;
                                score += (4 - bulles[j].taille) * 100;

                                if (bulles[j].taille > 0)
                                {
                                    // DIVISER : chercher 2 slots libres
                                    int crees = 0;
                                    int k;
                                    for (k = 0; k < Max_Bulle && crees < 2; k++)
                                    {
                                        if (!bulles[k].actif)
                                        {
                                            bulles[k].actif = 1;
                                            bulles[k].x = bulles[j].x + (crees == 0 ? -15 : 15);
                                            bulles[k].y = bulles[j].y;
                                            bulles[k].vx = (crees == 0) ? -2.5 : 2.5;
                                            bulles[k].vy = -3.5;
                                            bulles[k].taille = bulles[j].taille - 1;
                                            // Rayon selon la taille
                                            if (bulles[k].taille == 2) bulles[k].rayon = 34;
                                            else if (bulles[k].taille == 1) bulles[k].rayon = 22;
                                            else bulles[k].rayon = 14;
                                            crees++;

                                        }
                                    }
                                }
                                // Supprimer la bulle touchee
                                bulles[j].actif = 0;
                                break;
                            }
                        }

                    }

                }

            }
            int b;
            int bulles_restantes = 0;
            for (b = 0; b < Max_Bulle; b++)
                if (bulles[b].actif) bulles_restantes++;

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
                    sauvegarder_partie(pseudo, score, niveau);
                    niveau++;

                    // Nettoyer tout
                    for (i = 0; i < Max_Bulle; i++) bulles[i].actif = 0;
                    for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;

                    // Creer les nouvelles bulles selon le niveau
                    for (i = 0; i < niveau && i < Max_Bulle; i++)
                    {
                        bulles[i].actif = 1;
                        bulles[i].x = (i + 1) * Ecran_X / (niveau + 1);
                        bulles[i].y = 50;
                        bulles[i].vx = 1.5 + niveau * 0.5;
                        bulles[i].vy = 0;
                        bulles[i].taille = 3;
                        bulles[i].rayon = 50;
                    }

                    // Replacer le joueur au centre
                    x = Ecran_X / 2 - tx / 2;

                    // Reset
                    niveau_gagner = 0;
                    timer_niveau = 0;
                }
            }
            char texte_score[50];
            sprintf(texte_score, "Score : %d", score);
            textout_ex(buffer, font, texte_score, 10, 10, makecol(255, 255, 255), -1);

            char texte_niveau[30];
            sprintf(texte_niveau, "Niveau : %d", niveau);
            textout_ex(buffer, font, texte_niveau, Ecran_X - 120, 10, makecol(255, 255, 255), -1);
        }


        else if (etat == ETAT_GAME_OVER)
        {
            if (!deja_sauvegarde)
            {
                sauvegarder_partie(pseudo, score, niveau);
                deja_sauvegarde = 1;
            }
            // Afficher game over + score
            textout_centre_ex(buffer, font, "GAME OVER",
                              Ecran_X/2, 200, makecol(255,50,50), -1);
            char texte[50];
            sprintf(texte, "Score final : %d", score);
            textout_centre_ex(buffer, font, texte,
                              Ecran_X/2, 250, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "Appuyez sur entrée pour rejouer",
                              Ecran_X/2, 350, makecol(255,255,255), -1);

            if (key[KEY_ENTER])
            {
                etat = ETAT_MENU;
            }
        }
        else if (etat == ETAT_REGLES)
        {
            // Titre
            textout_centre_ex(buffer, font, "REGLES DU JEU",
                              Ecran_X/2, 80, makecol(255,255,0), -1);

            // Les regles, ligne par ligne
            textout_centre_ex(buffer, font, "- Detruisez toutes les bulles pour passer au niveau suivant",
                              Ecran_X/2, 150, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Les grosses bulles se divisent en 2 petites quand vous les touchez",
                              Ecran_X/2, 180, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Si une bulle vous touche, c'est perdu !",
                              Ecran_X/2, 210, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "- Utilisez les fleches pour vous deplacer",
                              Ecran_X/2, 260, makecol(200,200,255), -1);
            textout_centre_ex(buffer, font, "- Maintenez ESPACE pour tirer",
                              Ecran_X/2, 290, makecol(200,200,255), -1);
            textout_centre_ex(buffer, font, "- Les petites bulles rapportent plus de points !",
                              Ecran_X/2, 320, makecol(200,200,255), -1);

            // Gerer le bouton RETOUR
            bouton_retour.survol = souris_dans_bouton(&bouton_retour);
            dessiner_bouton(buffer, &bouton_retour);

            // Clic sur retour
            if ((mouse_b & 1) && !clic_presse)
            {
                clic_presse = 1;
                if (bouton_retour.survol)
                {
                    etat = ETAT_MENU;
                }
            }
            if (!(mouse_b & 1)) clic_presse = 0;
        }

        if (key[KEY_ESC]) fin = 1;


        scare_mouse();
        blit(buffer, screen, 0, 0, 0, 0, Ecran_X, Ecran_Y);
        unscare_mouse();
        rest(16);
    }
  destroy_bitmap(buffer);
  exit(EXIT_SUCCESS);








}
END_OF_MAIN();