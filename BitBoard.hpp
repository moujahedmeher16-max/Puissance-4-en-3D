#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include <random>
#include "Move.hpp"
#include "Couleur.hpp"

typedef unsigned __int128 u128;
typedef uint64_t          u64;

/**
 * @file BitBoard.hpp
 * @brief Représentation bitboard du plateau — moteur interne de l'IA.
 *
 * PRINCIPE :
 *   blanc : entier 128 bits — un bit à 1 par perle blanche
 *   noir  : entier 128 bits — un bit à 1 par perle noire
 *
 * INDEXATION : case (col, prof, niv) → indice = col*20 + prof*4 + niv
 *   80 cases au total (4 col × 5 prof × 4 niv).
 *
 * MASQUES PRÉCALCULÉS :
 *   122 masques — un par ligne gagnante possible (toutes directions 3D).
 *   Pour chaque case, ~6 masques passent par elle (MASQUES_PAR_CASE).
 *   Victoire = (couleur & masque) == masque → une seule instruction CPU.
 *
 * HASH ZOBRIST :
 *   Mis à jour incrémentalement à chaque poser()/annuler().
 *   Permet à la table de transposition de retrouver une position déjà vue.
 */
class BitBoard {
private:
    u128 blanc;              ///< bits des perles blanches
    u128 noir;               ///< bits des perles noires
    int  niveauLibre[4][5];  ///< prochain niveau libre par bâtonnet
    u64  hash;               ///< hash Zobrist courant

    // ── Données statiques partagées (initialisées une seule fois) ────────────
    static u128             MASQUES[122];         ///< 122 lignes gagnantes
    static std::vector<int> MASQUES_PAR_CASE[80]; ///< masques passant par chaque case
    static bool             masquesInitialises;
    static u64              ZOBRIST_TABLE[80][2]; ///< valeurs aléatoires Zobrist
    static bool             zobristInitialise;
    static int              nbMasquesTotal;       ///< nombre réel de masques générés

    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Génère les 122 masques des lignes gagnantes en 3D.
     *
     * 13 directions possibles (horizontale, verticale, 11 diagonales 3D).
     * Pour chaque case de départ + chaque direction, on vérifie si 4 cases
     * consécutives restent dans le plateau. Si oui, on crée un masque 128 bits
     * avec les 4 bits correspondants à 1.
     * Les doublons sont filtrés (même ligne, départ différent).
     */
    static void initMasques() {
        static const int DIRS[13][3] = {
            {1,0,0},{0,1,0},{0,0,1},
            {1,1,0},{1,-1,0},{1,0,1},{1,0,-1},
            {0,1,1},{0,1,-1},
            {1,1,1},{1,1,-1},{1,-1,1},{1,-1,-1}
        };

        int  nbMasques = 0;
        u128 dejaVu[300];
        int  nbDejaVu = 0;

        for (int d = 0; d < 13; d++) {
            int dc = DIRS[d][0], dp = DIRS[d][1], dn = DIRS[d][2];
            for (int c = 0; c < 4; c++)
            for (int p = 0; p < 5; p++)
            for (int n = 0; n < 4; n++) {
                bool valide = true;
                int  idx[4];
                for (int i = 0; i < 4; i++) {
                    int nc = c+dc*i, np = p+dp*i, nn = n+dn*i;
                    if (nc<0||nc>=4||np<0||np>=5||nn<0||nn>=4) { valide=false; break; }
                    idx[i] = indice(nc, np, nn);
                }
                if (!valide) continue;

                u128 masque = 0;
                for (int i = 0; i < 4; i++) masque |= (u128)1 << idx[i];

                bool doublon = false;
                for (int m = 0; m < nbDejaVu; m++)
                    if (dejaVu[m] == masque) { doublon = true; break; }
                if (doublon) continue;

                dejaVu[nbDejaVu++] = masque;
                MASQUES[nbMasques] = masque;
                for (int i = 0; i < 4; i++)
                    MASQUES_PAR_CASE[idx[i]].push_back(nbMasques);
                nbMasques++;
            }
        }
        nbMasquesTotal = nbMasques;
    }

