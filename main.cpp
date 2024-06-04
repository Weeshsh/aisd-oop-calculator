#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


#define MIN 'n'
#define MAX 'x'
#define LEFT 1
#define RIGHT 2
#define IF 'i'
#define MAX_DIGITS 14 //MIN_INT + spacja
#define ERROR 'E'
#define NEG 'N'
#define GROWTH 8


struct node {
    int data;
    node* next;
    node(int x) {
        data = x;
        next = NULL;
    }
};


void stack_push(node** top, int data) {
    node* new_node = new node(data);

    if (new_node) {
        new_node->next = *top;
        *top = new_node;
    }
}
int stack_empty(node* top) {
    return top == NULL;
}
int stack_top(node* top) {
    if (!stack_empty(top))
        return top->data;
    return NULL;
}
void stack_pop(node** top) {
    node* old_node = NULL;
    if (!stack_empty(*top)) {
        old_node = *top;
        *top = (*top)->next;
        delete old_node;
    }
}


struct string {
    char* str;
    bool errorFlag;
    int size;
    int len;

    string() : str{ new char[GROWTH] {'\0'} }, errorFlag{ false }, size{ GROWTH }, len{ 0 } {};

    void push_back(char c) {

        if (len + 1 >= size) {
            size *= GROWTH;
            char* temp = new char[size];

            std::strcpy(temp, str);

            delete[] str;
            str = temp;
        }

        str[len] = c;
        str[len + 1] = '\0';
        len++;
    }

    void push_back(const char* ch) {
        int add_len = std::strlen(ch);

        if (len + add_len >= size) {
            size *= GROWTH;
            char* temp = new char[size];

            std::strcpy(temp, str);

            delete[] str;
            str = temp;
        }
        
        std::strcpy(str + len, ch);
        str[len + add_len] = '\0';
        len += add_len;
    }


    void push_back(const char* format, int value) {
        char buffer[MAX_DIGITS];
        std::sprintf(buffer, format, value);
        push_back(buffer);
    }

    void clear() {
        delete[] str;
        str = new char[1] {'\0'};
        size = 1;
        errorFlag = false;
        len = 0;
    }

    ~string() {
        delete[] str;
    }
};


int operator_precedence(char operand) {
    if (operand == '+' || operand == '-')
        return 2;

    if (operand == '*' || operand == '/')
        return 3;

    if (operand == NEG)
        return 4;

    return 0;
}
int operator_associativity(char operand) {
    if (operand == '+' || operand == '-' || operand == '*' || operand == '/')
        return LEFT;

    if (operand == NEG)
        return RIGHT;

    return 0;
}
int calculate(int a, int b, char operand) {
    switch (operand) {
    case '+':
        return a + b;

    case '-':
        return a - b;

    case '*':
        return a * b;

    case '/':
        return a / b;
    }
    return 0;
}
void print_operator(char op, node** args_stack) {
    int args = stack_top(*args_stack);

    switch (op) {
    case MIN:
        printf("MIN");
        printf("%d  ", args);
        stack_pop(args_stack);
        break;

    case MAX:
        printf("MAX");
        printf("%d  ", args);
        stack_pop(args_stack);
        break;

    case IF:
        printf("IF  ");
        break;

    default:
        printf("%c  ", op);
        break;
    }
}
void operator_apply(node** operators, node** values, node** args, string* output) {
    char op = stack_top(*operators);
    stack_pop(operators);

    int args_counter = stack_top(*args);
    print_operator(op, args);

    if (output->errorFlag)
        return;

    switch (op) {
    case MIN:
        output->push_back("MIN");
        output->push_back("%d ", args_counter);
        break;

    case MAX:
        output->push_back("MAX");
        output->push_back("%d ", args_counter);
        break;

    case IF:
        output->push_back("IF  ");
        break;

    default:
        output->push_back("%c ", op);
        break;
    }

    node* current = *values;
    while (current != NULL) {
        output->push_back("%d ", current->data); //wypisanie calego stosu
        current = current->next;
    }
    //nie trzeba usuwac current, struktura lokalna

    int value1 = stack_top(*values);
    stack_pop(values);

    if (op == IF) {
        int value2 = stack_top(*values);
        stack_pop(values);
        int value3 = stack_top(*values);
        stack_pop(values);

        stack_push(values, (value3 > 0 ? value2 : value1)); //operacja IF, kolejnosc odwrocona
    }
    else if (op == MIN || op == MAX) {
        int temp = (op == MIN ? INT_MAX : INT_MIN);

        while (args_counter--) {
            if (op == MIN) {
                if (value1 < temp)
                    temp = value1;
            }
            else {
                if (value1 > temp)
                    temp = value1;
            }

            value1 = stack_top(*values);
            if (args_counter)
                stack_pop(values);
        }
        stack_push(values, temp);
    }
    else if (op == NEG) {
        stack_push(values, -value1);
    }
    else {
        int value2 = stack_top(*values);
        stack_pop(values);

        if (op == '/' && value1 == 0) {
            output->push_back("\nERROR\n"); //dzielenie przez zero
            output->errorFlag = true;
            return;
        }

        stack_push(values, calculate(value2, value1, op));
    }

    output->push_back('\n');
}


