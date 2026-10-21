#include "Echiquier.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

// ------------------------------------------------------------------
// Index du bit de poids faible (portable GCC/Clang/MSVC)
// ------------------------------------------------------------------
static inline int lowestBitIndex(uint64_t x)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(x);
#elif defined(_MSC_VER)
    unsigned long i;
    _BitScanForward64(&i, x);
    return static_cast<int>(i);
#else
    int i = 0;
    while ((x & 1) == 0) { x >>= 1; ++i; }
    return i;
#endif
}

// ------------------------------------------------------------------
// Formatage des temps
// ------------------------------------------------------------------
string formatTime(double us)
{
    ostringstream oss;
    oss << fixed;
    if (us >= 1'000'000.0)
        oss << setprecision(3) << (us / 1'000'000.0) << " s";
    else if (us >= 1'000.0)
        oss << setprecision(3) << (us / 1'000.0) << " ms";
    else
        oss << setprecision(2) << us << " us";
    return oss.str();
}

// ------------------------------------------------------------------
// Constructeur
// ------------------------------------------------------------------
Echiquier::Echiquier(int n)
    : m_N(n),
      m_colOfRow(n, -1),
      m_solutionCount(0),
      m_elapsedUs(0.0),
      m_firstSolutionUs(0.0),
      m_foundFirst(false)
{
}

// ------------------------------------------------------------------
// Enregistrement d'une solution (appelé au cas de base)
// ------------------------------------------------------------------
void Echiquier::enregistrerSolution()
{
    ++m_solutionCount;
    if (!m_foundFirst) {
        m_foundFirst      = true;
        m_premiereSolution = m_colOfRow;
        auto now = Clock::now();
        m_firstSolutionUs = chrono::duration<double, micro>(
                                now - m_startTime).count();
    }
}

// ------------------------------------------------------------------
// Backtracking avec bitmasks — O(1) par placement
//
//   cols  : bits des colonnes occupées
//   diag1 : bits des diagonales "\" occupées, déjà décalées pour
//           la ligne courante (chaque ligne ajoute un cran à gauche)
//   diag2 : bits des diagonales "/" occupées, déjà décalées pour
//           la ligne courante (chaque ligne ajoute un cran à droite)
// ------------------------------------------------------------------
void Echiquier::resoudreRecBitmask(int ligne, uint64_t cols,
                                   uint64_t diag1, uint64_t diag2)
{
    if (ligne == m_N) {
        enregistrerSolution();
        return;
    }

    // Masque des colonnes disponibles sur cette ligne
    const uint64_t full      = (m_N == 64) ? ~0ULL : ((1ULL << m_N) - 1);
    uint64_t       available = full & ~(cols | diag1 | diag2);

    while (available) {
        // Isoler le bit de poids le plus faible
        uint64_t bit = available & (~available + 1);
        available ^= bit;

        int col = lowestBitIndex(bit);
        m_colOfRow[ligne] = col;

        // Récursion : mettre à jour les trois masques
        resoudreRecBitmask(ligne + 1,
                           cols  | bit,
                           (diag1 | bit) << 1,
                           (diag2 | bit) >> 1);
    }
    m_colOfRow[ligne] = -1;
}

// ------------------------------------------------------------------
// Fallback pour N > 64 (bitmask 64 bits insuffisant).
// Représentation 1D, vérification O(N) — lent mais correct.
// ------------------------------------------------------------------
bool Echiquier::positionSureClassique(int ligne, int col) const
{
    for (int i = 0; i < ligne; ++i) {
        int c = m_colOfRow[i];
        if (c == col) return false;
        if (std::abs(c - col) == ligne - i) return false;
    }
    return true;
}

void Echiquier::resoudreRecClassique(int ligne)
{
    if (ligne == m_N) {
        enregistrerSolution();
        return;
    }
    for (int col = 0; col < m_N; ++col) {
        if (positionSureClassique(ligne, col)) {
            m_colOfRow[ligne] = col;
            resoudreRecClassique(ligne + 1);
        }
    }
    m_colOfRow[ligne] = -1;
}

// ------------------------------------------------------------------
// Point d'entrée public
// ------------------------------------------------------------------
long long Echiquier::resoudre()
{
    m_solutionCount   = 0;
    m_elapsedUs       = 0.0;
    m_firstSolutionUs = 0.0;
    m_foundFirst      = false;
    m_premiereSolution.clear();
    fill(m_colOfRow.begin(), m_colOfRow.end(), -1);

    m_startTime = Clock::now();

    if (m_N <= 64)
        resoudreRecBitmask(0, 0, 0, 0);
    else
        resoudreRecClassique(0);

    auto end    = Clock::now();
    m_elapsedUs = chrono::duration<double, micro>(end - m_startTime).count();

    return m_solutionCount;
}

// ------------------------------------------------------------------
// Affichage
// ------------------------------------------------------------------
void Echiquier::afficherPremiereSolution() const
{
    if (!m_foundFirst) {
        cout << "(aucune solution)\n";
        return;
    }
    for (int i = 0; i < m_N; ++i) {
        for (int j = 0; j < m_N; ++j)
            cout << (m_premiereSolution[i] == j ? "Q " : ". ");
        cout << "\n";
    }
}

void Echiquier::afficherStatistiques() const
{
    cout << "  - Solutions trouvees  : " << m_solutionCount << "\n";
    cout << "  - Temps total         : " << formatTime(m_elapsedUs) << "\n";
    if (m_foundFirst)
        cout << "  - Temps 1ere solution : " << formatTime(m_firstSolutionUs) << "\n";
    else
        cout << "  - Temps 1ere solution : - (aucune)\n";
}
