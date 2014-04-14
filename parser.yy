
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


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	std::string * string_value;
	pair<Data_Type, string> * decl;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
};

%token <integer_value> INTEGER_NUMBER BBNUM
%token <float_value> FLOAT_NUMBER 
%token <string_value> NAME 
%token RETURN INTEGER FLOAT DOUBLE VOID IF ELSE GOTO ASSIGN_OP 

%left NE,EQ
%left LT,LE,GT,GE

%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <symbol_entry> variable_declaration
%type <symbol_table> declaration_statement_list
%type <symbol_table> varlist
%type <symbol_entry> declaration_statement
%type <symbol_entry> var
%type <decl> declaration
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> input_params_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> goto_stmt
%type <ast> if_else_stmt
%type <ast> relational_expression
%type <ast> expression
%type <ast> variable
%type <ast> constant
%type <ast> arithmetic_expression
%type <ast> term
%type <ast> factor
%type <ast> function_call

%start program

%%

program:
	declaration_statement_list
	{
		if($1!=NULL){
			program_object.set_global_table(*$1);
		}
		// program_object.global_list_in_proc_map_check();
	}

	function_list
	{	
		
	}
;

function_list:
	function_list 	function
|
	function
;

function:
	procedure_definition
;

////////////////////////////////////////////////////////////////////

declaration_statement_list:
	
	declaration_statement
	{
		int line = get_line_number();

		string var_name = $1->get_variable_name();

		if(program_object.get_procedure(var_name)!= NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable name cannot be same as procedure name");					
		}

		// if (current_procedure)
		// {
		// 	int line = get_line_number();
		// 	current_procedure->check_with_arg_list(var_name,line);
		// }


		$$ = new Symbol_Table();

		$$->push_symbol($1);

	}
|
	declaration_statement_list declaration_statement
	
	{
		int line = get_line_number();
		//program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable name cannot be same as procedure name");					
		}

		if (current_procedure)
		{
			int line = get_line_number();
			// current_procedure->check_with_arg_list(var_name,line);
		}


		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable is declared twice");					
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($2);
	}
|
	declaration_statement_list prototype
	{
		$$ = $1;
	}
|
	prototype
	{
		$$ = NULL;
	}
;

declaration_statement:
	INTEGER NAME ';'
	
	{	
		int line = get_line_number();
		$$ = new Symbol_Table_Entry(*$2, int_data_type,line);
		delete $2;
	}
|
	FLOAT NAME ';'
	
	{
		int line = get_line_number();	
		$$ = new Symbol_Table_Entry(*$2, float_data_type,line);
		delete $2;
	}
|
	DOUBLE NAME ';'

	{	
		int line = get_line_number();
		$$ = new Symbol_Table_Entry(*$2, float_data_type,line);
		delete $2;
	}	
;
// optional_declaration_list:
// 	{
// 		if (NOT_ONLY_PARSE)
// 		{
// 			Symbol_Table * global_table = new Symbol_Table();
// 			program_object.set_global_table(*global_table);
// 		}
// 	}
// |
// 	variable_declaration_list
// 	{
// 		if (NOT_ONLY_PARSE)
// 		{
// 			Symbol_Table * global_table = $1;

// 			CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

// 			program_object.set_global_table(*global_table);
// 		}
// 	}
// ;
/////////////////////////////////////////////////////////////////////

prototype:
	INTEGER NAME '(' varlist ')' ';' 
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");		
		}

		new_procedure = new Procedure(int_data_type, *$2,line);
		new_procedure->set_local_list(*$4);
		new_procedure->set_params_list(*$4);
		program_object.set_procedure_map(new_procedure,line);
	}
|
	INTEGER NAME '(' ')'  ';'
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{			
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(int_data_type, *$2,line);
		new_procedure->set_local_list(*new Symbol_Table());
		new_procedure->set_params_list(*new Symbol_Table());
		program_object.set_procedure_map(new_procedure,line);
	}
|
	FLOAT NAME '(' varlist ')' ';'
	{

		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(float_data_type, *$2,line);
		new_procedure->set_local_list(*$4);
		new_procedure->set_params_list(*$4);
		program_object.set_procedure_map(new_procedure,line);
	}
|
	FLOAT NAME '(' ')'  ';'
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(float_data_type, *$2,line);
		new_procedure->set_local_list(*new Symbol_Table());
		new_procedure->set_params_list(*new Symbol_Table());
		program_object.set_procedure_map(new_procedure,line);
	}
