/*
 * Sam Wortzman
 * CruzID: swortzma
 * Assignment: pa6
 */

#include "Dictionary.h"
#include <stdlib.h>
#include <string.h>

char* getLineEZ(FILE* fileIn){
    int count = 0;
    int c = fgetc(fileIn);
    if(c == EOF){
        return NULL;
    }
    int size = 5;
    char* bfr = (char*)malloc(sizeof(char)*size);
    while(!feof(fileIn)){
        if(count == size - 1){
            bfr = (char*)realloc(bfr,size=size*2);
        }
        if(c == '\n'){
            break;
        }
        bfr[count++] = c;
        c = fgetc(fileIn);
    }
    bfr[count] = '\0';
    return bfr;
}

int main(int argc, char** argv){
    
    if(argc != 3){
        fprintf(stderr,"Usage error: Requires exactly 2 args, %d were given",argc - 1);
        exit(1);
    }
    char* strIn = argv[1];
    char* strOut = argv[2];
    
    FILE* fileIn = fopen(strIn,"r");
    Dictionary D = newDictionary(0);
    char* temp;
    while( (temp = getLineEZ(fileIn)) ){
        insert(D,temp,NULL);
    }
    fclose(fileIn);
    FILE* fileOut = fopen(strOut,"w");
    char* key;
    fprintf(fileOut,"******************************************************\n");
    fprintf(fileOut,"PRE-ORDER:\n");
    fprintf(fileOut,"******************************************************\n");
    printDictionary(fileOut,D,"preOrder");
    fprintf(fileOut,"\n\n");
    
    fprintf(fileOut,"******************************************************\n");
    fprintf(fileOut,"IN-ORDER:\n");
    fprintf(fileOut,"******************************************************\n");
    printDictionary(fileOut,D,"inOrder");
    fprintf(fileOut,"\n\n");

    fprintf(fileOut,"******************************************************\n");
    fprintf(fileOut,"POST-ORDER:\n");
    fprintf(fileOut,"******************************************************\n");
    printDictionary(fileOut,D,"postOrder");

    beginForward(D);
    while(1){
        key = currentKey(D);
        if(key == KEY_UNDEF){
            break;
        }
        //fprintf(fileOut,"%s",key);
        free(key);
        next(D);
    }
    fclose(fileOut);
    freeDictionary(&D);
    return EXIT_SUCCESS;
}