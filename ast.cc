
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

#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

int Ast::get_BBnum()
{
	return BBnum;
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	// cout<<"-------"<<lhs->get_data_type()<<"-----"<<rhs->get_data_type()<<endl;

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		// cout<<"assign waal ---"<<node_data_type<<endl;
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:";

	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	Eval_Result & final = *new  Eval_Result_Value_Int();
	final.set_value(0);

	return final;
}

Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();

	if(load_register == NULL)cout<<rhs->get_data_type()<<endl;
	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	if ( (typeid(*rhs) != typeid(Arith_Expr_Ast)) && (typeid(*rhs) != typeid(Unary_Expr_Ast)) &&  (typeid(*rhs) != typeid(Typecast_Expr_Ast)) && (typeid(*rhs) != typeid(Relational_Expr_Ast)) && (typeid(*rhs) != typeid(Fn_Call_Ast)) ) {
		lra.optimize_lra(mc_2m, lhs, rhs);
	}
	
	Code_For_Ast load_stmt = rhs->compile_and_optimize_ast(lra);

	Register_Descriptor * result_register = load_stmt.get_reg();

	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);


	Symbol_Table_Entry *  destination_symbol_entry = & lhs->get_symbol_entry();
	Register_Descriptor * destination_register = destination_symbol_entry->get_register();

	if(destination_symbol_entry){
		if (destination_register)
			destination_symbol_entry->free_register(destination_register); 

		destination_symbol_entry->update_register(result_register);
	}


	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	return *assign_stmt;
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		// CHECK_INVARIANT((loc_var_val->get_result_enum() == int_result) || (loc_var_val->get_result_enum() == float_result), "Result type can only be int or float");
		// file_buffer << loc_var_val->get_value() << "\n";
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer  << fixed << setprecision(0) << (loc_var_val->get_value()).i << "\n";
		else if (loc_var_val->get_result_enum() == float_result)
			file_buffer  << fixed << setprecision(2) << (loc_var_val->get_value()).f << "\n";
	}

	else
	{
		CHECK_INVARIANT( (glob_var_val->get_result_enum() == int_result) || (glob_var_val->get_result_enum() == float_result), 
			"Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else
			if(glob_var_val->get_result_enum()==int_result)
				file_buffer << fixed << setprecision(0) << (glob_var_val->get_value()).i << "\n";
			else if(glob_var_val->get_result_enum()==float_result)
				file_buffer << fixed << setprecision(2) << (glob_var_val->get_value()).f << "\n";
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}
	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	if (variable_symbol_entry->get_data_type() == int_data_type)
		i = new Eval_Result_Value_Int();
	else if (variable_symbol_entry->get_data_type() == float_data_type)
		i = new Eval_Result_Value_Float();
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (result.get_result_enum() == int_result)
	 	i->set_value( (result.get_value()).i );
	else if (result.get_result_enum() == float_result)
	 	i->set_value( (result.get_value()).f );
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt = new Move_IC_Stmt(store, register_opd, opd);

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Descriptor * result_register = NULL;
	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();
	// Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(load, opd, register_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}


Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		load_stmt = new Move_IC_Stmt(load, opd, register_opd);
			
		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;

	ast_num_child = zero_arity;

	lineno = line;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value( (int)constant);

		return result;
	}
	else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value( (float)constant);

		return result;
	}
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Descriptor * result_register = NULL;
	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);

	Ics_Opd * opd = NULL;

	if(node_data_type == int_data_type)
		opd = new Const_Opd<int>(constant);
	else 
		opd = new Const_Opd<float>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);

	return num_code;
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	
	Ics_Opd * opd = NULL;

	if(node_data_type == int_data_type)
		opd = new Const_Opd<int>(constant);
	else 
		opd = new Const_Opd<float>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////
Return_Ast::Return_Ast(Ast * temp_lhs,Procedure * proc,int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;

	this->lhs=temp_lhs;
	this->proc=proc;
}

Return_Ast::~Return_Ast()
{
	delete lhs;
	delete proc;
}

