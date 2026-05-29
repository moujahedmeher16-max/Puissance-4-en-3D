#pragma once
#include <iostream>
#include "Board.hpp"
#include "Player.hpp"
#include "Couleur.hpp"

/**
 * @file Game.hpp
 * @brief Orchestre la partie du début à la fin.
 *
 * RESPONSABILITÉS :
 *   - Gérer l'alternance des tours (joueurs[0] → joueurs[1] → ...)
 *   - Appeler joueur->jouer(plateau) à chaque tour
 *   - Appeler plateau.placerPerle() avec le coup retourné
 *   - Détecter victoire (via BitBoard.estGagne) et match nul (plateau plein)
 *   - Libérer les joueurs dans le destructeur
 *
 * OWNERSHIP :
 *   Game prend ownership des deux Player* passés au constructeur
 *   et les libère dans ~Game().
 *
 * DÉTECTION VICTOIRE :
 *   Via plateau.getBitBoard().estGagne() — utilise les masques précalculés,
 *   opération O(1) (test bitwise sur ~6 masques par case).
 */
class Game {
private:
    Board   plateau;       ///< plateau de jeu (grille + bitboard synchronisés)
    Player* joueurs[2];    ///< les deux joueurs (owned par Game)
    int     tourCourant;   ///< indice du joueur actif (0 ou 1)

public:
    /**
     * @brief Constructeur.
     * @param j0 Premier joueur (Blancs — commence)
     * @param j1 Deuxième joueur (Noirs)
     */
    Game(Player* j0, Player* j1) : tourCourant(0) {
        joueurs[0] = j0;
        joueurs[1] = j1;
    }

    /**
     * @brief Destructeur — libère les deux joueurs.
     *
     * POURQUOI ~Game() libère les joueurs ?
     *   Game prend ownership au constructeur.
     *   Sans ce delete, les Player* fuieraient en mémoire.
     */
    ~Game() {
        delete joueurs[0];
        delete joueurs[1];
    }

    /**
     * @brief Lance la partie et retourne la couleur gagnante.
     *
     * Boucle principale :
     *   1. Afficher le plateau
     *   2. Demander un coup au joueur actif (jouer())
     *   3. Poser la perle (placerPerle)
     *   4. Vérifier victoire ou match nul
     *   5. Passer au joueur suivant
     *
     * @return Couleur du gagnant (BLANC ou NOIR), ou VIDE si match nul
     */
    Couleur lancer() {
        std::cout << "=== Puissance 4 3D ===\n";
        std::cout << "O = Blancs (Humain)   X = Noirs (IA)\n";
        std::cout << "Colonnes : A B C D    Profondeur : 0 a 4\n\n";

        while (true) {
            Player* joueurActif = joueurs[tourCourant];
            Couleur couleur     = joueurActif->getCouleur();

            plateau.afficher();

            // Le joueur choisit son coup (clavier ou IA)
            Move coup = joueurActif->jouer(plateau);

            // On pose la perle (met à jour grille ET bitboard)
            plateau.placerPerle(coup, couleur);

            // Victoire ? — on teste via le bitboard (O(1))
            if (plateau.getBitBoard().estGagne(
                    coup.getColonne(), coup.getProfondeur(), couleur)) {
                plateau.afficher();
                std::cout << "*** " << joueurActif->getNom() << " gagne ! ***\n";
                return couleur;
            }

            // Match nul ?
            if (plateau.estPlein()) {
                plateau.afficher();
                std::cout << "*** Match nul ! Le plateau est plein. ***\n";
                return VIDE;
            }

            // Prochain joueur : 0→1→0→1...
            tourCourant = 1 - tourCourant;
        }
    }
};
