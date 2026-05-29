#pragma once
#include <cstdlib>
#include <vector>
#include "Strategy.hpp"

/**
 * @file RandomStrategy.hpp
 * @brief Stratégie aléatoire — pour les tests et le débogage uniquement.
 *
 * Choisit un coup au hasard parmi les coups légaux.
 * Utile pour tester que les règles du jeu fonctionnent
 * sans l'influence d'une IA sophistiquée.
 */
class RandomStrategy : public Strategy {
public:
    /**
     * @brief Retourne un coup choisi aléatoirement parmi les coups possibles.
     *
     * Utilise rand() — appeler srand() dans main() pour avoir
     * des séquences différentes à chaque partie.
     *
     * @param bb     Plateau courant
     * @param coulIA Non utilisé (le coup aléatoire ignore la couleur)
     * @return Un Move aléatoire parmi les coups légaux
     */
    Move meilleurCoup(const BitBoard& bb, Couleur /*coulIA*/) override {
        std::vector<Move> coups = bb.getCoupsPossibles();
        return coups[rand() % coups.size()];
    }
};
