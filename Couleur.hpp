#pragma once

/**
 * @file Couleur.hpp
 * @brief Les trois états possibles d'une case du plateau.
 *
 * VIDE  = case libre
 * BLANC = perle du joueur humain
 * NOIR  = perle de l'IA
 */
enum Couleur {
    VIDE  = 0,
    BLANC = 1,
    NOIR  = 2
};
