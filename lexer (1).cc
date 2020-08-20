#include <iostream>
	#include <istream>
	#include <vector>
	#include <string>
	#include <cctype>
	#include <stdlib.h>
	#include <string.h>
	#include <algorithm>
	
	#include "lexer.h"
	#include "inputbuf.h"
	
	using namespace std;
	
	string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOOL", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here (as string)
	};
	
	#define KEYWORDS_COUNT 11
	string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };
	
	LexicalAnalyzer lexer;
	Token token;
	TokenType tempTokenType;
	
	int Count = 4;
	
	struct sTableEntry
	{ 
	// every element of the symbol table has name, line no , type and print
	string name;
	int lineNo;
	int print;
	int type;
	
	};
	
	struct sTable
	{
	// symbol table is represented through a linked list 
	sTable *prev;
	sTableEntry* item;
	sTable *next;
	};
	
	sTable* symbolTable;
	char* lResolve;
	char* rResolve;
	int line = 0;
	
	
	
	void addtoList(std::string name, int line, int type)
	{
	if(symbolTable == NULL)
	{
	sTable* newEntry = new sTable();
	sTableEntry* newItem = new sTableEntry();
	
	newItem->name = name;
	newItem->lineNo = token.line_no;
	newItem->type = type;
	newItem->print = 0;
	
	newEntry->item = newItem;
	newEntry->next = NULL;
	newEntry->prev = NULL;
	
	symbolTable = newEntry;
	
	}
	else
	{
	sTable* temp = symbolTable;
	while(temp->next != NULL)
	{
	temp = temp->next;
	}
	
	sTable* newEntry = new sTable();
	sTableEntry* newItem = new sTableEntry();
	
	newItem->name = name;
	newItem->lineNo = token.line_no;
	newItem->type = type;
	newItem->print = 0;
	
	newEntry->item = newItem;
	newEntry->next = NULL;
	newEntry->prev = temp;
	temp->next = newEntry;
	}
	}
	
	
	
	int SearchList(std::string n)
	{
	sTable* temp = symbolTable;
	bool found = false;
	if (symbolTable == NULL)
	{
	//list is empty then element is added to the list
	addtoList(n, token.line_no, Count);
	Count++;
	return (4);
	}
	else
	{
	while(temp->next != NULL)
	{
	if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
	{
	found = true;
	// item is found in the list and its type is returned 
	return(temp->item->type);
	}
	else
	{
	// searching through the list till all the elements are compared
	temp = temp->next;
	}
	}
	if(strcmp(temp->item->name.c_str(), n.c_str()) == 0)
	{ 
	//if the last element in the list matches then its type is returned 
	found = true;
	return(temp->item->type);
	}
	if(!found)
	{ 
	// if the last element also does not match then the element is added to the list
	addtoList(n, token.line_no, Count);
	Count++;
	int t = Count-1;
	return(t);
	}
	else
	{
	
	}
	}
	return(0);
	}
	
	
	void Token::Print()
	{
	cout << "{" << this->lexeme << " , "
	<< reserved[(int) this->token_type] << " , "
	<< this->line_no << "}\n";
	}
	
	LexicalAnalyzer::LexicalAnalyzer()
	{
	this->line_no = 1;
	tmp.lexeme = "";
	tmp.line_no = 1;
	line = 1;
	tmp.token_type = ERROR;
	}
	
	bool LexicalAnalyzer::SkipSpace()
	{
	char c;
	bool space_encountered = false;
	
	input.GetChar(c);
	line_no += (c == '\n');
	line = line_no;
	
	while (!input.EndOfInput() && isspace(c))
	{
	space_encountered = true;
	input.GetChar(c);
	line_no += (c == '\n');
	line = line_no;
	}
	
	if (!input.EndOfInput())
	{
	input.UngetChar(c);
	}
	return space_encountered;
	}
	
	bool LexicalAnalyzer::SkipComment()
	{
	bool comment_encountered = false;
	char c;
	if(input.EndOfInput() )
	{
	input.UngetChar(c);
	return comment_encountered;
	}
	
	input.GetChar(c);
	
	if(c == '/')
	{
	input.GetChar(c);
	if(c == '/')
	{
	comment_encountered = true;
	while(c != '\n')
	{
	comment_encountered= true;
	input.GetChar(c);
	}
	line_no++;
	line = line_no;
	SkipComment();
	}
	else
	{
	comment_encountered = false;
	cout << "Syntax Error\n";
	exit(0);
	}
	}
	else
	{
	input.UngetChar(c);
	return comment_encountered;
	}
	return(0);
	}
	
	bool LexicalAnalyzer::IsKeyword(string s)
	{
	for (int i = 0; i < KEYWORDS_COUNT; i++)
	{
	if (s == keyword[i])
	{
	return true;
	}
	}
	return false;
	}
	
	TokenType LexicalAnalyzer::FindKeywordIndex(string s)
	{
	for (int i = 0; i < KEYWORDS_COUNT; i++)
	{
	if (s == keyword[i])
	{
	return (TokenType) (i + 1);
	}
	}
	return ERROR;
	}
	
	Token LexicalAnalyzer::ScanNumber()
	{
	char c;
	bool realNUM = false;
	input.GetChar(c);
	if (isdigit(c))
	{
	if (c == '0')
	{
	tmp.lexeme = "0";
	input.GetChar(c);
	if(c == '.')
	{
	input.GetChar(c);
	
	if(!isdigit(c))
	{
	input.UngetChar(c);
	}
	else
	{
	while (!input.EndOfInput() && isdigit(c))
	{
	tmp.lexeme += c;
	input.GetChar(c);
	realNUM = true;
	}
	input.UngetChar(c);
	}
	}
	else
	{
	input.UngetChar(c);
	}
	}
	else
	{
	tmp.lexeme = "";
	while (!input.EndOfInput() && isdigit(c))
	{
	tmp.lexeme += c;
	input.GetChar(c);
	}
	if(c == '.')
	{
	input.GetChar(c);
	
	if(!isdigit(c))
	{
	input.UngetChar(c);
	}
	else
	{
	while (!input.EndOfInput() && isdigit(c))
	{
	tmp.lexeme += c;
	input.GetChar(c);
	realNUM = true;
	}
	}
	}
	if (!input.EndOfInput())
	{
	input.UngetChar(c);
	}
	}
	// TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
	if(realNUM)
	{
	tmp.token_type = REALNUM;
	}
	else
	{
	tmp.token_type = NUM;
	}
	tmp.line_no = line_no;
	return tmp;
	}
	else
	{
	if (!input.EndOfInput())
	{
	input.UngetChar(c);
	}
	tmp.lexeme = "";
	tmp.token_type = ERROR;
	tmp.line_no = line_no;
	return tmp;
	}
	}
	
	Token LexicalAnalyzer::ScanIdOrKeyword()
	{
	char c;
	input.GetChar(c);
	
	if (isalpha(c))
	{
	tmp.lexeme = "";
	while (!input.EndOfInput() && isalnum(c))
	{
	tmp.lexeme += c;
	input.GetChar(c);
	}
	if (!input.EndOfInput())
	{
	input.UngetChar(c);
	}
	tmp.line_no = line_no;
	
	if (IsKeyword(tmp.lexeme))
	{
	tmp.token_type = FindKeywordIndex(tmp.lexeme);
	}
	else
	{
	tmp.token_type = ID;
	}
	}
	else
	{
	if (!input.EndOfInput())
	{
	input.UngetChar(c);
	}
	tmp.lexeme = "";
	tmp.token_type = ERROR;
	}
	return tmp;
	}
	
	// you should unget tokens in the reverse order in which they
	// are obtained. If you execute
	//
	// t1 = lexer.GetToken();
	// t2 = lexer.GetToken();
	// t3 = lexer.GetToken();
	//
	// in this order, you should execute
	//
	// lexer.UngetToken(t3);
	// lexer.UngetToken(t2);
	// lexer.UngetToken(t1);
	//
	// if you want to unget all three tokens. Note that it does not
	// make sense to unget t1 without first ungetting t2 and t3
	//
	
	TokenType LexicalAnalyzer::UngetToken(Token tok)
	{
	tokens.push_back(tok);;
	return tok.token_type;
	}
	
	Token LexicalAnalyzer::GetToken()
	{
	char c;
	
	// if there are tokens that were previously
	// stored due to UngetToken(), pop a token and
	// return it without reading from input
	if (!tokens.empty())
	{
	tmp = tokens.back();
	tokens.pop_back();
	return tmp;
	}
	
	SkipSpace();
	SkipComment();
	SkipSpace();
	tmp.lexeme = "";
	tmp.line_no = line_no;
	input.GetChar(c);
	
	switch (c)
	{
	case '!':
	tmp.token_type = NOT;
	return tmp;
	case '+':
	tmp.token_type = PLUS;
	return tmp;
	case '/':
	tmp.token_type = DIV;
	return tmp;
	case '*':
	tmp.token_type = MULT;
	return tmp;
	case '-':
	tmp.token_type = MINUS;
	return tmp;
	
	case '>':
	input.GetChar(c);
	if(c == '=')
	{
	tmp.token_type = GTEQ;
	}
	else
	{
	input.UngetChar(c);
	tmp.token_type = GREATER;
	}
	return tmp;
	case '<':
	input.GetChar(c);
	if(c == '=')
	{
	tmp.token_type = LTEQ;
	}
	else if (c == '>')
	{
	tmp.token_type = NOTEQUAL;
	}
	else
	{
	input.UngetChar(c);
	tmp.token_type = LESS;
	}
	return tmp;
	case '(':
	tmp.token_type = LPAREN;
	return tmp;
	case ')':
	tmp.token_type = RPAREN;
	return tmp;
	case '=':
	tmp.token_type = EQUAL;
	return tmp;
	case ':':
	tmp.token_type = COLON;
	return tmp;
	case ',':
	tmp.token_type = COMMA;
	return tmp;
	case ';':
	tmp.token_type = SEMICOLON;
	return tmp;
	case '{':
	tmp.token_type = LBRACE;
	return tmp;
	case '}':
	tmp.token_type = RBRACE;
	return tmp;
	default:
	if (isalpha(c))
	{
	input.UngetChar(c);
	return  ScanIdOrKeyword();
	}
	else if (isdigit(c))
	{
	input.UngetChar(c);
	return ScanNumber();
	}
	else if (input.EndOfInput())
	{
	tmp.token_type = END_OF_FILE;
	}
	else
	{
	tmp.token_type = ERROR;
	}
	return tmp;
	}
	}
	
	int parse_varlist(void)
	{
	token = lexer.GetToken();
	int tempI;
	char* lexeme = (char*)malloc(sizeof(token.lexeme)+1);
	memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size()+1);
	addtoList(lexeme, token.line_no, 0);
	
	if(token.token_type == ID)
	{
	token = lexer.GetToken();
	if(token.token_type == COMMA)
	{
	//var_list is id comma var_list
	tempI = parse_varlist();
	}
	else if(token.token_type == COLON)
	{
	tempTokenType = lexer.UngetToken(token);
	//var_list is id
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_body(void);
	Token token1;
	
	int parse_unaryOperator(void)
	{
	token = lexer.GetToken();
	
	if(token.token_type == NOT)
	{
	
	//unary_operator is not 
	return(1);
	}
	else
	{
	cout << "\n Syntax Error\n";
	return(0);
	}
	}
	
	int parse_binaryOperator(void)
	{
	token = lexer.GetToken();
	if(token.token_type == PLUS )
	{
	// binary operator is plus
	return(15);
	}
	else if(token.token_type == MINUS )
	{
	//binary opeartor is minus
	return(16);
	}
	else if(token.token_type == MULT)
	{
	//binary opeartor is multiply
	return(17);
	}
	else if(token.token_type == DIV )
	{
	// binary opeartor is divison
	return(18);
	}
	else if(token.token_type == GREATER)
	{
	//binary opeartor is greater than
	return(20);
	}
	else if(token.token_type == LESS )
	{
	// binary opeartor is less than
	return(23);
	}
	else if(token.token_type == GTEQ )
	{
	//binary opeartor is greater than equal to
	return(19);
	}
	else if(token.token_type == LTEQ)
	{
	//binary opeartor is less than equal to
	return(21);
	}
	else if(token.token_type == EQUAL )
	{
	//binary operator is equal to
	return(24);
	}
	else if(token.token_type == NOTEQUAL)
	{
	//binary operator is not equal to
	return(22);
	}
	else
	{
	cout << "\n Syntax Error \n";
	return(-1);
	}
	}
	
	int parse_primary(void)
	{
	token = lexer.GetToken();
	if(token.token_type == ID )
	{
	return(SearchList(token.lexeme));
	//primary is id 
	}
	else if(token.token_type == NUM )
	{
	
	//primary is num
	return(1);
	}
	else if(token.token_type == REALNUM)
	{
	
	//primary is real num
	return(2);
	}
	else if(token.token_type == TR )
	{
	
	//primary is true
	return(3);
	}
	else if(token.token_type == FA)
	{
	//primary is false
	return(3);
	}
	else
	{
	cout << "\n Syntax Error \n";
	return(0);
	}
	}
	
	bool Expr(int i){
	if(i == LESS || i == DIV || i == MULT || i == MINUS || i == GREATER || i == PLUS || i == LTEQ || i == GTEQ || i == NOTEQUAL || i == EQUAL){
	return true;
	}
	else
	{
	return false;
	}
	}
	
	bool isExpress(int c){
	if(c != 15 && c != 16 && c != 17 && c != 18 && c != 19 && c != 20 && c != 21 && c != 22 && c != 23 && c != 24){
	return true;
	}
	else{
	return false;
	}
	}
	
	int parse_expression(void)
	{
	
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == ID || token.token_type == TR || token.token_type == NUM || token.token_type == FA || token.token_type == REALNUM )
	{
	//expression is  primary
	lexer.UngetToken(token);
	tempI = parse_primary();
	}
	else if(Expr(token.token_type))
	{
	//expression is binary_operator expression expression
	int left;
	int right;
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_binaryOperator();
	left = parse_expression();
	right = parse_expression();
	if((left != right) || isExpress(tempI))
	{
	
	if(tempI == 15 || tempI == 16 || tempI == 17 || tempI == 18)
	{
	if(left <= 2 && right > 3)
	{
	updateTypes(right, left);
	right = left;
	}
	else if(left > 3 && right <= 2)
	{
	updateTypes(right, left);
	left = right;
	}
	else if(left > 3 && right > 3)
	{
	updateTypes(right, left);
	right = left;
	}
	else
	{
	cout << "TYPE MISMATCH" << token.line_no << "C2"<<endl;
	exit(1);
	}
	}
	else if(tempI == 19 || tempI == 20 || tempI == 21 || tempI == 22 || tempI == 23 || tempI == 24)
	{
	if(right > 3 && left > 3)
	{
	updateTypes(right, left);
	right = left;
	return(3);
	}
	else
	{
	cout << "TYPE MISMATCH" << token.line_no << "C2"<<endl;
	exit(1);
	}
	}
	else
	{
	cout << "TYPE MISMATCH" << token.line_no << "C2"<<endl;
	exit(1);
	}
	}
	if(tempI == 19 || tempI == 20 || tempI == 21 || tempI == 23 || tempI == 24 )
	{
	tempI = 3;
	}
	else
	{
	tempI = right;
	}
	}
	else if(token.token_type == NOT)
	{
	//expression is unary_operator expression
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_unaryOperator(); 
	tempI = parse_expression();
	if(tempI != 3)
	{
	cout << "TYPE MISMATCH" << token.line_no << "C3"<<endl;
	exit(1);
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	return(0);
	}
	return tempI;
	}
	
	void compare_L(int line_No, int token_Type)
	{
	sTable* temp = symbolTable;
	while(temp->next != NULL)
	{
	if(temp->item->lineNo == line_No)
	{
	temp->item->type = token_Type;
	}
	temp = temp->next;
	}
	if(temp->item->lineNo == line_No)
	{
	temp->item->type = token_Type;
	}
	}
	
	void updateTypes(int currentType, int newType)
	{
	sTable* temp = symbolTable;
	
	while(temp->next != NULL)
	{
	if(temp->item->type == currentType)
	{
	temp->item->type = newType;
	}
	temp = temp->next;
	}
	if(temp->item->type == currentType)
	{
	temp->item->type = newType;
	}
	}
	
	bool isExp(int n){
	
	if(n == ID || n == NUM || n == REALNUM || n == TR || n == FA || n == PLUS || n == MINUS || n == MULT || n == DIV || n == LESS || n == GREATER || n== GTEQ || n== LTEQ || n == EQUAL || n == NOTEQUAL || n == NOT)
	{
	return true;
	}
	else{
	return false;
	}
	}
	
	int parse_assstmt(void)
	{
	int tempI;
	string name;
	int LHS;
	int RHS;
	token = lexer.GetToken();
	if(token.token_type == ID)
	{
	LHS = SearchList(token.lexeme);
	token = lexer.GetToken();
	if(token.token_type == EQUAL)
	{
	token = lexer.GetToken();
	if(isExp(token.token_type))
	{
	lexer.UngetToken(token);
	RHS = parse_expression();
	if(LHS == 1 || LHS == 2 || LHS == 3)
	{
	if(LHS == RHS)
	{
	
	}
	else
	{
	if(LHS < 3)
	{
	cout << "TYPE MISMATCH" << token.line_no << "C1" << endl;
	exit(1);
	}
	else
	{
	updateTypes(RHS,LHS);
	RHS = LHS;
	}
	}
	}
	else
	{
	updateTypes(LHS,RHS);
	LHS = RHS;
	}
	token = lexer.GetToken();
	if(token.token_type == SEMICOLON)
	{
	//assignment_stmt is ID EQUAL expression SEMICOLON
	
	}
	else
	{
	cout << "\n Syntax Error " << token.token_type << " \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_case(void)
	{
	
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == CASE )
	{
	token = lexer.GetToken();
	if(token.token_type == NUM)
	{
	token = lexer.GetToken();
	if(token.token_type == COLON)
	{
	// case -> CASE NUM COLON body
	tempI = parse_body();
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_caselist(void)
	{
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == CASE)
	{
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_case();
	token = lexer.GetToken();
	if(token.token_type == CASE)
	{
	tempTokenType = lexer.UngetToken(token);
	//case_list is case case_List
	tempI = parse_caselist();
	}
	else if(token.token_type == RBRACE)
	{
	tempTokenType = lexer.UngetToken(token);
	//case_list is case
	}
	}
	return(0);
	}
	
	int parse_switchstmt(void)
	{
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == SWITCH)
	{
	token = lexer.GetToken();
	if(token.token_type == LPAREN)
	{
	tempI = parse_expression();
	if(tempI <= 3 && tempI != 1)
	{
	cout<< "TYPE MISMATCH " << token.line_no << " C5"<<endl;
	exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type == RPAREN)
	{
	token = lexer.GetToken();
	if(token.token_type == LBRACE)
	{
	tempI = parse_caselist();
	token = lexer.GetToken();
	if(token.token_type == RBRACE)
	{
	//switch_stmt is SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE 
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_whilestmt(void)
	{
	int tempI;
	
	token = lexer.GetToken();
	if(token.token_type == WHILE)
	{
	token = lexer.GetToken();
	if(token.token_type == LPAREN)
	{
	tempI = parse_expression();
	if(tempI != 3)
	{
	cout<< "TYPE MISMATCH" << token.line_no << "C4" << endl;
	exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type == RPAREN)
	{
	// whilestmt is WHILE LPAREN expression RPAREN body
	tempI = parse_body();
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_ifstmt(void)
	{
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == IF)
	{
	token = lexer.GetToken();
	if(token.token_type == LPAREN)
	{
	tempI = parse_expression();
	if(tempI != 3)
	{
	cout<< "TYPE MISMATCH" << token.line_no << "C4" << endl;
	exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type == RPAREN)
	{
	//ifstmt is IF LPAREN expression RPAREN body 
	tempI = parse_body();
	
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_stmt(void)
	{
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == ID)
	{
	
	tempTokenType = lexer.UngetToken(token);
	// stmt is assignment_stmt
	tempI = parse_assstmt();
	
	}
	else if(token.token_type == IF)
	{
	tempTokenType = lexer.UngetToken(token);
	
	// stmt is if_stmt
	tempI = parse_ifstmt();
	}
	
	else if(token.token_type == SWITCH)
	{
	tempTokenType = lexer.UngetToken(token);
	
	// stmt is switch_stmt
	tempI = parse_switchstmt();
	}
	else if(token.token_type == WHILE)
	{
	tempTokenType = lexer.UngetToken(token);
	
	// stmt is while_stmt
	tempI = parse_whilestmt();
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_stmtlist(void)
	{
	token = lexer.GetToken();
	
	int tempI;
	if(token.token_type == IF || token.token_type == ID || token.token_type == SWITCH || token.token_type == WHILE)
	{
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_stmt();
	token = lexer.GetToken();

	if(token.token_type == IF || token.token_type == ID || token.token_type == WHILE || token.token_type == SWITCH)
	{
	tempTokenType = lexer.UngetToken(token);
	//stmt_list is stmt stmt_list
	tempI = parse_stmtlist();
	
	}
	else if (token.token_type == RBRACE)
	{
	tempTokenType = lexer.UngetToken(token);
	// stmt_list is stmt
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_body(void)
	{
	token = lexer.GetToken();

	int tempI;
	if(token.token_type == LBRACE)
	{
	// scope is ID LBRACE public_vars private_vars stmt_list RBRACE
	tempI = parse_stmtlist();
	token = lexer.GetToken();
	
	
	if(token.token_type == RBRACE)
	{
	// body is LBRACE stmt_list RBRACE
	return(0);
	}
	else
	{
	cout << "\n Syntax Error\n ";
	return(0);
	}
	}
	else if(token.token_type == END_OF_FILE)
	{
	tempTokenType = lexer.UngetToken(token);
	return(0);
	}
	else
	{
	cout << "\n Syntax Error \n ";
	return(0);
	}
	}
	
	int parse_typename(void)
	{
	token = lexer.GetToken();
	if(token.token_type == INT || token.token_type == REAL || token.token_type == BOO)
	{
	compare_L(token.line_no, token.token_type);
	}
	else
	{
	cout << "\n Syntax Error in parse_typename \n";
	}
	return(0);
	}
	
	
	int parse_vardecl(void)
	{
	int tempI;
	token = lexer.GetToken();
	if(token.token_type == ID)
	{
	//
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_varlist();
	token = lexer.GetToken();
	if(token.token_type == COLON)
	{
	tempI = parse_typename();
	

	token = lexer.GetToken();
	if(token.token_type == SEMICOLON)
	{
	//var_decl is  var_list COLON type_name SEMICOLON
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	}
	else
	{
	cout << "\n Syntax Error \n";
	}
	return(0);
	}
	
	int parse_vardecllist(void)
	{
	int tempI;
	token = lexer.GetToken();
	while(token.token_type == ID)
	{
	tempTokenType = lexer.UngetToken(token);
	tempI = parse_vardecl();
	token = lexer.GetToken();
	if(token.token_type != ID)
	{
	
	// var_decl_list is var_decl
	
	}
	else
	{

	// var_decl_list is var_decl var_decl_list
	}
	}
	tempTokenType = lexer.UngetToken(token);
	return(0);
	}
	
	string global = "::";
	
	
	int parse_globalVars(void)
	{
	
	
	token = lexer.GetToken();
	int tempI;
	if(token.token_type == ID)
	{
	tempTokenType = lexer.UngetToken(token);
	//globalVars is var_decl_list 
	tempI = parse_vardecllist();
	}
	else
	{
	cout << "Syntax Error";
	}
	return(0);
	}
	
	
	int parse_program(void)
	{
	
	token = lexer.GetToken();
	int tempI;
	while (token.token_type != END_OF_FILE)
	{
	
	if(token.token_type == ID)
	{
	tempTokenType = lexer.UngetToken(token);
	// program is global_vars scope 
	tempI = parse_globalVars();
	tempI = parse_body();
	}
	else if(token.token_type == LBRACE)
	
	{
	tempTokenType = lexer.UngetToken(token);
	//global_vars is epsilon 
	tempI = parse_body();
	}
	
	else if(token.token_type == END_OF_FILE)
	{
	return(0);
	}
	else
	{
	cout << "\n Syntax Error\n";
	return(0);
	}
	token = lexer.GetToken();
	}
	return(0);
	}
	
	string output = "";
	
	
	void printList(void)
	{
	sTable* temp = symbolTable;
	int temp1;
	
	while(temp->next != NULL)
	{
	if(temp->item->type > 3 && temp->item->print == 0)
	{
	temp1 = temp->item->type;
	output += temp->item->name;
	temp->item->print = 1;
	while(temp->next != NULL)
	{
	temp = temp->next;
	if(temp->item->type == temp1)
	{
	output += ", " + temp->item->name;
	temp->item->print = 1;
	}
	else
	{
	
	}
	}
	
	output += ": ? #";
	cout << output <<endl;
	temp->item->print = 1;
	output = "";
	temp = symbolTable;
	}
	else if(temp->item->type < 4 && temp->item->print == 0)
	{
	string lCase = keyword[(temp->item->type)-1 ];
	int temp1 = temp->item->type;
	output = temp->item->name + ": " + lCase + " #";
	cout << output <<endl;
	output = "";
	temp->item->print = 1;
	
	while(temp->next != NULL && temp->next->item->type == temp1)
	{
	temp = temp->next;
	string lCase2 = keyword[(temp->item->type)-1];
	output = temp->item->name + ": " + lCase2 + " #";
	cout << output <<endl;
	temp->item->print = 1;
	output = "";
	}
	}
	else
	{
	temp = temp->next;
	}
	}
	if(temp->item->type <= 3 && temp->item->print == 0)
	{
	string lCase3 = keyword[(temp->item->type)-1];
	output += temp->item->name + ": " + lCase3 + " #";
	cout << output <<endl;
	output = "";
	}
	else if (temp->item->type > 3 && temp->item->print == 0)
	{
	output += temp->item->name + ":" + " ? " + "#";
	cout << output <<endl;
	output = "";
	}
	else
	{
	
	}
	}
	
	
	
	
	char null[] = "NULL";
	int main()
	{
	int i;
	i = parse_program();
	printList();
	
	
	}// reference from google  
