cmake_minimum_required(VERSION 4.0)
project(SpaceGame C)

set(CMAKE_C_STANDARD 23)

add_executable(SpaceGame main.c
        sauvegarde.h
        sauvegarde.c
        entites.h
        graphique.h
        graphique.c
        "physique.h"
        physique.c
        jeu.h
        jeu.c
        boss.h
        boss.c
        powerup.h
        powerup.c
        eclair.h
        eclair.c
        menu.h
        menu.c)
target_link_libraries(SpaceGame -lmingw32 -lalleg44)
file(COPY ${CMAKE_SOURCE_DIR}/ressources DESTINATION ${CMAKE_BINARY_DIR})
