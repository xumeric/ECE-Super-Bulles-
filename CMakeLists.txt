cmake_minimum_required(VERSION 4.0)
project(SpaceGame C)

set(CMAKE_C_STANDARD 23)

add_executable(SpaceGame main.c
        sauvegarde/sauvegarde.c
        entites.h
        graphique/graphique.c
        physique/physique.c
        jeu/jeu.c
        boss/boss.c
        powerup/powerup.c
        eclair/eclair.c
        menu.c
        assets/assets.c
        partie/partie.c
        etat_jeu/etat_jeu.c
        etat_boss/etat_boss.c
        etat_bonus/etat_bonus.h
        etat_bonus/etat_bonus.c
        constantes.h)

target_include_directories(SpaceGame PRIVATE sauvegarde graphique physique jeu boss powerup eclair assets partie etat_jeu etat_boss)
target_link_libraries(SpaceGame -lmingw32 -lalleg44)

MESSAGE(STATUS "Copie des ressources")
file(COPY ${CMAKE_SOURCE_DIR}/ressources DESTINATION ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "Initialisation de SpaceGame - done")