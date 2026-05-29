#pragma once
#include <iostream>
#include "Cell.hpp"
#include "Move.hpp"
#include "Couleur.hpp"
#include "BitBoard.hpp"

/**
 * @file Board.hpp
 * @brief Le plateau de jeu 3D — affichage terminal et interaction humain.
 *
 * DOUBLE REPRÉSENTATION :
 *   grille[4][5][4] → tableau de Cell pour l'affichage et la validation
 *   bitboard        → BitBoard pour le moteur IA (synchronisé automatiquement)
 *
 * SYNCHRONISATION AUTOMATIQUE :
 *   placerPerle() met à jour grille ET bitboard en une seule opération.
 *   Il n'y a jamais de désynchronisation possible entre les deux.
 *
 * SÉPARATION DES RÔLES :
 *   Board sert à l'affichage et à la validation des coups humains.
 *   BitBoard (exposé via getBitBoard()) sert au calcul IA.
 */
class Board {
private:
    Cell     grille[4][5][4];  ///< représentation case par case pour l'affichage
    int      niveauLibre[4][5]; ///< prochain niveau libre par bâtonnet
    BitBoard bitboard;          ///< moteur IA bitboard, synchronisé avec grille

public:
    /**
     * @brief Constructeur — plateau vide.
     * Initialise tous les niveaux libres à 0 (les bâtonnets sont vides).
     */
    Board() {
        for (int c = 0; c < 4; c++)
            for (int p = 0; p < 5; p++)
                niveauLibre[c][p] = 0;
    }

    /**
     * @brief Pose une perle sur le bâtonnet indiqué par le coup.
     *
     * La perle tombe au niveau le plus bas libre (gravité).
     * Met à jour grille ET bitboard simultanément.
     *
     * @param coup Coup contenant la colonne et la profondeur choisies
     * @param coul Couleur de la perle à poser
     */
    void placerPerle(Move coup, Couleur coul) {
        int col  = coup.getColonne();
        int prof = coup.getProfondeur();
        int n    = niveauLibre[col][prof];
        grille[col][prof][n].occuper(coul);
        bitboard.poser(col, prof, coul);
        niveauLibre[col][prof]++;
    }

    /**
     * @brief Retourne true si le bâtonnet (col, prof) est plein.
     * @param col  Colonne (0 à 3)
     * @param prof Profondeur (0 à 4)
     */
    bool estBatonnetPlein(int col, int prof) const {
        return niveauLibre[col][prof] == 4;
    }

    /**
     * @brief Retourne true si tous les bâtonnets sont pleins (match nul).
     */
    bool estPlein() const {
        for (int c = 0; c < 4; c++)
            for (int p = 0; p < 5; p++)
                if (!estBatonnetPlein(c, p)) return false;
        return true;
    }

    /**
     * @brief Retourne une référence constante sur la case (col, prof, niv).
     * @param col  Colonne (0 à 3)
     * @param prof Profondeur (0 à 4)
     * @param niv  Niveau (0 à 3)
     */
    const Cell& getCell(int col, int prof, int niv) const {
        return grille[col][prof][niv];
    }

    /**
     * @brief Retourne le niveau de la dernière perle posée sur (col, prof).
     * Retourne -1 si le bâtonnet est vide.
     */
    int getNiveauActuel(int col, int prof) const {
        return niveauLibre[col][prof] - 1;
    }

    /**
     * @brief Expose le BitBoard pour AIPlayer et Game.
     *
     * AIPlayer récupère le BitBoard via board.getBitBoard()
     * pour le passer à sa Strategy sans jamais utiliser Board directement.
     */
    const BitBoard& getBitBoard() const {
        return bitboard;
    }

    /**
     * @brief Affiche le plateau dans le terminal.
     *
     * FORMAT :
     *   Niveaux de 3 (haut) à 0 (bas), de gauche à droite par profondeur.
     *   O = Blanc (humain), X = Noir (IA), . = case vide.
     */
    void afficher() const {
        const char COLS[] = "ABCD";
        std::cout << "\n";
        std::cout << "         ";
        for (int p = 0; p < 5; p++) std::cout << "  Prof." << p << "    ";
        std::cout << "\n";

        for (int n = 3; n >= 0; n--) {
            std::cout << " Niv " << n << " |";
            for (int p = 0; p < 5; p++) {
                for (int c = 0; c < 4; c++) {
                    if      (!grille[c][p][n].estOccupee())         std::cout << " .";
                    else if (grille[c][p][n].getCouleur() == BLANC) std::cout << " O";
                    else                                             std::cout << " X";
                }
                std::cout << "  |";
            }
            std::cout << "\n";
        }

        std::cout << "       +";
        for (int p = 0; p < 5; p++) std::cout << "---------+";
        std::cout << "\n";

        std::cout << "         ";
        for (int p = 0; p < 5; p++) {
            std::cout << " ";
            for (int c = 0; c < 4; c++) std::cout << " " << COLS[c];
            std::cout << "   ";
        }
        std::cout << "\n\n";
        std::cout << "  O = Blanc (Humain)   X = Noir (IA)   . = vide\n\n";
    }
};
