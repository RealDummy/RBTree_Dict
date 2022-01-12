/*
 * Sam Wortzman
 * CruzID: swortzma
 * Assignment: pa5
 */

#include "Dictionary.h"
#include <stdint.h>

//#define DEBUG

#ifdef DEBUG
#define dPrint(x) puts(x);
#define dPrintf(name,format,var) printf("%s: ",name); printf(format,var); printf("\n");
#else //DEBUG not def
#define dPrint(x)
#define dPrintf(name,format,var)
#endif //DEBUG

#define RED (uint8_t)(1)
#define BLACK (uint8_t)(0)

#define DictError(op,pre) fprintf(stderr,"Dict Error: Calling %s on %s\n",op,pre); exit(1);
#define DictNullGuard(dict,op) if(!(dict)) {DictError(op,"null ptr");}


struct NodeObj{
    uint8_t color;
    struct NodeObj* left;
    struct NodeObj* right;
    struct NodeObj* parent;
    KEY_TYPE key;
    VAL_TYPE data;
};

typedef struct NodeObj* Node;

struct DictionaryObj{
    Node head;
    Node NIL;
    int size;
    int unique;
    Node iter;
};

Node newNode(Dictionary D, VAL_TYPE x){
    Node ret = (Node)malloc(sizeof(struct NodeObj));
    if(!ret)exit(1);
    ret->data = x;
    ret->color = RED;
    ret->left = D->NIL;
    ret->right = D->NIL;
    return ret;
}

//not sure ill need->->->
Node nilNode(){
    Node ret = (Node)malloc(sizeof(struct NodeObj));
    if(!ret)exit(1);
    ret->data = VAL_UNDEF;
    ret->key = KEY_UNDEF;
    ret->color = BLACK;
    ret->left = ret;
    ret->right = ret;
    return ret;
}
Dictionary newDictionary(int unique){
    Dictionary dict = (Dictionary)malloc(sizeof(struct DictionaryObj));
    if(!dict){
        exit(1);
    }
    dict->NIL = nilNode();
    dict->head = dict->NIL;
    dict->unique = unique;
    dict->size = 0;
    dict->iter = NULL;
    return dict;
}

Node findNode(Dictionary D, KEY_TYPE k);
void freeNodes(Dictionary D, Node head);

void freeDictionary(Dictionary* pD){
    if(!pD){
        DictError("freeDictionary","null Dictionary pointer pointer");
    }
    Dictionary D = *pD;
    DictNullGuard(D,"freeDictionary");
    freeNodes(D,D->head);
    free(D->NIL);
    free(D);
    *pD = NULL;
}

int size(Dictionary D){
    DictNullGuard(D,"size");
    return D->size;
}

int getUnique(Dictionary D){
    DictNullGuard(D,"getUnique");
    if(D->unique){
        return 1;
    }
    return 0;
}

VAL_TYPE lookup(Dictionary D, KEY_TYPE k){
    Node z = findNode(D,k);
    if(z){
        return z->data;
    }
    return VAL_UNDEF;
}

void push_black(Node u){
    u->color = RED;
    u->left->color = BLACK;
    u->right->color = BLACK;
}

//returns 1 if not nil, 0 if nil
int notNil(Dictionary D, Node u){
    return (u != D->NIL ? 1 : 0);
}

void pull_black(Node u){
    u->color = BLACK;
    u->left->color = RED;
    u->right->color = RED;
}

void swap_colors(Dictionary D,Node a, Node b){
    dPrint("swapping colors")
    uint8_t temp = a->color;
    a->color = b->color;
    b->color = temp;
    D->NIL->color = BLACK;
    dPrint("colors swapped")
    
}

