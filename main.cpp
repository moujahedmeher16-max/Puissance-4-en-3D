/**
 * @file main.cpp
 * @brief Point d'entrée — crée les joueurs et lance la partie.
 *
 * Au démarrage, le joueur choisit :
 *   1. Qui commence (Humain ou IA)
 *   2. La partie se lance avec les Blancs en premier (règle officielle)
 *
 * Si l'humain choisit de commencer → Humain = Blancs, IA = Noirs
 * Si l'IA choisit de commencer     → IA = Blancs, Humain = Noirs
 */
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include "Game.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "AlphaBetaStrategy.hpp"
#include "Evaluator.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ── Choix de qui commence ─────────────────────────────────────────────────
    std::cout << "=== Puissance 4 3D ===\n\n";
    std::cout << "Qui commence ?\n";
    std::cout << "  1. Humain (Blancs)\n";
    std::cout << "  2. IA     (Blancs)\n";
    std::cout << "Votre choix (1 ou 2) : ";

    int choix = 0;
    while (choix != 1 && choix != 2) {
        if (!(std::cin >> choix) || (choix != 1 && choix != 2)) {
            std::cout << "Choix invalide, entrez 1 ou 2 : ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choix = 0;
        }
    }
    std::cout << "\n";

    // ── Création des joueurs selon le choix ───────────────────────────────────
    // Les Blancs commencent toujours (règle officielle du Puissance 4 3D).
    // On assigne la couleur BLANC au joueur qui commence.

    Player* joueur0; // joueurs[0] = Blancs = commence
    Player* joueur1; // joueurs[1] = Noirs

    AlphaBetaStrategy* strategie = new AlphaBetaStrategy(
        new EvaluatorAvance(), // feuilles — précis  (~10 µs)
        new EvaluatorMoyen(),  // tri      — rapide  (~0.4 µs)
        10,                     // profondeur maximale
        7500                   // temps limite par coup (ms)
    );

    if (choix == 1) {
        // Humain commence → Humain = Blancs, IA = Noirs
        std::cout << "Vous jouez les Blancs (O) et commencez.\n\n";
        joueur0 = new HumanPlayer(BLANC, "Humain");
        joueur1 = new AIPlayer(NOIR, "IA", strategie);
    } else {
        // IA commence → IA = Blancs, Humain = Noirs
        std::cout << "L'IA joue les Blancs (O) et commence.\n\n";
        joueur0 = new AIPlayer(BLANC, "IA", strategie);
        joueur1 = new HumanPlayer(NOIR, "Humain");
    }

    Game partie(joueur0, joueur1);
    partie.lancer();

    return 0;
}
