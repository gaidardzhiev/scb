#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct {
	char name[64];
	char val[256];
} V;

typedef struct {
	char target[64];
	char deps[32][64];
	int dep_count;
	char cmds[512][256];
	int cmd_count;
} R;

V vars[128]; int var_count = 0; R rules[256]; int rule_count = 0;

char *trim(char *s) {
	char *e;
	while (*s == ' ' || *s == '\t') s++;
	if (*s == 0) return s;
	e = s + strlen(s) - 1;
	while (e > s && (*e == ' ' || *e == '\t' || *e == '\n' || *e == '\r')) e--;
	*(e + 1) = 0;
	return s;
}

const char *get_val(const char *name) {
	for (int i = 0; i < var_count; i++)
		if (strcmp(vars[i].name, name) == 0)
			return vars[i].val;
	return NULL;
}

void substitute(char *line) {
	char buf[512];
	int pos = 0;
	char *p = line;
	while (*p) {
		if (*p == '$' && *(p + 1) == '(') {
			p += 2;
			char var[64];
			int vi = 0;
			while (*p && *p != ')' && vi < (int)sizeof(var) - 1)
				var[vi++] = *p++;
			var[vi] = 0;
			if (*p == ')') p++;
			const char *val = get_val(var);
			if (val) {
				int len = strlen(val);
				if (pos + len < (int)sizeof(buf)) {
					strcpy(&buf[pos], val);
					pos += len;
				}
			}
		} else {
			if (pos < (int)sizeof(buf) - 1)
				buf[pos++] = *p++;
		}
	}
	buf[pos] = 0;
	strcpy(line, buf);
}

void parse_var(char *line) {
	char *eq = strchr(line, '=');
	if (!eq) return;
	*eq = 0;
	char *name = trim(line);
	char *val = trim(eq + 1);
	if (var_count < 128) {
		strncpy(vars[var_count].name, name, sizeof(vars[var_count].name) - 1);
		vars[var_count].name[sizeof(vars[var_count].name) - 1] = 0;
		strncpy(vars[var_count].val, val, sizeof(vars[var_count].val) - 1);
		vars[var_count].val[sizeof(vars[var_count].val) - 1] = 0;
		var_count++;
	}
}

void parse_rule(char *line) {
	char *colon = strchr(line, ':');
	if (!colon) return;
	*colon = 0;
	char *target = trim(line);
	char *deps_str = trim(colon + 1);
	if (rule_count >= 256) return;
	R *r = &rules[rule_count];
	strncpy(r->target, target, sizeof(r->target) - 1);
	r->target[sizeof(r->target) - 1] = 0;
	r->dep_count = 0;
	r->cmd_count = 0;
	char *tok = strtok(deps_str, " \t");
	while (tok && r->dep_count < 32) {
		strncpy(r->deps[r->dep_count], tok, sizeof(r->deps[0]) - 1);
		r->deps[r->dep_count][sizeof(r->deps[0]) - 1] = 0;
		r->dep_count++;
		tok = strtok(NULL, " \t");
	}
	rule_count++;
}

R *find_rule(const char *target) {
	for (int i = 0; i < rule_count; i++)
		if (strcmp(rules[i].target, target) == 0)
			return &rules[i];
	return NULL;
}

int execute_rule(R *r);

int build_target(const char *target) {
	R *r = find_rule(target);
	if(!r) return 0;
	if(strcmp(target, "clean") == 0) {
        return execute_rule(r);
	}
	for(int i=0; i < r->dep_count; i++) {
		int res = build_target(r->deps[i]);
		if(res != 0) return res;
	}
	return execute_rule(r);
}

int execute_rule(R *r) {
	char cwd[256];
	getcwd(cwd, sizeof(cwd));
	printf("CWD: %s\n", cwd);
	for (int i = 0; i < r->cmd_count; i++) {
		char cmd[512];
		strncpy(cmd, r->cmds[i], sizeof(cmd) - 1);
		cmd[sizeof(cmd) - 1] = 0;
		substitute(cmd);
		printf("run: %s\n", cmd);
		int ret = system(cmd);
		printf("ret: %d\n", ret);
		if (ret != 0) {
			fprintf(stderr, "failed: %s\n", cmd);
			return ret;
		}
	}
	return 0;
}

char *trim_cmd(char *line) {
	while (*line == '\t' || *line == ' ') line++;
	return line;
}

int main(int argc, char *argv[]) {
	FILE *f = fopen("builder", "r");
	if(!f) {
		fprintf(stderr, "no builder file\n");
		return 1;
	}
	char line[4096];
	R *current = NULL;
	while(fgets(line, sizeof(line), f)) {
		char *t = trim(line);
		if(t[0] == '#' || t[0] == 0) continue;
		if(strchr(t, '=') && !strchr(t, ':')) {
			parse_var(t);
			current = NULL;
		} else if(strchr(t, ':')) {
			parse_rule(t);
			current = &rules[rule_count - 1];
		} else if(current && (line[0] == '\t' || line[0] == ' ')) {
			char *cmd = trim_cmd(line);
			if(current->cmd_count < 512) {
				strncpy(current->cmds[current->cmd_count], cmd, sizeof(current->cmds[0]) - 1);
				current->cmds[current->cmd_count][sizeof(current->cmds[0]) - 1] = 0;
				current->cmd_count++;
			}
		} else {
			current = NULL;
		}
	}
	fclose(f);
	if(rule_count == 0) {
		fprintf(stderr, "no rules\n");
		return 1;
	}
	const char *target = NULL;
	if(argc > 1) target = argv[1];
	else target = rules[0].target;
	int res = build_target(target);
	if(res == 0)
		printf("success\n");
	return res;
}
