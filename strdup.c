#include <string.h>

char * strdup(s)
	char *s;
{
	char  *string;

	if(s == NULL || *s == 0)
		return((char *)0);
	if( (string = (char *)malloc( strlen(s) * sizeof(char) )) == NULL) {
		printf("strdup: malloc error/n");
		return(NULL);
	}
	strncpy(string, s, strlen(s) );
	return (string);
}
