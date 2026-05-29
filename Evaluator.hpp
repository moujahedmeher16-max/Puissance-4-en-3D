#pragma once
#include <cstdint>
#include "BitBoard.hpp"
#include "Couleur.hpp"

/**
 * @file Evaluator.hpp
 * @brief Évaluateurs de position pour l'IA.
 *
 * HIÉRARCHIE :
 *   Evaluator (interface abstraite)
 *     ├── EvaluatorAvance  : évaluation complète aux feuilles (~10 µs)
 *     └── EvaluatorMoyen   : évaluation rapide pour le tri (~0.4 µs)
 *
 * CONVENTION DE SCORE :
 *   score > 0 → bon pour NOIR (IA)
 *   score < 0 → bon pour BLANC (humain)
 *   score = 0 → position équilibrée
 *
 * CORRECTIONS APPLIQUÉES :
 *   1. scoreFourchettes : ne compte que les cases PHYSIQUEMENT ACCESSIBLES
 *      (niveauLibre[col][prof] == niv). Avant, on détectait des fourchettes
 *      sur des cases impossibles à jouer immédiatement.
 *   2. EvaluatorAvance utilise maintenant scoreAlignementsMasques
 *      (via les 122 masques + popcount) au lieu de la boucle triple O(13×4×5×4).
 *      Résultat identique, 8x plus rapide.
 */

// ─── Utilitaire ──────────────────────────────────────────────────────────────

/**
 * @brief Compte les bits à 1 dans un entier 128 bits.
 * Utilise deux appels à __builtin_popcountll (64 bits chacun).
 */
static inline int popcount128(u128 x) {
    return __builtin_popcountll((uint64_t)x) +
           __builtin_popcountll((uint64_t)(x >> 64));
}

// ─── scoreAlignementsMasques ─────────────────────────────────────────────────

/**
 * @brief Évalue les alignements partiels via les 122 masques précalculés.
 *
 * PRINCIPE :
 *   Pour chaque ligne de 4 cases (masque) :
 *     - Si les deux couleurs sont présentes → ligne bloquée → skip
 *     - Sinon → score selon le nombre de perles (3=100, 2=10, 1=1)
 *   Score positif pour NOIR, négatif pour BLANC → différence directe.
 *
 * Bonus de position : les cases centrales valent plus car elles participent
 * à davantage de lignes gagnantes en 3D.
 *
 * @param bb Plateau à évaluer
 * @return Score net (NOIR - BLANC)
 */
static int scoreAlignementsMasques(const BitBoard& bb) {
    // Bonus position — cases centrales strategiquement plus fortes
    static const int bonusCol [4] = {0, 3, 3, 0};
    static const int bonusProf[5] = {0, 2, 4, 2, 0};
    static const int bonusNiv [4] = {1, 3, 3, 1};

    int score = 0;

    // Bonus de position pour chaque perle déjà placée
    for (int c = 0; c < 4; c++)
    for (int p = 0; p < 5; p++)
    for (int n = 0; n < 4; n++) {
        int idx   = c * 20 + p * 4 + n;
        int bonus = bonusCol[c] + bonusProf[p] + bonusNiv[n];
        if (bb.getBit(idx, NOIR))  score += bonus;
        if (bb.getBit(idx, BLANC)) score -= bonus;
    }

    // Score des alignements via les 122 masques
    int nb = BitBoard::nbMasques();
    for (int m = 0; m < nb; m++) {
        u128 masque = BitBoard::getMasque(m);
        u128 n_bits = bb.getNoir()  & masque;
        u128 b_bits = bb.getBlanc() & masque;

        // Ligne bloquée (les deux couleurs présentes) → aucun intérêt
        if (n_bits && b_bits) continue;

        if (n_bits) {
            int cnt = popcount128(n_bits);
            if      (cnt == 3) score += 100;
            else if (cnt == 2) score +=  10;
            else if (cnt == 1) score +=   1;
        }
        if (b_bits) {
            int cnt = popcount128(b_bits);
            if      (cnt == 3) score -= 100;
            else if (cnt == 2) score -=  10;
            else if (cnt == 1) score -=   1;
        }
    }

    return score;
}

// ─── scoreFourchettes ────────────────────────────────────────────────────────

