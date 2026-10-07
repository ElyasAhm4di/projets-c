#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 100000
#define MESSAGE_SIZE 128
#define SHM_NAME "/tp5_message"
#define SEM_NAME "/tp5_message_ready"

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

static void wait_for_child(pid_t pid)
{
    if (waitpid(pid, NULL, 0) == -1)
        die("waitpid");
}

static char *create_shared_message(int *fd)
{
    *fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (*fd == -1)
        die("shm_open");
    if (ftruncate(*fd, MESSAGE_SIZE) == -1)
        die("ftruncate");

    char *message = mmap(NULL, MESSAGE_SIZE, PROT_READ | PROT_WRITE,
                         MAP_SHARED, *fd, 0);
    if (message == MAP_FAILED)
        die("mmap");
    return message;
}

static char *open_shared_message(int *fd)
{
    *fd = shm_open(SHM_NAME, O_RDWR, 0600);
    if (*fd == -1)
        die("shm_open (lancer writer avant reader)");

    char *message = mmap(NULL, MESSAGE_SIZE, PROT_READ | PROT_WRITE,
                         MAP_SHARED, *fd, 0);
    if (message == MAP_FAILED)
        die("mmap");
    return message;
}

static void close_shared_message(char *message, int fd)
{
    if (munmap(message, MESSAGE_SIZE) == -1)
        die("munmap");
    if (close(fd) == -1)
        die("close");
}

/* Exercice 1.b : mémoire partagée anonyme entre un père et son fils. */
static void exercise1b(void)
{
    char *message = mmap(NULL, MESSAGE_SIZE, PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (message == MAP_FAILED)
        die("mmap");

    pid_t pid = fork();
    if (pid == -1)
        die("fork");
    if (pid == 0) {
        snprintf(message, MESSAGE_SIZE, "bonjour");
        _exit(EXIT_SUCCESS);
    }

    wait_for_child(pid);
    printf("Pere : %s\n", message);
    if (munmap(message, MESSAGE_SIZE) == -1)
        die("munmap");
}

/* Exercice 1.c : mémoire partagée nommée entre un père et son fils. */
static void exercise1c(void)
{
    int fd;
    char *message = create_shared_message(&fd);
    message[0] = '\0';

    pid_t pid = fork();
    if (pid == -1)
        die("fork");
    if (pid == 0) {
        snprintf(message, MESSAGE_SIZE, "bonjour");
        close_shared_message(message, fd);
        _exit(EXIT_SUCCESS);
    }

    wait_for_child(pid);
    printf("Pere : %s\n", message);
    close_shared_message(message, fd);
    if (shm_unlink(SHM_NAME) == -1)
        die("shm_unlink");
}

/* Exercice 1.d : processus writer indépendant. */
static void writer(void)
{
    int fd;
    char *message = create_shared_message(&fd);
    snprintf(message, MESSAGE_SIZE, "bonjour");
    printf("Writer : message ecrit\n");
    close_shared_message(message, fd);
}

/* Exercice 1.d : processus reader indépendant, synchronisé provisoirement
   par sleep comme demandé dans l'énoncé. */
static void reader(void)
{
    sleep(1);
    int fd;
    char *message = open_shared_message(&fd);
    printf("Reader : %s\n", message);
    close_shared_message(message, fd);
    if (shm_unlink(SHM_NAME) == -1)
        die("shm_unlink");
}

static int *create_shared_counter(int *fd)
{
    *fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (*fd == -1)
        die("shm_open");
    if (ftruncate(*fd, sizeof(int)) == -1)
        die("ftruncate");

    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                        MAP_SHARED, *fd, 0);
    if (counter == MAP_FAILED)
        die("mmap");
    return counter;
}

static void close_shared_counter(int *counter, int fd)
{
    if (munmap(counter, sizeof(*counter)) == -1)
        die("munmap");
    if (close(fd) == -1)
        die("close");
}

/* Exercice 2.a : accès concurrent sans exclusion mutuelle. */
static void exercise2a(void)
{
    int fd;
    int *counter = create_shared_counter(&fd);
    *counter = 0;

    pid_t pid = fork();
    if (pid == -1)
        die("fork");
    if (pid == 0) {
        for (int i = 0; i < N; ++i)
            ++(*counter);
        printf("Fils : %d\n", *counter);
        _exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < N; ++i)
        --(*counter);
    wait_for_child(pid);
    printf("Pere : %d (resultat attendu : 0)\n", *counter);
    close_shared_counter(counter, fd);
    if (shm_unlink(SHM_NAME) == -1)
        die("shm_unlink");
}

/* Exercice 2.b : exclusion mutuelle de la section critique. */
static void exercise2b(void)
{
    int fd;
    int *counter = create_shared_counter(&fd);
    *counter = 0;

    sem_t *sem = mmap(NULL, sizeof(*sem), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED)
        die("mmap");
    if (sem_init(sem, 1, 1) == -1)
        die("sem_init");

    pid_t pid = fork();
    if (pid == -1)
        die("fork");
    if (pid == 0) {
        for (int i = 0; i < N; ++i) {
            sem_wait(sem);
            ++(*counter);
            sem_post(sem);
        }
        _exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < N; ++i) {
        sem_wait(sem);
        --(*counter);
        sem_post(sem);
    }
    wait_for_child(pid);
    printf("Resultat avec semaphore : %d (attendu : 0)\n", *counter);
    if (sem_destroy(sem) == -1)
        die("sem_destroy");
    if (munmap(sem, sizeof(*sem)) == -1)
        die("munmap");
    close_shared_counter(counter, fd);
    if (shm_unlink(SHM_NAME) == -1)
        die("shm_unlink");
}

/* Exercice 2.c : reader attend le signal du writer. */
static void synchronized_writer(void)
{
    int fd;
    char *message = create_shared_message(&fd);
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
    if (sem == SEM_FAILED && errno == EEXIST) {
        sem_unlink(SEM_NAME);
        sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
    }
    if (sem == SEM_FAILED)
        die("sem_open");

    snprintf(message, MESSAGE_SIZE, "bonjour");
    printf("Writer : message ecrit\n");
    if (sem_post(sem) == -1)
        die("sem_post");
    sem_close(sem);
    close_shared_message(message, fd);
}

static void synchronized_reader(void)
{
    int fd;
    char *message = open_shared_message(&fd);
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED)
        die("sem_open (lancer writer-sync avant reader-sync)");
    if (sem_wait(sem) == -1)
        die("sem_wait");

    printf("Reader : %s\n", message);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    close_shared_message(message, fd);
    if (shm_unlink(SHM_NAME) == -1)
        die("shm_unlink");
}

static void usage(const char *program)
{
    fprintf(stderr,
            "Usage : %s ex1b | ex1c | writer | reader | ex2a | ex2b |\n"
            "         writer-sync | reader-sync\n",
            program);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "ex1b") == 0)
        exercise1b();
    else if (strcmp(argv[1], "ex1c") == 0)
        exercise1c();
    else if (strcmp(argv[1], "writer") == 0)
        writer();
    else if (strcmp(argv[1], "reader") == 0)
        reader();
    else if (strcmp(argv[1], "ex2a") == 0)
        exercise2a();
    else if (strcmp(argv[1], "ex2b") == 0)
        exercise2b();
    else if (strcmp(argv[1], "writer-sync") == 0)
        synchronized_writer();
    else if (strcmp(argv[1], "reader-sync") == 0)
        synchronized_reader();
    else {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
