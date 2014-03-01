
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


**********************************************************************************************/

 

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;	
};

%token <integer_value> INTEGER_NUMBER BASIC_BLOCK
%token <float_value> FLOAT_NUMBER 
%token <string_value> NAME
%token RETURN INTEGER FLOAT DOUBLE VOID IF ELSE GOTO

%right ASSIGN_OP
%left NE,EQ
%left LT,LE,GT,GE

// %type <symbol_table> declaration_statement_list
// %type <symbol_entry> declaration_statement
// %type <basic_block_list> basic_block_list
// %type <ast_list> assignments_statement_list
// %type <basic_block> basic_block
// %type <ast_list> executable_statement_list
// %type <ast> executable_statement
// %type <ast> assignments_statement
// %type <ast> relational_expression
// %type <ast> if_else_loop
// %type <ast> go_to_statement
// %type <ast> expression
// %type <ast> variable
// %type <ast> constant
// %type <ast> arithmetic_expression
// %type <ast> term
// %type <ast> factor

%start program

%%


program:
	declaration_statement_list

	prototype_list

	function_list
|
	declaration_statement_list

	function_list
|
	prototype_list

	function_list
|
	function_list
;

prototype_list:
	prototype_list 	prototype
|
	prototype
;

prototype:
	INTEGER NAME '(' varlist ')' ';'
|
	INTEGER NAME '(' ')'  ';'
|
	FLOAT NAME '(' varlist ')' ';'
|
	FLOAT NAME '(' ')'  ';'
|
	DOUBLE NAME '(' varlist ')' ';'
|
	DOUBLE NAME '(' ')'  ';'
|
	VOID NAME '(' varlist ')' ';'
|
	VOID NAME '(' ')'  ';'
;

varlist:
	varlist ','

	var
|
	var
;

var: 
	vartype 

	NAME
;

vartype:
	INTEGER | FLOAT | DOUBLE | VOID 
;


function_list:
	function_list 

	function
|
	function
;

function:
	procedure_name

	procedure_body
;

procedure_name:
	NAME '(' varlist ')'
|
	NAME '('  ')'
;

procedure_body:
	'{' declaration_statement_list  
	
	// {
	// 	current_procedure->set_local_list(*$2);
	// 	delete $2;
	
		
	// }

	basic_block_list '}'
	// {
	// 	//return_statement_used_flag == false;
	// 	// if (return_statement_used_flag == false)
	// 	// {
	// 	// 	int line = get_line_number();
	// 	// 	report_error("Atleast 1 basic block should have a return statement", line);
	// 	// }
	// 	current_procedure->set_basic_block_list(*$4);
	// 	bb_existence_check($4,goto_blocks);		

	// 	delete $4;
	// }
	
|
	'{' basic_block_list '}'
	
	// {
	// 	//return_statement_used_flag == false;
	// 	// if (return_statement_used_flag == false)
		
	// 	// {
	// 	// 	int line = get_line_number();
	// 	// 	report_error("Atleast 1 basic block should have a return statement", line);
	// 	// }

	// 	current_procedure->set_basic_block_list(*$2);
	// 	bb_existence_check($2,goto_blocks);

	// 	delete $2;
	// }
	
;

declaration_statement_list:
	
	
	// {
		// int line = get_line_number();
		// program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		// string var_name = $1->get_variable_name();
		// if (current_procedure && current_procedure->get_proc_name() == var_name)
		// {
		// 	int line = get_line_number();
		// 	report_error("Variable name cannot be same as procedure name", line);
		// }

		// $$ = new Symbol_Table();
		// $$->push_symbol($1);
	 // }
	declaration_statement_list declaration_statement
	
	// {
	// 	// if declaration is local then no need to check in global list
	// 	// if declaration is global then this list is global list

	// 	int line = get_line_number();
	// 	program_object.variable_in_proc_map_check($2->get_variable_name(), line);

	// 	string var_name = $2->get_variable_name();
	// 	if (current_procedure && current_procedure->get_proc_name() == var_name)
	// 	{
	// 		int line = get_line_number();
	// 		report_error("Variable name cannot be same as procedure name", line);
	// 	}

	// 	if ($1 != NULL)
	// 	{
	// 		if($1->variable_in_symbol_list_check(var_name))
	// 		{
	// 			int line = get_line_number();
	// 			report_error("Variable is declared twice", line);
	// 		}

	// 		$$ = $1;
	// 	}

	// 	else
	// 		$$ = new Symbol_Table();

	// 	$$->push_symbol($2);
	// }