    /**
     * @brief Initialise la table Zobrist avec des valeurs pseudo-aléatoires.
     *
     * ZOBRIST_TABLE[idx][0] = valeur pour BLANC à la case idx
     * ZOBRIST_TABLE[idx][1] = valeur pour NOIR  à la case idx
     *
     * Le hash d'une position = XOR de toutes les valeurs des perles présentes.
     * Propriété utile : annuler un coup = XOR à nouveau avec la même valeur.
     */
    static void initZobrist() {
        std::mt19937_64 rng(123456789ULL);
        for (int i = 0; i < 80; i++) {
            ZOBRIST_TABLE[i][0] = rng();
            ZOBRIST_TABLE[i][1] = rng();
        }
    }

public:
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Convertit (col, prof, niv) en indice bit [0..79].
     *
     * Formule : col*20 + prof*4 + niv
     * Choix de cette formule : les cases d'un même bâtonnet (col, prof)
     * sont consécutives en mémoire → meilleure localité de cache.
     */
    static int indice(int col, int prof, int niv) {
        return col * 20 + prof * 4 + niv;
    }

    /** @brief Retourne le masque numéro m. */
    static u128 getMasque(int m) {
        return MASQUES[m];
    }

    /** @brief Retourne la liste des indices de masques passant par la case idx. */
    static const std::vector<int>& getMasquesPourCase(int idx) {
        return MASQUES_PAR_CASE[idx];
    }

    /** @brief Retourne le nombre total de masques générés (doit être 122). */
    static int nbMasques() {
        return nbMasquesTotal;
    }

    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Constructeur — plateau vide.
     *
     * Initialise blanc=0, noir=0, hash=0, tous niveaux libres à 0.
     * Lance l'initialisation statique des masques et de Zobrist
     * si ce n'est pas déjà fait (pattern "initialisation paresseuse").
     */
    BitBoard() : blanc(0), noir(0), hash(0) {
        memset(niveauLibre, 0, sizeof(niveauLibre));
        if (!masquesInitialises) { initMasques(); masquesInitialises = true; }
        if (!zobristInitialise)  { initZobrist(); zobristInitialise  = true; }
    }

    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Pose une perle sur le bâtonnet (col, prof).
     *
     * La perle tombe au niveau niveauLibre[col][prof] (gravité simulée).
     * Met à jour le bit correspondant dans blanc ou noir,
     * et met à jour le hash Zobrist par XOR.
     *
     * @param col  Colonne (0 à 3)
     * @param prof Profondeur (0 à 4)
     * @param coul Couleur de la perle (BLANC ou NOIR)
     */
    void poser(int col, int prof, Couleur coul) {
        int niv = niveauLibre[col][prof];
        int idx = indice(col, prof, niv);
        if (coul == BLANC) blanc |= (u128)1 << idx;
        else               noir  |= (u128)1 << idx;
        hash ^= ZOBRIST_TABLE[idx][coul - 1];
        niveauLibre[col][prof]++;
    }

    /**
     * @brief Annule le dernier coup sur le bâtonnet (col, prof).
     *
     * Opération inverse de poser() — utilisée dans l'alpha-bêta
     * pour "défaire" un coup exploré.
     * Le hash Zobrist est restauré par XOR (XOR est son propre inverse).
     *
     * @param col  Colonne (0 à 3)
     * @param prof Profondeur (0 à 4)
     * @param coul Couleur de la perle à retirer
     */
    void annuler(int col, int prof, Couleur coul) {
        niveauLibre[col][prof]--;
        int niv = niveauLibre[col][prof];
        int idx = indice(col, prof, niv);
        if (coul == BLANC) blanc &= ~((u128)1 << idx);
        else               noir  &= ~((u128)1 << idx);
        hash ^= ZOBRIST_TABLE[idx][coul - 1];
    }

