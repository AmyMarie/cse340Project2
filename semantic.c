/*----------------------------------------------------------------------
note: the code in this file is not to be shared with anyone or posted online.
(c) Rida Bazzi, 2013
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "syntax.h"

#define TRUE 1
#define FALSE 0

//----------------------------- token types ------------------------------
#define KEYWORDS 14
#define RESERVED 38
#define VAR 1
#define BEGIN 2
#define END 3
#define ASSIGN 4
#define IF 5
#define WHILE 6
#define DO 7
#define THEN 8
#define PRINT 9
#define INT 10
#define REAL 11
#define STRING 12
#define BOOLEAN 13
#define TYPE 14
#define PLUS 15
#define MINUS 16
#define DIV 17
#define MULT 18
#define EQUAL 19
#define COLON 20
#define COMMA 21
#define SEMICOLON 22
#define LBRAC 23
#define RBRAC 24
#define LPAREN 25
#define RPAREN 26
#define NOTEQUAL 27
#define GREATER 28
#define LESS 29
#define LTEQ 30
#define GTEQ 31
#define DOT 32
#define ID 33
#define NUM 34
#define REALNUM 35
#define ERROR 36
#define LBRACE 37
#define RBRACE 38
#define NOOP 39

//------------------- reserved words and token strings -----------------------
char *reserved[] = {"",
	"VAR",
	"BEGIN",
	"END",
	"ASSIGN",
	"IF",
	"WHILE",
	"DO",
	"THEN",
	"print",
	"INT",
	"REAL",
	"STRING",
	"BOOLEAN",
	"TYPE",
	"+",
	"-",
	"/",
	"*",
	"=",
	":",
	",",
	";",
	"[",
	"]",
	"(",
	")",
	"<>",
	">",
	"<",
	"<=",
	">=",
	".",
	"ID",
	"NUM",
	"REALNUM",
	"ERROR",
	"{",
	"}"};

int printToken(int ttype)
{
	if (ttype <= RESERVED) {
		printf("%s\n", reserved[ttype]);
		return 1;
	} else
		return 0;
}
//---------------------------------------------------------

//---------------------------------------------------------
// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100

char token[MAX_TOKEN_LENGTH]; // token string
int ttype; // token type
int activeToken = FALSE;
int tokenLength;

int numOfTypes; 
int numOfVars;
struct typeVar* arrayOfTypes[100];
struct typeVar* arrayOfVars[100];

int line_no = 1;

//----------------------------------------------------------

void skipSpace()
{
	char c;

	c = getchar();
	line_no += (c == '\n');
	while (!feof(stdin) && isspace(c)) {
		c = getchar();
		line_no += (c == '\n');
	}

	// return character to input buffer if eof is not reached
	if (!feof(stdin))
		ungetc(c, stdin);
}

int isKeyword(char *s)
{
	int i;

	for (i = 1; i <= KEYWORDS; i++)
		if (strcmp(reserved[i], s) == 0)
			return i;
	return FALSE;
}

// ungetToken() simply sets a flag so that when getToken() is called
// the old ttype is returned and the old token is not overwritten
// NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
// AT LEAST ONE CALL TO getToken()
// CALLING TWO ungetToken() WILL NOT UNGET TWO TOKENS

void ungetToken()
{
	activeToken = TRUE;
}

int scan_number()
{
	char c;

	c = getchar();
	if (isdigit(c)) { // First collect leading digits before dot
		// 0 is a nNUM by itself
		if (c == '0') {
			token[tokenLength] = c;
			tokenLength++;
			token[tokenLength] = '\0';
		} else {
			while (isdigit(c)) {
				token[tokenLength] = c;
				tokenLength++;
				;
				c = getchar();
			}
			ungetc(c, stdin);
			token[tokenLength] = '\0';
		}

		// Check if leading digits are integer part of a REALNUM
		c = getchar();
		if (c == '.') {
			c = getchar();
			if (isdigit(c)) {
				token[tokenLength] = '.';
				tokenLength++;
				while (isdigit(c)) {
					token[tokenLength] = c;
					tokenLength++;
					c = getchar();
				}
				token[tokenLength] = '\0';
				if (!feof(stdin))
					ungetc(c, stdin);
				return REALNUM;
			} else {
				ungetc(c, stdin); // note that ungetc returns characters on a stack, so we first
				c = '.'; // return the second character and set c to '.' and return c again
				ungetc(c, stdin);
				return NUM;
			}
		} else {
			ungetc(c, stdin);
			return NUM;
		}
	} else
		return ERROR;
}

int scan_id_or_keyword()
{
	int ttype;
	char c;

	c = getchar();
	if (isalpha(c)) {
		while (isalnum(c)) {
			token[tokenLength] = c;
			tokenLength++;
			c = getchar();
		}
		if (!feof(stdin))
			ungetc(c, stdin);

		token[tokenLength] = '\0';
		ttype = isKeyword(token);
		if (ttype == 0)		//if it isn't a reserved token, it is an id
			ttype = ID;
		return ttype;		//else return the number of the reserved token
	} else
		return ERROR;
}

int getToken()
{
	char c;

	if (activeToken) {
		activeToken = FALSE;
		return ttype;
	} // we do not need an else because the function returns in the body
	// of the if

	skipSpace();
	tokenLength = 0;
	c = getchar();
	switch (c) {
		case '.': return DOT;
		case '+': return PLUS;
		case '-': return MINUS;
		case '/': return DIV;
		case '*': return MULT;
		case '=': return EQUAL;
		case ':': return COLON;
		case ',': return COMMA;
		case ';': return SEMICOLON;
		case '[': return LBRAC;
		case ']': return RBRAC;
		case '(': return LPAREN;
		case ')': return RPAREN;
		case '{': return LBRACE;
		case '}': return RBRACE;
		case '<':
			c = getchar();
			if (c == '=')
				return LTEQ;
			else if (c == '>')
				return NOTEQUAL;
			else {
				ungetc(c, stdin);
				return LESS;
			}
		case '>':
			c = getchar();
			if (c == '=')
				return GTEQ;
			else {
				ungetc(c, stdin);
				return GREATER;
			}
		default:
			if (isdigit(c)) {
				ungetc(c, stdin);
				return scan_number();
			} else if (isalpha(c)) {
				ungetc(c, stdin);
				return scan_id_or_keyword();
			} else if (c == EOF)
				return EOF;
			else
				return ERROR;
	}
}



/*----------------------------------------------------------------------------
SYNTAX ANALYSIS SECTION
----------------------------------------------------------------------------*/
#define PRIMARY 0
#define EXPR 1

