//
// Created by xumer on 21/04/2026.
//

#ifndef SPACEGAME_GRAPHIQUE_H
#define SPACEGAME_GRAPHIQUE_H


#include <allegro.h>
#include "entites.h"

void dessiner_bouton(BITMAP *buffer, Bouton *b);
int souris_dans_bouton(Bouton *b);



#endif //SPACEGAME_GRAPHIQUE_H