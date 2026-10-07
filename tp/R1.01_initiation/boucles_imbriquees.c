#include <stdio.h>

int main() {
    int n;
    int S = 0;
    
  
    printf(" n : ");
    scanf("%d", &n);
    

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            S = S + 1;
        }
    }
    
    printf(" S est : %d\n", S);
    
    return 0;
}