/*-------------------------------UTILITIE---------------------------*/
void syntax_error(char* NT, int line_no)
{
	printf("Syntax error while parsing %s line %d\n", NT, line_no);
}
/*-------------------------------------------------------------------
	DEALLOCATING MEMORY
--------------------------------------------------------------------*/
void freeProgramMem(struct programNode* program)
{
	freeDeclMem(program->decl);
	program->decl = NULL;
	freeBodyMem(program->body);
	program->body = NULL;
	free(program);
	program = NULL;
}

void freeDeclMem(struct declNode* dec)
{
	if (dec->type_decl_section != NULL) {
		freeType_decl_sectionMem(dec->type_decl_section);
		dec->type_decl_section = NULL;
	}
	if (dec->var_decl_section != NULL) {
		freeVar_decl_sectionMem(dec->var_decl_section);
		dec->var_decl_section = NULL;
	}
	free(dec);

}

void freeType_decl_sectionMem(struct type_decl_sectionNode* typeDeclSection)
{
	if (typeDeclSection->type_decl_list != NULL)
	{
		freeType_decl_listiMem(typeDeclSection->type_decl_list);
		typeDeclSection->type_decl_list = NULL;
	}

	free(typeDeclSection);
}

void freeType_decl_listMem(struct type_decl_listNode* typeDeclList)
{
	if(typeDeclList->type_decl != NULL)
	{
		freeType_declMem(typeDeclList->type_decl);
		typeDeclList->type_decl = NULL;
	}
	if (typeDeclList->type_decl_list != NULL)
	{
		freeType_decl_listMem(typeDeclList->type_decl_list);
		typeDecList->type_decl = NULL;
	}

	free(typeDeclList);
}

