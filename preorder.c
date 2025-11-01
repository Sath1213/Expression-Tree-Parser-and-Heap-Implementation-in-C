/*
Name: Sathursan Rasatheepan
login: srasathe
Student ID: 1343472
Date September 26, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef enum { NODE_OP, NODE_NUM, NODE_VAR } NodeType;

typedef struct Node {
    NodeType type;
    char op;
    double val;
    char varname[4];
    struct Node *left, *right;
} Node;

// Recursively frees all nodes in the tree
void free_tree(Node *n) {
    if (!n) return;
    free_tree(n->left);
    free_tree(n->right);
    free(n);
}


// Checks if a character is a valid arithmetic operator
int is_valid_op(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Creates a numeric node
Node *make_num(double v) {
    Node *n = malloc(sizeof(Node));
    if (!n) exit(EXIT_FAILURE);
    n->type = NODE_NUM;
    n->val = v;
    n->left = n->right = NULL;
    return n;
}


// Creates a variable node
Node *make_var(const char *v) {
    Node *n = malloc(sizeof(Node));
    if (!n) exit(EXIT_FAILURE);
    n->type = NODE_VAR;
    strcpy(n->varname, v);
    n->left = n->right = NULL;
    return n;
}


// Creates an operator node with given children
Node *make_op(char op, Node *l, Node *r) {
    Node *n = malloc(sizeof(Node));
    if (!n) exit(EXIT_FAILURE);
    n->type = NODE_OP;
    n->op = op;
    n->left = l;
    n->right = r;
    return n;
}

Node *parse_expr(const char *s, int *i);

// Parse a number
Node *parse_number(const char *s, int *i) {
    if (!isdigit((unsigned char)s[*i])) return NULL;
    char a = s[*i]; (*i)++;

    if (s[*i] != '.') return NULL;
    (*i)++;

    if (!isdigit((unsigned char)s[*i])) return NULL;
    char b = s[*i]; (*i)++;

    if (!isdigit((unsigned char)s[*i])) return NULL;
    char c = s[*i]; (*i)++;

    char buf[8] = {a, '.', b, c, '\0'};
    double val = strtod(buf, NULL);
    return make_num(val);
}

// Parse a variable
Node *parse_variable(const char *s, int *i) {
    if (s[*i] != 'x') return NULL;
    (*i)++;

    if (!isdigit((unsigned char)s[*i])) return NULL;
    char d = s[*i]; (*i)++;

    if (d < '1' || d > '9') return NULL;

    char buf[4] = {'x', d, '\0'};
    return make_var(buf);
}


// Parse an operand
Node *parse_operand(const char *s, int *i) {
    if (s[*i] == '(') return parse_expr(s, i);
    if (s[*i] == 'x') return parse_variable(s, i);
    if (isdigit((unsigned char)s[*i])) return parse_number(s, i);
    return NULL;
}


// Parse an expression
Node *parse_expr(const char *s, int *i) {
    if (s[*i] != '(') return NULL;
    (*i)++;

    // Parse the left operand
    Node *left = parse_operand(s, i);
    if (!left) return NULL;

    // Expect an operator next
    if (!is_valid_op(s[*i])) {
        free_tree(left);
        return NULL;
    }

    char op = s[*i];
    (*i)++;

    // Parse the right operand
    Node *right = parse_operand(s, i);
    if (!right) {
        free_tree(left);
        return NULL;
    }

    // The expression must end with a closing parenthesis
    if (s[*i] != ')') {
        free_tree(left);
        free_tree(right);
        return NULL;
    }
    (*i)++;

    return make_op(op, left, right);
}

// Entry point for parsing
Node *build_tree(const char *s) {
    int i = 0;
    Node *root = parse_expr(s, &i);

    if (!root || s[i] != '\0') {
        if (root) free_tree(root);
        return NULL;
    }

    return root;
}

// Visit the root first, then recursively traverse left and right subtrees
void preorder(Node *n) {
    if (!n) return;

    if (n->type == NODE_OP)
        printf("%c ", n->op);
    else if (n->type == NODE_NUM)
        printf("%.2f ", n->val);
    else
        printf("%s ", n->varname);

    preorder(n->left);
    preorder(n->right);
}

int main(int argc, char **argv) {
    // Validate command-line argument
    if (argc < 2) {
        fprintf(stderr, "Error: syntax error.\n");
        return 1;
    }

    // Build expression tree from input string
    Node *root = build_tree(argv[1]);
    if (!root) {
        fprintf(stderr, "Error: syntax error.\n");
        return 1;
    }

    // Print the tree using preorder traversal
    preorder(root);
    printf("\n");

    free_tree(root);
    return 0;
}