|
	declaration_statement
;

declaration_statement:
	INTEGER NAME ';'
	
	// {	//do we need to check for repetition again ????
	// 	$$ = new Symbol_Table_Entry(*$2, int_data_type);
	// 	delete $2;
	// }
|
	FLOAT NAME ';'
	
	// {	//do we need to check for repetition again ????
	// 	$$ = new Symbol_Table_Entry(*$2, float_data_type);
	// 	delete $2;
	// }
|
	DOUBLE NAME ';'

	// {	//do we need to check for repetition again ????
	// 	$$ = new Symbol_Table_Entry(*$2, float_data_type);
	// 	delete $2;
	// }
	
;

basic_block_list:
	basic_block_list basic_block
	
	// {
	// 	if (!$2)
	// 	{
	// 		int line = get_line_number();
	// 		report_error("Basic block doesn't exist", line);
	// 	}

	// 	bb_strictly_increasing_order_check($$, $2->get_bb_number());
	// 	$$ = $1;
	// 	($$->back())->set_successor(true);
	// 	//cout<<"succesdddddddddddsor : " <<($$->back())->get_successor() << " : bb num :" << ($$->back())->get_bb_number() <<endl;

	// 	$$->push_back($2);
	// }
	
|
	basic_block
	
	// {
	// 	if (!$1)
	// 	{
	// 		int line = get_line_number();
	// 		report_error("Basic block doesn't exist", line);
	// 	}

	// 	$$ = new list<Basic_Block *>;
	// 	$$->push_back($1);
	// }
	
;

basic_block:
	BASIC_BLOCK ':' executable_statement_list
	
	// {
		

	// 	if ($1 < 2)
	// 	{
	// 		int line = get_line_number();
	// 		report_error("Illegal basic block lable", line);
	// 	}		

	// 	if ($3 != NULL)
	// 		$$ = new Basic_Block($1,*$3,successor);
	// 	else
	// 	{
	// 		list<Ast *> * ast_list = new list<Ast *>;
	// 		$$ = new Basic_Block($1, *ast_list,successor);
	// 	}

	// 	//cout<<"successor : " <<successor << " : bb num :" << $1 <<endl;

	// 	delete $3;		
	//  }
	
;

executable_statement_list:

	assignments_statement_list executable_statement
	// {
	// 	if ($1 == NULL)
	// 		$$ = new list<Ast *>;		
	// 	else
	// 		$$ = $1;
	// 	if($2 != NULL)
	// 		$$->push_back($2);	
	// }
	
|
	assignments_statement_list  RETURN  relational_expression ';'
	// {
	// 	Ast * ret = new Return_Ast();

	// 	//return_statement_used_flag = true;// Current procedure has an occurrence of return statement

	// 	if ($1 != NULL)
	// 		$$ = $1;
	// 	else
	// 		$$ = new list<Ast *>;

	// 	$$->push_back(ret);
	// 	successor=true;
	// }
|
	assignments_statement_list  RETURN  ';'
;

executable_statement:
	if_else_loop
	// {
	// 	$$=$1;
	// 	successor=true;
	// }
|
	go_to_statement
	// {
	// 	$$=$1;
	// 	successor=true;
	// }
|
	// {
	// 	$$ = NULL;
	// 	successor = false;	
	// }
;

assignments_statement_list:

	// {
	// 	$$ = NULL;	
	// }

|
	assignments_statement_list assignments_statement
	
	// {
	// 	if ($1 == NULL)
	// 		$$ = new list<Ast *>;
	// 	else
	// 		$$ = $1;

	// 	$$->push_back($2);		
	// }
	
;

assignments_statement:
	variable ASSIGN_OP relational_expression ';'
	// {
	// 	$$ = new Assignment_Ast($1,$3);

	// 	int line = get_line_number();
	// 	$$->check_ast(line);		
	//  }
