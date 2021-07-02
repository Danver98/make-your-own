#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MIN (-2147483647 - 1)
#define PN printf("\n");
#define BUFFER_SIZE 256
#define E 2.718281828459045
#define PI 3.141592653589793
//TODO()
// Add validation input check
// Add multiargument support for several functions

typedef enum
{
    NUMBER,
    OPERATOR,
    OPENING_BRACKET,
    CLOSING_BRACKET,
    VARIABLE,
    POSTFIX_FUNCTION,
    PREFIX_FUNCTION,
    EXPRESSION,
    UNDEFINED
} Type;

typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    GRADE,
    NONE
} Operation;

typedef enum
{
    //By default, every item here is PREFIX_FUNCTION
    EXP,
    FACT, // POSTFIX_FUNCTION
    SIN,
    COS,
    TAN,
    LOG,
    SQRT,
    UNARY_MINUS,
    NOT_STATED
} Function;

typedef enum
{
    LOW,
    MIDDLE,
    HIGH,
    GREATEST
} Priority;

typedef struct Token
{
    Type type;
    struct Token *next;
    union
    {
        double number; // both integers and doubles etc.
        char symbol;   // symbol or operation
        char *str;     //expression
        Operation operator: 4;
        Function function : 4;
    };
} Token;

Token *First, *Last;

typedef struct Stack
{
    int top;
    Token **tokens;
} Stack;

int is_empty(Stack *stack)
{
    return stack->top == -1 ? 1 : 0;
}

Priority get_priority(Token *token)
{
    switch (token->operator)
    {
    case ADD:
    case SUB:
        return MIDDLE;
    case MUL:
    case DIV:
    case MOD:
        return HIGH;
    case GRADE:
        return GREATEST;
    default:
        return LOW;
    }
}

void push(Stack *stack, Token *token)
{
    stack->top++;
    stack->tokens[stack->top] = token;
}

Token *pop(Stack *stack)
{
    if (stack->top == -1)
    {
        printf("Stack is empty\n");
        return NULL;
    }
    return stack->tokens[stack->top--];
}

void free_memory(Token *first, Stack *stack, Stack *result_stack)
{
    for (Token *el = First, *tmp; el != NULL;)
    {
        tmp = el;
        el = el->next;
        free(tmp);
    }
    free(stack->tokens);
    free(result_stack->tokens);
}

int factorial(double number)
{
    if ((number == 0) || (number == 1))
        return 1;
    int num = (int)number;
    for (int i = num - 1; i > 1; i--)
    {
        num *= i;
    }
    return num;
}

int is_digit(char s)
{
    if ((s >= '0') && (s <= '9'))
        return 1;
    return 0;
}

int is_letter(char s)
{
    if ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z'))
        return 1;
    return 0;
}

char to_lower(char s)
{
    if (s >= 'A' && s <= 'Z')
        return s + 'a' - 'A';
    return s;
}

int forward_letters_count(char *s, int start)
{
    int count = 0, i = 0;
    while (is_letter(s[start + i++]))
        count++;
    return count;
}

int substr_equals_string(char *s1, char *string, int pos)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        if (s1[pos + i] == '\0')
            return 0;
        if (to_lower(s1[pos + i]) != to_lower(string[i]))
        {
            return 0;
        }
    }
    return 1;
}

char *substr(char *s, int pos, int count)
{
    char *res = (char *)malloc(sizeof(char) * (count));
    for (int i = 0; i < count; i++)
    {
        res[i] = s[pos + i];
    }
    return res;
}

char *num_sub_str(char *s, int pos)
{
    int size = 0;
    while (is_digit(s[pos + size]) || (s[pos + size] == '.'))
    {
        size++;
    }
    if (s[pos + size] == '.')
        size++;
    char *number = (char *)malloc(sizeof(char) * (size + 1));
    for (int i = 0; i < size; i++)
    {
        number[i] = s[pos + i];
    }
    number[size] = '\0';
    return number;
}