    /** @brief Retourne true si le bâtonnet (col, prof) est plein (4 perles). */
    bool estBatonnetPlein(int col, int prof) const {
        return niveauLibre[col][prof] == 4;
    }

    /**
     * @brief Retourne true si le plateau est complètement plein.
     * Dans ce cas, la partie est nulle.
     */
    bool plateauPlein() const {
        for (int c = 0; c < 4; c++)
            for (int p = 0; p < 5; p++)
                if (niveauLibre[c][p] < 4) return false;
        return true;
    }

    /**
     * @brief Retourne la liste de tous les coups jouables.
     *
     * Un coup est jouable si le bâtonnet correspondant n'est pas plein.
     * @return Vecteur de Move, un par bâtonnet non plein.
     */
    std::vector<Move> getCoupsPossibles() const {
        std::vector<Move> coups;
        for (int c = 0; c < 4; c++)
            for (int p = 0; p < 5; p++)
                if (!estBatonnetPlein(c, p))
                    coups.push_back(Move(c, p));
        return coups;
    }

    /**
     * @brief Vérifie si le dernier coup joué en (col, prof) est gagnant.
     *
     * PRINCIPE :
     *   On calcule l'indice de la dernière perle posée (niveauLibre - 1).
     *   On teste uniquement les ~6 masques passant par cette case.
     *   Si (couleur & masque) == masque → 4 perles alignées → victoire.
     *
     * @param col  Colonne du dernier coup
     * @param prof Profondeur du dernier coup
     * @param coul Couleur du joueur qui vient de jouer
     * @return true si ce coup crée un alignement de 4
     */
    bool estGagne(int col, int prof, Couleur coul) const {
        int niv = niveauLibre[col][prof] - 1;
        int idx = indice(col, prof, niv);
        u128 b  = (coul == BLANC) ? blanc : noir;
        for (int m : MASQUES_PAR_CASE[idx])
            if ((b & MASQUES[m]) == MASQUES[m]) return true;
        return false;
    }

    /**
     * @brief Retourne la valeur du bit idx pour la couleur donnée.
     * @param idx Indice de la case (0 à 79)
     * @param coul Couleur à tester
     * @return true si une perle de cette couleur est à cet indice
     */
    bool getBit(int idx, Couleur coul) const {
        u128 b = (coul == BLANC) ? blanc : noir;
        return (b >> idx) & 1;
    }

    /**
     * @brief Retourne le niveau de la dernière perle posée sur (col, prof).
     * Retourne -1 si le bâtonnet est vide.
     */
    int getNiveauActuel(int col, int prof) const {
        return niveauLibre[col][prof] - 1;
    }

    /**
     * @brief Retourne le prochain niveau libre sur le bâtonnet (col, prof).
     *
     * UTILISÉ PAR scoreFourchettes pour vérifier qu'une case est
     * physiquement accessible (seul ce niveau est jouable immédiatement).
     */
    int getNiveauLibre(int col, int prof) const {
        return niveauLibre[col][prof];
    }

    /** @brief Retourne le hash Zobrist de la position courante. */
    u64 getHash() const {
        return hash;
    }

    /** @brief Retourne le bitboard des perles noires. */
    u128 getNoir() const {
        return noir;
    }

    /** @brief Retourne le bitboard des perles blanches. */
    u128 getBlanc() const {
        return blanc;
    }
};

// ─── Définitions des membres statiques ───────────────────────────────────────
// Obligatoire en C++ : les membres statiques non-const doivent être définis
// dans exactement un fichier de traduction (.cpp).
// On les déclare ici avec "inline" (C++17) pour éviter un .cpp dédié.

inline u128             BitBoard::MASQUES[122];
inline std::vector<int> BitBoard::MASQUES_PAR_CASE[80];
inline bool             BitBoard::masquesInitialises = false;
inline u64              BitBoard::ZOBRIST_TABLE[80][2];
inline bool             BitBoard::zobristInitialise  = false;
inline int              BitBoard::nbMasquesTotal     = 0;