void rotate_left(Dictionary D, Node x){
    dPrint("rotate left starting")
    Node y = x->right;
    //dPrintf("y","%p",y);
    x->right = y->left;
    if(notNil(D,y->left)){
        y->left->parent = x;
    }
    y->parent = x->parent;
    if(x == D->head){
        D->head = y;
    }
    else if(x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    //dPrint("rot left finish")
}

void rotate_right(Dictionary D, Node x){
    dPrint("rotate right starting")
    Node y = x->left;
    //dPrintf("y","%p",y);
    x->left = y->right;
    if(notNil(D,y->right)){
        y->right->parent = x;
    }
    y->parent = x->parent;
    if(x == D->head){
        D->head = y;
    }
    else if(x == x->parent->right){
        x->parent->right = y;
    }
    else{
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}


void flip_left(Dictionary D, Node u){
    swap_colors(D,u,u->right);
    rotate_left(D,u);
}

void flip_right(Dictionary D, Node u){
    swap_colors(D,u,u->left);
    rotate_right(D,u);
}

void add_fixup(Dictionary D, Node z){
    //dPrint("add_Fixup start")
    while(z->parent->color == RED){
        if(z->parent == z->parent->parent->left){
            Node y = z->parent->parent->right;
            if(y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else{
                if(z == z->parent->right){
                    z = z->parent;
                    rotate_left(D,z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_right(D,z->parent->parent);
            }
        }
        else{ // z->parent is a right child
            Node y = z->parent->parent->left;
            if (y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else{
                if(z == z->parent->left){
                    z = z->parent;
                    rotate_right(D,z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotate_left(D,z->parent->parent);
            }
        }
    }
    D->head->color = BLACK;
    D->NIL->color = BLACK;
}

void add_node(Dictionary D, Node head, Node u, int unique){
    //dPrint("addNode")
    
    while(notNil(D,head)){
        int compare = KEY_CMP(u->key,head->key);
        if(unique && compare == 0){
            DictError("insert","non-unique key");
        }
        Node* next = compare >= 0 ? &(head->right) : &(head->left);
        if(!notNil(D,*next)){
            *next = u;
            u->parent = head;
            return;
        }
        head = *next;
    }
}

void insert(Dictionary D, KEY_TYPE k, VAL_TYPE v){
    Node u = newNode(D,v);
    u->key = k;
    //dPrintf("D->head","%p",D->head)
    if(D->size != 0){
        add_node(D,D->head,u,D->unique);
        add_fixup(D,u);
    }
    else{
        D->head = u;
        u->color = BLACK;
        u->parent = D->NIL;
    }
    D->size += 1;
    //dPrint("insert finished")
}
void transplant(Dictionary D, Node u, Node v){
    if (u->parent == D->NIL){
        D->head = v;
    }
    else if (u == u->parent->left){
        u->parent->left = v;
    }
    else{
        u->parent->right = v;
    }
    v->parent = u->parent;
}

Node findNode(Dictionary D, KEY_TYPE k){
    Node head = D->head;
    while(notNil(D,head)){
        int cmp = KEY_CMP(k,head->key);
        if (cmp == 0){
            return head;
        }
        head = cmp >= 0 ? head->right : head->left;
    }
    return NULL;
}

void delete_fixup(Dictionary D, Node x){
    while(x->color == BLACK && x != D->head){
        if(x == x->parent->left){
            Node w = x->parent->right;
            if(w->color == RED){
                w->color = BLACK;
                x->parent->color = RED;
                rotate_left(D,x->parent);
                w = x->parent->right;
            }
            if(w->left->color == BLACK && w->left->color == BLACK){
                w->color = RED;
                x = x->parent;
            }
            else{
                if(w->right->color == BLACK){
                    w->left->color = BLACK;         
                    w->color = RED;                 
                    rotate_right(D, w);              
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotate_left(D,x->parent);
                x = D->head;
            }
        }
        else{ // x is a right child
            Node w = x->parent->left;
            if(w->color == RED){
                w->color = BLACK;
                x->parent->color = RED;
                rotate_right(D,x->parent);
                w = x->parent->left;
            }
            if(w->right->color == BLACK && w->left->color == BLACK){
                w->color = RED;
                x = x->parent;
            }
            else{
                if(w->left->color == BLACK){
                    w->right->color = BLACK;              
                    w->color = RED;                       
                    rotate_left(D, w);                    
                    w = x->parent->left;
                }
                w->color = x->parent->color;              
                x->parent->color = BLACK;                 
                w->left->color = BLACK;                   
                rotate_right(D, x->parent);               
                x = D->head;
            }
        }
    }
    x->color = BLACK;
}

void freeNodes(Dictionary D, Node head){
    if(!notNil(D,head)){
        return;
    }
    freeNodes(D,head->left);
    freeNodes(D,head->right);
    free(head);
}

void delete_node(Dictionary D, Node z){
    Node y = z;
    uint8_t yColor = y->color;
    Node x;
    if (z->left == D->NIL){
        x = z->right;
        transplant(D, z, z->right);
    }
   else if (z->right == D->NIL){
      x = z->left;
      transplant(D, z, z->left);
   }
   else{
        y = z->right;
        //treemin;
        while(y->left != D->NIL){
            y = y->left;
        }
        yColor = y->color;
        x = y->right;
        if (y->parent == z){
            x->parent = y;
        }
        else{
            transplant(D, y, y->right);
            y->right = z->right;
            y->right->parent = y;
      }
      transplant(D, z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
   }
   if (yColor == BLACK){
       delete_fixup(D, x);
   }
      
}



void delete(Dictionary D, KEY_TYPE k){
    Node n = findNode(D,k);
    if(!n){
        DictError("delete","key not in Dictionary");
    }
    if(n == D->iter) D->iter = NULL;
    delete_node(D,n);
    free(n);
    D->size -= 1;
}



void makeEmpty(Dictionary D){
    DictNullGuard(D,"makeEmpty");
    freeNodes(D,D->head);
    D->head = D->NIL;
    D->iter = NULL;
    D->size = 0;
}

VAL_TYPE beginForward(Dictionary D){
    DictNullGuard(D,"beginForward");
    if(D->size < 1){
        return VAL_UNDEF;
    }
    D->iter = D->head;
    while(notNil(D,D->iter->left)){
        D->iter = D->iter->left;
    }
    return D->iter->data;
    
}

VAL_TYPE beginReverse(Dictionary D){
    DictNullGuard(D,"beginbeginReverse");
    if(D->size < 1){
        return VAL_UNDEF;
    }
    D->iter = D->head;
    while(notNil(D,D->iter->right)){
        D->iter = D->iter->right;
    }
    return D->iter->data;
}

KEY_TYPE currentKey(Dictionary D){
    DictNullGuard(D,"currentKey");
    if(D->iter){
        return D->iter->key;
    }
    return KEY_UNDEF;
}

VAL_TYPE currentVal(Dictionary D){
    DictNullGuard(D,"currentVal");
    if(D->iter){
        return D->iter->data;
    }
    return VAL_UNDEF;
}

VAL_TYPE next(Dictionary D){
    DictNullGuard(D,"currentVal");
    
    Node forward = D->iter->right;
    //if iter has a right child, it should explore it
    if(notNil(D,forward)){
        while(notNil(D,forward->left)){
        forward = forward->left;
        }
        D->iter = forward;
    }
    else{
        //if iter doesn't have right child, the next highest node is in another branch
        //the branch starts when the child was a left child, not a right child
       Node iterTemp = D->iter;
       Node backtracker = iterTemp->parent;
       while(notNil(D,backtracker) && backtracker->right == iterTemp){
           iterTemp = backtracker;
           backtracker = backtracker->parent;
       }
       D->iter = backtracker;
    }
    if(notNil(D,D->iter)){
        return D->iter->data;
    }
    //this is when we tried to look for the parent of the root, meaning we explored all other nodes
    return VAL_UNDEF;
}
 
//same as next, but with left and right swapped
VAL_TYPE prev(Dictionary D){
    DictNullGuard(D,"currentVal");
    
    Node forward = D->iter->left;
    if(notNil(D,forward)){
        while(notNil(D,forward->right)){
        forward = forward->right;
        }
        D->iter = forward;
    }
    else{
       Node iterTemp = D->iter;
       Node backtracker = iterTemp->parent;
       while(notNil(D, backtracker) && backtracker->left == iterTemp){
           iterTemp = backtracker;
           backtracker = backtracker->parent;
       }
       D->iter = backtracker;
    }
    if(notNil(D,D->iter)){
        return D->iter->data;
    }
    return VAL_UNDEF;
}

void nodePrint(FILE* out, Node N){
    fprintf(out,KEY_FORMAT,N->key);
    if(N->data){
        fprintf(out," ");
        fprintf(out, VAL_FORMAT,N->data);
    }
    fprintf(out, "\n");
}

void printNodesInOrder(FILE* out, Dictionary D, Node head){
    if(!notNil(D,head)) return;
    printNodesInOrder(out,D,head->left);
    nodePrint(out,head);
    printNodesInOrder(out,D,head->right);
}

void printNodesPreOrder(FILE* out, Dictionary D, Node head){
    if(!notNil(D,head)) return;
    nodePrint(out,head);
    printNodesPreOrder(out,D,head->left);
    printNodesPreOrder(out,D,head->right);
}

void printNodesPostOrder(FILE* out, Dictionary D, Node head){
    if(!notNil(D,head)) return;
    printNodesPostOrder(out,D,head->left);
    printNodesPostOrder(out,D,head->right);
    nodePrint(out,head);
}

void printDictionary(FILE* out, Dictionary D, const char* ord){
    DictNullGuard(D,"printDictionary");
    if(!ord){
        DictError("printDictionary","null ord");
    }
    int maxStrlen = 9;
    char lowerOrd[10];
    lowerOrd[maxStrlen] = '\0';
    int toLower = 'a' - 'A';
    for(int i = 0; i < maxStrlen; ++i){
        char temp = ord[i];
        if (temp >= 'A' && temp < 'a'){
            temp += toLower;
        }
        lowerOrd[i] = temp;
        if(temp == '\0') break;
    }
    if(strcmp(lowerOrd,"inorder") == 0 || strcmp(lowerOrd,"in") == 0){
        printNodesInOrder(out,D,D->head);
    }
    else if(strcmp(lowerOrd,"preorder") == 0 || strcmp(lowerOrd,"pre") == 0){
        printNodesPreOrder(out,D,D->head);
    }
    else if(strcmp(lowerOrd,"postorder") == 0 || strcmp(lowerOrd,"post") == 0){
        printNodesPostOrder(out,D,D->head);
    }
    else{
        DictError("printDictionary","not supported traversal order");
    }

}