int str_len(char *s)
{
    int len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

int mem_cmp(char *s1, char *s2, int symbols){
    for(int i=0; i < symbols; i++){
        if(s1[i] > s2[i])
            return 1;
        if(s1[i] < s2[i])
            return -1;
    }
    return 0;
}

double num_from_str(char *str)
{
    double num = 0;
    double factor = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if ((str[i] == '.') || (str[i] == ','))
        {
            if (factor)
                return (double)MIN;
            factor = 1;
        }
        else if (is_digit(str[i]))
        {
            if (factor)
            {
                num += factor * (str[i] - '0') / 10;
                factor /= 10;
            }
            else
                num = num * 10 + (str[i] - '0');
        }
        else
            return (double)MIN;
    }
    return num;
}

int add_token(const void *value, Type type)
{
    Token *token = (Token *)malloc(sizeof(Token));
    if (token == NULL)
    {
        return -1;
    }
    token->next = NULL;
    if (First == NULL)
        First = Last = token;
    else
    {
        Last->next = token;
        Last = Last->next;
        Last->next = NULL;
    }
    token->type = type;
    switch (type)
    {
    case NUMBER:
        token->number = *((double *)value);
        break;
    case OPERATOR:
        token->operator= *((Operation *)value);
        break;
    case OPENING_BRACKET:
    case CLOSING_BRACKET:
    case VARIABLE:
        token->symbol = *((char *)value);
        break;
    case PREFIX_FUNCTION:
    case POSTFIX_FUNCTION:
        token->function = *((Function *)value);
        break;
    case UNDEFINED:
        break;
    default:
    {
        printf("\nCannot parse token\n");
        return -1;
    }
        break;
    }
    return 0;
}

int parse_tokens(char *s)
{
    int i = 0, size = 0;
    double number = 0;
    char *str;
    while (s[i] != '\0')
    {
        if (is_digit(s[i]))
        {
            i += str_len(str = num_sub_str(s, i));
            number = num_from_str(str); // check conformance of number input
            add_token(&number, NUMBER);
            size++;
        }
        else if (is_letter(s[i]))
        {
            int count = forward_letters_count(s, i);
            Function f = NOT_STATED;
            switch (count)
            {
            case 1:
                if (s[i] == 'e')
                {
                    double e = E;
                    add_token(&e, NUMBER);                 
                }
                else
                {
                    //printf("\nWrong symbol(s)\n");
                    //return -1;
                    add_token(&s[i], VARIABLE);
                };            
                break;
            case 2:
                if (substr_equals_string(s, "Pi", i)){
                    double pi = PI;
                    add_token(&pi, NUMBER);
                }
                else
                {
                    printf("\nWrong symbol(s)\n");
                    return -1;
                }
                break;
            // here we mainly add functions
            case 3:
                if(substr_equals_string(s, "mod", i)){
                    Operation op = MOD;
                    add_token(&op, OPERATOR);
                }
                else if (substr_equals_string(s, "exp", i))
                    f = EXP;
                else if (substr_equals_string(s, "sin", i))
                    f = SIN;
                else if (substr_equals_string(s, "cos", i))                
                    f = COS;               
                else if (substr_equals_string(s, "tan", i))
                    f = TAN;                   
                else if (substr_equals_string(s, "log", i))
                    f = LOG;
                else
                {
                    printf("\nWrong symbol(s)\n");
                    return -1;
                }
                break;
            case 4:
                if (substr_equals_string(s, "sqrt", i))
                    f = SQRT;
                else
                {
                    printf("\nWrong symbol(s)\n");
                    return -1;
                }
                break;
            default:
            {
                printf("\nWrong symbol(s)\n");
                return -1;
            }
            break;
            }
            if(f != NOT_STATED)
                add_token(&f, PREFIX_FUNCTION);
            size++;
            i += count;
            
        }
        else
        {
            Operation op = NONE;
            Function func;
            switch (s[i])
            {
            case '(':
                /*
                if((Last)&&((Last->type != OPENING_BRACKET) || (Last->type != OPERATOR))
                ||((Last->type != PREFIX_FUNCTION))){
                    printf("\nWrong symbol(s) before closing bracket\n");
                    return -1;
                }*/
                add_token(&s[i], OPENING_BRACKET);
                size++;
                break;
            case ')':
                /*
                if((!Last) || (Last->type != CLOSING_BRACKET) || (Last->type != NUMBER)){
                    printf("\nWrong symbol(s) before closing bracket\n");
                    return -1;
                }
                */
                add_token(&s[i], CLOSING_BRACKET);
                size++;
                break;
            case '!':
                func = FACT;
                /*if((!Last) || (Last->type == !CLOSING_BRACKET) || (Last->type != NUMBER)){
                    printf("\nWrong symbol(s) before factorial\n");
                    return -1;
                }
                */
                add_token(&func, POSTFIX_FUNCTION);
                size++;
                break;
            case '^':
                op = GRADE;
                break;
            case '*':
                op = MUL;
                break;
            case '/':
                op = DIV;
                break;
            case '+':
                op = ADD;
                break;
            case '-':
                if ((!Last) || (Last->type == OPERATOR) || (Last->type == OPENING_BRACKET))
                {
                    Function func = UNARY_MINUS;
                    add_token(&func, PREFIX_FUNCTION);
                    size++;
                }
                else
                {
                    op = SUB;
                }
                break;
            case ' ':
            case '\n':
                break;
            default:
                printf("\nWrong symbol\n");
                return -1;
            }
            if (op != NONE)
            {
                // if ((op != SUB) &&(Last->type == OPERATOR)){
                //printf("\nWrong input\n");
                //return -1;
                //}
                add_token(&op, OPERATOR);
                size++;
            }
            i++;
        }
    }
    return size;
}

