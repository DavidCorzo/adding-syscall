#include <stdbool.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include "print.h"

#define itp_SIZE 100

typedef struct {
    char item[itp_SIZE]; 
    int top; 
} itp_OperatorStack;

// func for stack operations
void itp_push(itp_OperatorStack *, char);
char itp_pop(itp_OperatorStack *);
int itp_isEmpty(itp_OperatorStack *);
char itp_stacktop(itp_OperatorStack *);

// utility function prototypes
int itp_isOpenMatch(char, char);
double itp_operate(double, double, char);
bool itp_isOperator(char c);
double itp_evalPostfix(char[]);
void itp_print_s(itp_OperatorStack *);
void itp_infix_to_postfix(char [], char []);
bool itp_precedence(char, char);
bool itp_isOperand(char);

void itp_push(itp_OperatorStack *sp, char c){
    if (sp->top == itp_SIZE - 1){
        PRINT(KERNEL_INFO_MACRO "Stack overflow.\n");
        // exit(1);
	return;
    }
    sp->top += 1;
    sp->item[sp->top] = c; 
}


char itp_pop(itp_OperatorStack *sp){
    if (sp->top == -1){
        PRINT(KERNEL_INFO_MACRO "Stack underflow");
        // exit(1);
	return -1;
    }
    return sp->item[sp->top--];
}


int itp_isEmpty(itp_OperatorStack *sp){
    return sp->top == -1;
}


char itp_stacktop(itp_OperatorStack *sp){
    if (itp_isEmpty(sp)){
        PRINT(KERNEL_INFO_MACRO "Stack underflow, terminating program.\n");
        // exit(1);
	return -1;
    }
    return sp->item[sp->top];
}

bool itp_isOperator(char c){
    return  (c == '+') ||
            (c == '-') || 
            (c == '*') || 
            (c == '/') || 
            (c == '$') ||
            (c == '(') ||
            (c == ')') ||
            (c == '[') ||
            (c == ']');
}

void itp_print_s(itp_OperatorStack *sp){
	int i;
    for (i = 0; i < sp->top; i++){
        PRINT(KERNEL_INFO_MACRO "stack: %c\n",sp->item[i]);
    }
}

bool itp_precedence(char stack_top, char token){
    /* ( is less precedente than (, 
    there are anidated parenthesis */
    if (stack_top == '(' || token == '(' || stack_top == '[' || token == '['){
        return false;
    } 
    /* if token is ) says that it is closing the expression
     within a higher itp_precedence parenthesis */
    if (token == ')' || (token == ']')){
        return true; 
    }   

    /* the *,/ has the higher itp_precedence than all the
     other operands except $ */
    if (stack_top == '*' || stack_top == '/'){
        /* In this case the stack top operator has a higher itp_precedence */
        if (token == '*' || token == '/' || token == '+' || token == '-'){
            return true;
        /* In this case it does not */
        } else {
            return false; 
        }
    }

    /* the stack_top can be +,- which have the same itp_precedence */
    if (stack_top == '+' || stack_top == '-'){
        if (token == '+' || token == '-'){
            return true; 
        } else {
            return false; 
        }
    }

    /* the $ has the highest itp_precedence of them all */
    if (stack_top == '$'){
        return true;
    } else {
        return false; 
    }
}

bool itp_isOperand(char token){
    return (token >= '0' && token <= '9');
}


void itp_infix_to_postfix(char infix[], char postfix[]){
    int j = 0, i = 0;
    itp_OperatorStack stack = {.top=-1}; 
    while (infix[i] != '\0'){
        char token = infix[i];
        // if the token is an operand.
        if (itp_isOperand(token)){
            postfix[j++] = token;
        // if the token is an operator.
        } else if (itp_isOperator(token)){
            while ((!itp_isEmpty(&stack)) && (itp_precedence(itp_stacktop(&stack),token))){
                char top_operator = itp_pop(&stack);
                postfix[j++] = top_operator;
            }
            if (token == ')' || (token == ']')){
                itp_pop(&stack); // discard the )
            } else {
                itp_push(&stack,token);
            }
        } else {
            PRINT(KERNEL_INFO_MACRO "Invalid symbol encountered \"%c\".\n",token);
            // exit(1);
			return;
        }
        i++;
    }

    while (!itp_isEmpty(&stack)){
        char top_operator = itp_pop(&stack);
        postfix[j++] = top_operator;
    }

    postfix[j] = '\0';
}

