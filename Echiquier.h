#ifndef ECHIQUIER_H_INCLUDED
#define ECHIQUIER_H_INCLUDED

#include <vector>
#include <string>
#include <chrono>
#include <cstdint>

class Echiquier {
public:
    static const int MIN_N = 1;
    // "Garde-fou" mémoire/sécurité, PAS une limite algorithmique.
    // N > 1000 ferait déborder la pile de récursion, mais N > 30
    // est de toute façon infaisable à compter.
    static const int MAX_N = 1000;
    // Au-delà, on prévient l'utilisateur que ça peut être très long.
    static const int MAX_N_RAISONNABLE = 15;

    explicit Echiquier(int n = 8);

    long long resoudre();

    void afficherPremiereSolution() const;
    void afficherStatistiques()     const;

    int       getN()               const { return m_N; }
    long long getSolutionCount()   const { return m_solutionCount; }
    double    getElapsedUs()       const { return m_elapsedUs; }
    double    getFirstSolutionUs() const { return m_firstSolutionUs; }
    bool      hasSolution()        const { return m_solutionCount > 0; }

private:
    using Clock = std::chrono::high_resolution_clock;

    int m_N;
    std::vector<int> m_colOfRow;         // col de la reine par ligne
    std::vector<int> m_premiereSolution; // 1ère solution trouvée

    long long        m_solutionCount;
    double           m_elapsedUs;
    double           m_firstSolutionUs;
    bool             m_foundFirst;
    Clock::time_point m_startTime;

    // Chemins d'exécution
    void resoudreRecBitmask (int ligne, uint64_t cols,
                             uint64_t diag1, uint64_t diag2);
    void resoudreRecClassique(int ligne);
    bool positionSureClassique(int ligne, int col) const;

    void enregistrerSolution();
};

std::string formatTime(double us);

#endif
