#include <stdbool.h>
#include "print.h"
#include <linux/kernel.h>
#include <linux/syscalls.h>

#define bc_SIZE 100

typedef struct {
    char item[bc_SIZE];
    int top; 
} bc_Stack; 

// function prototypes 
void bc_push(bc_Stack *, char);
char bc_pop(bc_Stack *);
int bc_isEmpty(bc_Stack *);
char bc_stacktop(bc_Stack *);

// utility function prototypes
int bc_isOpenMatch(char, char);

// the bracket checking program function protoype
bool bc_checkExpr(char[]);
void bc_print_s(bc_Stack *);

void bc_push(bc_Stack *sp, char c){
    if (sp->top == bc_SIZE - 1){
        PRINT(KERNEL_INFO_MACRO "bc_Stack overflow.\n");
        // exit(1);
	return;
    }
    sp->top += 1;
    sp->item[sp->top] = c; 
}


char bc_pop(bc_Stack *sp){
    if (sp->top == -1){
        PRINT(KERNEL_INFO_MACRO "bc_Stack underflow");
        //exit(1);
	return -1;
    }
    return sp->item[sp->top--];
}


int bc_isEmpty(bc_Stack *sp){
    return sp->top == -1;
}


char bc_stacktop(bc_Stack *sp){
    if (bc_isEmpty(sp)){
        PRINT(KERNEL_INFO_MACRO "bc_Stack underflow, terminating program.\n");
        // exit(1);
    }
    return sp->item[sp->top];
}

int bc_isOpenMatch(char opening, char closing){
    bool match = false;
    switch (opening) {
        case '(':
            if (closing == ')') {
                match = true;
            }
            break;

        case '{':
            if (closing == '}'){
                match = true;
            }
            break;
        case '[':
            if (closing == ']'){
                match = true; 
            }
            break;
        default: 
            PRINT(KERNEL_INFO_MACRO "Invalid symbol, program is terminating.");
            // exit(1);
    }
    return match; 
}

void bc_print_s(bc_Stack *sp){
	int i;
    for (i = 0; i < sp->top; i++){
        PRINT(KERNEL_INFO_MACRO "stack: %c\n",sp->item[i]);
    }
}

bool bc_checkExpr(char str[]){
    bool error = false; 
    bc_Stack s = {.top=-1};

    int i = 0;
    while (str[i] != '\0'){
        if ( (str[i] == '(') || (str[i] == '{') || (str[i] == '[')){
            bc_push(&s,str[i]);
        } else if ( (str[i] == ')') || (str[i] == '}') || (str[i] == ']')){
            if (bc_isEmpty(&s)) {
                error = true;
                break;
            } else if (bc_isOpenMatch(bc_stacktop(&s),str[i])) {
                bc_pop(&s);
            } else {
                error = true; 
                break; 
            }
        }
        i++;
    }

    if (!error && !bc_isEmpty(&s)) {
        error = true; 
    }

    return !error; 
}
