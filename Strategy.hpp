#pragma once
#include "BitBoard.hpp"
#include "Move.hpp"
#include "Couleur.hpp"

/**
 * @file Strategy.hpp
 * @brief Interface abstraite pour les algorithmes de jeu de l'IA.
 *
 * PATTERN STRATÉGIE :
 *   AIPlayer possède un pointeur sur Strategy.
 *   À chaque tour, il appelle meilleurCoup() sans savoir quel algorithme
 *   est derrière (RandomStrategy, AlphaBetaStrategy, etc.).
 *   On peut changer d'algorithme sans toucher à AIPlayer.
 *
 * Toutes les Strategy travaillent exclusivement sur BitBoard,
 * jamais sur Board — séparation claire affichage / calcul.
 *
 * DESTRUCTEUR VIRTUEL :
 *   Obligatoire pour pouvoir delete une Strategy* polymorphe.
 */
class Strategy {
public:
    virtual ~Strategy() {}

    /**
     * @brief Calcule et retourne le meilleur coup pour la couleur donnée.
     * @param bb     Plateau courant (lecture seule)
     * @param coulIA Couleur de l'IA qui joue
     * @return Le meilleur Move trouvé
     */
    virtual Move meilleurCoup(const BitBoard& bb, Couleur coulIA) = 0;
};
