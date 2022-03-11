#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <stdbool.h>

// #define PRINT printk
// #define KERNEL_INFO_MACRO KERN_INFO

/////////////////////////////////////////////

#define peval_SIZE 100

typedef struct {
  long int item[peval_SIZE];
  int top;
} peval_OperandStack;

// func prototypes
// func for stack operations
void peval_push(peval_OperandStack *, long int);
long int peval_pop(peval_OperandStack *);
int peval_isEmpty(peval_OperandStack *);
char peval_stacktop(peval_OperandStack *);

// utility function prototypes
int peval_isOpenMatch(char, char);
long int peval_operate(long int, long int, char);
bool peval_isOperator(char c);
long int peval_evalPostfix(char[]);
void peval_print_s(peval_OperandStack *);

void peval_push(peval_OperandStack *sp, long int c) {
  if (sp->top == peval_SIZE - 1) {
    printk(KERN_INFO "Stack overflow.\n");
    // exit(1);
	return;
  }
  sp->top += 1;
  sp->item[sp->top] = c;
}

long int peval_pop(peval_OperandStack *sp) {
  if (sp->top == -1) {
    printk(KERN_INFO "Stack underflow");
    // exit(1);
	return -999999;
  }
  return sp->item[sp->top--];
}

int peval_isEmpty(peval_OperandStack *sp) { return sp->top == -1; }

char peval_stacktop(peval_OperandStack *sp) {
  if (peval_isEmpty(sp)) {
    printk(KERN_INFO "Stack underflow, terminating program.\n");
    // exit(1);
	return -1;
  }
  return sp->item[sp->top];
}

int peval_isOpenMatch(char opening, char closing) {
  bool match = false;
  switch (opening) {
  case '(':
    if (closing == ')') {
      match = true;
    }
    break;

  case '{':
    if (closing == '}') {
      match = true;
    }
    break;
  case '[':
    if (closing == ']') {
      match = true;
    }
    break;
  default:
    printk(KERN_INFO "Invalid symbol, program is terminating.");
    // exit(1);
	return -1;
  }
  return match;
}

long int peval_operate(long int left, long int right, char operator) {
	long int result = 0;
  switch (operator) {
  case '+':
    result = left + right;
    break;
  case '-':
    result = left - right;
    break;
  case '*':
    result = left * right;
    break;
  case '/':
    result = left / right;
    break;
  default:
    printk(KERN_INFO "%c is not a valid operator\n", operator);
    // exit(1);
	return -99999;
  }
  return result;
}

bool peval_isOperator(char c) {
  return (c == '+') || (c == '-') || (c == '*') || (c == '/');
}

long int peval_evalPostfix(char postfix[]) {
  peval_OperandStack stack = {.top = -1};
  int i = 0;
  while (postfix[i] != '\0') {
    char token = postfix[i];
    if (token >= '0' && token <= '9') {
      int v = token - '0'; // you can also use a atoi func
      peval_push(&stack, (long int)v);
    } else if (peval_isOperator(token)) {
      long int right = peval_pop(&stack);
      long int left = peval_pop(&stack);
      long int result = peval_operate(left, right, token);
      peval_push(&stack, result);
    } else {
      printk(KERN_INFO "Invalid symbol encontered \"%c\"", token);
      // exit(1);
		return -99999;
    }
    ++i;
  }
  return peval_pop(&stack);
}

void peval_print_s(peval_OperandStack *sp) {
	int i;
  for (i = 0; i < sp->top; i++) {
    printk(KERN_INFO "stack: %ld\n", sp->item[i]);
  }
}

//////////////////////////////////////////////////

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
        printk(KERN_INFO "Stack overflow.\n");
        // exit(1);
	return;
    }
    sp->top += 1;
    sp->item[sp->top] = c; 
}


char itp_pop(itp_OperatorStack *sp){
    if (sp->top == -1){
        printk(KERN_INFO "Stack underflow");
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
        printk(KERN_INFO "Stack underflow, terminating program.\n");
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
        printk(KERN_INFO "stack: %c\n",sp->item[i]);
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
            printk(KERN_INFO "Invalid symbol encountered \"%c\".\n",token);
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


/////////////////////////////////////////////////

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
        printk(KERN_INFO "bc_Stack overflow.\n");
        // exit(1);
	return;
    }
    sp->top += 1;
    sp->item[sp->top] = c; 
}


char bc_pop(bc_Stack *sp){
    if (sp->top == -1){
        printk(KERN_INFO "bc_Stack underflow");
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
        printk(KERN_INFO "bc_Stack underflow, terminating program.\n");
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
            printk(KERN_INFO "Invalid symbol, program is terminating.");
            // exit(1);
    }
    return match; 
}

void bc_print_s(bc_Stack *sp){
	int i;
    for (i = 0; i < sp->top; i++){
        printk(KERN_INFO "stack: %c\n",sp->item[i]);
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

/////////////////////////////////////////


unsigned int str_length(char *str) {
  unsigned int index = 0;
  while (str[index] != '\0') {
    index++;
  }
  return (index);
}

SYSCALL_DEFINE2(expr_checker, long*, value, char*, expr) {
  // is correctly closed_
  long is_correctly_closed = bc_checkExpr(expr);
  if (!is_correctly_closed) {
    *value = -9999999;
  }
  else {
    // allocate for postfix string
    char postfix[str_length(expr)];
    // get postfix notation
    itp_infix_to_postfix(expr, postfix);
    // evaluate expression
    *value = peval_evalPostfix(postfix);
  }
    printk(KERN_INFO "value=%ld, is_correct=%d", *value, is_correctly_closed);
  return is_correctly_closed;
}


