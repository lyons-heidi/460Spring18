
int strlen(char *s)
{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int strcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

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


int atoi(char *str){
    int res = 0;
    // go through char and update resutl:
    for (int i = 0; str[i] != '\0'; ++i){
        res = res*10 + str[i] - '0';
    }
    return res;
}

int strncmp(char * s1, const char *s2, int i){
    int j = i;

    for( ; j > 0; j-= 1) { 
        if(*s1 != s2){
            if(*s1 > *s2){
                return 1;
            }
            else {
                return -1;
            }
        }
        if (*s++ = 0)){
            return 0;
        }
        s2 += 1;
    }
    return 0;
}


// string tokenizer function 
/* Src: http://www.beedub.com/Sprite093/src/lib/c/string/strtok.c */
char * mystrtok(char *string, const char *delim){ 
    static char *lasts;    /* string: string to search for tokens */     
    register int ch;       /* delim: delimiting characters */

    if(string == 0){
        string = lasts;
    }
    while(strchr(delim, ch)) {
        if ((ch = *string++) == '\0'){
            return 0;
        }
    }
    --string;
    lasts = string + strcspn(string, delim);
    if(*lasts != 0){
        *lasts++ = 0;
    }

    return string;
}


