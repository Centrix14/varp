/*
 * varp interpreter
 * v0.3
 * 23.02.2020
 * by Centrix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 7

typedef struct _var {
	char name[256];
	char val[256];
	int is_atom;
} var;

int is_func(char*);
int is_such_var(char*);
char *del_sym(char *str, int sym);
char *get_token(char*, int, int*);
void run(char*);

void print(char*),
	 end(char*),
	 comment(char*),
	 let(char*),
	 to_func(char*),
	 to_atom(char*),
	 var_info(char*);

void (*funcs[])(char*) = {print, end, comment, let, to_func, to_atom, var_info};
char *names[] = {"print", "end", "\'", "let", "-f", "-a", "-i"};

var array[2048];
int count = 0;

char result[256];

int main(int argc, char *argv[]) {
	char input[256];

	if (argc == 2)
		freopen(argv[1], "r", stdin);	

	gets(input);
	while (input && !feof(stdin)) {
		run(input);
		
		gets(input);
	}
	return 0;
}

void run(char *line) {
	int op_code = -1, mpos = 0;
	char *tok = get_token(line, '/', &mpos);

	while (tok) {
		if (is_func(tok) >= 0)
			op_code = is_func(tok);
		else if (op_code >= 0)
			(*funcs[op_code])(tok);
		else if (*tok == '@' && is_such_var(del_sym(tok, '@')) >= 0)
			sprintf(result, "%s", array[is_such_var(del_sym(tok, '@'))].val);	
		else
			sprintf(result, "%s", tok);

		tok = get_token(line, '/', &mpos);
	}
}

int is_func(char *word) {
	for (int i = 0; i < LEN; i++)
		if (!strcmp(names[i], word)) return i;
	return -1;	
}

int is_such_var(char *name) {
	for (int i = 0; i < count; i++)
		if (!strcmp(array[i].name, name)) return i;
	return -1;
}

char *del_sym(char *str, int sym) {
	static char out[256];
	char *optr = out;

	while (*str) {
		if (*str != sym) {
			*optr = *str;
			optr++;
		}
		str++;
	}
	*optr = '\0';
	printf("ds %s\n", out);

	return out;
}

char *get_token(char *str, int delim, int *pos) {
	static char out[256];
	char *sptr = str+(*pos), *optr = out;
	int sticking = 0;

	if (!*sptr) return NULL;

	while (*sptr) {
		if (*sptr == delim && !sticking)
			break;

		if (*sptr == '\"') {
			sticking = !sticking;
			sptr++;
			continue;
		}

		*optr = *sptr;
		sptr++; optr++;
	}
	*optr = '\0';
	*pos = (sptr-str)+1;

	return out;
}

void print(char *arg) {
	char *tmp = "";

	run(arg);
	puts(result);
	strcpy(result, "#1");
}

void end(char *arg) {
	exit(0);
}

void comment(char *arg) {;}

void let(char *arg) {
	int indx = -1, lpos = 0;
	char *tmp = get_token(arg, ' ', &lpos);

	if (!tmp) {
		fprintf(stderr, "can\'t read arg\n");	
		strcpy(result, "#0");
		return;
	}

	indx = is_such_var(tmp);
	if (indx >= 0) {
		tmp = get_token(arg, ' ', &lpos);

		run(tmp);
		tmp = result;

		if (tmp)
			sprintf(array[indx].val, "%s", tmp);
		else
			sprintf(array[indx].val, "#0");
		strcpy(result, "#1");
	}
	else {
		sprintf(array[count].name, "%s", tmp);

		tmp = get_token(arg, ' ', &lpos);
		run(tmp);
		tmp = result;

		if (tmp)
			sprintf(array[count].val, "%s", tmp);
		else
			sprintf(array[count].val, "#0");

		array[count].is_atom = 0;

		strcpy(result, array[count].name);
		++count;
	}
}

void get_val(char *arg) {
	int indx = -1;

	indx = is_such_var(arg);
	if (indx >= 0)
		strcpy(result, array[indx].val);
	else
		strcpy(result, "#err");
}

void to_func(char *arg) {
	int indx = 0;

	run(arg);
	indx = is_such_var(result);

	if (indx >= 0)
		array[indx].is_atom = 0;	
	else
		fprintf(stderr, "no such var %s\n", arg);	
	strcpy(result, arg);
}

void to_atom(char *arg) {
	int indx = 0;

	run(arg);
	indx = is_such_var(result);

	if (indx >= 0)
		array[indx].is_atom = 1;
	else
		fprintf(stderr, "no such var %s\n", arg);	
	strcpy(result, arg);
}

void var_info(char *arg) {
	int indx = 0;

	printf("-i %s\n", arg);
	run(arg);
	printf("-i %s\n", result);
	indx = is_such_var(result);

	if (indx >= 0)
		sprintf(result, "NAME: %s\nVALUE: %s\nIS_ATOM: %d\n", array[indx].name, array[indx].val, array[indx].is_atom);	
	else
		strcpy(result, "#err");	
}