int infix_to_polish(Token *start, Stack *op_stack, Stack *result_stack)
{
    Token *el = start, *tmp;
    int count = 0;
    while (el != NULL) // || while(el)
    {
        switch (el->type)
        {
        case NUMBER:
        case POSTFIX_FUNCTION:
            push(result_stack, el);
            break;
        case PREFIX_FUNCTION:
        case OPENING_BRACKET:
            push(op_stack, el);
            break;
        case CLOSING_BRACKET:
            if (is_empty(op_stack))
            {
                printf("\n Wrong bracket's position or delimeter is absent (stack is empty)\n");
                return -1;
            }
            while ((tmp = pop(op_stack))->type != OPENING_BRACKET)
            {
                if (is_empty(op_stack))
                {
                    printf("\n Wrong bracket's position or delimeter is absent (stack is empty)\n");
                    return -1;
                }
                push(result_stack, tmp);
                // smth more 'bout brackets & functions
            }
            break;
        case OPERATOR:
            if (!is_empty(op_stack))
            {
                tmp = op_stack->tokens[op_stack->top];
                while ((!is_empty(op_stack)) && ((tmp->type == PREFIX_FUNCTION) || (get_priority(tmp) > get_priority(el)) || ((get_priority(tmp) == get_priority(el)) && (tmp->operator!= GRADE))))
                {
                    push(result_stack, pop(op_stack));
                    tmp = op_stack->tokens[op_stack->top];
                }
            }
            push(op_stack, el);
            break;
        case VARIABLE:
            printf(" %c ,", (char)el->symbol);
            break;
        case UNDEFINED:
            break;
        default:
            printf(" %s ,", el->str);
        }
        //printf("\n COUNT = %d, op_stack top = %d, result_stack top = %d, element type was %s \n", count,op_stack->top, result_stack->top, el->type);
        el = el->next;
        count++;
    }
    while (!is_empty(op_stack))
    {
        tmp = pop(op_stack);
        if ((tmp->type != OPERATOR) && (tmp->type != PREFIX_FUNCTION))
        {
            printf("\n Wrong bracket's position or delimeter is absent\n");
            //printf("\n %c \n", tmp->symbol);
            return -1;
        }
        push(result_stack, tmp);
    }
    return 0;
}

