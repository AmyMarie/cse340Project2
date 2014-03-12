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

int numOfExpTypes=0; 
int numOfImpTypes=0;
struct typeOrVar* arrayExplicitTypes[100];
struct typeOrVar* arrayImplicitTypes[100];

int numOfExpVars=0;
int numOfImpVars=0;
struct typeOrVar* arrayExplicitVars[100];
struct typeOrVar* arrayImplicitVars[100];
	int arrayTEx[100];
	int arrayTIm[100];
	int numTExArray=0;
	int numTImArray=0;
	int arrayVEx[100];
	int arrayVIm[100];
	int numVExArray=0;
	int numVImArray=0;

int numOfTypes=0;
//int typeOrVar; //0=type, 1=var, used for placement in arrayT vs arrayV

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

void freeArrays()
{
	int i;
	int j;
	for(i=0; i<numOfImpTypes;i++)
	{
		free(arrayImplicitTypes[i]->id);
		free(arrayImplicitTypes[i]);
	}

	for(j=0;j<numOfExpTypes;j++)
	{
		free(arrayExplicitTypes[j]->id);
		free(arrayExplicitTypes[j]);
	}
	for(j=0;j<numOfExpVars;j++)
	{
		free(arrayExplicitVars[j]->id);
		free(arrayExplicitVars[j]);
	}
	
	for(j=0;j<numOfImpVars;j++)
	{
		free(arrayImplicitVars[j]->id);
		free(arrayImplicitVars[j]);
	}
}

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
		freeType_decl_listMem(typeDeclSection->type_decl_list);
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
		typeDeclList->type_decl = NULL;
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
	free(typeName->id);
	free(typeName);
}

void freeId_listMem(struct id_listNode* idList)
{
	if (idList->id_list != NULL) {
		freeId_listMem(idList->id_list);
		idList->id_list = NULL;
	}
	free(idList->id);
	free(idList);
}

void freeVar_decl_sectionMem(struct var_decl_sectionNode* varDeclSection)
{
	if (varDeclSection->var_decl_list != NULL)
	{
		freeVar_decl_listMem(varDeclSection->var_decl_list);
		varDeclSection->var_decl_list = NULL;
	}
	free(varDeclSection);
}

void freeVar_decl_listMem(struct var_decl_listNode* varDeclList)
{
	if(varDeclList->var_decl != NULL)
	{
		freeVar_declMem(varDeclList->var_decl);
		varDeclList->var_decl = NULL;
	}

	if (varDeclList->var_decl_list != NULL)
	{
		freeVar_decl_listMem(varDeclList->var_decl_list);
		varDeclList->var_decl_list = NULL;
	}
	
	free(varDeclList);
}

void freeVar_declMem(struct var_declNode* varDecl)
{
	if(varDecl->id_list != NULL)
	{
		freeId_listMem(varDecl->id_list);
		varDecl->id_list = NULL;
	}

	if(varDecl-> type_name != NULL)
	{
		freeType_nameMem(varDecl->type_name);
		varDecl->type_name = NULL;
	}

	free(varDecl);
}

void freeBodyMem(struct bodyNode* body)
{
	if(body->stmt_list != NULL)
	{
		freeStmt_listNodeMem(body->stmt_list);
		body->stmt_list = NULL;
	}

	free(body);
}

void freeStmt_listNodeMem(struct stmt_listNode* stmtNode)
{
	if(stmtNode->stmt != NULL)
	{
		freeStmtNodeMem(stmtNode->stmt);
		stmtNode->stmt = NULL;
	}

	if(stmtNode->stmt_list != NULL)
	{
		freeStmt_listNodeMem(stmtNode->stmt_list);
		stmtNode->stmt_list = NULL;
	}
	
	free(stmtNode);
}

void freeStmtNodeMem(struct stmtNode* stmt)
{

	
	if(stmt->stmtType == WHILE)
	{
		freeWhile_stmtNodeMem(stmt->while_stmt);
		stmt->while_stmt = NULL;
	}

	else
	{
		freeAssign_stmtNodeMem(stmt->assign_stmt);
		stmt->assign_stmt = NULL;
	}
	
	free(stmt);
}

void freeWhile_stmtNodeMem(struct while_stmtNode* whileNode)
{
	if(whileNode->condition != NULL)
	{
		freeConditionNodeMem(whileNode->condition);
		whileNode->condition = NULL;
	}

	if(whileNode->body != NULL)
	{
		freeBodyMem(whileNode->body);
		whileNode->body = NULL;
	}

	free(whileNode);
}

void freeConditionNodeMem(struct conditionNode* condNode)
{
	if(condNode->left_operand != NULL)
	{
		freePrimaryNodeMem(condNode->left_operand);
		condNode->left_operand = NULL;
	}

	if(condNode->right_operand != NULL)
	{
		freePrimaryNodeMem(condNode->right_operand);
		condNode->right_operand = NULL;
	}

	free(condNode);
}

void freePrimaryNodeMem(struct primaryNode* primNode)
{
	free(primNode->id);
	free(primNode);
}