|
	DOUBLE NAME '(' varlist ')' ';'
	{
		int line = get_line_number();

		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(float_data_type, *$2,line);
		new_procedure->set_local_list(*$4);
		new_procedure->set_params_list(*$4);
		program_object.set_procedure_map(new_procedure,line);
	}
|
	DOUBLE NAME '(' ')'  ';'
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(float_data_type, *$2,line);
		new_procedure->set_local_list(*new Symbol_Table());
		new_procedure->set_params_list(*new Symbol_Table());
		program_object.set_procedure_map(new_procedure,line);
	}
|
	VOID NAME '(' varlist ')' ';'
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(void_data_type, *$2,line);
		new_procedure->set_local_list(*$4);
		new_procedure->set_params_list(*$4);
		program_object.set_procedure_map(new_procedure,line);
	}
|
	VOID NAME '(' ')'  ';'
	{
		int line = get_line_number();
		if(program_object.get_procedure(*$2) != NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Overloading of the procedure is not allowed");
		}

		new_procedure = new Procedure(void_data_type, *$2,line);
		new_procedure->set_local_list(*new Symbol_Table());
		new_procedure->set_params_list(*new Symbol_Table());
		program_object.set_procedure_map(new_procedure,line);
	}
;

varlist:
	varlist ','	var
	{
		
		string var_name = $3->get_variable_name();

		if($1->variable_in_symbol_list_check(var_name))
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Formal Parameter declared twice");			
		}

		if(  program_object.get_procedure($3->get_variable_name())!=NULL )
		{			
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Formal parameter list cannot be same as function name");			
		}

		if($1==NULL)
		{
			$$ = new Symbol_Table();
		}
		$$->push_symbol($3);
	}
|
	var
	{
		if(  program_object.get_procedure($1->get_variable_name())!=NULL )
		{			
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Formal parameter list cannot be same as function name");			
		}

		$$ = new Symbol_Table();
		
		$$->push_symbol($1);
	}

;

var: 
	INTEGER	NAME
	{
		$$ = new Symbol_Table_Entry(*$2, int_data_type,get_line_number());
	}
|
	FLOAT	NAME
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type,get_line_number());
	}
|
	DOUBLE	NAME
	{
		$$ = new Symbol_Table_Entry(*$2, float_data_type,get_line_number());
	}
;

/////////////////////////////////////////////////////////////////////


procedure_definition:
	proc_name proc_body
;

// proc_name:
// 	NAME '(' ')'
// 	{
// 		if (NOT_ONLY_PARSE)
// 		{
// 			CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");

// 			string proc_name = *$1;

// 			current_procedure = new Procedure(void_data_type, proc_name, get_line_number());
// 		}
// 	}
// ;
proc_name:
	NAME '(' varlist ')'
	{
		int line = get_line_number();

		if(program_object.get_procedure(*$1) == NULL)
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Procedure corresponding to the name is not found");			
		}	

		

		if(!program_object.check_if_prototype_exist($1,$3))
		{
			current_procedure = program_object.get_procedure(*$1);
		}
		else
		{
			
		 	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Last return statement type, of procedure, and its prototype should match");
		}

		if(program_object.variable_in_symbol_list_check(*$1))
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, " Procedure name cannot be same as global variable");
		}
	}
|
	NAME '('  ')'
	{
		int line = get_line_number();

		if(program_object.get_procedure(*$1) == NULL)
		{
			
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Procedure corresponding to the name is not found");
		}	

		if(!program_object.check_if_prototype_exist($1,NULL))
		{
			current_procedure = program_object.get_procedure(*$1);
		}
		else
		{		
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Last return statement type, of procedure, and its prototype should match");	
		}
	}
;

proc_body:
	'{' optional_variable_declaration_list
	{
		if (NOT_ONLY_PARSE)
		{

			CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

			Symbol_Table * local_table = $2;

			if (local_table == NULL)
				local_table = new Symbol_Table();

			if($2)current_procedure->append_local_list(*$2);

			// scurrent_procedure->set_local_list(*local_table);
		}
	}

	basic_block_list '}'
	{
		if (NOT_ONLY_PARSE)
		{
			list<Basic_Block *> * bb_list = $4;

			CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
			CHECK_INVARIANT((bb_list != NULL), "Basic block list cannot be null");

			current_procedure->set_basic_block_list(*bb_list);
			bb_existence_check($4,current_procedure->get_goto_list(),get_line_number());
		}
	}
