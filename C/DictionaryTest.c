

#include "Dictionary.h"

int main(){

    Dictionary D = newDictionary(0);
    int z = 5;
    int* zp = &z;
    printf("dictionary made, unique = %s\n",getUnique(D)?"TRUE":"FALSE");
    insert(D,"m",zp);
    puts("inserted first element");
    insert(D,"a",zp);
    insert(D,"b",zp);
    insert(D,"c",zp);
    insert(D,"d",zp);
    insert(D,"e",zp);
    insert(D,"f",zp);
    insert(D,"g",zp);
    insert(D,"h",zp);
    insert(D,"i",zp);
    puts("elements inserted");

    printf("preorder traversal: \n");
    printDictionary(stdout,D,"preorder");
    printf("\npostorder traversal: \n");
    printDictionary(stdout,D,"PoStOrDeR");
    printf("\ninorder traversal: \n");
    printDictionary(stdout,D,"INORDER");

    delete(D,"d");
    delete(D, "h");
    delete(D,"f");
    printf("\ninorder traversal(some elements deleted): \n");
    printDictionary(stdout,D,"IN");
    puts("lookup \"tests\"");

    printf("lookup d: %s\n", lookup(D,"a") == zp ? "TRUE":"FALSE");
    printf("lookup z: %s\n", lookup(D,"z") == zp ? "TRUE":"FALSE");
    printf("lookup d: %s\n", lookup(D,"d") == zp ? "TRUE":"FALSE");
    puts("forward iteration:");

    for(beginForward(D);currentKey(D) != KEY_UNDEF; next(D)){
        printf("%s: %p\n",currentKey(D),currentVal(D));
    }
    puts("backward iteration:");
    for(beginReverse(D);currentKey(D) != KEY_UNDEF; prev(D)){
        printf("%s: %p\n",currentKey(D),currentVal(D));
    }
    makeEmpty(D);
    printf("D size should be 0: %d\n",size(D));
    freeDictionary(&D);
}