void freeType_declMem(struct type_declNode* typeDecl)
{
	if(typeDecl->id_list != NULL)
	{
		freeId_listMem(typeDecl->id_list);
		typeDecl->id_list = NULL;
	}

	if(typeDecl->type_name)
	{
		freeType_nameMem(typeDecl->type_name);
		typeDecl->type_name = NULL;
	}

	free(typeDecl);
}

void freeType_nameMem(struct type_nameNode* typeName)
{
	free(typeName);
}

void freeId_listMem(struct id_listNode* idList)
{
	if (idList->id_list != NULL) {
		freeId_listMem(idList->id_list);
		idList->id_list = NULL;
	}
	
	free(idList);
}

void freeVar_decl_sectionMem(struct var_decl_sectionNode* varDeclSection)
{
	if (varDeclSection->var_decl_list != NULL)
	{
		freeVar_decl_listMem(varDeclSection->var_decl_list);
		varDelcSection->var_decl_list = NULL;
	}
	free(varDeclSection);
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
	print_var_decl(varDeclList->var_decl);
	if (varDeclList->var_decl_list != NULL)
		print_var_decl_list(varDeclList->var_decl_list);
}

void print_var_decl(struct var_declNode* varDecl)
{
	print_id_list(varDecl->id_list);
	printf(": ");
	print_type_name(varDecl->type_name);
	printf(";\n");
}
/*--------------------------------------------------------------------
  PRINTING PARSE TREE
---------------------------------------------------------------------*/
void print_parse_tree(struct programNode* program)
{
	print_decl(program->decl);
	print_body(program->body);
}

void print_decl(struct declNode* dec)
{
	if (dec->type_decl_section != NULL) {
		print_type_decl_section(dec->type_decl_section);
	}
	if (dec->var_decl_section != NULL) {
		print_var_decl_section(dec->var_decl_section);
	}
}

void print_body(struct bodyNode* body)
{
	printf("{\n");
	print_stmt_list(body->stmt_list);
	printf("}\n");

}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
	printf("VAR\n");
	if (varDeclSection->var_decl_list != NULL)
		print_var_decl_list(varDeclSection->var_decl_list);
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
	print_var_decl(varDeclList->var_decl);
	if (varDeclList->var_decl_list != NULL)
		print_var_decl_list(varDeclList->var_decl_list);
}

void print_var_decl(struct var_declNode* varDecl)
{
	print_id_list(varDecl->id_list);
	printf(": ");
	print_type_name(varDecl->type_name);
	printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
	printf("TYPE\n");
	if (typeDeclSection->type_decl_list != NULL)
		print_type_decl_list(typeDeclSection->type_decl_list);
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
	print_type_decl(typeDeclList->type_decl);
	if (typeDeclList->type_decl_list != NULL)
		print_type_decl_list(typeDeclList->type_decl_list);
}

void print_type_decl(struct type_declNode* typeDecl)
{
	print_id_list(typeDecl->id_list);
	printf(": ");
	print_type_name(typeDecl->type_name);
	printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
	if (typeName->type != ID)
		printf("%s ", reserved[typeName->type]);
	else
		printf("%s ", typeName->id);
}

void print_id_list(struct id_listNode* idList)
{
	printf("%s ", idList->id);
	if (idList->id_list != NULL) {
		printf(", ");
		print_id_list(idList->id_list);
	}
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
	print_stmt(stmt_list->stmt);
	if (stmt_list->stmt_list != NULL)
		print_stmt_list(stmt_list->stmt_list);

}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
	printf("%s ", assign_stmt->id);
	printf("= ");
	print_expression_prefix(assign_stmt->expr);
	printf("; \n");
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
	

}

void print_stmt(struct stmtNode* stmt)
{
	if (stmt->stmtType == ASSIGN)
		print_assign_stmt(stmt->assign_stmt);
}

