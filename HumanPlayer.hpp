#pragma once
#include <iostream>
#include <limits>
#include "Player.hpp"

/**
 * @file HumanPlayer.hpp
 * @brief Joueur humain — lit et valide les coups depuis le clavier.
 *
 * VALIDATION :
 *   - Lettre de colonne : A/B/C/D (accepte aussi a/b/c/d)
 *   - Profondeur : entier entre 0 et 4
 *   - Bâtonnet non plein (via board.estBatonnetPlein)
 *   En cas de saisie invalide, on redemande sans quitter la boucle.
 */
class HumanPlayer : public Player {
public:
    /**
     * @brief Constructeur.
     * @param c Couleur du joueur humain (BLANC en général)
     * @param n Nom affiché dans le terminal
     */
    HumanPlayer(Couleur c, const std::string& n) : Player(c, n) {}

    /**
     * @brief Lit et valide un coup au clavier.
     *
     * Boucle infinie jusqu'à obtenir un coup valide :
     *   1. Lire une lettre de colonne (A-D)
     *   2. Lire un entier de profondeur (0-4)
     *   3. Vérifier que le bâtonnet n'est pas plein
     *
     * @param board Plateau courant (pour valider estBatonnetPlein)
     * @return Le coup saisi, validé
     */
    Move jouer(const Board& board) override {
        char lettreColonne;
        int  colonne, profondeur;

        while (true) {
            std::cout << "Votre tour " << nom << " (O = Blanc) !\n";

            // ── Lecture de la colonne ─────────────────────────────────────────
            std::cout << "Colonne    (A/B/C/D) : ";
            std::cin  >> lettreColonne;

            // Accepte les minuscules
            if (lettreColonne >= 'a' && lettreColonne <= 'z')
                lettreColonne = lettreColonne - 'a' + 'A';

            if (lettreColonne < 'A' || lettreColonne > 'D') {
                std::cout << "Colonne invalide !\n\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            colonne = lettreColonne - 'A';

            // ── Lecture de la profondeur ──────────────────────────────────────
            std::cout << "Profondeur (0/1/2/3/4) : ";
            if (!(std::cin >> profondeur)) {
                std::cout << "Saisie invalide !\n\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (profondeur < 0 || profondeur > 4) {
                std::cout << "Profondeur invalide !\n\n";
                continue;
            }

            // ── Validation : bâtonnet non plein ──────────────────────────────
            if (board.estBatonnetPlein(colonne, profondeur)) {
                std::cout << "Ce batonnet est plein !\n\n";
                continue;
            }

            break; // coup valide
        }

        std::cout << "\n";
        return Move(colonne, profondeur);
    }
};
