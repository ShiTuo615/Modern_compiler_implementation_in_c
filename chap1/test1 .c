#include "util.h"
#include <stddef.h>
#include <string.h>
typedef struct tree *T_tree;
struct tree{T_tree left; string key; void *binding; T_tree right;};
T_tree Tree(T_tree l, string k, void *binding, T_tree r){
    T_tree t = checked_malloc(sizeof(*t));
    t -> left = l;
    t -> key = k;
    t -> right = r;
    t->binding = binding;
    return t;
}

T_tree insert(string key, void *binding, T_tree t){
    if(t ==NULL){
        return Tree(NULL, key, binding, NULL);
    }else if(strcmp(key, t->key) < 0){
        return Tree(insert(key, binding, t-> left), binding, t->key, t->right);
    }else if (strcmp(key, t->key) > 0){
        return Tree(t->left, t->key, binding, insert(key, binding, t->right));
    }else{
        return Tree(t->left, key, binding, t->right);
    }
}

bool member(string key, T_tree t){
    if(t == NULL){
        return FALSE;
    }
    if(strcmp(key,  t->key) < 0){
        return t->left != NULL? member(key, t->left) : FALSE;
    }else if(strcmp(key, t->key) > 0){
        return t->right != NULL? member(key, t->right) : FALSE;
    }else{
        return TRUE;
    }
}

void * lookup(string key, T_tree t){
    if(t == NULL){
        return NULL;
    }
    if(strcmp(key, t->key) < 0){
        return t->left != NULL? lookup(key, t->left) : NULL;
    }else if(strcmp(key, t->key) > 0){
        return t->right != NULL? lookup(key, t->right) : NULL;
    }else{
        return t->binding;
    }
}



