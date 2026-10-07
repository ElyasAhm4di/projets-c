#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void lance_1000() {
    int t = 1000;
    int pile = 0;
    int face = 0;

    for (int i = 0; i < t; i++) {
        if (rand() % 2 == 0) {
            pile++;
        } else {
            face++;
        }
    }

    printf("Tirages : %d\n", t);
    printf("Pile : %d\n", pile);
    printf("Face : %d\n", face);
}

int main() {
    srand(time(NULL));
    lance_1000();
    return 0;
}
