#ifndef DICTIONARY_H
#define DICTIONARY_H

typedef struct Dictionary {
    int size;
    char** words;
} Dictionary;

Dictionary createDict( const char* );

int spellCheck( Dictionary, const char* );

void toLower( char* );

#endif
