#include <stdio.h>
#include <allegro.h>
#include <time.h>
#include <math.h>

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




    allegro_init();
    install_keyboard();
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




    while (!fin)
    {
        clear(buffer);
        if (etat == ETAT_MENU)
        {
            // Afficher le menu, attendre que le joueur appuie sur ESPACE
            textout_centre_ex(buffer, font, "SUPER BULLES",
                              Ecran_X/2, 200, makecol(255,255,0), -1);
            textout_centre_ex(buffer, font, "Appuyez sur TAB pour jouer",
                              Ecran_X/2, 300, makecol(255,255,255), -1);

            if (key[KEY_TAB])
            {
                etat = ETAT_JEU;
                // Reset du jeu
                score = 0;
                niveau = 1;
                // Nettoyer bulles et tirs
                for (i = 0; i < Max_Bulle; i++) bulles[i].actif = 0;
                for (i = 0; i < Max_Tire; i++) tirs[i].actif = 0;
                // Creer la premiere bulle
                bulles[0].actif = 1;
                bulles[0].x = 400;
                bulles[0].y = 50;
                bulles[0].vx = 2;
                bulles[0].vy = 0;
                bulles[0].taille = 3;
                bulles[0].rayon = 50;
            }
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
        }
        else if (etat == ETAT_GAME_OVER)
        {
            // Afficher game over + score
            textout_centre_ex(buffer, font, "GAME OVER",
                              Ecran_X/2, 200, makecol(255,50,50), -1);
            char texte[50];
            sprintf(texte, "Score final : %d", score);
            textout_centre_ex(buffer, font, texte,
                              Ecran_X/2, 250, makecol(255,255,255), -1);
            textout_centre_ex(buffer, font, "Appuyez sur ESPACE pour rejouer",
                              Ecran_X/2, 350, makecol(255,255,255), -1);

            if (key[KEY_SPACE])
            {
                etat = ETAT_MENU;
            }
        }

        if (key[KEY_ESC]) fin = 1;

        blit(buffer, screen, 0, 0, 0, 0, Ecran_X, Ecran_Y);
        rest(16);
    }
  destroy_bitmap(buffer);
  exit(EXIT_SUCCESS);








}
END_OF_MAIN();