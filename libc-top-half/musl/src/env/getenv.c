#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>

// Store environment variable locally (they need to persist between calls)
// We query them everytime because they can change (e.g. COLUMNS, ROWS)
static char **names;
static char **values;
static int numValues = 0;

char *getenv(const char *name)
{
	// a-Shell version: asks directly for env variable:
	// but store it locally because the pointer must remain valid
	int position = 0; 

	if (numValues == 0) {
		position = 0;
		names = malloc(sizeof(char**)); 
		values = malloc(sizeof(char**));
		names[0]  = malloc(PATH_MAX * sizeof(char)); 
		values[0] = malloc(PATH_MAX * sizeof(char)); 
	    numValues += 1; 
	} else {
		// did we meet this one before?
		for (; position < numValues; position++) {
			if (strcmp(names[position], name) == 0) break;
		}
	    if ((position >= numValues) || (strcmp(names[position], name) != 0)) {
	    	// We don't have it
	    	names = (char **) realloc(names, (numValues + 2) * sizeof(char**));
	    	values = (char **) realloc(values, (numValues + 2) * sizeof(char**));
	    	position = numValues;
	    	numValues += 1; 
	    	names[position]  = malloc(PATH_MAX * sizeof(char)); 
			values[position] = malloc(PATH_MAX * sizeof(char)); 
		} 
	}

	strcpy(names[position], name);
	memset(values[position], 0, PATH_MAX * sizeof(char));
	
	size_t bufsize = PATH_MAX;
	size_t bufused;

	__wasi_errno_t error = __wasi_ashell_getenv(name, strlen(name), (uint8_t*)values[position], bufsize, &bufused);

	if (bufused == 0) { 
		return NULL;
	}
	return values[position];

	/*
#ifdef __wasilibc_unmodified_upstream // Lazy environment variable init.
#else
	// This specialized header is included within the function body to arranges for
	// the environment variables to be lazily initialized. It redefined `__environ`,
	// so don't remove or reorder it with respect to other code.
#include "wasi/libc-environ-compat.h"
#endif
size_t l = __strchrnul(name, '=') - name;
if (l && !name[l] && __environ)
for (char **e = __environ; *e; e++)
if (!strncmp(name, *e, l) && l[*e] == '=')
return *e + l+1;
return 0;
*/
}
