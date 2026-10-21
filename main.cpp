#include <iostream>
#include <string>
#include <cstdlib>

#include "Echiquier.h"

using namespace std;

// Référence OEIS A000170 (nombre de solutions)
static long long solutionsConnues(int n)
{
    switch (n) {
        case 1:  return 1;
        case 2:  return 0;
        case 3:  return 0;
        case 4:  return 2;
        case 5:  return 10;
        case 6:  return 4;
        case 7:  return 40;
        case 8:  return 92;
        case 9:  return 352;
        case 10: return 724;
        case 11: return 2680;
        case 12: return 14200;
        case 13: return 73712;
        case 14: return 365596;
        case 15: return 2279184;
        case 16: return 14772512;
        case 17: return 95815104;
        case 18: return 666090624;
        default: return -1;
    }
}

int main(int argc, char** argv)
{
    int n = 8;
    if (argc > 1) n = atoi(argv[1]);
    else {
        cout << "Entrez N (defaut 8) : ";
        if (!(cin >> n)) n = 8;
    }

    if (n < Echiquier::MIN_N || n > Echiquier::MAX_N) {
        cerr << "Erreur : N doit etre entre " << Echiquier::MIN_N
             << " et " << Echiquier::MAX_N << "\n";
        return 1;
    }

    // Avertissements graduels selon la taille
    if (n > Echiquier::MAX_N_RAISONNABLE) {
        cout << "\n[ATTENTION] N=" << n << " depasse le seuil raisonnable ("
             << Echiquier::MAX_N_RAISONNABLE << ").\n";
        if (n <= 17)      cout << "Estimation : quelques minutes.\n";
        else if (n <= 19) cout << "Estimation : heures.\n";
        else if (n <= 22) cout << "Estimation : jours ou plus.\n";
        else              cout << "Estimation : annees, siecles, voire plus.\n";
        cout << "Continuer ? (o/n) : ";
        char c; cin >> c;
        if (c != 'o' && c != 'O') return 0;
    }

    cout << "\n=================================\n";
    cout << "  PROBLEME DES " << n << " REINES\n";
    cout << "=================================\n\n";

    Echiquier echiquier(n);
    long long count = echiquier.resoudre();

    cout << "[RESULTATS]\n";
    if (echiquier.hasSolution())
        cout << "  Une solution existe pour N=" << n << ".\n";
    else
        cout << "  AUCUNE solution n'existe pour N=" << n << ".\n";
    echiquier.afficherStatistiques();

    // Vérification croisée
    long long ref = solutionsConnues(n);
    if (ref >= 0) {
        cout << "  Verification OEIS    : "
             << (ref == count ? "OK" : "ERREUR")
             << " (" << ref << " attendu)\n";
    }

    // Affichage de la 1ère solution (si raisonnable)
    if (echiquier.hasSolution() && n <= 20) {
        cout << "\n[PREMIERE SOLUTION]\n";
        echiquier.afficherPremiereSolution();
    }

    cout << "\n";
    return 0;
}