void Return_Ast::print(ostream & file_buffer)
{
	if(lhs==NULL){
		file_buffer<<endl<<AST_SPACE << "RETURN <NOTHING>\n";
	}else{
		file_buffer<<endl<< AST_SPACE << "RETURN ";
		lhs->print(file_buffer);
		file_buffer<<endl;
	}
	
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{	
	print(file_buffer);
	proc->return_check=1;
	if(lhs==NULL){
		Eval_Result & result = *new Eval_Result_Value_Int();
		return result;
	}
	else{ 
		return lhs->evaluate(eval_env,file_buffer);	
	}	
}

Code_For_Ast & Return_Ast::compile()
{
	list<Icode_Stmt *> ic_list;

	if(lhs)
	{
		Code_For_Ast & load_stmt = lhs->compile();
		Register_Descriptor * load_register = load_stmt.get_reg();
		ic_list = load_stmt.get_icode_list();

		load_register->used_for_expr_result = true;
		if(proc->get_return_type()==int_data_type){
			Ics_Opd *  v_one_opd  = new Register_Addr_Opd(load_register);
			Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[v1]);

			Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
			ic_list.push_back(m_stmt);

		}
		else if(proc->get_return_type()==float_data_type){
			Ics_Opd *  v_one_opd  = new Register_Addr_Opd(load_register);
			Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[f0]);
			Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
			ic_list.push_back(m_stmt);
		}
		load_register->used_for_expr_result = false;
	}

	Ics_Opd * const_opd = new Const_Opd<string>(proc->get_proc_name());
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(ret,NULL,NULL,const_opd);
	ic_list.push_back(goto_stmt);

	Code_For_Ast & ret_code = *new Code_For_Ast(ic_list, NULL);

	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> ic_list;

	if(lhs)
	{
		lra.optimize_lra( mc_2r , NULL , lhs );
		Code_For_Ast & load_stmt = lhs->compile_and_optimize_ast(lra);
		Register_Descriptor * load_register = load_stmt.get_reg();
		ic_list = load_stmt.get_icode_list();

		load_register->used_for_expr_result = true;
		if(proc->get_return_type()==int_data_type){
			Ics_Opd *  v_one_opd  = new Register_Addr_Opd(load_register);
			Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[v1]);

			Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
			ic_list.push_back(m_stmt);

		}
		else if(proc->get_return_type()==float_data_type){
			Ics_Opd *  v_one_opd  = new Register_Addr_Opd(load_register);
			Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[f0]);
			Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
			ic_list.push_back(m_stmt);
		}
		load_register->used_for_expr_result = false;
	}
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(ret,NULL,NULL,NULL);
	ic_list.push_back(goto_stmt);

	Code_For_Ast & ret_code = *new Code_For_Ast(ic_list, NULL);

	return ret_code;
}

template class Number_Ast<int>;
template class Number_Ast<float>;


/////////////////////////////////////////////////////////////////////////////////////////////////////////

goto_stmt::goto_stmt(int temp_BBnum, int line)
{
	BBnum = temp_BBnum;

	ast_num_child = zero_arity;;
	node_data_type = void_data_type;

	lineno = line;
}

goto_stmt::~goto_stmt()
{
}

int goto_stmt::get_BBnum()
{
	return BBnum;
}

void goto_stmt::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Goto statement:\n";
	file_buffer<<AST_NODE_SPACE<<"Successor: "<<BBnum;
}

Eval_Result & goto_stmt::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print(file_buffer);
	file_buffer<<"\n" << AST_SPACE << "GOTO (BB "<<get_BBnum()<<")\n";
	Eval_Result & result = * new Eval_Result_Value_Int();
	result.set_value(BBnum);
	return result;
}

