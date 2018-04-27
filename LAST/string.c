#define NULL 0

// int strlen(const char *s)
// {
//   int i = 0;
//   while(*s){
//     i++; s++;
//   }
//   return i;
// }

// int strcmp(char *s1, char *s2)
// {
//   while((*s1 && *s2) && (*s1==*s2)){
//       s1++; s2++;
//   }
//   if ((*s1==0) && (*s2==0))
//     return 0;
//   return(*s1 - *s2);

// }

// int strcpy(char *dest, char *src)
// {
//   while(*src){
//     *dest++ = *src++;
//   }
//   *dest = 0;
// }

char *my_strcpy(char *destination, const char *source) {
	int i = 0;
	while (source[i] != '\0') {
		destination[i] = source[i];
		i++;
	}
	destination[i] = '\0';
	return destination;
}

int my_strlen(char *string) {
	int length = 0;
	while( string[length] != '\0') {
		length++;
	}
	return length;
}

//String.h grabbed from : http://clc-wiki.net/wiki/C_standard_library:string.h
//strchr: searches first occurence of c in string pointed by s
char *strchr(const char *s, int c){
    while(*s){
        if(*s == c){
            break;
        }
        s++;
    }
    return s;
}

int strspn(const char *str, const char *str2){
    int res = 0;
    while(*str && strchr(str2,*str)){
        res++;
    }
    return res;
}

int strcspn(const char *str1, const char *str2){
    int res = 0;
    while(*str1){
        if(strchr(str2,*str1)){
            return res;
        }
        else{
            str1++; res++;
        }
    }
    return res;
}


// int atoi(char *str){
//     int res = 0;
//     int i;
//     // go through char and update resutl:
//     for (i = 0; str[i] != '\0'; ++i){
//         res = res*10 + str[i] - '0';
//     }
//     return res;
// }




// string tokenizer function 
// thanks, Apple:
// (source): https://opensource.apple.com/source/Libc/Libc-167/string.subproj/strtok.c.auto.html

char *
mystrtok(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;

    //prints("Inside mystrtok fn\n");
	if (s == NULL && (s = last) == NULL){
        return (NULL);
    }
		

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}




