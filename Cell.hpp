#pragma once
#include "Couleur.hpp"

/**
 * @file Cell.hpp
 * @brief Une case individuelle du plateau 3D.
 *
 * ENCAPSULATION :
 *   m_occupee et m_couleur sont privés.
 *   On passe obligatoirement par les méthodes pour les lire ou les modifier.
 */
class Cell {
private:
    bool    m_occupee;  ///< true si une perle est posée sur cette case
    Couleur m_couleur;  ///< couleur de la perle (VIDE si case libre)

public:
    /**
     * @brief Constructeur — case vide par défaut.
     * Initialisation par liste : m_occupee=false, m_couleur=VIDE.
     */
    Cell() : m_occupee(false), m_couleur(VIDE) {}

    /** @brief Retourne true si la case est occupée. */
    bool    estOccupee() const { return m_occupee; }

    /** @brief Retourne la couleur de la perle (VIDE si libre). */
    Couleur getCouleur() const { return m_couleur; }

    /**
     * @brief Pose une perle de couleur c sur cette case.
     * @param c Couleur de la perle à poser (BLANC ou NOIR).
     */
    void occuper(Couleur c) {
        m_occupee = true;
        m_couleur = c;
    }

    /** @brief Remet la case à l'état vide. */
    void vider() {
        m_occupee = false;
        m_couleur = VIDE;
    }
};