void print_expression_prefix(struct exprNode* expr)
{
	if (expr->tag == EXPR) {
		printf("%s ", reserved[expr->operator]);
		print_expression_prefix(expr->leftOperand);
		print_expression_prefix(expr->rightOperand);
	} else
		if (expr->tag == PRIMARY) {
		if (expr->primary->tag == ID)
			printf("%s ", expr->primary->id);
		else
			if (expr->primary->tag == NUM)
			printf("%d ", expr->primary->ival);
		else
			if (expr->primary->tag == REALNUM)
			printf("%.4f ", expr->primary->fval);

	}
}

/*--------------------------------------------------------------------
  CREATING PARSE TREE NODE
---------------------------------------------------------------------*/
struct programNode* make_programNode()
{
	return (struct programNode*) malloc(sizeof (struct programNode));
}

struct declNode* make_declNode()
{
	return (struct declNode*) malloc(sizeof (struct declNode));
}

struct type_decl_sectionNode* make_type_decl_sectionNode()
{
	return (struct type_decl_sectionNode*) malloc(sizeof (struct type_decl_sectionNode));
}

struct var_decl_sectionNode* make_var_decl_sectionNode()
{
	return (struct var_decl_sectionNode*) malloc(sizeof (struct var_decl_sectionNode));
}

struct var_declNode* make_var_declNode()
{
	return (struct var_declNode*) malloc(sizeof (struct var_declNode));
}

struct type_declNode* make_type_declNode()
{
	return (struct type_declNode*) malloc(sizeof (struct type_declNode));
}

struct type_decl_listNode* make_type_decl_listNode()
{
	return (struct type_decl_listNode*) malloc(sizeof (struct type_decl_listNode));
}

struct var_decl_listNode* make_var_decl_listNode()
{
	return (struct var_decl_listNode*) malloc(sizeof (struct var_decl_listNode));
}

struct id_listNode* make_id_listNode()
{
	return (struct id_listNode*) malloc(sizeof (struct id_listNode));
}

struct type_nameNode* make_type_nameNode()
{
	return (struct type_nameNode*) malloc(sizeof (struct type_nameNode));
}

struct bodyNode* make_bodyNode()
{
	return (struct bodyNode*) malloc(sizeof (struct bodyNode));
}

struct stmt_listNode* make_stmt_listNode()
{
	return (struct stmt_listNode*) malloc(sizeof (struct stmt_listNode));
}

struct stmtNode* make_stmtNode()
{
	return (struct stmtNode*) malloc(sizeof (struct stmtNode));
}

struct while_stmtNode* make_while_stmtNode()
{
	return (struct while_stmtNode*) malloc(sizeof (struct while_stmtNode));
}

struct assign_stmtNode* make_assign_stmtNode()
{
	return (struct assign_stmtNode*) malloc(sizeof (struct assign_stmtNode));
}

struct exprNode* make_exprNode()
{
	return (struct exprNode*) malloc(sizeof (struct exprNode));
}

struct primaryNode* make_primaryNode()
{
	return (struct primaryNode*) malloc(sizeof (struct primaryNode));
}

struct conditionNode* make_conditionNode()
{
	return (struct conditionNode*) malloc(sizeof (struct conditionNode));
}
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  PARSING AND BUILDING PARSE TREE
---------------------------------------------------------------------*/
struct primaryNode* primary()
{
	struct primaryNode* primNode;

	ttype = getToken();

	if(ttype == ID || ttype == NUM || ttype == REALNUM)
	{
		primNode = make_primaryNode();
		
		primNode-> tag = ttype;
		
		switch(ttype)
		{
			case (ID) :
				primNode->id = token;
				break;

			case (NUM):	
				primNode ->ival = atoi(token);
				break;	
		
			default :	
				primNode->fval = atoi(token);


		}

		return primNode;
	}
	else
		syntax_error("primary. ID, NUM, or REALNUM expected", line_no);
		
}
///////////////////////////////////////////////////////////////////////
struct conditionNode* condition()
{
	struct conditionNode* condNode;
	ttype = getToken();
	if(ttype == ID ||ttype == NUM || ttype == REALNUM)
	{
		condNode = make_conditionNode();
		ungetToken();

		condNode->left_operand = primary();
		
		ttype=getToken();

		if(ttype == GREATER || ttype == GTEQ || ttype == LESS || ttype == NOTEQUAL || ttype == LTEQ)
		{
			condNode->relop = ttype;
			if(ttype == ID || ttype == NUM || ttype == REALNUM)
			{
				ungetToken();
				condNode->right_operand = primary();
				return condNode;
			}

			else
			{
				syntax_error("condition. ID, NUM, or REALNUM expected", line_no);
				exit(0);
			}
	}

		else if(ttype == LBRACE)
		{
			ungetToken();
			condNode->right_operand = NULL;
			return condNode; 
		}

		else
		{
			syntax_error("condition. REALNUM, GREATER, GTEQ, LESS, NOTEQUAL, or LTEQ expected", line_no);
			exit(0);
		}
	}
	