;

optional_variable_declaration_list:
	{
		if (NOT_ONLY_PARSE)
		{
			$$ = NULL;
		}
	}
|
	variable_declaration_list
	{
		if (NOT_ONLY_PARSE)
		{
			CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

			$$ = $1;
		}
	}
;

variable_declaration_list:
	variable_declaration
	{
		if (NOT_ONLY_PARSE)
		{
			Symbol_Table_Entry * decl_stmt = $1;

			CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

			string decl_name = decl_stmt->get_variable_name();
			CHECK_INPUT ((program_object.variable_in_proc_map_check(decl_name) == false),
					"Variable name cannot be same as the procedure name", get_line_number());

			if (current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}

			Symbol_Table * decl_list = new Symbol_Table();
			decl_list->push_symbol(decl_stmt);

			$$ = decl_list;
		}
	}
|
	variable_declaration_list variable_declaration
	{
		if (NOT_ONLY_PARSE)
		{
			// if declaration is local then no need to check in global list
			// if declaration is global then this list is global list

			Symbol_Table_Entry * decl_stmt = $2;
			Symbol_Table * decl_list = $1;

			CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");
			CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

			string decl_name = decl_stmt->get_variable_name();
			CHECK_INPUT((program_object.variable_in_proc_map_check(decl_name) == false),
				"Procedure name cannot be same as the variable name", get_line_number());
			if (current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}

			CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
					"Variable is declared twice", get_line_number());

			decl_list->push_symbol(decl_stmt);
			$$ = decl_list;
		}
	}
;

variable_declaration:
	declaration ';'
	{
		if (NOT_ONLY_PARSE)
		{
			pair<Data_Type, string> * decl_stmt = $1;

			CHECK_INVARIANT((decl_stmt != NULL), "Declaration cannot be null");

			Data_Type type = decl_stmt->first;
			string decl_name = decl_stmt->second;

			Symbol_Table_Entry * decl_entry = new Symbol_Table_Entry(decl_name, type, get_line_number());

			$$ = decl_entry;

		}
	}
;

declaration:
	INTEGER NAME
	{
		if (NOT_ONLY_PARSE)
		{
			CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

			string name = *$2;
			Data_Type type = int_data_type;

			pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

			$$ = declar;
		}
	}
|
	FLOAT NAME
	{
		if (NOT_ONLY_PARSE)
		{
			CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

			string name = *$2;
			Data_Type type = float_data_type;

			pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

			$$ = declar;
		}
	}
|
	DOUBLE NAME
	{
		if (NOT_ONLY_PARSE)
		{
			CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

			string name = *$2;
			Data_Type type = float_data_type;

			pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

			$$ = declar;
		}
	}
;

basic_block_list:
	basic_block_list basic_block
	{
		if (NOT_ONLY_PARSE)
		{
			list<Basic_Block *> * bb_list = $1;
			Basic_Block * bb = $2;

			CHECK_INVARIANT((bb_list != NULL), "New basic block cannot be null");
			CHECK_INVARIANT((bb != NULL), "Basic block cannot be null");

			bb_strictly_increasing_order_check(bb_list, bb->get_bb_number());

			bb_list->push_back($2);
			$$ = bb_list;
		}
	}
|
	basic_block
	{
		if (NOT_ONLY_PARSE)
		{
			Basic_Block * bb = $1;

			CHECK_INVARIANT((bb != NULL), "Basic block cannot be null");

			list<Basic_Block *> * bb_list = new list<Basic_Block *>;
			bb_list->push_back(bb);

			$$ = bb_list;
		}
	}
;

basic_block:
	BBNUM ':' executable_statement_list
	{
		if (NOT_ONLY_PARSE)
		{
			int bb_number = $1;
			list<Ast *> * exe_stmt = $3;

			CHECK_INPUT((bb_number >= 2), "Illegal basic block lable", get_line_number());

			Basic_Block * bb = new Basic_Block(bb_number, get_line_number());

			if (exe_stmt != NULL)
				bb->set_ast_list(*exe_stmt);
			else
			{
				list<Ast *> * ast_list = new list<Ast *>;
				bb->set_ast_list(*ast_list);
			}

			$$ = bb;
		}
	}