|
	function_call ';'
;

if_else_loop:
	IF '(' relational_expression ')'  go_to_statement  ELSE go_to_statement
	// {		
	// 	$$ = new If_Else_Loop_Ast($3,$5,$7);
	// }
;

go_to_statement:
	GOTO BASIC_BLOCK ';'
	// {
	// 	//Check basic block number exist

	// 	$$ = new Goto_Ast( $2 );
	// 	goto_blocks->push_back($2);
	// }
;


relational_expression:

	relational_expression EQ relational_expression 
	// {
	// 	// $$ = new Relational_Expr_Ast($1,$3,$2);
	// 	$$ = new Relational_Expr_Ast($1,$3,"EQ");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	relational_expression NE relational_expression 
	// {
	// 	// $$ = new Relational_Expr_Ast($1,$3,$2);
	// 	$$ = new Relational_Expr_Ast($1,$3,"NE");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	relational_expression LT relational_expression 
	// {
	// 	// $$ = new Relational_Expr_Ast($1,$3,$2);
	// 	$$ = new Relational_Expr_Ast($1,$3,"LT");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	relational_expression LE relational_expression 
	// {
	// 	$$ = new Relational_Expr_Ast($1,$3,"LE");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	relational_expression GT relational_expression 
	// {
	// 	$$ = new Relational_Expr_Ast($1,$3,"GT");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	relational_expression GE relational_expression
	// {
	// 	$$ = new Relational_Expr_Ast($1,$3,"GE");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|	
	arithmetic_expression
	// {
	// 	$$=$1;
	// }
;


arithmetic_expression:
	arithmetic_expression '+' term 
	// {
	// 	$$ = new Arith_Expr_Ast($1,$3,"PLUS");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	arithmetic_expression '-' term
	// {
	// 	$$ = new Arith_Expr_Ast($1,$3,"MINUS");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	term
	// {
	// 	$$=$1;
	// }
;

term:
	term '*' factor
	// {
	// 	$$ = new Arith_Expr_Ast($1,$3,"MULT");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	term '/'  factor
	// {
	// 	$$ = new Arith_Expr_Ast($1,$3,"DIV");
	// 	int line = get_line_number();
	// 	$$->check_ast(line);
	// }
|
	factor
	// {
	// 	$$=$1;
	// }
;

factor:
	'-' expression
	// {
	// 	$$ = new Unary_Expr_Ast($2);
	// }
|
	'(' FLOAT ')' expression
	// {
		
	// 	$$ = new Typecast_Expr_Ast($4,"float");
	// }
|
	'(' INTEGER ')' expression
	// {
		
	// 	$$ = new Typecast_Expr_Ast($4,"integer");
	// }
|
	'(' DOUBLE ')' expression
	// {	
	// 	$$ = new Typecast_Expr_Ast($4,"double");
	// }
|
	expression
	// {
	// 	$$=$1;
	// }
;

expression:
	variable 
	// {
	// 	$$=$1;
	// }
|
	constant
	// {
	// 	$$=$1;
	// }
|
	'(' relational_expression ')'
	// {
	// 	$$=$2;
	// }
;

variable:
	NAME

	// {
	// 	Symbol_Table_Entry var_table_entry;

	// 	if (current_procedure->variable_in_symbol_list_check(*$1))
	// 		 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

	// 	else if (program_object.variable_in_symbol_list_check(*$1))
	// 		var_table_entry = program_object.get_symbol_table_entry(*$1);

	// 	else
	// 	{
	// 		int line = get_line_number();
	// 		report_error("Variable has not been declared", line);
	// 	}

	// 	$$ = new Name_Ast(*$1, var_table_entry);

	// 	delete $1;
	//  }
;

constant:
	INTEGER_NUMBER
	// {
	// 	$$ = new Number_Ast<int>($1, int_data_type);
	// }	
|
	FLOAT_NUMBER
	// {
	// 	$$ = new Number_Ast<float>($1, float_data_type);
	// }
|
	function_call
;



function_call:
	NAME '(' input_params_list ')'

;

input_params_list:
	input_params_list ',' relational_expression
|
	relational_expression
|
;