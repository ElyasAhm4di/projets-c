#include <stdio.h>
#include <time.h>

void algo1(long long n, long long *res_n, long long *res_c) {
    *res_c = 0;
    while (n > 1) {
        n = n - 1;
        *res_c = *res_c + 1;
    }
    *res_n = n;
}

void algo2(long long n, long long *res_n, long long *res_c) {
    *res_c = 0;
    while (n > 1) {
        n = n / 2;
        *res_c = *res_c + 1;
    }
    *res_n = n;
}

int main() {
    long long valeurs_n[] = {100000, 1000000, 100000000};
    int nb_valeurs = 3;
    long long res_n, res_c;
    clock_t debut, fin;
    double temps;

    for (int i = 0; i < nb_valeurs; i++) {
        long long n = valeurs_n[i];
        printf("--- Tests pour n = %lld ---\n", n);

        debut = clock();
        algo1(n, &res_n, &res_c);
        fin = clock();
        temps = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Algo 1 : Resultat=(%lld, %lld) | Temps = %f s\n", res_n, res_c, temps);

        debut = clock();
        algo2(n, &res_n, &res_c);
        fin = clock();
        temps = (double)(fin - debut) / CLOCKS_PER_SEC;
        printf("Algo 2 : Resultat=(%lld, %lld) | Temps = %f s\n\n", res_n, res_c, temps);
    }

    return 0;
}