double calculate(Stack *num_stack, Stack *result_stack)
{
    Token *token, *left, *right;
    int pos = 0;
    while (!is_empty(num_stack))
        pop(num_stack);
    while (pos <= result_stack->top)
    {
        token = result_stack->tokens[pos];
        switch (token->type)
        {
        case NUMBER:
            push(num_stack, token);
            break;
        case OPERATOR: // trigonometry functions, factorial, unary operations etc.
            right = pop(num_stack);
            left = pop(num_stack);
            switch (token->operator)
            {
            case ADD:
                left->number = left->number + right->number;
                break;
            case SUB:
                left->number = left->number - right->number;
                break;
            case MUL:
                left->number = left->number * right->number;
                break;
            case DIV:
                if (right->number == 0){
                    printf("\nYou cannot divide by 0. Try again!\n");
                    return MIN;
                }         
                left->number = left->number / right->number;
                break;
            case MOD:
                if (right->number == 0){
                    printf("\nYou cannot take remainder by 0. Try again!\n");
                    return MIN;
                }
                left->number = ((int)left->number) % ((int)right->number);
                break;
            case GRADE:
                left->number = pow(left->number, right->number);
                break;
                // other operations...
            }
            push(num_stack, left);
            break;
        case PREFIX_FUNCTION:
        case POSTFIX_FUNCTION: // does it differ?
            left = pop(num_stack);
            switch (token->function)
            {
            case FACT: // is it here?
                left->number = factorial(left->number);
                break;
            case EXP:
                left->number = exp(left->number);
                break;
            case SIN:
                left->number = sin(left->number);
                break;
            case COS:
                left->number = cos(left->number);
                break;
            case TAN:
                left->number = tan(left->number);
                break;
            case LOG:
                left->number = log(left->number);
                break;
            case SQRT:
                left->number = sqrt(left->number);
                break;
            case UNARY_MINUS:
                left->number = -left->number;
                break;
            }
            push(num_stack, left);
            break;
        }
        pos++;
    }
    return pop(num_stack)->number;
}

void start_calculation(char *str)
{
    First = Last = NULL;
    Stack stack, result_stack;
    int size = parse_tokens(str);
    if (size < 2)
    {
        printf("\n Errors in input. Parsing function returned -1.\n");
        return;
    }
    stack.top = -1;
    result_stack.top = -1;
    stack.tokens = (Token **)malloc(sizeof(Token *) * size); // equals *tokens[size]?
    result_stack.tokens = (Token **)malloc(sizeof(Token *) * size);
    if ((result_stack.tokens == NULL) || (stack.tokens == NULL))
    {
        printf("\n Program couldn't emit memory\n");
        return;
    }
    if (infix_to_polish(First, &stack, &result_stack) != -1)
    {
        /*
        printf("\nPolish list:\n");
        for(int i =0; i <= result_stack.top; i++){
            p_value(result_stack.tokens[i]);
        }
        */
        double result = calculate(&stack, &result_stack);
        free_memory(First, &stack, &result_stack);
        if (result != MIN)
            printf("\n The answer is %f. Come on! \n", result);
    }
}

void main(int argc, char *argv[])
{   
    printf("\nSimple command line calculator\n");
    char buffer[BUFFER_SIZE];
    do
    {
        printf("\nPlease enter expression('quit' to exit):\n");
        scanf("%[^\n]", buffer);
        if (!mem_cmp(buffer, "quit", 4))
            break;
        start_calculation(buffer);
        getchar();
    } while (1);
}
