#pragma once
#include <iostream>
#include "Player.hpp"
#include "Strategy.hpp"

/**
 * @file AIPlayer.hpp
 * @brief Joueur IA — délègue le calcul à une Strategy.
 *
 * PATTERN STRATÉGIE :
 *   AIPlayer ne contient aucun algorithme.
 *   Il possède un pointeur sur Strategy* et appelle strategy->meilleurCoup().
 *   On peut changer l'algorithme (RandomStrategy, AlphaBetaStrategy...)
 *   sans modifier AIPlayer.
 *
 * EXTRACTION DU BITBOARD :
 *   AIPlayer récupère board.getBitBoard() et le passe à la Strategy.
 *   La Strategy ne reçoit jamais Board directement — séparation nette
 *   entre affichage (Board) et calcul (BitBoard).
 *
 * OWNERSHIP DE STRATEGY :
 *   AIPlayer prend ownership de la Strategy passée au constructeur
 *   et la libère dans son destructeur (delete strategy).
 */
class AIPlayer : public Player {
private:
    Strategy* strategy;  ///< algorithme de jeu (owned par AIPlayer)

public:
    /**
     * @brief Constructeur.
     * @param c     Couleur de l'IA (NOIR en général)
     * @param n     Nom affiché dans le terminal
     * @param strat Stratégie à utiliser (AIPlayer prend ownership)
     */
    AIPlayer(Couleur c, const std::string& n, Strategy* strat)
        : Player(c, n), strategy(strat) {}

    /**
     * @brief Destructeur — libère la Strategy.
     *
     * POURQUOI VIRTUEL ?
     *   Game fait delete joueurs[i] via un Player* → sans destructeur virtuel,
     *   ~AIPlayer() ne serait pas appelé → fuite mémoire sur strategy.
     */
    ~AIPlayer() {
        delete strategy;
    }

    /**
     * @brief Calcule et retourne le meilleur coup via la Strategy.
     *
     * Extrait le BitBoard depuis board, le passe à la Strategy,
     * affiche le coup choisi dans le terminal et le retourne.
     *
     * @param board Plateau courant (pour extraire le BitBoard)
     * @return Le meilleur coup calculé par la Strategy
     */
    Move jouer(const Board& board) override {
        const BitBoard& bb = board.getBitBoard();
        std::cout << nom << " reflechit...\n";
        Move coup = strategy->meilleurCoup(bb, couleur);
        char lettreCol = 'A' + coup.getColonne();
        std::cout << nom << " joue : colonne " << lettreCol
                  << ", profondeur " << coup.getProfondeur() << "\n\n";
        return coup;
    }
};
