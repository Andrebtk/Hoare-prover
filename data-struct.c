#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-struct.h"



ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_BIN_OP;

    res->binary_op.op = strdup(type);
    res->binary_op.left = left;
    res->binary_op.right = right;
    return res;
}

ASTNode* create_node_number(int num) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_NUMBER;

    res->number = num;
    return res;
}

ASTNode* create_node_id(char *input) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_ID;

    res->id_name = strdup(input);
    return res;
}


ASTNode* create_node_assign(char* id, ASTNode* expr){
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_ASSIGN;
    res->Assign.id = strdup(id);
    res->Assign.expr = expr;
    return res;
}

ASTNode* create_node_If(ASTNode* condition, DLL* block) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_IF;

    res->If_While.condition = condition;
    res->If_While.block = block;
    return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block){
     ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_WHILE;

    res->If_While.condition = condition;
    res->If_While.block = block;
    return res;
}

DLL* create_DLL() {
    DLL* res = malloc(sizeof(DLL));
    res->first = NULL;
    res->last = NULL;
    return res;
}

line_linkedlist* create_ll(ASTNode* node) {
    line_linkedlist* list = malloc(sizeof(line_linkedlist));
    list->next = NULL;
    list->node=node;
    return list;
}

void DLL_append(DLL* list, ASTNode* node) {
    line_linkedlist* l = create_ll(node);
    
    if(list->last == NULL) {    //0 element in list
        list->first = l;
        list->last = l;
    } else {
        list->last->next = l;
        list->last = l;
    }
}

void print_ASTNode(ASTNode* node) {
    if(node == NULL) {
        printf("node is NULL");
        return;
    }
    
    switch(node->type) {
        case NODE_BIN_OP: {
            printf("Node binary: %s\n", node->binary_op.op);
            printf("Left node: ");
            print_ASTNode(node->binary_op.left);
            printf("Right node: ");
            print_ASTNode(node->binary_op.right);
            break;
        };

        case NODE_ID: {
            printf("Node ID: %s\n", node->id_name);

            break;
        };

        case NODE_NUMBER: {
            printf("Node NUMBER: %d\n", node->number);
            break;
        };

        case NODE_ASSIGN: {
            printf("Node ASSIGN: %s\n", node->Assign.id);
            printf("Expr: ");
            print_ASTNode(node->Assign.expr);
            printf("\n");

            break;
        };

        case NODE_IF: {
            printf("Node IF:\n");
            printf("condition : ");
            print_ASTNode(node->If_While.condition);
            printf("\nblock : ");
            print_DLL(node->If_While.block);
            break;
        };

        case NODE_WHILE: {
           printf("Node WHILE:\n");
            printf("condition : ");
            print_ASTNode(node->If_While.condition);
            printf("\nblock : ");
            print_DLL(node->If_While.block);
            break;
        };

        case NODE_BLOCK: {
            printf("Node block");
            break;
        };
        
        default:
            printf("Unknown node type: %d\n", node->type);
            break;
    }
    
    
}

void print_line_linkedlist(line_linkedlist* list) {
    while(list != NULL) {
        if (list->node == NULL) {
            printf("Null node in linked list\n");
        } else {
           print_ASTNode(list->node);
        }
        list = list->next;
    }
}

void print_DLL(DLL* dll) {
    if(dll == NULL) {
        printf("DLL is NULL\n");
        return;
    }
    if(dll->first == NULL ){
        printf("DLL first is NULL\n");
    }
    print_line_linkedlist(dll->first);
    
   
}