	else
	{
		syntax_error("condition. ID, NUM, or REALNUM expected", line_no);
		exit(0);
	}
	
	
}

///////////////////////////////////////////////////////////////////////
struct exprNode* factor()
{
	struct exprNode* facto;

	ttype = getToken();

	if (ttype == LPAREN) {
		facto = expr();
		ttype = getToken();
		if (ttype == RPAREN)
			return facto;
		else {
			syntax_error("factor. RPAREN expected", line_no);
			exit(0);
		}
	} else
		if (ttype == NUM) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = NUM;
		facto->primary->ival = atoi(token);
		return facto;
	} else
		if (ttype == REALNUM) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = REALNUM;
		facto->primary->fval = atof(token);
		return facto;
	} else
		if (ttype == ID) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->operator = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = ID;
		facto->primary->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(facto->primary->id, token);
		return facto;
	} else {
		syntax_error("factor. NUM, REALNUM, or ID, expected", line_no);
		exit(0);
	}
}

struct exprNode* term()
{
	struct exprNode* ter;
	struct exprNode* f;


	ttype = getToken();
	if ((ttype == ID) | (ttype == LPAREN) | (ttype == NUM) | (ttype == REALNUM)) {
		ungetToken();
		f = factor();
		ttype = getToken();
		if ((ttype == MULT) | (ttype == DIV)) {
			ter = make_exprNode();
			ter->operator = ttype;
			ter->leftOperand = f;
			ter->rightOperand = term();
			ter->tag = EXPR;
			return ter;
		} else
			if ((ttype == SEMICOLON) | (ttype == PLUS) | (ttype == MINUS) | (ttype == RPAREN)) {
			ungetToken();
			return f;
		} else {
			syntax_error("term. MULT or DIV expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("term. ID, LPAREN, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct exprNode* expr()
{
	struct exprNode* exp;
	struct exprNode* t;

	ttype = getToken();
	if ((ttype == ID) | (ttype == LPAREN) | (ttype == NUM) | (ttype == REALNUM)) {
		ungetToken();
		t = term();
		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS)) {
			exp = make_exprNode();
			exp->operator = ttype;
			exp->leftOperand = t;
			exp->rightOperand = expr();
			exp->tag = EXPR;
			return exp;
		} else
			if ((ttype == SEMICOLON) | (ttype == MULT) | (ttype == DIV) | (ttype == RPAREN)) {
			ungetToken();
			return t;
		} else {
			syntax_error("expr. PLUS, MINUS, or SEMICOLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct assign_stmtNode* assign_stmt()
{
	struct assign_stmtNode* assignStmt;

	ttype = getToken();
	if (ttype == ID) {
		assignStmt = make_assign_stmtNode();
		assignStmt->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(assignStmt->id, token);
		ttype = getToken();
		if (ttype == EQUAL) {
			assignStmt->expr = expr();
			return assignStmt;
		} else {
			syntax_error("assign_stmt. EQUAL expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("assign_stmt. id expected", line_no);
		exit(0);
	}
}

/////////////////////////////////////////////////////////////

struct while_stmtNode* while_stmt()
{
/*struct while_stmtNode{
	struct conditionNode* condition;
	struct bodyNode* boy;
};*/

	struct while_stmtNode *whileNode;

	ttype = getToken();
	
	if(ttype == WHILE)
	{

		ttype = getToken();
		
		if(ttype == ID ||ttype == NUM || ttype == REALNUM)
		{
			ungetToken();
					
			whileNode = make_while_stmtNode();
	
			whileNode->condition = condition();

			ttype = getToken();
			if(ttype == LBRACE)
			{
				ungetToken();
				whileNode->body = body();
				return whileNode;
			}		
			
			else
			{
				syntax_error("while_stmt. LEFTB expected", line_no);
				exit(0);
				
			}
		}
		else
		{
			syntax_error("while_stmt. LEFTB expected", line_no);
			exit(0);
		}
	
	}

	else
	{
		syntax_error("while_stmt. WHILE expected", line_no);
	}


}

/////////////////////////////////////////////////////////////
struct stmtNode* stmt()
{
	struct stmtNode* stm;

	ttype = getToken();
	stm = make_stmtNode();

	if (ttype == ID) // assign_stmt
	{
		ungetToken();
		stm->assign_stmt = assign_stmt();
		stm->stmtType = ASSIGN;
		ttype = getToken();
		if (ttype == SEMICOLON) {
			return stm;
		} else {
			syntax_error("stmt. SEMICOLON expected", line_no);
			exit(0);
		}
	}
	else if (ttype == WHILE) // while_stmt
	{
		ungetToken();
		stm->while_stmt = while_stmt();
		stm->stmtType = WHILE;
		return stm;
	}
	else // syntax error
	{
		syntax_error("stmt. ID or WHILE expected", line_no);
		exit(0);
	}
}

struct stmt_listNode* stmt_list()
{
	struct stmt_listNode* stmtList;

	ttype = getToken();


	if ((ttype == ID) | (ttype == WHILE)) {
		ungetToken();
		stmtList = make_stmt_listNode();
		stmtList->stmt = stmt();
		ttype = getToken();
		if (ttype == ID) {
			ungetToken();
			stmtList->stmt_list = stmt_list();
			return stmtList;
		} else // If the next token is not in FOLLOW(stmt_list),
			// let the caller handle it.
		{
			ungetToken();
			return stmtList;
		}
	} else {
		syntax_error("stmt_list. ID or WHILE expected", line_no);
		exit(0);
	}

}

struct bodyNode* body()
{
	struct bodyNode* bod;

	ttype = getToken();
	if (ttype == LBRACE) {
		bod = make_bodyNode();
		bod->stmt_list = stmt_list();
		ttype = getToken();
		if (ttype == RBRACE)
			return bod;
		else {
			syntax_error("body. RBRACE expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("body. LBRACE expected", line_no);
		exit(0);
	}
}

struct type_nameNode* type_name()
{
	struct type_nameNode* tName;
	tName = make_type_nameNode();

	ttype = getToken();
	if ((ttype == ID) | (ttype == INT) | (ttype == REAL)
			| (ttype == STRING) | (ttype == BOOLEAN)) {
		tName->type = ttype;
		if (ttype == ID) {
			tName->id = (char *) malloc(tokenLength + 1);
			strcpy(tName->id, token);
		}
		return tName;
	} else {
		syntax_error("type_name. type name expected", line_no);
		exit(0);
	}
}

struct id_listNode* id_list()
{
	struct id_listNode* idList;
	idList = make_id_listNode();
	ttype = getToken();
	if (ttype == ID) {
		idList->id = (char*) malloc(tokenLength + 1);
		strcpy(idList->id, token);
		ttype = getToken();
		if (ttype == COMMA) {
			idList->id_list = id_list();
			return idList;

		} else
			if (ttype == COLON) {
			ungetToken();
			idList->id_list = NULL;
			return idList;
		} else {
			syntax_error("id_list. COMMA or COLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("id_list. ID expected", line_no);
		exit(0);
	}

}

struct var_declNode* var_decl()
{
	struct var_declNode* varDecl;
	varDecl = make_var_declNode();
	ttype = getToken();
	if (ttype == ID) {
		ungetToken();
		varDecl->id_list = id_list();
		ttype = getToken();
		if (ttype == COLON) {
			varDecl->type_name = type_name();
			ttype = getToken();
			if (ttype == SEMICOLON) {
				return varDecl;
			} else {
				syntax_error("var_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		} else {
			syntax_error("var_decl. COLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("var_decl. ID expected", line_no);
		exit(0);
	}
}

struct var_decl_listNode* var_decl_list()
{
	struct var_decl_listNode* varDeclList;
	varDeclList = make_var_decl_listNode();

	ttype = getToken();
	if (ttype == ID) {
		ungetToken();
		varDeclList->var_decl = var_decl();
		ttype = getToken();
		if (ttype == ID) {
			ungetToken();
			varDeclList->var_decl_list = var_decl_list();
			return varDeclList;
		} else {
			ungetToken();
			return varDeclList;
		}
	} else {
		syntax_error("var_decl_list. ID expected", line_no);
		exit(0);
	}
}


struct var_decl_sectionNode* var_decl_section()
{
	struct var_decl_sectionNode *varDeclSection;
	varDeclSection = make_var_decl_sectionNode();

	ttype = getToken();
	if (ttype == VAR) { // no need to ungetToken()
		varDeclSection->var_decl_list = var_decl_list();
		return varDeclSection;
	} else {
		syntax_error("var_decl_section. VAR expected", line_no);
		exit(0);
	}
}

struct type_decl_sectionNode* type_decl_section()
{
	struct type_decl_sectionNode *typeDeclSection;
	typeDeclSection = make_type_decl_sectionNode();

	ttype = getToken();
	if (ttype == TYPE) {
		typeDeclSection->type_decl_list = type_decl_list();
		return typeDeclSection;
	} else {
		syntax_error("type_decl_section. TYPE expected", line_no);
		exit(0);
	}
}

struct type_decl_listNode* type_decl_list()
{
	struct type_decl_listNode* typeDeclList;
	typeDeclList = make_type_decl_listNode();

	ttype = getToken();
	if (ttype == ID) {
		ungetToken();
		typeDeclList->type_decl = type_decl();
		ttype = getToken();
		if (ttype == ID) {
			ungetToken();
			typeDeclList->type_decl_list = type_decl_list();
			return typeDeclList;
		} else {
			ungetToken();
			return typeDeclList;
		}
	} else {
		syntax_error("type_decl_list. ID expected", line_no);
		exit(0);
	}
}

struct type_declNode* type_decl()
{
	struct type_declNode* typeDecl;
	typeDecl = make_type_declNode();
	ttype = getToken();
	if (ttype == ID) {
		ungetToken();
		typeDecl->id_list = id_list();
		ttype = getToken();
		if (ttype == COLON) {
			typeDecl->type_name = type_name();
			ttype = getToken();
			if (ttype == SEMICOLON) {
				return typeDecl;
			} else {
				syntax_error("type_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		} else {
			syntax_error("type_decl. COLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("type_decl. ID expected", line_no);
		exit(0);
	}
}

struct declNode* decl()
{
	struct declNode* dec;
	dec = make_declNode();

	ttype = getToken();
	if (ttype == TYPE) {
		ungetToken();
		dec->type_decl_section = type_decl_section();
		ttype = getToken();
		if (ttype == VAR) { // type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
		} else {
			ungetToken();
			dec->var_decl_section = NULL;
		}
		return dec;
	} else {
		dec->type_decl_section = NULL;
		if (ttype == VAR) { // type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
			return dec;
		} else {
			if (ttype == LBRACE) {
				ungetToken();
				dec->var_decl_section = NULL;
				return dec;
			} else {
				syntax_error("decl. LBRACE expected", line_no);
				exit(0); // stop after first syntax error
			}
		}
	}
}

struct programNode* program()
{
	struct programNode* prog;

	prog = make_programNode();
	ttype = getToken();
	if ((ttype == TYPE) || (ttype == VAR) || (ttype == LBRACE)) {
		ungetToken();
		prog->decl = decl();
		prog->body = body();
		return prog;
	} else {
		syntax_error("program. TYPE or VAR or LBRACE expected", line_no);
		exit(0); // stop after first syntax error
	}
}

// COMMON mistakes:
//    *     = instead of ==
//    *     not allocating space before strcpy

int main()
{
	struct programNode* parseTree;
	parseTree = program();
	print_parse_tree(parseTree);
	printf("\nSUCCESSFULLY PARSED INPUT!\n");
	return 0;
}


