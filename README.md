# Projets C

Mes programmes en C du BUT Informatique. Pour l'instant ce sont des TP : des petits programmes qui illustrent une notion à la fois. Les projets plus longs iront dans `projets/`, qui est vide aujourd'hui.

## Ce qu'il y a dedans

**`tp/R1.01_initiation/`**, initiation au développement (R1.01) :

- `pile_ou_face.c` tire 1000 fois à pile ou face et affiche les totaux.
- `complexite_algos.c` compare deux algorithmes (retirer 1 à chaque tour, contre diviser par 2) sur `n` = 100 000, 1 000 000 et 100 000 000. Il mesure le temps avec `clock()` pour montrer la différence entre une complexité linéaire et logarithmique.
- `boucles_imbriquees.c` compte les tours de deux boucles imbriquées, ce qui donne `n²`.

**`tp/R1.04_systemes/`**, systèmes d'exploitation (R1.04) :

- `tp5_memoire_partagee_semaphores.c` regroupe les exercices du TP5 : mémoire partagée anonyme et nommée entre un père et son fils, processus `writer` et `reader` indépendants, accès concurrent à un compteur avec et sans sémaphore. Un seul exécutable, on choisit l'exercice en argument.

## Compiler et lancer

Les trois premiers programmes se compilent seuls :

```bash
gcc -Wall -Wextra -std=c11 -o programme tp/R1.01_initiation/complexite_algos.c
./programme
```

Le TP5 utilise la mémoire partagée et les sémaphores POSIX, il faut donc Linux ou macOS (sous Windows, passer par WSL) :

```bash
gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L \
    -o tp5 tp/R1.04_systemes/tp5_memoire_partagee_semaphores.c -pthread -lrt
./tp5 ex1b      # ex1b | ex1c | writer | reader | ex2a | ex2b | writer-sync | reader-sync
```

Le contraste entre `ex2a` et `ex2b` est le but du TP : sans sémaphore, le compteur partagé finit avec une valeur imprévisible (lors d'un essai : 74 152 au lieu de 0), avec sémaphore il vaut bien 0. Les modes `writer` et `reader` se lancent dans deux terminaux différents.

## État

Les quatre programmes compilent sans avertissement avec `gcc`, et `ex1b`, `ex2a` et `ex2b` du TP5 donnent le résultat attendu à l'exécution. Il n'y a pas de tests automatisés ni de `Makefile` : ce sont des TP, pas des projets finis.

J'ai laissé de côté deux brouillons qui ne compilent pas (un début de TP sur `read`/`write` et un début de grille en C). Ils reviendront quand ils fonctionneront.
