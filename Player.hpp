#pragma once
#include <string>
#include "Move.hpp"
#include "Board.hpp"
#include "Couleur.hpp"

/**
 * @file Player.hpp
 * @brief Classe abstraite représentant un joueur (humain ou IA).
 *
 * POLYMORPHISME :
 *   Game possède un tableau Player*[2].
 *   Il appelle joueur->jouer(plateau) sans savoir si c'est un humain ou une IA.
 *   HumanPlayer et AIPlayer redéfinissent chacun jouer() à leur façon.
 *
 * jouer() reçoit Board (pas BitBoard) :
 *   HumanPlayer l'utilise pour valider (estBatonnetPlein).
 *   AIPlayer extrait le BitBoard via board.getBitBoard().
 *
 * MEMBRES PROTÉGÉS :
 *   couleur et nom sont protected (pas private) pour que les sous-classes
 *   puissent y accéder directement sans passer par des getters.
 *
 * DESTRUCTEUR VIRTUEL :
 *   Indispensable : Game fait delete joueurs[0] et delete joueurs[1]
 *   via des Player*, donc le destructeur de la sous-classe doit être appelé.
 */
class Player {
protected:
    Couleur     couleur;  ///< couleur de ce joueur (BLANC ou NOIR)
    std::string nom;      ///< nom affiché dans le terminal

public:
    /**
     * @brief Constructeur — initialise la couleur et le nom.
     * @param c Couleur du joueur (BLANC ou NOIR)
     * @param n Nom affiché dans le terminal
     */
    Player(Couleur c, const std::string& n) : couleur(c), nom(n) {}

    /**
     * @brief Destructeur virtuel — obligatoire pour le polymorphisme.
     */
    virtual ~Player() {}

    /**
     * @brief Demande au joueur de choisir un coup.
     *
     * Méthode purement virtuelle — chaque sous-classe l'implémente
     * à sa façon (lecture clavier ou calcul IA).
     *
     * @param board Plateau courant (pour validation ou extraction BitBoard)
     * @return Le coup choisi
     */
    virtual Move jouer(const Board& board) = 0;

    /** @brief Retourne la couleur de ce joueur. */
    Couleur getCouleur() const { return couleur; }

    /** @brief Retourne le nom de ce joueur. */
    std::string getNom() const { return nom; }
};
