/*
Name: Sathursan Rasatheepan
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

// Creates an operator node
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

    // Parse left side
    Node *left = parse_operand(s, i);
    if (!left) return NULL;

    if (!is_valid_op(s[*i])) {
        free_tree(left);
        return NULL;
    }

    char op = s[*i]; (*i)++;

    // Parse right side
    Node *right = parse_operand(s, i);
    if (!right) {
        free_tree(left);
        return NULL;
    }

    // Must end with ')'
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


// Variable structure holds a name, value, and a flag if it was used
typedef struct {
    char name[4];
    double val;
    int used;
} Var;

// Parse variable assignments
int parse_assigns(const char *s, Var *v, int max) {
    if (!s || !*s) return 0;

    int n = 0, i = 0, len = strlen(s);

    while (i < len) {
        if (n >= max) return -1; // too many variables

        if (s[i] != 'x') return -1;
        i++;

        if (!isdigit((unsigned char)s[i])) return -1;
        char d = s[i++];

        if (d < '1' || d > '9') return -1;
        if (s[i] != '=') return -1;
        i++;

	// Parse the numeric value
        if (i + 2 >= len) return -1;
        if (!isdigit((unsigned char)s[i])) return -1;
        char a = s[i++];

        if (s[i] != '.') return -1;
        i++;

        if (!isdigit((unsigned char)s[i])) return -1;
        char b = s[i++];

        if (!isdigit((unsigned char)s[i])) return -1;
        char c = s[i++];

        char buf[8] = {a, '.', b, c, '\0'};
        v[n].val = strtod(buf, NULL);
        sprintf(v[n].name, "x%c", d);
        v[n].used = 0;
        n++;

        if (i == len) break;
        if (s[i] == ';') { i++; continue; }

        return -1;
    }

    return n;
}

// Recursively evaluate the expression tree
double eval(Node *n, Var *v, int nvar, int *divz, int *undef) {
    // Base case: numeric value
    if (n->type == NODE_NUM)
        return n->val;

    // Base case: variable lookup
    if (n->type == NODE_VAR) {
        for (int i = 0; i < nvar; i++) {
            if (strcmp(n->varname, v[i].name) == 0) {
                v[i].used = 1;
                return v[i].val;
            }
        }
        *undef = 1;
        return 0.0;
    }

    // Evaluate left and right sides recursively
    double L = eval(n->left, v, nvar, divz, undef);
    if (*divz || *undef) return 0.0;

    double R = eval(n->right, v, nvar, divz, undef);
    if (*divz || *undef) return 0.0;

    // Apply the operator
    switch (n->op) {
        case '+': return L + R;
        case '-': return L - R;
        case '*': return L * R;
        case '/':
            if (fabs(R) < 1e-12) {
                *divz = 1;
                return 0.0;
            }
            return L / R;
        default:
            *divz = 1;
            return 0.0;
    }
}

// Round a value to 2 decimal places
double round2(double x) {
    double sign = (x < 0) ? -1.0 : 1.0;
    double a = fabs(x);

    // Convert to thousandths for rounding precision
    long long thousandths = (long long)(a * 1000.0 + 1e-9);
    int third = thousandths % 10;
    long long first_two = thousandths / 10;

    if (third >= 5)
        first_two++;

    return sign * first_two / 100.0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: syntax error.\n");
        return 1;
    }

    // Build the expression tree from input
    Node *root = build_tree(argv[1]);
    if (!root) {
        fprintf(stderr, "Error: syntax error.\n");
        return 1;
    }

    Var vars[20];
    int nvar = 0;

    // Parse variable assignments
    if (argc >= 3) {
        nvar = parse_assigns(argv[2], vars, 20);
        if (nvar < 0) {
            fprintf(stderr, "Error: syntax error.\n");
            free_tree(root);
            return 1;
        }
    }

    int divz = 0, undef = 0;
    double val = eval(root, vars, nvar, &divz, &undef);

    // Handle runtime errors
    if (divz) {
        fprintf(stderr, "Error: division by zero.\n");
        free_tree(root);
        return 1;
    }

    if (undef) {
        fprintf(stderr, "Error: undefined argument.\n");
        free_tree(root);
        return 1;
    }

    // Print the rounded result
    printf("%.2f\n", round2(val));

    free_tree(root);
    return 0;
}