Code_For_Ast & goto_stmt::compile()
{
	Ics_Opd * const_opd = new Const_Opd<int>(BBnum);

	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(Goto,NULL,NULL,const_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(goto_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, NULL);

	return load_code;
}

Code_For_Ast & goto_stmt::compile_and_optimize_ast(Lra_Outcome & lra)
{

	Ics_Opd * const_opd = new Const_Opd<int>(BBnum);

	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(Goto,NULL,NULL,const_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(goto_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, NULL);

	return load_code;
}

/////////////////////////////////////////////////////////////////////////////////////////////

if_else_stmt::if_else_stmt(Ast * temp_rel_expr ,  Ast * temp_lhs  , Ast * temp_rhs  , int line)
{
	rel_expr=temp_rel_expr;
	lhs=temp_lhs;
	rhs=temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

if_else_stmt::~if_else_stmt()
{
	delete lhs;
	delete rhs;
	delete rel_expr;
}

void if_else_stmt::print(ostream & file_buffer)
{
	file_buffer<<"\n" << AST_SPACE << "If_Else statement:";
	rel_expr->print(file_buffer);	
	file_buffer <<"\n";
	file_buffer << AST_NODE_SPACE<<"True Successor: "<<lhs->get_BBnum()<<"\n";
	file_buffer << AST_NODE_SPACE<<"False Successor: "<<rhs->get_BBnum();
}

Eval_Result & if_else_stmt::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	print(file_buffer);

	Eval_Result & result = * new Eval_Result_Value_Int();	
	

	Eval_Result & eval_relation = rel_expr->evaluate(eval_env, file_buffer);
	if((eval_relation.get_value()).i){
		file_buffer<<"\n" << AST_SPACE << "Condition True : Goto (BB "<< lhs->get_BBnum() <<")\n";
		result.set_value(lhs->get_BBnum());
		return result;
	}
	else{
		file_buffer<<"\n"<< AST_SPACE << "Condition False : Goto (BB "<< rhs->get_BBnum() <<")\n";
		result.set_value(rhs->get_BBnum());
		return result;
	}
}

Code_For_Ast & if_else_stmt::compile()
{

	Code_For_Ast & rel_expr_stmt = rel_expr->compile();
	Register_Descriptor * rel_expr_register = rel_expr_stmt.get_reg();

	Ics_Opd *  rel_expr_opd = new Register_Addr_Opd(rel_expr_register);

	Ics_Opd *  zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[zero]);

	Icode_Stmt * jump_stmt = new Control_Flow_IC_Stmt(bne,rel_expr_opd,zero_opd,new Const_Opd<int>(lhs->get_BBnum()));
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(Goto,NULL,NULL,new Const_Opd<int>(rhs->get_BBnum()));

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (rel_expr_stmt.get_icode_list().empty() == false)
		ic_list = rel_expr_stmt.get_icode_list();

	ic_list.push_back(jump_stmt);
	ic_list.push_back(goto_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, NULL);

	return *assign_stmt;
}

Code_For_Ast & if_else_stmt::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & rel_expr_stmt = rel_expr->compile_and_optimize_ast(lra);

	Register_Descriptor * rel_expr_register = rel_expr_stmt.get_reg();

	Ics_Opd *  rel_expr_opd = new Register_Addr_Opd(rel_expr_register);

	Ics_Opd *  zero_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[zero]);

	Icode_Stmt * jump_stmt = new Control_Flow_IC_Stmt(bne,rel_expr_opd,zero_opd,new Const_Opd<int>(lhs->get_BBnum()));
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(Goto,NULL,NULL,new Const_Opd<int>(rhs->get_BBnum()));

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (rel_expr_stmt.get_icode_list().empty() == false)
		ic_list = rel_expr_stmt.get_icode_list();

	ic_list.push_back(jump_stmt);
	ic_list.push_back(goto_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, NULL);

	return *assign_stmt;
}
/////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op ,int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	op= temp_op;

	ast_num_child = binary_arity;
	node_data_type = int_data_type;

	lineno = line;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return int_data_type;
}

