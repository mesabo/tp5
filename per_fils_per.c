#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MSG1 "Salut fils,quel mot secret veux tu cherher ? "
#define MSG3 "Fils ton pere t'aime !"
#define NON "0"

#define BUFFER_SIZE 1000

/* Function declarations */
int indexOf(FILE *fptr, const char *word, int *line, int *col);

int main()
{
    FILE *fptr;
    char path[100];
    char word[50];
    char asw[50];
    int line, col;
    int tube1[2], tube2[2], tube3[3];
    int pid;
    int REPONSE = 0;
    pipe(tube1);
    pipe(tube2);
    pipe(tube3);

    pid = (int)fork();
    if (pid == 0)
    { /* le processus fils */
        close(tube3[1]);
        close(tube2[0]);
        close(tube1[1]);

        read(tube1[0], MSG1, sizeof(MSG1));
        printf("%s\n\n", MSG1);

        /* Input word to search in file */
        printf("Saisir mot: ");
        scanf("%s", word);
        write(tube2[1], &word, sizeof(word));

        read(tube3[0], &REPONSE, sizeof(REPONSE));
        printf("\nApres recherche, la reponse est: %d\n", REPONSE);
        exit(0);
    }
    else
    { /* Processus père*/
        /* Essayez d'ouvrir le fichier */
        fptr = fopen("test.txt", "r");
        if (fptr == NULL)
        {
            printf("Impossible d'ouvrir le fichier.\n");
            printf("Veuillez vérifier que vous disposez des privilèges de lecture / écriture.\n");
            exit(EXIT_FAILURE);
        }

        close(tube1[0]);
        close(tube2[1]);
        close(tube3[0]);

        write(tube1[1], MSG1, sizeof(MSG1));

        read(tube2[0], &word, sizeof(word));

        //Nous recherchons et envoyons le resultat
        // Find index of word in fptr
        indexOf(fptr, word, &line, &col);
        if (line != -1)
        {
            REPONSE = 1;
            write(tube3[1], &REPONSE, sizeof(REPONSE));
        }
        else
        {
            REPONSE = 0;
            write(tube3[1], &REPONSE, sizeof(REPONSE));
        }
        // Close file

        close(tube1[1]);
        close(tube2[0]);
        close(tube3[1]);
    }
    fclose(fptr);
    return 0;
}

/* 
* Dans cette fonction, nous recherchons le mot en question
 */
int indexOf(FILE *fptr, const char *word, int *line, int *col)
{
    char str[BUFFER_SIZE];
    char *pos;

    *line = -1;
    *col = -1;

    while ((fgets(str, BUFFER_SIZE, fptr)) != NULL)
    {
        *line += 1;
        // Rechercher la première occurrence du mot dans str
        pos = strstr(str, word);

        if (pos != NULL)
        {
            // Le premier index du mot dans str est
            // Adresse mémoire de pos - mémoire
            // adresse de str.
            *col = (pos - str);
            break;
        }
    }

    // Si le mot n'est pas trouvé, définissez la ligne sur -1
    if (*col == -1)
        *line = -1;

    return *col;
}