;

executable_statement_list:
	assignment_statement_list
	{
		if (NOT_ONLY_PARSE)
		{
			$$ = $1;
		}
	}
|
	assignment_statement_list RETURN ';'
	{
		if (NOT_ONLY_PARSE)
		{
			list<Ast *> * assign_list = $1;
			Ast * ret = new Return_Ast(NULL,current_procedure,get_line_number());
			list<Ast *> * exe_list = NULL;

			if (assign_list)
				exe_list = assign_list;

			else
				exe_list = new list<Ast *>;

			exe_list->push_back(ret);

			$$ = exe_list;
		}
	}
|
	assignment_statement_list RETURN relational_expression ';'
	{
		if (NOT_ONLY_PARSE)
		{
			list<Ast *> * assign_list = $1;
			Ast * ret = new Return_Ast($3,current_procedure,get_line_number());
			list<Ast *> * exe_list = NULL;

			if (assign_list)
				exe_list = assign_list;

			else
				exe_list = new list<Ast *>;

			exe_list->push_back(ret);

			$$ = exe_list;
		}
	}
|
	assignment_statement_list goto_stmt
	{
		if (NOT_ONLY_PARSE)
		{
			list<Ast *> * assign_list = $1;
			list<Ast *> * exe_list = NULL;

			if (assign_list)
				exe_list = assign_list;

			else
				exe_list = new list<Ast *>;

			exe_list->push_back($2);

			$$ = exe_list;
		}
	}
|
	assignment_statement_list if_else_stmt
	{
		if (NOT_ONLY_PARSE)
		{
			list<Ast *> * assign_list = $1;
			list<Ast *> * exe_list = NULL;

			if (assign_list)
				exe_list = assign_list;

			else
				exe_list = new list<Ast *>;

			exe_list->push_back($2);

			$$ = exe_list;
		}
	}
;

if_else_stmt:
	IF '(' relational_expression ')'  goto_stmt  ELSE goto_stmt
	{		
		if (NOT_ONLY_PARSE)
		{
			int line = get_line_number();
			$$ = new if_else_stmt($3,$5,$7,line);
		}
	}
;

goto_stmt:
	GOTO BBNUM ';'
	{
		if (NOT_ONLY_PARSE)
		{
			int line = get_line_number();
			$$ = new goto_stmt($2,line);
			current_procedure->add_to_goto_list($2);
		}
	}
;

assignment_statement_list:
	{
		if (NOT_ONLY_PARSE)
		{
			$$ = NULL;
		}
	}
|
	assignment_statement_list assignment_statement
	{
		if (NOT_ONLY_PARSE)
		{
			list<Ast *> * assign_list = $1;
			Ast * assign_stmt = $2;
			list<Ast *> * assign_list_new = NULL;

			CHECK_INVARIANT((assign_stmt != NULL), "Assignment statement cannot be null");

			if (assign_list == NULL)
				assign_list_new = new list<Ast *>;

			else
				assign_list_new = assign_list;

			assign_list_new->push_back(assign_stmt);

			$$ = assign_list_new;
		}
	}
;

assignment_statement:
	variable ASSIGN_OP relational_expression ';'
	{
		if (NOT_ONLY_PARSE)
		{
			CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

			Ast * lhs = $1;
			Ast * rhs = $3;

			Ast * assign = new Assignment_Ast(lhs, rhs, get_line_number());

			assign->check_ast();

			$$ = assign;
		}
	}
|
	function_call ';'
	{
		$$=$1;
	}
;

relational_expression:
	relational_expression EQ relational_expression 
	{
		int line = get_line_number();
		$$ = new Relational_Expr_Ast($1,$3,"EQ",line);
		
	}
|
	relational_expression NE relational_expression 
	{
		int line = get_line_number();
		$$ = new Relational_Expr_Ast($1,$3,"NE",line);
	}
|
	relational_expression LT relational_expression 
	{
		int line = get_line_number();		
		$$ = new Relational_Expr_Ast($1,$3,"LT",line);		
	}
|
	relational_expression LE relational_expression 
	{
		int line = get_line_number();
		$$ = new Relational_Expr_Ast($1,$3,"LE",line);		
	}
|
	relational_expression GT relational_expression 
	{
		int line = get_line_number();
		$$ = new Relational_Expr_Ast($1,$3,"GT",line);
	}