void Relational_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer<<"\n"<< AST_NODE_SPACE<<"Condition: "<<op <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print(file_buffer);
	file_buffer << ")\n";

	if(rhs!=NULL)
	{
		file_buffer << AST_NODE_SPACE << "   RHS (";
		rhs->print(file_buffer);
		file_buffer << ")";
	}
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & result_rhs = rhs->evaluate(eval_env, file_buffer);
	Eval_Result & result = * new Eval_Result_Value_Int();

	if(lhs->get_data_type()==int_data_type){
		if(op == "LT")
		{
			if( (result_lhs.get_value()).i < (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "GT") 
		{
			if((result_lhs.get_value()).i > (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "LE")
		{
			if((result_lhs.get_value()).i <= (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "GE")
		{
			if((result_lhs.get_value()).i >= (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "EQ")
		{
			if((result_lhs.get_value()).i == (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "NE")
		{
			if((result_lhs.get_value()).i != (result_rhs.get_value()).i)result.set_value(1);			
			else result.set_value(0);
		} 
	}
	else{
		if(op == "LT")
		{
			if( (result_lhs.get_value()).f < (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "GT") 
		{
			if((result_lhs.get_value()).f > (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "LE")
		{
			if((result_lhs.get_value()).f <= (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "GE")
		{
			if((result_lhs.get_value()).f >= (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "EQ")
		{
			if((result_lhs.get_value()).f == (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
		else if(op == "NE")
		{
			if((result_lhs.get_value()).f != (result_rhs.get_value()).f)result.set_value(1);			
			else result.set_value(0);
		} 
	}
	return result;	
}

Code_For_Ast & Relational_Expr_Ast::compile()
{
	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = machine_dscr_object.get_new_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compute_stmt = NULL;

	if(op == "GT")
	{
		compute_stmt = new Compute_IC_Stmt(sgt,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "GE")
	{
		compute_stmt = new Compute_IC_Stmt(sge,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "LT")
	{
		compute_stmt = new Compute_IC_Stmt(slt,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "LE")
	{
		compute_stmt = new Compute_IC_Stmt(sle,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "EQ")
	{
		compute_stmt = new Compute_IC_Stmt(seq,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "NE")
	{
		compute_stmt = new Compute_IC_Stmt(sne,lhs_opd,rhs_opd,res_opd);		
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Illegal local register allocation scenario (:-)");
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(compute_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	rhs_register->used_for_expr_result=false;

	return *assign_stmt;
}

Code_For_Ast & Relational_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	lra.optimize_lra( mc_2r , NULL , lhs );
	Code_For_Ast  lhs_stmt = lhs->compile_and_optimize_ast(lra);
	
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	lra.optimize_lra( mc_2r , NULL ,rhs );
	Code_For_Ast rhs_stmt = rhs->compile_and_optimize_ast(lra);


	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = machine_dscr_object.get_new_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * compute_stmt = NULL;

	if(op == "GT")
	{
		compute_stmt = new Compute_IC_Stmt(sgt,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "GE")
	{
		compute_stmt = new Compute_IC_Stmt(sge,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "LT")
	{
		compute_stmt = new Compute_IC_Stmt(slt,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "LE")
	{
		compute_stmt = new Compute_IC_Stmt(sle,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "EQ")
	{
		compute_stmt = new Compute_IC_Stmt(seq,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "NE")
	{
		compute_stmt = new Compute_IC_Stmt(sne,lhs_opd,rhs_opd,res_opd);		
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Illegal local register allocation scenario (:-)");
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(compute_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	rhs_register->used_for_expr_result=false;

	return *assign_stmt;
}

//////////////////////////////////////////////////////////////////////////////////////////

Arith_Expr_Ast::Arith_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op ,int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	op= temp_op;

	ast_num_child = binary_arity;
	lineno = line;
}

Arith_Expr_Ast::~Arith_Expr_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Arith_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Arith_Expr_Ast::check_ast()
{
	
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}
	CHECK_INVARIANT(false,"Arithmetic statement data type not compatible");
}

void Arith_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_NODE_SPACE<<"Arith: "<<op <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	if(rhs!=NULL)
	{
		file_buffer<<"\n" << AST_NODE_SPACE << "   RHS (";
		rhs->print(file_buffer);
		file_buffer << ")";
	}
}

Eval_Result & Arith_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & result_rhs = rhs->evaluate(eval_env, file_buffer);
	
	Eval_Result & int_result = * new Eval_Result_Value_Int();
	Eval_Result & float_result = * new Eval_Result_Value_Float();

	if( (lhs->get_data_type() == int_data_type) && (rhs->get_data_type() == int_data_type) )
	{
		if(op == "PLUS")
		{
			int_result.set_value(  (result_lhs.get_value().i+result_rhs.get_value().i) ) ;
		} 
		else if(op == "MINUS") 
		{
			int_result.set_value(  (result_lhs.get_value().i-result_rhs.get_value().i) ) ;
		} 
		else if(op == "MULT")
		{
			int_result.set_value(  (result_lhs.get_value().i * result_rhs.get_value().i) ) ;
		}
		else if(op == "DIV")
		{
			int_result.set_value(  (result_lhs.get_value().i / result_rhs.get_value().i) ) ;
		} 
		return int_result;
	}
	else
	{
		if(op == "PLUS")
		{
			float_result.set_value( result_lhs.get_value().f+result_rhs.get_value().f ) ;
		} 
		else if(op == "MINUS") 
		{
			float_result.set_value( result_lhs.get_value().f-result_rhs.get_value().f ) ;
		} 
		else if(op == "MULT")
		{
			float_result.set_value( result_lhs.get_value().f * result_rhs.get_value().f ) ;
		}
		else if(op == "DIV")
		{
			float_result.set_value( result_lhs.get_value().f / result_rhs.get_value().f ) ;
		} 
		return float_result;
	}	
}

Code_For_Ast & Arith_Expr_Ast::compile()
{
	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * Compute_stmt = NULL;

	if(op == "PLUS")
	{
		Compute_stmt = new Compute_IC_Stmt(add,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "MINUS")
	{
		Compute_stmt = new Compute_IC_Stmt(sub,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "DIV")
	{
		Compute_stmt = new Compute_IC_Stmt(divide,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "MULT")
	{
		Compute_stmt = new Compute_IC_Stmt(mul,lhs_opd,rhs_opd,res_opd);		
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Illegal local register allocation scenario (:-)");
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	rhs_register->used_for_expr_result=false;

	return *assign_stmt;
}

Code_For_Ast & Arith_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	// lra.optimize_lra( mc_2m , lhs , rhs );

	lra.optimize_lra( mc_2r , NULL , lhs );
	Code_For_Ast & lhs_stmt = lhs->compile_and_optimize_ast(lra);
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	lra.optimize_lra( mc_2r , NULL , rhs );
	Code_For_Ast & rhs_stmt = rhs->compile_and_optimize_ast(lra);
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * Compute_stmt = NULL;

	if(op == "PLUS")
	{
		Compute_stmt = new Compute_IC_Stmt(add,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "MINUS")
	{
		Compute_stmt = new Compute_IC_Stmt(sub,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "DIV")
	{
		Compute_stmt = new Compute_IC_Stmt(divide,lhs_opd,rhs_opd,res_opd);		
	}
	else if(op == "MULT")
	{
		Compute_stmt = new Compute_IC_Stmt(mul,lhs_opd,rhs_opd,res_opd);		
	}
	else
	{
		CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,"Illegal local register allocation scenario (:-)");
	}

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	rhs_register->used_for_expr_result=false;

	return *assign_stmt;
}

////////////////////////////////////////////////////////////////////////////////////////////

Typecast_Expr_Ast::Typecast_Expr_Ast(Ast * temp_lhs , string temp_op, int line)
{
	lhs = temp_lhs;
	type=temp_op;

	ast_num_child = unary_arity;
	lineno = line;
	if(type=="integer")
		node_data_type=int_data_type;
	else
		node_data_type=float_data_type;
}

Typecast_Expr_Ast::~Typecast_Expr_Ast()
{
	delete lhs;
}

Data_Type Typecast_Expr_Ast::get_data_type()
{
	return node_data_type;
}

void Typecast_Expr_Ast::print(ostream & file_buffer)
{
	lhs->print(file_buffer);
}

Eval_Result & Typecast_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);

	Eval_Result & int_result = * new Eval_Result_Value_Int();
	Eval_Result & float_result = * new Eval_Result_Value_Float();

	if(this->get_data_type() == int_data_type) 
	{
		if(lhs->get_data_type()==int_data_type)
			int_result.set_value( result_lhs.get_value().i );
		else if(lhs->get_data_type()==float_data_type)
			int_result.set_value( result_lhs.get_value().f );
		return int_result;
	}

	else if(this->get_data_type() == float_data_type)
	{
		if(lhs->get_data_type()==int_data_type)
			float_result.set_value( result_lhs.get_value().i );
		else if(lhs->get_data_type()==float_data_type)
			float_result.set_value( result_lhs.get_value().f );
		return float_result;
	} 
	
		
}

Code_For_Ast & Typecast_Expr_Ast::compile()
{
	// cout<<type<<"-------"<<get_data_type()<<endl;
	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);
	
	Icode_Stmt * Compute_stmt=NULL;

	if(this->get_data_type()==int_data_type && lhs->get_data_type()==float_data_type)
		Compute_stmt = new Compute_IC_Stmt(mfc1,lhs_opd,NULL,res_opd);
	else if(this->get_data_type()==float_data_type && lhs->get_data_type()==int_data_type)
		Compute_stmt = new Compute_IC_Stmt(mtc1,lhs_opd,NULL,res_opd);
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if(Compute_stmt)
		ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
		
	if (ic_list.empty() == false)
	{	
		if(Compute_stmt)
			assign_stmt = new Code_For_Ast(ic_list, result_register);
		else
			assign_stmt = new Code_For_Ast(ic_list, lhs_register);
	}

	lhs_register->used_for_expr_result=false;
	return *assign_stmt;
}

Code_For_Ast & Typecast_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	lra.optimize_lra( mc_2r , NULL , lhs );
	Code_For_Ast & lhs_stmt = lhs->compile_and_optimize_ast(lra);
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	// lhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if(node_data_type == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);
	
	Icode_Stmt * Compute_stmt=NULL;

	if(this->get_data_type()==int_data_type && lhs->get_data_type()==float_data_type)
		Compute_stmt = new Compute_IC_Stmt(mfc1,lhs_opd,NULL,res_opd);
	else if(this->get_data_type()==float_data_type && lhs->get_data_type()==int_data_type)
		Compute_stmt = new Compute_IC_Stmt(mtc1,lhs_opd,NULL,res_opd);
	
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if(Compute_stmt)
		ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
		
		if(Compute_stmt)
			assign_stmt = new Code_For_Ast(ic_list, result_register);
		else
			assign_stmt = new Code_For_Ast(ic_list, lhs_register);

	return *assign_stmt;
}
////////////////////////////////////////////////////////////////////////////////////////////

Unary_Expr_Ast::Unary_Expr_Ast(Ast * temp_lhs, int line)
{
	lhs = temp_lhs;
	ast_num_child = unary_arity;
	lineno = line;
}

Unary_Expr_Ast::~Unary_Expr_Ast()
{
	delete lhs;
}

Data_Type Unary_Expr_Ast::get_data_type()
{
	return lhs->get_data_type();
}

void Unary_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_NODE_SPACE<<"Arith: UMINUS" <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Unary_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);

	Eval_Result & int_result = * new Eval_Result_Value_Int();
	Eval_Result & float_result = * new Eval_Result_Value_Float();

	if(lhs->get_data_type() == int_data_type){	
		int_result.set_value(-1 * result_lhs.get_value().i);
		return int_result;
	}

	else if(lhs->get_data_type() == float_data_type)
	{
		float_result.set_value( -1 * result_lhs.get_value().f );
		return float_result;
	}
}

Code_For_Ast & Unary_Expr_Ast::compile()
{

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if( this->get_data_type() == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);
	
	Icode_Stmt * Compute_stmt = new Compute_IC_Stmt(uminus,lhs_opd,NULL,res_opd);		

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	return *assign_stmt;
	
}

Code_For_Ast & Unary_Expr_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	lra.optimize_lra( mc_2r , NULL , lhs );
	Code_For_Ast & lhs_stmt = lhs->compile_and_optimize_ast(lra);
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->used_for_expr_result=true;

	Register_Descriptor * result_register = NULL;

	if( this->get_data_type() == int_data_type)
		result_register = machine_dscr_object.get_new_register();
	else 
		result_register = machine_dscr_object.get_new_float_register();

	Ics_Opd *  lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd *  res_opd = new Register_Addr_Opd(result_register);
	
	Icode_Stmt * Compute_stmt = new Compute_IC_Stmt(uminus,lhs_opd,NULL,res_opd);		

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	ic_list.push_back(Compute_stmt);

	Code_For_Ast * assign_stmt;
	assign_stmt = new Code_For_Ast(ic_list, result_register);

	lhs_register->used_for_expr_result=false;
	return *assign_stmt;
}


////////////////////////////////////////////////////////////////////////////////////////////

Fn_Call_Ast::Fn_Call_Ast(Procedure * temp_proc, list<Ast *> * ast_list,int line)
{
	ast_num_child = unary_arity;
	lineno = line;
	proc = temp_proc;
	if(ast_list==NULL) var_list = new list<Ast *>;
	else var_list  = ast_list;
}

Fn_Call_Ast::~Fn_Call_Ast()
{
	delete proc;
	delete var_list;
}

Data_Type Fn_Call_Ast::get_data_type()
{
	return proc->get_return_type();
}

void Fn_Call_Ast::print(ostream & file_buffer)
{

	file_buffer << "\n" << AST_SPACE << "FN CALL: ";
	file_buffer << proc->get_proc_name() <<"(";
	
	if(var_list!=NULL){
		list<Ast *>::iterator i;
		for (i = var_list->begin(); i != var_list->end(); i++)
		{
			file_buffer <<"\n";
			file_buffer << AST_NODE_SPACE;
			(*i)->print(file_buffer);
		}
	}

	file_buffer << ")";
}

Eval_Result & Fn_Call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	// cout<<"Size of var_list : "<<(var_list->get_symbol_table())->size()<<"  Size of params list : "<<((proc->get_params_list())->get_symbol_table()).size()<<endl;

	list<Eval_Result_Value *> eval_result_list;

	list<Ast *>::iterator j;
   	for ( j = var_list->begin(); j != var_list->end();j++)
   	{		

   		Eval_Result & r = ((*j)->evaluate(eval_env, file_buffer));

   		if( !r.is_variable_defined() )
		{
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Variable should be defined before its use");
		}

		if(r.get_result_enum()==int_result)
		{
			Eval_Result_Value_Int * j = new Eval_Result_Value_Int();
			j->set_value( (r.get_value()).i);
			eval_result_list.push_back(j);
		}
		else if(r.get_result_enum()==float_result)
		{
			Eval_Result_Value_Float * j = new Eval_Result_Value_Float();
			j->set_value( (r.get_value()).f);			
			eval_result_list.push_back(j);
		}
   	}

	Eval_Result & result = proc->evaluate(file_buffer,eval_result_list);
	proc->return_check=0;
	return result;
}

Code_For_Ast & Fn_Call_Ast::compile()
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	int movestack = proc->get_params_list().get_size() * -1;
	Ics_Opd * size_opd = new Const_Opd<int>(movestack);

	Ics_Opd *  sp_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[sp]);	

	list<Ast *>::reverse_iterator i;
	list<Symbol_Table_Entry *>::reverse_iterator j;
	
	if(var_list){
		int size=0 ;
		for(i=var_list->rbegin() , j = proc->get_params_list().get_symbol_table().rbegin() ; i!=var_list->rend() ;  i++,j++)
		{	
			string temp = (*j)->get_variable_name();

			Symbol_Table_Entry * s = new Symbol_Table_Entry(temp,(*j)->get_data_type(),lineno);
			(*s).set_start_offset(size);
			int sp = proc->get_params_list().get_size_of_value_type((*j)->get_data_type());
			size+=sp;
			(*s).set_end_offset(size);
			(*s).set_symbol_scope(param);

			Ast * lhs = new Name_Ast( temp , *s , lineno );

			Code_For_Ast & load_stmt = (*i)->compile();

			Register_Descriptor * load_register = load_stmt.get_reg();

			if(load_register == NULL)cout<<"sdlklsdfkj----------sdfsdkjjsdf----------2"<<endl;
			Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);
			if (load_stmt.get_icode_list().empty() == false)
				ic_list.splice(ic_list.end(), load_stmt.get_icode_list());

			if (store_stmt.get_icode_list().empty() == false)
				ic_list.splice(ic_list.end(), store_stmt.get_icode_list());
		}
	}
	if(movestack != 0){
		Icode_Stmt * sp_sub = new Compute_IC_Stmt(sub,sp_opd,size_opd,sp_opd);
		ic_list.push_back(sp_sub);
	}
	Ics_Opd * ra_opd = new Const_Opd<string>(proc->get_proc_name());
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(call,NULL,NULL,ra_opd);
	ic_list.push_back(goto_stmt);

	if(movestack!=0){
		Icode_Stmt * sp_add = new Compute_IC_Stmt(add,sp_opd,size_opd,sp_opd);
		ic_list.push_back(sp_add);
	}
	Register_Descriptor * r;
	if(proc->get_return_type()==int_data_type){
		r =  machine_dscr_object.get_new_register();
		Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(r);
		Ics_Opd *  v_one_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[v1]);
		Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
		ic_list.push_back(m_stmt);

	}
	else if(proc->get_return_type()==float_data_type){
		r = machine_dscr_object.get_new_float_register();
		Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(r);
		Ics_Opd *  v_one_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[f0]);
		Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
		ic_list.push_back(m_stmt);
	}


	
	Code_For_Ast & ret_code = *new Code_For_Ast(ic_list, r);

	return ret_code;	
}

Code_For_Ast & Fn_Call_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	int movestack = proc->get_params_list().get_size() * -1;
	Ics_Opd * size_opd = new Const_Opd<int>(movestack);

	Ics_Opd *  sp_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[sp]);	

	list<Ast *>::reverse_iterator i;
	list<Symbol_Table_Entry *>::reverse_iterator j;
	
	if(var_list){
		int size=0 ;
		for(i=var_list->rbegin() , j = proc->get_params_list().get_symbol_table().rbegin() ; i!=var_list->rend() ;  i++,j++)
		{	
			string temp = (*j)->get_variable_name();

			Symbol_Table_Entry * s = new Symbol_Table_Entry(temp,(*j)->get_data_type(),lineno);
			(*s).set_start_offset(size);
			int sp = proc->get_params_list().get_size_of_value_type((*j)->get_data_type());
			size+=sp;
			(*s).set_end_offset(size);
			(*s).set_symbol_scope(param);

			Ast * lhs = new Name_Ast( temp , *s , lineno );

			lra.optimize_lra( mc_2r , NULL , (*i) );
			Code_For_Ast & load_stmt = (*i)->compile_and_optimize_ast(lra);

			Register_Descriptor * load_register = load_stmt.get_reg();

			Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);
			if (load_stmt.get_icode_list().empty() == false)
				ic_list.splice(ic_list.end(), load_stmt.get_icode_list());

			if (store_stmt.get_icode_list().empty() == false)
				ic_list.splice(ic_list.end(), store_stmt.get_icode_list());
		}
	}
	if(movestack != 0){
		Icode_Stmt * sp_sub = new Compute_IC_Stmt(sub,sp_opd,size_opd,sp_opd);
		ic_list.push_back(sp_sub);
	}
	Ics_Opd * ra_opd = new Const_Opd<string>(proc->get_proc_name());
	Icode_Stmt * goto_stmt = new Control_Flow_IC_Stmt(call,NULL,NULL,ra_opd);
	ic_list.push_back(goto_stmt);

	machine_dscr_object.clear_local_register_mappings();

	if(movestack!=0){
		Icode_Stmt * sp_add = new Compute_IC_Stmt(add,sp_opd,size_opd,sp_opd);
		ic_list.push_back(sp_add);
	}
	Register_Descriptor * r;
	if(proc->get_return_type()==int_data_type){
		r =  machine_dscr_object.get_new_register();
		Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(r);
		Ics_Opd *  v_one_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[v1]);
		Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
		ic_list.push_back(m_stmt);

	}
	else if(proc->get_return_type()==float_data_type){
		r = machine_dscr_object.get_new_float_register();
		Ics_Opd *  v_zero_opd  = new Register_Addr_Opd(r);
		Ics_Opd *  v_one_opd  = new Register_Addr_Opd(machine_dscr_object.spim_register_table[f0]);
		Icode_Stmt * m_stmt = new Move_IC_Stmt(moveit,v_one_opd,v_zero_opd);
		ic_list.push_back(m_stmt);
	}	

	
	Code_For_Ast & ret_code = *new Code_For_Ast(ic_list, r);

	return ret_code;	
}