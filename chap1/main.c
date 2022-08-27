#include<stdio.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"
//运行就用makefile运行，这个在win下的命令是mingw32-make
int exp_helper(A_expList expList){
    if(expList->kind == A_lastExpList){
        return 1;
    }
    return 1 + exp_helper(expList->u.pair.tail);
}

//找到子表达式中print的最大参数个数
int maxargs(A_stm stm){
    switch (stm->kind)
    {
    case A_compoundStm:{
        int left = maxargs(stm->u.compound.stm1);
        int right = maxargs(stm->u.compound.stm2);
        return  left > right ? left : right;
    }
    case A_assignStm:{
        A_exp exp = stm->u.assign.exp;
        if(exp->kind == A_eseqExp){
            return maxargs(exp->u.eseq.stm);
        }else{
            return 0;
        }
    }
    case A_printStm:{
        A_expList expList = stm->u.print.exps;
        return exp_helper(expList);
    }
    default:
        return -1;
    }
};

typedef struct table *Table_;
struct table {string id; int value; Table_ tail;};
int ID_VALID = 1;
Table_ Table(string id, int value, struct table *tail){
    Table_ t = checked_malloc(sizeof(*t));
    t->id= id;
    t->value = value;
    t->tail = tail;
    return t;
};

Table_ update(Table_ t, string id, int value){
    return Table(id, value, t);
}

int lookup(Table_ t, string key){
    while((t != NULL) && (t->id != key)){
        t = t->tail;
    }
    if(t != NULL){
        ID_VALID = 1;
        return t->value;
    }else{
        ID_VALID = 0;
        return -1;
    }
}

typedef struct IntAndTable *ITable_;
struct IntAndTable{int i; Table_ t;};
ITable_ ITable(int i, Table_ t){
    ITable_ it = checked_malloc(sizeof(*it));
    it->i = i;
    it->t = t;
    return it; 
};

Table_ interpStm(A_stm s, Table_ t);
ITable_ interpExp(A_exp e, Table_ t);
Table_ interpExpList(A_expList expList, Table_ t);

Table_ interpStm(A_stm s, Table_ t){
    switch (s->kind)
    {
    case A_compoundStm:{
        t = interpStm(s->u.compound.stm1, t);
        t = interpStm(s->u.compound.stm2, t);
        return t;
    }
    case A_assignStm:{
        ITable_ itable = interpExp(s->u.assign.exp, t);
        t = update(itable->t, s->u.assign.id, itable->i);
        return t;
    }
    case A_printStm:{
        t = interpExpList(s->u.print.exps, t);
        printf("\n");
        return t;
    }
    default:
        return t;
    }
};

ITable_ interpExp(A_exp e, Table_ t){
    switch (e->kind)
    {
    case A_idExp:{
        int value = lookup(t, e->u.id);
        if(ID_VALID == 1){
            return ITable(value, t);
        }
        printf("[error] Identifier %s does not exist!\n", e->u.id);
        break;
    }
    case A_numExp:
        return ITable(e->u.num, t);
    case A_opExp:{
        ITable_ left = interpExp(e->u.op.left, t);
        ITable_ right = interpExp(e->u.op.right, t);
        switch (e->u.op.oper)
        {
        case A_plus:
            return ITable(left->i + right->i, t);
        case A_minus:
            return ITable(left->i - right->i, t);
        case A_times:
            return ITable(left->i * right->i, t);
        case A_div:
            return ITable(left->i / right->i, t);
        default:
            return NULL;
        }
    }
    case A_eseqExp:{
        t = interpStm(e->u.eseq.stm, t);
        ITable_ it = interpExp(e->u.eseq.exp, t);
        return it;
    }
    default:
        return NULL;
    }
};


Table_ interpExpList(A_expList expList, Table_ t){
    switch (expList->kind)
    {
    case A_pairExpList:{
        ITable_ it = interpExp(expList->u.pair.head, t);
        printf("%d ", it->i);
        t = interpExpList(expList->u.pair.tail, it->t);
        return t;
    }
    case A_lastExpList:{
        ITable_ it = interpExp(expList->u.last, t);
        printf("%d", it->i);
        return it->t;
    }
    default:
        return t;
    }
}

void interp(A_stm stm){
    interpStm(stm, NULL);
}

int main(){
    printf(">> Right Prog Section:\n");
    A_stm p = prog();
    printf("the maximum number of arguments of any print statement is %d\n", maxargs(p));
    interp(p);
    while (1);
    return 0;
}