/**
 * @brief Détecte les fourchettes réelles — cases accessibles avec menaces multiples.
 *
 * UNE FOURCHETTE = une case vide accessible par laquelle passent
 * au moins 2 lignes incomplètes de notre couleur (sans adverse).
 * L'adversaire ne peut bloquer qu'une case par tour → victoire forcée.
 *
 * CORRECTION BUG CRITIQUE :
 *   On n'itère plus sur les 80 cases mais sur les 20 bâtonnets.
 *   Pour chaque bâtonnet (c, p), seul niveauLibre[c][p] est jouable.
 *   Avant : on évaluait des cases à des niveaux supérieurs inaccessibles,
 *   créant des fourchettes fictives (+500/+1500 pour rien).
 *
 * SCORES :
 *   Victoire immédiate disponible (menace-3) → +5000 (priorité absolue)
 *   Triple fourchette (3 menaces-2)          → +1500 (quasi-imblocable)
 *   Double fourchette (2 menaces-2)          → +500  (fourchette classique)
 *
 * @param bb   Plateau à évaluer
 * @param coul Couleur pour laquelle on cherche les fourchettes
 * @return Score des fourchettes pour cette couleur (toujours >= 0)
 */
static int scoreFourchettes(const BitBoard& bb, Couleur coul) {
    int total = 0;

    u128 maCouleur = (coul == NOIR) ? bb.getNoir()  : bb.getBlanc();
    u128 coulAdv   = (coul == NOIR) ? bb.getBlanc() : bb.getNoir();
    u128 occupees  = maCouleur | coulAdv;

    for (int c = 0; c < 4; c++)
    for (int p = 0; p < 5; p++) {
        // Seul le prochain niveau libre sur ce bâtonnet est jouable immédiatement
        int niv = bb.getNiveauLibre(c, p);
        if (niv >= 4) continue;  // bâtonnet plein, aucun coup possible ici

        int idx = BitBoard::indice(c, p, niv);

        // Vérification de cohérence : cette case doit être vide
        if ((occupees >> idx) & 1) continue;

        int menaces3 = 0; // lignes avec 3 perles de notre couleur → victoire imminente
        int menaces2 = 0; // lignes avec 2 perles de notre couleur → menace simple

        for (int m : BitBoard::getMasquesPourCase(idx)) {
            u128 masque  = BitBoard::getMasque(m);
            u128 c_bits  = maCouleur & masque;
            u128 ad_bits = coulAdv   & masque;

            // Ligne bloquée par l'adversaire → pas une menace
            if (ad_bits) continue;

            int cnt = popcount128(c_bits);
            if      (cnt == 3) menaces3++;  // si on joue ici on gagne !
            else if (cnt == 2) menaces2++;  // 2 perles + cette case = 3 alignés
        }

        // Victoire immédiate disponible → score maximal, on s'arrête là
        if (menaces3 >= 1) {
            total += 5000;
            continue;
        }

        // Fourchette : 2+ menaces sur la même case accessible
        if      (menaces2 >= 3) total += 1500;
        else if (menaces2 >= 2) total +=  500;
    }

    return total;
}

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Interface abstraite pour noter une position de jeu.
 *
 * Toutes les stratégies utilisent un Evaluator via cette interface.
 * Le polymorphisme permet d'avoir plusieurs évaluateurs interchangeables.
 *
 * DESTRUCTEUR VIRTUEL :
 *   Obligatoire pour pouvoir delete un Evaluator* polymorphe
 *   sans fuite mémoire.
 */
class Evaluator {
public:
    virtual ~Evaluator() {}

    /**
     * @brief Évalue la position et retourne un score.
     * @param bb Plateau à évaluer
     * @return Score (>0 = bon pour NOIR, <0 = bon pour BLANC)
     */
    virtual int evaluer(const BitBoard& bb) const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Évaluateur complet — utilisé aux feuilles de l'arbre alpha-bêta.
 *
 * Combine scoreAlignementsMasques + scoreFourchettes (corrigé).
 * Précis (~10 µs par appel). Appelé uniquement aux feuilles.
 */
class EvaluatorAvance : public Evaluator {
public:
    /**
     * @brief Évalue la position avec alignements + fourchettes.
     * @param bb Plateau à évaluer
     * @return Score net (NOIR - BLANC)
     */
    int evaluer(const BitBoard& bb) const override {
        int score = 0;
        score += scoreAlignementsMasques(bb);
        score += scoreFourchettes(bb, NOIR)  - scoreFourchettes(bb, BLANC);
        return score;
    }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Évaluateur léger — utilisé pour le Move Ordering.
 *
 * Utilise uniquement scoreAlignementsMasques (sans fourchettes).
 * ~0.4 µs par appel vs ~10 µs pour EvaluatorAvance.
 * Appelé pour trier les coups avant l'alpha-bêta (x27 fois plus souvent).
 *
 * PRINCIPE DU MOVE ORDERING :
 *   En explorant les meilleurs coups en premier, l'alpha-bêta peut couper
 *   davantage de branches → profondeur effective plus grande dans le même temps.
 */
class EvaluatorMoyen : public Evaluator {
public:
    /**
     * @brief Évalue la position avec alignements uniquement (sans fourchettes).
     * @param bb Plateau à évaluer
     * @return Score net (NOIR - BLANC)
     */
    int evaluer(const BitBoard& bb) const override {
        return scoreAlignementsMasques(bb);
    }
};