void freeAssign_stmtNodeMem(struct assign_stmtNode* assignNode)
{
	if(assignNode->expr != NULL)
	{
		freeExprNodeMem(assignNode->expr);
		assignNode->expr = NULL;
	}

	free(assignNode->id);
	free(assignNode);
}

void freeExprNodeMem(struct exprNode* exNode)
{
	if(exNode->primary != NULL)
	{
		freePrimaryNodeMem(exNode->primary);
		exNode->primary = NULL;
	}

	if(exNode->leftOperand != NULL)
	{
		freeExprNodeMem(exNode->leftOperand);
		exNode->leftOperand = NULL;
	}

	if(exNode->rightOperand != NULL)
	{
		freeExprNodeMem(exNode->rightOperand);
		exNode->rightOperand = NULL;
	}

	free(exNode);
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


int getTypeNum(int x)
{

	if(x == INT)
	{
		return 10;
	}

	else if(x == REAL)
	{
		return 11;
	}

	else if(x == STRING)
	{
		return 12;
	}

	else if(x == BOOLEAN)
	{
		return 13;
	}

	else
	{
		return 14+numOfImpVars+numOfExpVars+numOfImpTypes+numOfExpTypes;
	}
	
}


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
			int num1=-1;
			int num2;
			//check if left and right operands are of same type
			//get type num of left operand
				int tag = ter->leftOperand->primary->tag;
				if(tag == NUM)
					num1 = 10;
				else if(tag == REALNUM)
					num1 = 11;
				else 
				{
					int i;
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,ter->leftOperand->primary->id)==0)
						{
							num1 = arrayImplicitVars[i]->identifier;
						}
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,ter->leftOperand->primary->id)==0)
						{
							num1 = arrayExplicitVars[i]->identifier;
						}
					}
					//if it hasn't been declared as a variable, assign it a type and add it to implicit var array
					if(num1=-1)
					{
						//get type num
						num1 = getTypeNum(tag);
						arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(ter->leftOperand->primary->id+1));
						strcpy(arrayImplicitVars[numOfImpVars]->id, ter->leftOperand->primary->id);
						arrayImplicitVars[numOfImpVars]->identifier = num1;
						numOfImpVars++;
					}
		
				}
				//get type num of right operand
				tag = ter->rightOperand->primary->tag;
				if(tag == NUM)
					num2 = 10;
				else if(tag == REALNUM)	
					num2 = 11;
				else
				{
					int i;	
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,ter->rightOperand->primary->id)==0)
						{
							num2 = arrayImplicitVars[i]->identifier;
						}
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,ter->rightOperand->primary->id)==0)
						{
							num2 = arrayExplicitVars[i]->identifier;
						}
					}
					//if it hasn't been declared as a variable, assign it a type and add it to implicit var array
					if(num2=-1)
					{
						//get type num
						num2 = getTypeNum(tag);
						arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(ter->rightOperand->primary->id+1));
						strcpy(arrayImplicitVars[numOfImpVars]->id,ter->rightOperand->primary->id);
						arrayImplicitVars[numOfImpVars]->identifier = num2;
						numOfImpVars++;
					}
				}
				//test if types are the same, if not type mismatch
				//check if they have different built in types, type mismatch
				if(num1 < 14 && num2 < 14 && num1 != num2)
				{
					printf("%s\n", "ERROR CODE 3"); //type mismatch
					exit(0);
				}
				int leftIsImp;	
				if(num1>13)
				{		
					 leftIsImp = checkImpVar(ter->leftOperand->primary->id);
				}
				else
					 leftIsImp = 0;
				int rightIsImp;	
				if(num2 >13)
				{	
					rightIsImp = checkImpVar(ter->rightOperand->primary->id);
				
				}	
				else
					rightIsImp = 0;
				//if left is impliciit and right is built in, change id type value to expr type value
				if(leftIsImp == 1 && num2 < 14)
				{
					//loop through all types and variables and change identifier to typeNum
						int j;
						for(j=0;j<numOfImpVars;j++)
						{
							if(arrayImplicitVars[j]->identifier == num1)
								arrayImplicitVars[j]->identifier = num2;
						}
						for(j=0;j<numOfExpVars;j++)
						{
							if(arrayExplicitVars[j]->identifier == num1)
								arrayExplicitVars[j]->identifier = num2;
						}
						for(j=0;j<numOfImpTypes;j++)
						{
							if(arrayImplicitTypes[j]->identifier == num1)
								arrayImplicitTypes[j]->identifier = num2;
						}
						for(j=0;j<numOfExpTypes;j++)
						{
							if(arrayExplicitTypes[j]->identifier == num1)
								arrayExplicitTypes[j]->identifier = num2;
						}
				}
	
				//id and expr are implicit and different types, change everything to type num of expr
				if(leftIsImp == 1 && rightIsImp == 1 && num1 != num2)
				{
					//loop through all types and variables and change identifier to typeNum
					int j;
					for(j=0;j<numOfImpVars;j++)
					{
						if(arrayImplicitVars[j]->identifier == num1)
							arrayImplicitVars[j]->identifier = num2;
					}
					for(j=0;j<numOfExpVars;j++)
					{
						if(arrayExplicitVars[j]->identifier == num1)
							arrayExplicitVars[j]->identifier = num2;
					}
					for(j=0;j<numOfImpTypes;j++)
					{
						if(arrayImplicitTypes[j]->identifier == num1)
							arrayImplicitTypes[j]->identifier = num2;
					}
					for(j=0;j<numOfExpTypes;j++)
					{
						if(arrayExplicitTypes[j]->identifier == num1)
							arrayExplicitTypes[j]->identifier = num2;
					}
				}
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
	if ((ttype == ID) | (ttype == LPAREN) | (ttype == NUM) | (ttype == REALNUM))
	{
		ungetToken();
		t = term();
		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS))
		 {
			exp = make_exprNode();
			exp->operator = ttype;
			exp->leftOperand = t;
			exp->rightOperand = expr();
			exp->tag = EXPR;
			int num1=-1;
			int num2;
			//check if left and right operands are of same type
			//get type num of left operand
				int tag = exp->leftOperand->primary->tag;
				if(tag == NUM)
					num1 = 10;
				else if(tag == REALNUM)
					num1 = 11;
				else 
				{
					int i;
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,exp->leftOperand->primary->id)==0)
						{
							num1 = arrayImplicitVars[i]->identifier;
						}
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,exp->leftOperand->primary->id)==0)
						{
							num1 = arrayExplicitVars[i]->identifier;
						}
					}
					//if it hasn't been declared as a variable, assign it a type and add it to implicit var array
					if(num1=-1)
					{
						//get type num
						num1 = getTypeNum(tag);
						arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(exp->leftOperand->primary->id+1));
						strcpy(arrayImplicitVars[numOfImpVars]->id, exp->leftOperand->primary->id);
						arrayImplicitVars[numOfImpVars]->identifier = num1;
						numOfImpVars++;
					}
		
				}
				//get type num of right operand
				tag = exp->rightOperand->primary->tag;
				if(tag == NUM)
					num2 = 10;
				else if(tag == REALNUM)	
					num2 = 11;
				else
				{
					int i;	
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,exp->rightOperand->primary->id)==0)
						{
							num2 = arrayImplicitVars[i]->identifier;
						}
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,exp->rightOperand->primary->id)==0)
						{
							num2 = arrayExplicitVars[i]->identifier;
						}
					}
					//if it hasn't been declared as a variable, assign it a type and add it to implicit var array
					if(num2=-1)
					{
						//get type num
						num2 = getTypeNum(tag);
						arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(exp->rightOperand->primary->id+1));
						strcpy(arrayImplicitVars[numOfImpVars]->id,exp->rightOperand->primary->id);
						arrayImplicitVars[numOfImpVars]->identifier = num2;
						numOfImpVars++;
					}
				}
				//test if types are the same, if not type mismatch
				//check if they have different built in types, type mismatch
				if(num1 < 14 && num2 < 14 && num1 != num2)
				{
					printf("%s\n", "ERROR CODE 3"); //type mismatch
					exit(0);
				}
				int leftIsImp;	
				if(num1>13)
				{		
					 leftIsImp = checkImpVar(exp->leftOperand->primary->id);
				}
				else
					 leftIsImp = 0;
				int rightIsImp;	
				if(num2 >13)
				{	
					rightIsImp = checkImpVar(exp->rightOperand->primary->id);
				
				}	
				else
					rightIsImp = 0;
				//if left is impliciit and right is built in, change id type value to expr type value
				if(leftIsImp == 1 && num2 < 14)
				{
					//loop through all types and variables and change identifier to typeNum
						int j;
						for(j=0;j<numOfImpVars;j++)
						{
							if(arrayImplicitVars[j]->identifier == num1)
								arrayImplicitVars[j]->identifier = num2;
						}
						for(j=0;j<numOfExpVars;j++)
						{
							if(arrayExplicitVars[j]->identifier == num1)
								arrayExplicitVars[j]->identifier = num2;
						}
						for(j=0;j<numOfImpTypes;j++)
						{
							if(arrayImplicitTypes[j]->identifier == num1)
								arrayImplicitTypes[j]->identifier = num2;
						}
						for(j=0;j<numOfExpTypes;j++)
						{
							if(arrayExplicitTypes[j]->identifier == num1)
								arrayExplicitTypes[j]->identifier = num2;
						}
				}
	
				//id and expr are implicit and different types, change everything to type num of expr
				if(leftIsImp == 1 && rightIsImp == 1 && num1 != num2)
				{
					//loop through all types and variables and change identifier to typeNum
					int j;
					for(j=0;j<numOfImpVars;j++)
					{
						if(arrayImplicitVars[j]->identifier == num1)
							arrayImplicitVars[j]->identifier = num2;
					}
					for(j=0;j<numOfExpVars;j++)
					{
						if(arrayExplicitVars[j]->identifier == num1)
							arrayExplicitVars[j]->identifier = num2;
					}
					for(j=0;j<numOfImpTypes;j++)
					{
						if(arrayImplicitTypes[j]->identifier == num1)
							arrayImplicitTypes[j]->identifier = num2;
					}
					for(j=0;j<numOfExpTypes;j++)
					{
						if(arrayExplicitTypes[j]->identifier == num1)
							arrayExplicitTypes[j]->identifier = num2;
					}
				}
					
			return exp;	
		}
		else if ((ttype == SEMICOLON) | (ttype == MULT) | (ttype == DIV) | (ttype == RPAREN))
		{
			ungetToken();
			return t;
		}
				
		else 
		{
				syntax_error("expr. PLUS, MINUS, or SEMICOLON expected", line_no);
				exit(0);
		}
	}	 
	else {
		syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

int checkExpType(char* token)
{
	int i;
	int flag=0;
	for(i=0;i<numOfExpTypes;i++)
	{
		if(strcmp(arrayExplicitTypes[i]->id,token)==0)
		{
			flag = 1;
		}
	}
	return flag;
}

int checkImpType(char* token)
{
	int i;
	int flag=0;
	for(i=0;i<numOfImpTypes;i++)
	{
		if(strcmp(arrayImplicitTypes[i]->id,token)==0)
		{
			flag = 1;
		}
	}
	return flag;
}


int checkExpVar(char* token)
{
	int i;
	int flag=0;
	for(i=0;i<numOfExpVars;i++)
	{
		if(strcmp(arrayExplicitVars[i]->id,token)==0)
		{
			flag = 1;
		}
	}
	return flag;
}

int checkImpVar(char* token)
{
	int i;
	int flag=0;
	for(i=0;i<numOfImpVars;i++)
	{
		if(strcmp(arrayImplicitVars[i]->id,token)==0)
		{
			flag = 1;
		}
	}
	return flag;
}
struct assign_stmtNode* assign_stmt()
{
	struct assign_stmtNode* assignStmt;

	ttype = getToken();
	if (ttype == ID) {
		assignStmt = make_assign_stmtNode();
		assignStmt->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(assignStmt->id, token);

		//check if token was declared as a type, if so error, type name redeclared as variable
		int check = checkExpType(token);
		int check2 = checkImpType(token);
		if(check==1 || check2==1)
		{
			printf("%s\n","ERROR CODE 1");
			exit(0);
		}

		//check if token is in explicit var array or implicit var array
		//if it isn't in either, add it to implicit var array and assign it a type value
		int checkIfMemAll = 0;
		//loop through explicit array
		int typeNumID=-1;
		int typeNum=-1;
		int i;
		for(i=0;i<numOfExpVars;i++)
		{
			if(strcmp(arrayExplicitVars[i]->id,token)==0)
			{
				typeNumID = arrayExplicitVars[i]->identifier;
			}

		}

		//loop through implicit var array
		for(i=0;i<numOfImpVars;i++)
		{
			if(strcmp(arrayImplicitVars[i]->id,token)==0)
			{
				typeNumID = arrayImplicitVars[i]->identifier;
			}

		}
		//if the var wasn't in either, add it to implicit var array
		if(typeNumID == -1)
		{
			//allocate memory
			arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
			arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(strlen(token+1)));
			checkIfMemAll = 1;
			strcpy(arrayImplicitVars[numOfImpVars]->id,token);
	                typeNumID=getTypeNum(ID);
		arrayImplicitVars[numOfImpVars]->identifier = typeNumID;
		numOfImpVars++;
	         }
		
		ttype = getToken();
		if (ttype == EQUAL) {
			assignStmt->expr = expr();
				
				//check if each side of assignment statment has same type
				//if they each have two different built in types, error code 3, type mismatch
				//if one is implicit and one is built in, change implicit to be same type as built in
				//if both are implicit, change one to match the other
				//find value of expression
				if(assignStmt->expr->tag == PRIMARY)
				{
					if(assignStmt->expr->primary->tag == NUM)
					{
						typeNum = 10;
						if(checkIfMemAll ==1)
							arrayImplicitVars[numOfImpVars-1]->identifier = 10;
					}
					else if(assignStmt->expr->primary->tag == REALNUM)
					{	typeNum = 11;
						if(checkIfMemAll ==1)
							arrayImplicitVars[numOfImpVars-1]->identifier = 11;
					}
					else
					{
					//if not an explicit or implicit var add to array
					int i;
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,token)==0)
							typeNum = arrayImplicitVars[i]->identifier;
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,token)==0)
							typeNum=arrayExplicitVars[i]->identifier;
					}
					if(typeNum == -1)
					{

						arrayImplicitVars[numOfImpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayImplicitVars[numOfImpVars]->id = (char*) malloc(sizeof(token+1));
						strcpy(arrayImplicitVars[numOfImpVars]->id,token);
						typeNum=getTypeNum(ID);
						arrayImplicitVars[numOfImpVars]->identifier = typeNum;
						numOfImpVars++;
		
				}
				}
	
				
				
				
				}

				else //else find type num of left operand 
				{	
					int tagL = assignStmt->expr->leftOperand->primary->tag;
					if(tagL == INT)
						typeNum = 10;
					else if(tagL==REAL)
						typeNum = 11;
					else
					{
					int i;
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(arrayImplicitVars[i]->id,assignStmt->expr->leftOperand->primary->id)==0)
							typeNum = arrayImplicitVars[i]->identifier;
					}
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(arrayExplicitVars[i]->id,assignStmt->expr->leftOperand->primary->id)==0)
							typeNum=arrayImplicitVars[i]->identifier;
					}
					
					}
	
				}
				//check if they have different built in types, type mismatch
				if(typeNumID < 14 && typeNum < 14 && typeNumID != typeNum)
				{
					printf("%s\n", "ERROR CODE 3"); //type mismatch
					exit(0);
				}
			//if id is impliciit and expr is built in, change id type value to expr type value
				if(typeNumID >13 && typeNum < 14)
				{
					//loop through all types and variables and change identifier to typeNum
					int j;
					for(j=0;j<numOfImpVars;j++)
					{
						if(arrayImplicitVars[j]->identifier == typeNumID)
							arrayImplicitVars[j]->identifier = typeNum;
					}
					for(j=0;j<numOfExpVars;j++)
					{
						if(arrayExplicitVars[j]->identifier == typeNumID)
							arrayExplicitVars[j]->identifier = typeNum;
					}
					for(j=0;j<numOfImpTypes;j++)
					{
						if(arrayImplicitTypes[j]->identifier == typeNumID)
							arrayImplicitTypes[j]->identifier = typeNum;
					}
					for(j=0;j<numOfExpTypes;j++)
					{
						if(arrayExplicitTypes[j]->identifier == typeNumID)
							arrayExplicitTypes[j]->identifier = typeNum;
					}
				}

				//id and expr are implicit and different types, change everything to type num of expr
				if(typeNumID >13 && typeNum>13 && typeNumID != typeNum)
				{
printf("GOT IN\n");
printf("BEFORE TYPENUMID:%d\n", typeNumID);
printf("BEFORE TYPENUM:%d\n", typeNum);
					//loop through all types and variables and change identifier to typeNum
					int j;
					for(j=0;j<numOfImpVars;j++)
					{
						if(arrayImplicitVars[j]->identifier == typeNumID)
							arrayImplicitVars[j]->identifier = typeNum;
					}
					for(j=0;j<numOfExpVars;j++)
					{
						if(arrayExplicitVars[j]->identifier == typeNumID)
							arrayExplicitVars[j]->identifier = typeNum;
					}
					for(j=0;j<numOfImpTypes;j++)
					{
						if(arrayImplicitTypes[j]->identifier == typeNumID)
							arrayImplicitTypes[j]->identifier = typeNum;
					}
					for(j=0;j<numOfExpTypes;j++)
					{
						if(arrayExplicitTypes[j]->identifier == typeNumID)
							arrayExplicitTypes[j]->identifier = typeNum;
					}
				

				}
					

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


	if ((ttype == ID) || (ttype == WHILE)) {
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
	if (ttype == ID) 
	{
		ungetToken();
		varDecl->id_list = id_list();
		ttype = getToken();
	
		if (ttype == COLON) 
		{	//check if type has already been declared and given a type value
			varDecl->type_name = type_name();
			//check if type name is int
		int num1;
		if(varDecl->type_name->type == ID)
		{
			int k;
			num1=-1;
			//check if its an explicit type
			for(k=0; k< numOfExpTypes; k++)
			{
				if(strcmp(arrayExplicitTypes[k]->id,varDecl->type_name->id)==0)
					num1 = arrayExplicitTypes[k]->identifier;
			}
			for(k=0; k< numOfImpTypes; k++)
			{
				if(strcmp(arrayImplicitTypes[k]->id,varDecl->type_name->id)==0)
					num1 = arrayImplicitTypes[k]->identifier;
			}
			//add to implicit array
			if(num1 == -1)
			{
			 num1 = getTypeNum(ID);
				//check if it has already been declared as a typename in variable declaration
				int i;
				int flag = 0;
			
				for(i=0;i<numOfExpVars;i++)
				{
					if(strcmp(arrayExplicitVars[i]->id,varDecl->type_name->id)==0)
						flag = 1;
				}
				if(flag == 0)
				{
					if(checkExpType(varDecl->type_name->id) == 0)
					{
					arrayImplicitTypes[numOfImpTypes] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
					arrayImplicitTypes[numOfImpTypes]->id = (char*) malloc(strlen(varDecl->type_name->id+1));
					arrayImplicitTypes[numOfImpTypes]->identifier = num1;
					strcpy(arrayImplicitTypes[numOfImpTypes]->id, varDecl->type_name->id);
					numOfImpTypes++;
					}
				}
				
				else //(C6)
				{
					printf("%s\n", "ERROR CODE 4"); // variable redeclared as type name
					exit(0);

				}
			}
		}
		else
			num1=getTypeNum(varDecl->type_name->type);
				struct id_listNode *node = varDecl->id_list;
				while(node != NULL)
				{
					//(C3) check to see if var has been declared in type section
					int i;
					int flag=0;
			
					//check explicit types
					for(i=0;i<numOfExpTypes;i++)
					{
						if(strcmp(node->id,arrayExplicitTypes[i]->id)==0)
							flag = 1;
					}
					
					for(i=0;i<numOfImpTypes;i++)
					{
						if(strcmp(node->id,arrayImplicitTypes[i]->id)==0)
							flag = 1;
					}

					if(flag == 1)
					{
						printf("%s\n","ERROR CODE 1"); //type redeclared as variable
						exit(0);
					}
		
					flag = 0;
					//(C4)check if var has already been declared in id_list
					for(i=0;i<numOfExpVars;i++)
					{
						if(strcmp(node->id,arrayExplicitVars[i]->id)==0)
							flag = 1;
					}
	
					if(flag == 1)
					{
						printf("%s\n","ERROR CODE 2"); //variable declared more than once
						exit(0);		
					}

					flag = 0;
					//(C5) check if var was already declared in var type decl
					for(i=0;i<numOfImpVars;i++)
					{
						if(strcmp(node->id,arrayImplicitVars[i]->id)==0)
						flag = 1;
					}
					if(flag == 1)
					{
						printf("%s\n","ERROR CODE 1"); //type redeclared as variable
						exit(0);
					
					}
					else
					{
						arrayExplicitVars[numOfExpVars] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
						arrayExplicitVars[numOfExpVars]->id = (char*) malloc(strlen(node->id+1));
						strcpy(arrayExplicitVars[numOfExpVars]->id, node->id);
						arrayExplicitVars[numOfExpVars]->identifier = num1;
						numOfExpVars++;
						numOfTypes++;
						node=node->id_list;
					}
				}
			
			ttype = getToken();
			if (ttype == SEMICOLON)
		 	{
				return varDecl;
			}
			else 
			{
				syntax_error("var_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		}
		else 
		{
			syntax_error("var_decl. COLON expected", line_no);
			exit(0);
		}
	}
	 else
	{
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

struct type_declNode* type_decl()
{
	struct type_declNode* typeDecl;
	typeDecl = make_type_declNode();
	ttype = getToken();
	if (ttype == ID) 
	{
		ungetToken();


		typeDecl->id_list = id_list();
		ttype = getToken();
		if (ttype == COLON) 
		{
			typeDecl->type_name = type_name();
	
			int num = getTypeNum(typeDecl->type_name->type);

			if(num>13) //is an ID, so implicit type, add to implicit array
			{
				arrayImplicitTypes[numOfImpTypes] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
				arrayImplicitTypes[numOfImpTypes]->id = (char*) malloc(strlen(typeDecl->type_name->id+1));
				strcpy(arrayImplicitTypes[numOfImpTypes]->id, typeDecl->type_name->id);
				arrayImplicitTypes[numOfImpTypes]->identifier = num;
				numOfImpTypes++;
				numOfTypes++;
			}

			struct id_listNode *node = typeDecl->id_list;
			while(node != NULL)
			{
				//C2 check if explicit type is already in an id list
				int i;
				int flag = 0;
				for(i=0;i<numOfExpTypes;i++)
				{
					if(strcmp(arrayExplicitTypes[i]->id,node->id)==0)
						flag = 1;
				}

				if(flag == 1) //ERROR CODE 0 (C2)
				{
					printf("%s\n","ERROR CODE 0"); //type name declared more than once
					exit(0);
				}
				
				flag = 0;
				for(i=0;i<numOfImpTypes;i++)
				{
					if(strcmp(arrayImplicitTypes[i]->id,node->id)==0)
						flag = 1;
				}
				
				if(flag == 1)
				{
					printf("%s\n","ERROR CODE 0"); //type name declared more than once
					exit(0);
				}
				
				else
				{
					arrayExplicitTypes[numOfExpTypes] = (struct typeOrVar*) malloc(sizeof(struct typeOrVar));
					arrayExplicitTypes[numOfExpTypes]->id = (char*) malloc(strlen(node->id+1));
					strcpy(arrayExplicitTypes[numOfExpTypes]->id, node->id);
					arrayExplicitTypes[numOfExpTypes]->identifier = num;
					numOfExpTypes++;
					numOfTypes++;
					node=node->id_list;
				}
			}
			
			ttype = getToken();
			if (ttype == SEMICOLON)
			{
				return typeDecl;
			}
			else
			{
				syntax_error("type_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		} 
		else
		{
			syntax_error("type_decl. COLON expected", line_no);
			exit(0);
		}
	}
	else
	{
		syntax_error("type_decl. ID expected", line_no);
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

struct declNode* decl()
{
	struct declNode* dec;
	dec = make_declNode();

	ttype = getToken();
	if (ttype == TYPE) 
	{
		ungetToken();
		dec->type_decl_section = type_decl_section();
		ttype = getToken();
		if (ttype == VAR)
		{ // type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
		}
	
		else 
		{
			ungetToken();
			dec->var_decl_section = NULL;
		}
		return dec;
	}
	
	else 
	{
		dec->type_decl_section = NULL;
		if (ttype == VAR)
		{ // type_decl_list is epsilon
			// or type_decl already parsed and the
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
			return dec;
		}
		else 
		{
			if (ttype == LBRACE) 
			{
				ungetToken();
				dec->var_decl_section = NULL;
				return dec;
			} 
			else 
			{
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
void loopPrintArrays(int x)
{
int m;
			//print explicit types that have same type
			for(m=0; m<numOfExpTypes;m++)
			{
		//		printf("%s%d\n","flag:",flag);
			if(arrayExplicitTypes[m]->identifier == x  && arrayExplicitTypes[m]->check == 0)
				{
					printf("%s%s", " ", arrayExplicitTypes[m]->id); 
					arrayExplicitTypes[m]->check = 1;
				}
			}

			//print implicit types that have same type
			for(m=0; m<numOfImpTypes;m++)
			{
				if(arrayImplicitTypes[m]->identifier == x && arrayImplicitTypes[m]->check == 0)
				{
					printf("%s%s", " ", arrayImplicitTypes[m]->id);
					arrayImplicitTypes[m]->check = 1;
				}
			}

			
			//print explicit variables that have same type
			for(m=0;m<numOfExpVars;m++)
			{
				if(arrayExplicitVars[m]->identifier == x && arrayExplicitVars[m]->check == 0)
				{
					printf("%s%s", " ", arrayExplicitVars[m]->id);
					arrayExplicitVars[m]->check = 1;
				}
				
			}
			//print implicit variables that have same type
			for(m=0;m<numOfImpVars;m++)
			{
				if(arrayImplicitVars[m]->identifier == x && arrayImplicitVars[m]->check == 0)
				{
					printf("%s%s", " ", arrayImplicitVars[m]->id);
					arrayImplicitVars[m]->check = 1;
				}
				
			}
}
void printTypes()
{
	int i;
//print all types of type int if there are any
//check if type int is in any array
int isInt;
isInt=0;
if(checkArray(arrayExplicitTypes,10,numOfExpTypes)==1)
	isInt = 1;
if(checkArray(arrayImplicitTypes,10,numOfImpTypes) == 1)
	isInt = 1;

if(checkArray(arrayExplicitVars,10,numOfExpVars) == 1)
	isInt = 1;

if(checkArray(arrayImplicitVars,10,numOfImpVars) == 1)
	isInt = 1;
if(isInt ==1)
{
	printf("INT:");
	loopPrintArrays(10);
	printf("\n");
}	

//print all REAL types
isInt=0;
if(checkArray(arrayExplicitTypes,11,numOfExpTypes)==1)
	isInt = 1;
if(checkArray(arrayImplicitTypes,11,numOfImpTypes) == 1)
	isInt = 1;

if(checkArray(arrayExplicitVars,11,numOfExpVars) == 1)
	isInt = 1;

if(checkArray(arrayImplicitVars,11,numOfImpVars) == 1)
	isInt = 1;
if(isInt ==1)
{
	printf("REAL:");
	loopPrintArrays(11);
	printf("\n");
}	
isInt=0;
if(checkArray(arrayExplicitTypes,12,numOfExpTypes)==1)
{
	isInt = 1;
}
if(checkArray(arrayImplicitTypes,12,numOfImpTypes) == 1)
	isInt = 1;

if(checkArray(arrayExplicitVars,12,numOfExpVars) == 1)
	isInt = 1;

if(checkArray(arrayImplicitVars,12,numOfImpVars) == 1)
	isInt = 1;
if(isInt ==1)
{
	printf("STRING:");
	loopPrintArrays(12);
	printf("\n");
}
isInt=0;
if(checkArray(arrayExplicitTypes,13,numOfExpTypes)==1)
	isInt = 1;
if(checkArray(arrayImplicitTypes,13,numOfImpTypes) == 1)
	isInt = 1;
if(checkArray(arrayExplicitVars,13,numOfExpVars) == 1)
	isInt = 1;

if(checkArray(arrayImplicitVars,13,numOfImpVars) == 1)
	isInt = 1;
if(isInt ==1)
{
	printf("BOOLEAN:");
	loopPrintArrays(13);
	printf("\n");
}	
	
	//print all types
	for(i=0; i<numOfExpTypes;i++)
	{
		int n;
		int j;
		//if type hasn't been printed
		if(arrayExplicitTypes[i]->check==0)
		{
			printf("%s%s", arrayExplicitTypes[i]->id,":");
		
			int n;
			//print explicit types that have same type
			for(n=i+1; n<numOfExpTypes;n++)
			{
				if(arrayExplicitTypes[n]->identifier == arrayExplicitTypes[i]->identifier && arrayExplicitTypes[n]->check == 0)
				{
					printf("%s%s", " ", arrayExplicitTypes[n]->id); 
					arrayExplicitTypes[n]->check = 1;
				}
			}
			int m;

			//print implicit types that have same type
			for(m=0; m<numOfImpTypes;m++)
			{
				if(arrayExplicitTypes[i]->identifier == arrayImplicitTypes[m]->identifier && arrayImplicitTypes[m]->check == 0)
				{
					printf("%s%s", " ", arrayImplicitTypes[m]->id);
					arrayImplicitTypes[m]->check = 1;
				}
			}

			
			//print explicit variables that have same type
			for(m=0;m<numOfExpVars;m++)
			{
				if(arrayExplicitTypes[i]->identifier == arrayExplicitVars[m]->identifier && arrayExplicitVars[m]->check==0)
				{
					printf("%s%s", " ", arrayExplicitVars[m]->id);
					arrayExplicitVars[m]->check = 1;
				}
				
			}
			//print implicit variables that have same type
			for(m=0;m<numOfImpVars;m++)
			{
				if(arrayExplicitTypes[i]->identifier == arrayImplicitVars[m]->identifier && arrayImplicitVars[m]->check == 0)
				{
					printf("%s%s", " ", arrayImplicitVars[m]->id);
					arrayImplicitVars[m]->check = 1;
				}
				
			}
		printf("\n");
		}
	}

	//print implicit types
	for(i=0;i<numOfImpTypes;i++)
	{	int k;

		if(arrayImplicitTypes[i]->check == 0)
		{
			printf("%s%s", arrayImplicitTypes[i]->id,":");
	
			int m;	
			//print explicit variables that have same type
			for(m=0;m<numOfExpVars;m++)
			{
				if(arrayImplicitTypes[i]->identifier == arrayExplicitVars[m]->identifier && arrayExplicitVars[m]->check == 0)
				{
					printf("%s%s", " ", arrayExplicitVars[m]->id);
					arrayExplicitVars[m]->check = 1;
				}
				
			}
			//print implicit variables that have same type
			for(m=0;m<numOfImpVars;m++)
			{
				if(arrayImplicitTypes[i]->identifier == arrayImplicitVars[m]->identifier && arrayImplicitVars[m]->check == 0)
				{
					printf("%s%s", " ", arrayImplicitVars[m]->id);
					arrayImplicitVars[m]->check = 1;
				}
				
			}

}		printf("\n");
	}
//	print implicit variables
	for(i=0;i<numOfImpVars;i++)
	{
		int first = 0;
		int j;
		if(arrayImplicitVars[i]->check == 0)
		{
			if(first == 0)
			{
				printf("%s%s", arrayImplicitVars[i]->id, ":");
				first = 1;
				arrayImplicitVars[i]->check = 1;
			}
			else
			{
				printf("%s%s"," ", arrayImplicitVars[i]->id);
				arrayImplicitVars[i]->check = 1;
			}
			int k;
			for(k=0;k<numOfImpVars;k++)
			{
				if(arrayImplicitVars[k]->check == 0 && arrayImplicitVars[k]->identifier == arrayImplicitVars[i]->identifier)
				{
					printf("%s%s", " ", arrayImplicitVars[k]->id);
					arrayImplicitVars[k]->check = 1;
				}
			}
		}
		
		printf("\n");
		
	}

}

int checkArray(struct typeOrVar* array[], int x,int length)
{
	int flag=0;
	int i;
	for(i=0;i<length;i++)
	{
	if(array[i]->identifier==x)
			flag = 1;
	}
	
	return flag;
}
/*
int checkArrayInt(int array[], char* id,int length)
{
	int flag=0;
	int i;
	for(i=0;i<length;i++)
	{
	if(strcmp(array[i],id)==0)
			flag = 1;
	}
	
	return flag;
}*/

// COMMON mistakes:
//    *     = instead of ==
//    *     not allocating space before strcpy

int main()
{
	struct programNode* parseTree;
	parseTree = program();
//	print_parse_tree(parseTree);
	freeProgramMem(parseTree);
	int i;

	for(i=0;i<numOfExpTypes;i++)
	{
		printf("%s%d%s%s%s%d\n", "ExplicitTypes[",i,"]:",arrayExplicitTypes[i]->id, "num: ",arrayExplicitTypes[i]->identifier);
	}
	for(i=0;i<numOfImpTypes;i++)
	{
		printf("%s%d%s%s%s%d\n", "ImplicitTypes[",i,"]:",arrayImplicitTypes[i]->id, "num: ",arrayImplicitTypes[i]->identifier);
	}
	for(i=0;i<numOfExpVars;i++)
	{
		printf("%s%d%s%s%s%d\n", "ExplicitVars[",i,"]:",arrayExplicitVars[i]->id, "num: ",arrayExplicitVars[i]->identifier);
	}
	for(i=0;i<numOfImpVars;i++)
	{
		printf("%s%d%s%s%s%d\n", "ImplicitVars[",i,"]:",arrayImplicitVars[i]->id, "num: ",arrayImplicitVars[i]->identifier);
	}
	
	printTypes();
	freeArrays();
	printf("\nSUCCESSFULLY PARSED INPUT!\n");
	return 0;
}


