#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const int WORD_LENGTH = 50;


typedef struct Dictionary {

    int size;           //number of words in dictionary
    char** words;       //array of strings for all words

} Dictionary;



Dictionary createDict(const char* path) {

    Dictionary dict;
    dict.size = 0;
    FILE* fp;

    if ((fp = fopen(path, "r")) == NULL) {
        printf("could not read supplied file\n");
        return dict;
    }

    char c;
    int lines = 0;

    while (c != EOF) {
       c = fgetc(fp);
        if (c == '\n')
            lines++;
    }

    rewind(fp);

    dict.size = lines;
    dict.words = malloc(lines * sizeof(char*));

    for ( int i = 0; i < dict.size; i++) {

        dict.words[i] = malloc(WORD_LENGTH * sizeof(char*));
        fgets(dict.words[i], WORD_LENGTH, fp);

        if (dict.words[i][strlen(dict.words[i])-1] == '\n') {

            dict.words[i][strlen(dict.words[i])-1] = 0;
        }

        toLower(dict.words[i]);
    }

    return dict;
}



int spellCheck(Dictionary dict, const char* word) {

    //Binary search - define low end (initally the first word in dict) and high end (initially the last word in dict)
    int low = 0,   high = dict.size -1;


    while (low <= high) {

        int middle = (low + high) / 2;

        int comp = strcmp(dict.words[middle], word);

        if (comp == 0) {

            return middle;

        } else if (comp < 0) {

            low = middle + 1;

        } else {

            high = middle - 1;
        }
    }
    return 0;
}


void toLower(char* str) {

    for (int i = 0; i < strlen(str); i++)
        str[i] = tolower(str[i]);

}

