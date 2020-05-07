#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* a-Shell: we do not need this function. */
/*
void __env_rm_add(char *old, char *new)
{
	static char **env_alloced;
	static size_t env_alloced_n;
	for (size_t i=0; i < env_alloced_n; i++)
		if (env_alloced[i] == old) {
			env_alloced[i] = new;
			free(old);
			return;
		} else if (!env_alloced[i] && new) {
			env_alloced[i] = new;
			new = 0;
		}
	if (!new) return;
	char **t = realloc(env_alloced, sizeof *t * (env_alloced_n+1));
	if (!t) return;
	(env_alloced = t)[env_alloced_n++] = new;
} */

int setenv(const char *var, const char *value, int overwrite)
{
	// a-Shell: direct communication with ios_system:
	__wasi_errno_t error = __wasi_ashell_setenv(var, strlen(var), value, strlen(value), overwrite);

	if (error != 0) {
		errno = error;
		return -1;
	}
	
	return 0;

	// version without a-Shell:
	/* 
	char *s;
	size_t l1, l2;

	if (!var || !(l1 = __strchrnul(var, '=') - var) || var[l1]) {
		errno = EINVAL;
		return -1;
	}
	if (!overwrite && getenv(var)) return 0;

	l2 = strlen(value);
	s = malloc(l1+l2+2);
	if (!s) return -1;
	memcpy(s, var, l1);
	s[l1] = '=';
	memcpy(s+l1+1, value, l2+1);
	return __putenv(s, l1, s); */
}