void clear(node** values, node** args_stack, string* output) {
    while (!stack_empty(*values))
        stack_pop(values);

    while (!stack_empty(*args_stack))
        stack_pop(args_stack);

    output->clear();
}


int main() {
    node* operators = NULL, * values = NULL, * args_stack = NULL;
    string output;
    int n;
    scanf_s("%d", &n);

    while (n--) {
        char input = getchar();
        while (input != '.') {
            if (input == '\n' || input == ' ') {
                input = getchar();

                if (input == '.')
                    break;
            }
            if (isdigit(input)) {
                char buff[MAX_DIGITS];
                int i = 0;

                while (input != ' ') {
                    buff[i++] = input;
                    input = getchar();
                }
                
                buff[i] = '\0';
                printf("%s  ", buff);

                if (!output.errorFlag) //jezeli jest error, nie marnujemy pamieci
                    stack_push(&values, atoi(buff));
            }
            else if (input == ',') {
                while (stack_top(operators) != '(')
                    operator_apply(&operators, &values, &args_stack, &output);

                //stack_top(operators) zwroci '(', wiec next
                if (!stack_empty(args_stack) && (stack_top(operators->next) == MIN || stack_top(operators->next) == MAX)) 
                    args_stack->data++; 
            }
            else if (input == '(') {
                stack_push(&operators, input);
            }
            else if (input == ')') {
                while (stack_top(operators) != '(')
                    if (!stack_empty(operators))
                        operator_apply(&operators, &values, &args_stack, &output);

                if (stack_top(operators) == '(') {
                    stack_pop(&operators);
                    if (stack_top(operators) == MIN || stack_top(operators) == MAX || stack_top(operators) == IF)
                        operator_apply(&operators, &values, &args_stack, &output);

                }
            }
            else if (input == 'M') {
                getchar(); //zawsze 'A'
                input = getchar(); //'X' lub 'N'

                stack_push(&operators, input == 'X' ? MAX : MIN);
                stack_push(&args_stack, 1);
            }
            else if (input == 'I') {
                getchar(); //zawsze 'F'
                stack_push(&operators, IF);
            }
            else {
                if (input != ' ' && input != '\n' && input != ',') {
                    while (stack_top(operators) != '(' &&
                        ((operator_precedence(stack_top(operators)) > operator_precedence(input)) ||
                            ((operator_precedence(stack_top(operators)) == operator_precedence(input)) && operator_associativity(input) == LEFT))) {
                        operator_apply(&operators, &values, &args_stack, &output);
                    }

                    stack_push(&operators, input);
                }
            }
            input = getchar();
        }

        while (!stack_empty(operators))
            operator_apply(&operators, &values, &args_stack, &output);

        printf("\n%s", output.str);
        if (!output.errorFlag)
            printf("%d\n", stack_top(values));

        clear(&values, &args_stack, &output);
    }

    return 0;
}