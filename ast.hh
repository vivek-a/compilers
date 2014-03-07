
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

#ifndef AST_HH
#define AST_HH

#include<string>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "

using namespace std;

//enum relation_operators {LE,GE,GT,LT,EQ,NE,AND,OR,NOT};
//enum arith_ops {PLUS,MINUS,MULT,DIV};

class Ast;
#include "basic-block.hh"
#include "procedure.hh"

class Ast
{
protected:
	Data_Type node_data_type;
	int basic_block_number;	
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
	int get_number();
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);
	string get_name();
	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Return_Ast:public Ast
{
	Ast * lhs;
public:
	Return_Ast(Ast * temp_lhs);
	~Return_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Goto_Ast:public Ast
{

public:
	Goto_Ast(int );
	~Goto_Ast();

	int get_number();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Relational_Expr_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;
	string op;

public:
	Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class If_Else_Loop_Ast:public Ast
{
	Ast * relational_ast;
	Ast * if_ast;
	Ast * else_ast;

public:
	If_Else_Loop_Ast(Ast * ,Ast * ,Ast * );
	~If_Else_Loop_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Arith_Expr_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;
	string op;

public:
	Arith_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op);
	~Arith_Expr_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Typecast_Expr_Ast:public Ast
{
	Ast * lhs;
	string type;

public:
	Typecast_Expr_Ast(Ast * temp_lhs,string temp_op);
	~Typecast_Expr_Ast();
	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Unary_Expr_Ast:public Ast
{
	Ast * lhs;

public:
	Unary_Expr_Ast(Ast * temp_lhs);
	~Unary_Expr_Ast();
	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Fn_Call_Ast:public Ast
{
	Procedure * proc;
	list<Ast *> * var_list;
public:
	Fn_Call_Ast(Procedure * temp_proc, list<Ast *> * ast_list);
	~Fn_Call_Ast();
	Data_Type get_data_type();
	void print_ast(ostream & file_buffer);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};


#endif