|
	relational_expression GE relational_expression
	{
		int line = get_line_number();
		$$ = new Relational_Expr_Ast($1,$3,"GE",line);
	}
|	
	arithmetic_expression
	{
		$$=$1;
	}
|
	function_call
	{
		$$ = $1;
	}
|
	'(' FLOAT ')' function_call
	{		
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"float",line);
	}
|
	'(' INTEGER ')' function_call
	{
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"integer",line);
	}
|
	'(' DOUBLE ')' function_call
	{	
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"double",line);
	}

;


arithmetic_expression:
	arithmetic_expression '+' term 
	{
		int line = get_line_number();
		$$ = new Arith_Expr_Ast($1,$3,"PLUS",line);
		$$->check_ast();
	}
|
	arithmetic_expression '-' term
	{
		int line = get_line_number();
		$$ = new Arith_Expr_Ast($1,$3,"MINUS",line);
		$$->check_ast();
	}
|
	term
	{
		$$=$1;
	}

;

term:
	term '*' factor
	{
		int line = get_line_number();
		$$ = new Arith_Expr_Ast($1,$3,"MULT",line);
		$$->check_ast();
	}
|
	term '/'  factor
	{

		int line = get_line_number();
		$$ = new Arith_Expr_Ast($1,$3,"DIV",line);
		$$->check_ast();
	}
|
	factor
	{
		$$=$1;
	}
;

factor:
	'-' expression
	{
		int line = get_line_number();
		$$ = new Unary_Expr_Ast($2,line);
	}
|
	'(' FLOAT ')' expression
	{		
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"float",line);
	}
|
	'(' INTEGER ')' expression
	{
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"integer",line);
	}
|
	'(' DOUBLE ')' expression
	{	
		int line = get_line_number();
		$$ = new Typecast_Expr_Ast($4,"double",line);
	}
|
	expression
	{
		$$=$1;
	}
;

expression:
	variable
	{
		$$=$1;
	}
|
	constant
	{
		$$=$1;
	}
|
	'(' relational_expression')'
	{
		$$=$2;
	}
;


variable:
	NAME
	{
		if (NOT_ONLY_PARSE)
		{
			Symbol_Table_Entry * var_table_entry;

			CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

			string var_name = *$1;

			if (current_procedure->variable_in_symbol_list_check(var_name) == true)
				 var_table_entry = &(current_procedure->get_symbol_table_entry(var_name));

			else if (program_object.variable_in_symbol_list_check(var_name) == true)
				var_table_entry = &(program_object.get_symbol_table_entry(var_name));

			else
				CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared");

			Ast * name_ast = new Name_Ast(var_name, *var_table_entry, get_line_number());

			$$ = name_ast;
		}
	}
;

constant:
	INTEGER_NUMBER
	{
		if (NOT_ONLY_PARSE)
		{
			int num = $1;
			Ast * num_ast = new Number_Ast<int>(num, int_data_type, get_line_number());
			$$ = num_ast;
		}
	}
|
	FLOAT_NUMBER
	{
		if (NOT_ONLY_PARSE)
		{
			float num = $1;
			$$ = new Number_Ast<float>(num, float_data_type, get_line_number());
		}
	}
;

function_call:
	NAME '(' input_params_list ')'
	{
		int line = get_line_number();

		$$ = new Fn_Call_Ast(program_object.get_procedure(*$1),$3,line);
		if(  (((program_object.get_procedure(*$1))->get_params_list()).get_symbol_table()).size() != $3->size()  )		
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Actual and formal parameter count do not match");		
		}

		if( program_object.input_params_type_check(  program_object.get_procedure(*$1) ,*$3) )
		{	
			int line = get_line_number();
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Actual and formal parameter count do not match");
		}
	}
|
	NAME '('  ')'
	{
		int line = get_line_number();
		$$ = new Fn_Call_Ast(program_object.get_procedure(*$1),NULL,line);
		if(  (((program_object.get_procedure(*$1))->get_params_list()).get_symbol_table()).size() != 0  )		
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Actual and formal parameter count do not match");
		}
	}
;

input_params_list:
	input_params_list ',' relational_expression
	{
		if($1==NULL){
			$$ = new list<Ast *>;
		}
		$$->push_back($3);
	}
|
	relational_expression
	{
		
		$$ = new list<Ast *>;
		
		$$->push_back($1);
	}
;

