#include <stdio.h>
#include <stdlib.h>

#define DELIM " -> "
#define MAX_OPERAND_LEN 16
#define MAX_RULE_STR_LEN (2 * MAX_OPERAND_LEN + sizeof(DELIM))

int str_cmp(const char* a, const char* b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *a == *b;
}

int str_find(const char* str, const char* substr)
{
    int i, j;
    for (i = 0; str[i]; i++) {
        if (str[i] == substr[0]) {
            for (j = 0; substr[j] && str[i + j]; j++) {
                if (str[i + j] != substr[j])
                    goto try_next;
            }
            return i;
        }
    try_next:;
    }
    return -1;
}

void handle_args(int argc, char** argv, char** prules, char** pstate)
{
    *prules = *pstate = NULL;
    for (int i = 1; i < argc; i++) {
        if (str_cmp(argv[i], "--rules"))
            *prules = argv[i + 1];
        if (str_cmp(argv[i], "--state"))
            *pstate = argv[i + 1];
    }
    if (!*prules) {
        fprintf(stderr, "No rules defined\n");
        exit(1);
    }
    if (!*pstate) {
        fprintf(stderr, "Initial state is not defined\n");
        exit(1);
    }
}

typedef struct node {
    char *left, *right;
    struct node* next;
} MarkovRule;

/*
 * "1 -> 0" left: "1", right: "0", end: 0
 * "00 -> 1." left: "00", right: "1", end: 1
 */
void parse_rule_str(const char* rule_str, char (*left)[MAX_OPERAND_LEN + 1],
    char (*right)[MAX_OPERAND_LEN + 1])
{
    int left_end;
    left_end = str_find(rule_str, DELIM);
    if (left_end <= 0) {
        fprintf(stderr, "Syntax error: \"%s\"\n", rule_str);
        exit(1);
    }
    if (left_end > MAX_OPERAND_LEN) {
        fprintf(stderr, "Left operand exceeded max length (%d): \"%s\"\n",
            MAX_OPERAND_LEN, rule_str);
        exit(1);
    }
    int i, j;
    for (i = 0; i < left_end; i++) {
        if (rule_str[i] == '.') {
            fprintf(stderr,
                "Unexpected '.' character in left operand: \"%s\"\n", rule_str);
            exit(1);
        }
        (*left)[i] = rule_str[i];
    }
    (*left)[i] = 0;
    for (i = left_end + sizeof(DELIM) - 1, j = 0; rule_str[i]; i++, j++) {
        if (j == MAX_OPERAND_LEN) {
            fprintf(stderr, "Right operand exceeded max length (%d): \"%s\"\n",
                MAX_OPERAND_LEN, rule_str);
            exit(1);
        }
        (*right)[j] = rule_str[i];
    }
    (*right)[j] = 0;
}

void add_rule(MarkovRule** phead, MarkovRule** ptail, const char* rule_str)
{
    char left[MAX_OPERAND_LEN + 1], right[MAX_OPERAND_LEN + 1];
    parse_rule_str(rule_str, &left, &right);
    printf("left: %s, right: %s;\n", left, right);
}

int main(int argc, char** argv)
{
    char *rules_file_name, *state;
    FILE* rules_file;

    handle_args(argc, argv, &rules_file_name, &state);
    printf("rules: %s, state: %s\n", rules_file_name, state);

    rules_file = fopen(rules_file_name, "r");
    if (!rules_file) {
        perror(rules_file_name);
        exit(1);
    }

    add_rule("01 -> 10");
    add_rule("06789012345 -> 0123456789012345");
    add_rule("06789012345 -> 01234567890123456");
    add_rule("-> 1");

    /*MarkovRule *rules_head, *rules_tail;*/
    /*char buf[64];*/
    /*while (fgets(buf, sizeof(buf), rules_file)) {*/

    /*}*/
    return 1;
}
