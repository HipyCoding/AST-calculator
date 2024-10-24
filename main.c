
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


typedef enum 
{
    NUMBER, ADD, SUBTRACT, MULTIPLY, DIVIDE 
}     NodeType;

typedef struct s_node 
{
    NodeType    type;
    int   value;
    struct s_node *left;
    struct s_node *right;
}     t_node;

t_node *parse_operation(char **expr);

int check_number(char *s) 
{
    return (*s >= '0' && *s <= '9');
}

// Tokenize input
NodeType tokenizer(char **c) 
{
    NodeType type;

    while (**c == ' ')
        (*c)++;

    // Check if token is number
    if (check_number(*c)) 
    {
        type = NUMBER;
        
        // Read complete number
        int value = 0;
        while (**c >= '0' && **c <= '9') 
        {
            value = value * 10 + (**c - '0');
            (*c)++;
        }
        printf("type = NUMBER, value = %d\n", value);
        return type;
    }
    // Check for operators or parentheses
    if (**c == '+') 
    {
        type = ADD;
        printf("type = ADD\n");
        (*c)++;
    }
    else if (**c == '-') 
    {
        type = SUBTRACT;
        printf("type = SUBTRACT\n");
        (*c)++;
    }
    else if (**c == '*') 
    {
        type = MULTIPLY;
        printf("type = MULTIPLY\n");
        (*c)++;
    }
    else if (**c == '/') 
    {
        type = DIVIDE;
        printf("type = DIVIDE\n");
        (*c)++;
    }
    else if (**c == '(') 
    {
        printf("Found opening parenthesis\n");
        (*c)++;
        return tokenizer(c); // Return next token type after (
    }
    else if (**c == ')')
    {
        printf("Found closing parenthesis\n");
        (*c)++;
    }
    else
    {
        printf("Unknown character: %c\n", **c);
        exit(1);
    }
    return type;
}

// create new AST node
t_node *create_node(NodeType type, int value)
{
    t_node *node = (t_node *)malloc(sizeof(t_node));
    node->type = type;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Parse a factor: NUMBER or expression in parentheses
t_node *parse_factor(char **expr)
{
    while (**expr == ' ') 
        (*expr)++;
    if (**expr == '(')
    {
        (*expr)++;
        t_node *node = parse_operation(expr); // Parse the expression inside parentheses
        while (**expr == ' ') 
            (*expr)++;
        if (**expr == ')') 
            (*expr)++;
        return node;
    }
        
    // mini atoi, if number
    int value = 0;
    while (**expr >= '0' && **expr <= '9')
    {
        value = value * 10 + (**expr - '0');
        (*expr)++;
    }
    return create_node(NUMBER, value);
}

// Parse a term: factor * factor or factor / factor
t_node *parse_term(char **expr)
{
    t_node *node = parse_factor(expr);

    while (**expr) 
    {
        while (**expr == ' ')
            (*expr)++;
        
        if (**expr == '*' || **expr == '/')
        {
            NodeType type = tokenizer(expr);

            t_node *right_node = parse_factor(expr);

            t_node *new_node = create_node(type, 0);
            new_node->left = node;
            new_node->right = right_node;
            node = new_node;
        } 
        else 
            break;
    }
    return node;
}

// Parse operation (term + term)
t_node *parse_operation(char **expr)
{
    t_node *node = parse_term(expr);

    while (**expr)
    {
        while (**expr == ' ') 
            (*expr)++;
        
        if (**expr == '+' || **expr == '-')
        {
            NodeType type = tokenizer(expr);

            t_node *right_node = parse_term(expr);

            t_node *new_node = create_node(type, 0);
            new_node->left = node;
            new_node->right = right_node;
            node = new_node;
        } 
        else
            break;
    }
    return node;
}

// go through AST recursively
int evaluate(t_node *node) 
{
    if (node->type == NUMBER) 
        return node->value;

    int left_value = evaluate(node->left);
    int right_value = evaluate(node->right);

    if (node->type == ADD)
        return left_value + right_value;
    else if (node->type == SUBTRACT)
        return left_value - right_value;
    else if (node->type == MULTIPLY)
        return left_value * right_value;
    else if (node->type == DIVIDE)
        return left_value / right_value;
    exit(1);
}


void free_ast(t_node *node) 
{
    if (!node) 
        return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}


int main(int argc, char **argv) 
{
    if (argc != 2) 
    {
        printf("Usage: %s <expression>\n", argv[0]);
        return 1;
    }

    char *expr = argv[1];

    // Parse the expression into an AST
    t_node *ast = parse_operation(&expr);

    // Evaluate the AST
    int result = evaluate(ast);
    printf("Result: %d\n", result);

    free_ast(ast);

    return 0;
}
