#pragma once

/**
 * @file Move.hpp
 * @brief Un coup = colonne + profondeur du bâtonnet choisi.
 *
 * colonne    : 0=A, 1=B, 2=C, 3=D
 * profondeur : 0 à 4
 *
 * La perle tombe automatiquement au niveau le plus bas libre.
 * Le niveau n'apparaît donc pas dans Move — Board s'en charge.
 */
class Move {
private:
    int m_colonne;
    int m_profondeur;

public:
    /**
     * @brief Constructeur par défaut — colonne 0, profondeur 0.
     */
    Move(int col = 0, int prof = 0)
        : m_colonne(col), m_profondeur(prof) {}

    /** @brief Retourne la colonne (0=A, 1=B, 2=C, 3=D). */
    int getColonne()    const { return m_colonne;    }

    /** @brief Retourne la profondeur (0 à 4). */
    int getProfondeur() const { return m_profondeur; }
};
