//CSci230 - Assignment 2 Question 2
//Patrick Dougherty
//patrick.r.dougherty@und.edu
//30Sep19

/*
C program for arithmetic statement evaluation using stacks.
Operators are restricted to +, -, *, /, (, and )
Operands are restricted to integers (eg. 1, -22, 531...)
Total number of operands and operators is limited to 100 characters
The program will exit upon finding:
	division by zero
	unmatched parenthesis
	unknown operators or characters
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char opStack[50];
int valStack[50]; 
int opTop = -1;
int valTop = -1; 

int performOp (int a, int b, char op)
{
	int res = 0;
	if (op == '+') {
		res = a + b;
	}
	else if (op == '-') {
		res = a - b;
	}
	else if (op == '*') {
		res = a * b;
	}
	else if (op == '/') {
		if (b == 0) {
			printf("Error - Divide by zero detected at %d%c%d",a,op,b);
			exit(0);
		}
		res = a / b;
	}
	else {
		printf("Error - Unknown operator encountered: %c", op);
		exit(0);
	}
	return res;
}

int determinePrecidence (char op1, char op2)
{
	if ((op1=='+'||op1=='-')&&(op2=='*'||op2=='/')) {
		return 1;
	}
	else if ((op1=='*'||op1=='/')&&(op2=='+'||op2=='-')) {
		return 0;
	}
	else if (op1=='(') {
		return 1;
	}
	else {
		return 0;
	}
}

int determineToken (char z)
{
	if (z=='+'||z=='-'||z=='*'||z=='/') {
		return 0;	
	}
	else if (z=='(') {
		return 1;
	}
	else if (z==')') {
		return 2;
	}
	else if (z== ' ') {
		return 4;
	}
	else if (z > 47 && z < 58) {
		return 3;
	}
	else {
		printf("Improper character encountered at: %c\n", z);
		exit(1);
	}
}

int main() {
char exp[100];
printf("This expression evaluator only supports integers, +, -, *, /, ( and ).\n"); 
printf("Please enter an expression to evaluate:\n");
fgets (exp, 100, stdin);
//strcpy(exp, "7+5 * 8-4/(2-1) +6/2*3- 6/(2*3)+(5+(3*(     2+1)+2)*6)*3");
//char * exp = "-102+12 *(32-42)+28";
size_t i=0;
int result, tokenType, opPrec, negFlag=0;
while(exp[i] != '\n' && exp[i] != '\0') 
{
	tokenType = determineToken(exp[i]);
	if (exp[i] == ' ') {
		++i;
	}
	else if (tokenType == 3) {
		if (i > 0 && exp[i-1] != ' ' && determineToken(exp[i-1]) == 3) {
			valStack[valTop] *= 10;
			if (valStack[valTop] < 0 ) {				
				valStack[valTop] -= (exp[i] - 48);
				++i;
			}
			else {
				valStack[valTop] += (exp[i] - 48);
				++i;
			}
		}
		else {
			++valTop;
			valStack[valTop] = exp[i] -48;
			++i;
		}
	}
	else if (tokenType == 1) {
		++opTop;
		opStack[opTop] = exp[i];
		++i;
	}
	else if (tokenType == 2) {
	    while (opStack[opTop] != '(') {
		if (opTop == -1) {
		    printf ("Error - Unbalanced parenthesis found.");
		    exit(0);
		}
		result=performOp(valStack[valTop-1],valStack[valTop],opStack[opTop]);
	      	opStack[opTop] = '\0';
	      	--opTop;
	      	valStack[valTop] = 0;
	      	--valTop;
	      	valStack[valTop] = result;
	    }
	    opStack[opTop] = '\0';
	    --opTop;
	    ++i;
	}
	else if (tokenType == 0) {
		opPrec = determinePrecidence(opStack[opTop], exp[i]);
		if (opTop == -1 && valTop == -1 && exp[i] == '-') {
			++valTop;
			valStack[valTop] = -1;
			++opTop;
			opStack[opTop] = '*';
			++i;
			continue;
		}
		if (opTop == -1 && valTop == -1 && exp[i] == '+') {
			++valTop;
			valStack[valTop] = 0;
			++opTop;
			opStack[opTop] = '+';
			++i;
		}
		int j = i-1;
		while (exp[j] == ' ') {
			--j;
		}
		if (determineToken(exp[j])==0||determineToken(exp[j])==1) {
		    if(exp[i] == '-') {
			++valTop;
			valStack[valTop] = -1;
			++opTop;
			opStack[opTop] = '*';
			++i;
			continue;
		    }
		    else if (exp[i] == '+') {
			++valTop;
			valStack[valTop] = 0;
			++opTop;
			opStack[opTop] = '+';
			++i;
			continue;
		    }
		    else if (exp[i] =='*' || exp[i] =='/') {
			printf("Improper operator placement detected at %c, %c. Exiting.\n",
                        exp[j], exp[i]);
            		exit(0);
          	    }
		}
		if (opTop  == -1 || opPrec) {
	  		++opTop;
			opStack[opTop] = exp[i];
	 		++i;
	  	}
	  	
		else {
	    		while (opTop != -1 && !(opPrec))
	    		{
	      		  result=performOp(valStack[valTop-1],valStack[valTop],opStack[opTop]);
		          opStack[opTop] = '\0';
		          --opTop;
		          valStack[valTop] = 0;
		          --valTop;
		          valStack[valTop] = result;
		          opPrec = determinePrecidence(opStack[opTop], exp[i]);
			}
	    		++opTop;
	    		opStack[opTop] = exp[i];
	    		++i;
	  	}
	}
}
while (opTop != -1)
{
	result=performOp(valStack[valTop-1],valStack[valTop],opStack[opTop]);
	opStack[opTop] = '\0';
	--opTop;
	valStack[valTop] = 0;
	--valTop;
	valStack[valTop] = result;
}    

exp[i] = '\0';
printf("The result of %s is %d\n", exp, valStack[0]);
return 0;
}
