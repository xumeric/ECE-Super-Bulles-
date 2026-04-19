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

int main(void)
{
    int fin=0;
    int x,y;
    int px,py;
    int tx,ty;
    int couleur;
    float bulle_x [Max_Bulle], bulle_y [Max_Bulle];
    float bulle_vy [Max_Bulle];
    float bulle_vx [Max_Bulle];
    int bulle_rayon [Max_Bulle];
    int bulle_actif [Max_Bulle];
    int bulle_taille [Max_Bulle];
    float Gravite= 0.15;
    float tir_x [Max_Tire];
    float tir_y [Max_Tire];
    int tir_actif [Max_Tire];
    int score = 0;
    int niveau = 1;
    int niveau_gagner=0;
    int timer_niveau=0;




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
        tir_actif[i]=0;
    int tir_cooldown=0;
    for (i = 0; i < Max_Bulle; i++)
        bulle_actif[i] = 0;

    // Creer la premiere bulle
    bulle_actif[0] = 1;
    bulle_x[0] = 400;
    bulle_y[0] = 50;
    bulle_vx[0] = 2;
    bulle_vy[0] = 0;
    bulle_taille[0] = 3;
    bulle_rayon[0] = 50;

    //double buffer qui permet de réduire l'effet de scintillance
    BITMAP *buffer = create_bitmap(Ecran_X,Ecran_Y);




    while (!fin)
    {
        clear(buffer);

        if (key[KEY_LEFT])  x -= px;
        if (key[KEY_RIGHT]) x += px;
        if (key[KEY_ESC])   fin = 1;

        // controle des bords
        if (x < 0) x = 0;
        if (x > Ecran_X - tx) x = Ecran_X - tx;

        rectfill(buffer, x, y, x+tx, y+ty, couleur);
        for (i=0; i<Max_Bulle; i++)
        {
            if (bulle_actif[i])
            {
                bulle_vy[i] += Gravite;
                bulle_y [i] += bulle_vy[i];
                bulle_x[i] += bulle_vx[i];
                if (bulle_y[i] + bulle_rayon[i] > Ecran_Y)  // 540 = position du sol
                {
                    bulle_y [i]= Ecran_Y - bulle_rayon[i];        // recoller au sol
                    bulle_vy[i] = -bulle_vy[i] * 0.82;        // inverser + perte d'energie

                    if (bulle_vy[i] > -2.5 && bulle_vy[i] < 0)
                    {
                        bulle_vy[i] = -(bulle_rayon[i] * 0.5 + 3.0);
                    }
                }
                if (bulle_x[i] + bulle_rayon[i] > x &&        // bulle pas trop a gauche
                   bulle_x[i] - bulle_rayon[i] < x + tx &&    // bulle pas trop a droite
                    bulle_y [i]+ bulle_rayon [i]> y)           // bulle assez basse
                {

                    fin = 1;  // game over
                }
                if (bulle_x[i] - bulle_rayon [i]< 0)
                {
                    bulle_x [i]= bulle_rayon[i];
                    bulle_vx[i] = -bulle_vx[i];   // inverser direction
                }
                if (bulle_x[i] + bulle_rayon[i] > Ecran_X)
                {
                    bulle_x[i] = Ecran_X - bulle_rayon[i];
                    bulle_vx[i] = -bulle_vx[i];

                }
                circlefill(buffer,bulle_x[i],bulle_y[i],bulle_rayon[i],couleur);
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
                if (!tir_actif[i])
                {
                    tir_actif[i] = 1;
                    tir_x[i] = x + tx / 2;
                    tir_y[i] = y ;
                    tir_cooldown = 5;  // attendre 10 frames avant le prochain tir
                    break;  // on a trouve un slot, on arrete de chercher
                }
            }
        }
        for (i = 0; i < Max_Tire; i++)
        {
            if (tir_actif[i])
            {
                tir_y[i] -= 7;
                if (tir_y[i] < 0)
                {
                    tir_actif[i] = 0;
                }
                else
                {
                    rectfill(buffer, tir_x[i] - 2, tir_y[i],
                             tir_x[i] + 2, tir_y[i] + 15,
                             makecol(255, 255, 0));
                }

                // Collision tir/bulle
                int j;
                for (j=0; j < Max_Bulle; j++)
                {

                    if (bulle_actif[j])
                    {
                        float dx = tir_x[i] - bulle_x[j];
                        float dy = tir_y[i] - bulle_y[j];
                        float distance = sqrt(dx*dx + dy*dy);
                        if (distance < bulle_rayon[j])
                        {
                            tir_actif[i] = 0;
                            score += (4 - bulle_taille[j]) * 100;

                            if (bulle_taille[j] > 0)
                            {
                                // DIVISER : chercher 2 slots libres
                                int crees = 0;
                                int k;
                                for (k = 0; k < Max_Bulle && crees < 2; k++)
                                {
                                    if (!bulle_actif[k])
                                    {
                                        bulle_actif[k] = 1;
                                        bulle_x[k] = bulle_x[j] + (crees == 0 ? -15 : 15);
                                        bulle_y[k] = bulle_y[j];
                                        bulle_vx[k] = (crees == 0) ? -2.5 : 2.5;
                                        bulle_vy[k] = -3.5;
                                        bulle_taille[k] = bulle_taille[j] - 1;
                                        // Rayon selon la taille
                                        if (bulle_taille[k] == 2) bulle_rayon[k] = 34;
                                        else if (bulle_taille[k] == 1) bulle_rayon[k] = 22;
                                        else bulle_rayon[k] = 14;
                                        crees++;

                                    }
                                }
                            }
                            // Supprimer la bulle touchee
                            bulle_actif[j] = 0;
                            break;
                        }
                    }

                }

            }

        }
        int b;
        int bulles_restantes = 0;
        for (b = 0; b < Max_Bulle; b++)
            if (bulle_actif[b]) bulles_restantes++;

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
                for (i = 0; i < Max_Bulle; i++) bulle_actif[i] = 0;
                for (i = 0; i < Max_Tire; i++) tir_actif[i] = 0;

                // Creer les nouvelles bulles selon le niveau
                for (i = 0; i < niveau && i < Max_Bulle; i++)
                {
                    bulle_actif[i] = 1;
                    bulle_x[i] = (i + 1) * Ecran_X / (niveau + 1);
                    bulle_y[i] = 50;
                    bulle_vx[i] = 1.5 + niveau * 0.5;
                    bulle_vy[i] = 0;
                    bulle_taille[i] = 3;
                    bulle_rayon[i] = 50;
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
        blit(buffer,screen,0,0,0,0,Ecran_X,Ecran_Y);
        rest(16);
    }
  destroy_bitmap(buffer);
  exit(EXIT_SUCCESS);








}
END_OF_MAIN();