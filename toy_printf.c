/* caspl-lab-1.c
 * Limited versions of printf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_meta_char,
  st_printf_percent,
  st_printf_octal2,
  st_printf_octal3
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')
#define is_dec_char(ch) ('0' <= (ch) && (ch) <= '9')



int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper(int n, int radix, const char *digit) {
  int result;
  if (n < radix) { //!!! changes are here !!!
    	putchar(digit[n]);
   	    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}


int print_int(int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper(-n, radix, digit);
  }
}


// !!!! helper functions !!! /
int print_unsignedInt_helper(unsigned int n, int radix, const char *digit) {
  int result;
  if (n < radix) { //!!! changes are here !!!
    	putchar(digit[n]);
   	    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_unsignedInt(unsigned int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

 // if (n > 0) {
    return print_unsignedInt_helper(n, radix, digit);
  //}
}



// !!!!!!!!!!!!!!!!!! end helper function !!!!!! // 


/* SUPPORTED:
 *   \f, \t, \n, \r -- formfeed, tab, newline, return
 *   \F, \T, \N, \R -- extensions: visible versions of the above
 *   \c -- extension: CASPL'2018
 *   \C -- extension: visible version of the above
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  int chars_printed = 0;
  int int_value = 0;
  int int_value_copy;
  int min_chars=0; // for 1D represent minumum chars to print
  int case1D = 0 ;
  int case1Dminus = 0;
  char *string_value;
  char char_value;
  char octal_char;
  va_list args;
  enum printf_state state;
  int* int_arr;
  char** c_pointer;
  char* tmp;
  //char str[200];
  int len = 0;
  char _case[3];



  va_start(args, fs);

  state = st_printf_init; 

  for (; *fs != '\0'; ++fs) {
    switch (state) {
    case st_printf_init:
      switch (*fs) {
      case '\\':
	state = st_printf_meta_char;
	break;
	
      case '%':
	state = st_printf_percent;
	break;

      default:
	putchar(*fs);
	++chars_printed;
      }
      break;

    case st_printf_meta_char:
      switch (*fs) {
      case '\\':
	putchar('\\');
	++chars_printed;
	state = st_printf_init;
	break;
	
      case '\"':
	putchar('\"');
	++chars_printed;
	state = st_printf_init;
	break;
	
      case 't':
	putchar('\t');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'T':
	chars_printed += toy_printf("<tab>\t");
	state = st_printf_init;
	break;

      case 'f':
	putchar('\f');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'F':
	chars_printed += toy_printf("<formfeed>\f");
	state = st_printf_init;
	break;

      case 'n':
	putchar('\n');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'N':
	chars_printed += toy_printf("<newline>\n");
	state = st_printf_init;
	break;

      case 'r':
	putchar('\r');
	++chars_printed;
	state = st_printf_init;
	break;

      case 'R':
	chars_printed += toy_printf("<return>\r");
	state = st_printf_init;
	break;

      case 'c':
	chars_printed += toy_printf("CASPL'2018");
	state = st_printf_init;
	break;

      case 'C':
	chars_printed += toy_printf("<caspl magic>");
	chars_printed += toy_printf("\\c");
	state = st_printf_init;
	break;

      default:
	if (is_octal_char(*fs)) {
	  octal_char = *fs - '0';
	  state = st_printf_octal2;
	}
	else {
	  toy_printf("Unknown meta-character: \\%c", *fs);
	  exit(-1);
	}
      }
      break;

    case st_printf_octal2:
      if (is_octal_char(*fs)) {
	octal_char = (octal_char << 3) + (*fs - '0');
	state = st_printf_octal3;
	break;
      }
      else {
	toy_printf("Missing second octal char. Found: \\%c", *fs);
	exit(-1);
      }

    case st_printf_octal3:
      if (is_octal_char(*fs)) {
	octal_char = (octal_char << 3) + (*fs - '0');
	putchar(octal_char);
	++chars_printed;
	state = st_printf_init;
	break;
      }
      else {
	toy_printf("Missing third octal char. Found: \\%c", *fs);
	exit(-1);
      }

    case st_printf_percent:
      switch (*fs) {
      case '%':
	putchar('%');
	++chars_printed;
	state = st_printf_init;
	break;

      case 's':
	string_value = va_arg(args, char *);
	len = strlen(string_value);
	state = st_printf_init;
	if(min_chars>0){
		int_value_copy = int_value;
		while(int_value_copy>0){
			len = len+1;
			int_value_copy = int_value_copy/10;
		}
		if(len>=min_chars){
			chars_printed += toy_printf(string_value);
		}
		else{
			len = min_chars - len;
			if(case1Dminus == 1 ){
				for(int i = 0 ; i< len ; i++){
					if(case1D == 0){
						putchar(' ');
					}
					else{
						putchar('0');
					}
				}
				chars_printed += toy_printf(string_value);
			}
			else{
				chars_printed += toy_printf(string_value);
				for(int i = 0 ; i< len ; i++){
					if(case1D == 0){
						putchar(' ');
					}
					else{
						putchar('0');
					}
				}	
				putchar('#');				
			}
		}	
	}
	else{
	chars_printed += toy_printf(string_value);
	}	
	break;

      case 'd':
	int_value = va_arg(args, int);
	state = st_printf_init;
	if(int_value<0){
		putchar('-');
		len = len +1;
		int_value = int_value * (-1);
	}
	if(min_chars>0){
		int_value_copy = int_value;
		while(int_value_copy>0){
			len = len+1;
			int_value_copy = int_value_copy/10;
		}
		if(len>=min_chars){
			chars_printed += print_int(int_value, 10, digit);
		}
		else{

			len = min_chars - len;
			if(case1Dminus == 1 ){
				for(int i = 0 ; i< len ; i++){
					if(case1D == 0){
						putchar(' ');
					}
					else{
						putchar('0');
					}
				}
				chars_printed += print_int(int_value, 10, digit);
			}
			else{
				chars_printed += print_int(int_value, 10, digit);
				for(int i = 0 ; i< len ; i++){
					if(case1D == 0){
						putchar(' ');
					}
					else{
						putchar('0');
					}
				}	
				putchar('#');				
			}

		}	
	}
	else{
		chars_printed += print_int(int_value, 10, digit);
	}	
	break;

	//!!!!!!!!!!!!!!!! 1B !!!!!!!!!!!!!!!!!!!!!!!!!//

      case 'u':
	int_value = va_arg(args, int);
		state = st_printf_init;
		chars_printed += print_unsignedInt(int_value, 10, digit);
	break;


	//!!!!!!!!!!!!!!!! 1C !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

		case 'A':
		state = st_printf_init;
		putchar('{');
		++fs;
		char type = fs[0];
		switch(type){

		      case 'c':
			    tmp = va_arg(args, char*);

				int_value = va_arg(args, int);
				for(int i = 0 ; i<int_value ; i++ ){
					++chars_printed;
					if(i == int_value -1){
						chars_printed +=toy_printf("'%c'", tmp[i] );
					
					}		
					else{
						chars_printed +=toy_printf("'%c', ", tmp[i] );

					}
				}				
				break;


			case 's':
				c_pointer = va_arg(args, char*);
				int_value = va_arg(args, int);
				for(int i = 0 ; i<int_value ; i++ ){
					if(i == int_value -1){
						putchar('"');
						chars_printed += toy_printf("%s", *c_pointer );
						putchar('"');
					
					}		
					else{
						putchar('"');
						chars_printed+=toy_printf("%s", *c_pointer );
						putchar('"');
						putchar(',');
						putchar(' ');

						c_pointer++;
					}	
				}				

				break;

			case 'x' : case 'b' : case 'u' : case 'o' : case 'd':  	
				_case[0] ='%';
				_case[1] = type;
				_case[2] = '\0';
			 	int_arr = va_arg(args, int*);
			 	int_value = va_arg(args, int);
			 	for(int i = 0 ; i<int_value ; i++ ){		 			
						chars_printed += toy_printf(_case, int_arr[i] );
			 			state = st_printf_init;
			 			if(i != int_value -1){
			 				putchar(',');
			 			}						
			 	}
			 	break;

		    default:
				toy_printf("Unhandled format %%%c...\n", *fs);
				exit(-1);
				break;
		}				
		putchar('}');


	break;	




     case 'b':
	int_value = va_arg(args, int);
		chars_printed += print_unsignedInt(int_value, 2, digit);
		state = st_printf_init;
		break;

      case 'o':
		int_value = va_arg(args, int);
		chars_printed += print_unsignedInt(int_value, 8, digit);
		state = st_printf_init;
		
	break;

	
      case 'x':
		int_value = va_arg(args, int);
		chars_printed += print_unsignedInt(int_value, 16, digit);
		state = st_printf_init;
		
	break;


      case 'X':
		int_value = va_arg(args, int);
		chars_printed += print_unsignedInt(int_value, 16, DIGIT);
		state = st_printf_init;
		
	break;



      case 'c':
	char_value = (char)va_arg(args, int);
	putchar(char_value);
	++chars_printed;
	state = st_printf_init;
	break;

	//!!!!!!!!!!!!!!!!!!!!!!!!!! 1D !!!!!!!!!!!!!!!!!!!
    	case ('-'):
    	case1Dminus = 1;

    	break;


	 default:
	if (is_dec_char(*fs)) {
		if(*fs == '0'){
			case1D = 1;
			++fs;
		}
		 while(is_dec_char(*fs)){
		 	char_value = *fs;
		 	min_chars = min_chars*10;
		 	min_chars += char_value - '0'; //int min_chars has the min char num
		 	++fs;  
		 }
		 --fs;
	}

	else {
	toy_printf("Unhandled format %%%c...\n", *fs);
	  exit(-1);
	}
      }
      break;


    default:
      toy_printf("toy_printf: Unknown state -- %d\n", (int)state);
      exit(-1);
    }
  }

  va_end(args);

  return chars_printed;
}
