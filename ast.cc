
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

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}


int Ast::get_number() {
	return basic_block_number;
}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

string Name_Ast::get_name()
{
	return variable_name;
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if(loc_var_val->get_result_enum()==float_result){
			file_buffer << fixed << setprecision(2) << loc_var_val->get_value() << "\n";
		}
		else if(loc_var_val->get_result_enum()==int_result){
			file_buffer << fixed << setprecision(0) << loc_var_val->get_value() << "\n";
		}
		else{
			report_internal_error("Result type can only be int and float");
		}
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << fixed << setprecision(0) << glob_var_val->get_value() << "\n";
		}
		else if (glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << fixed << setprecision(2) << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	
	if (result.get_result_enum() == int_result)
	{
		//cout<<"int me hun \n"<<result.get_value();
		Eval_Result_Value_Int * i = new Eval_Result_Value_Int();
	 	//i->set_value( (int) result.get_value());
		i->set_value( result.get_value());

	 	if (eval_env.does_variable_exist(variable_name))
			eval_env.put_variable_value(*i, variable_name);
		else
			interpreter_global_table.put_variable_value(*i, variable_name);
	}
	else if (result.get_result_enum() == float_result)
	{
		//cout<<"float me hun \n";
		Eval_Result_Value_Float * i = new Eval_Result_Value_Float();
	 	i->set_value(result.get_value());

	 	if (eval_env.does_variable_exist(variable_name))
			eval_env.put_variable_value(*i, variable_name);
		else
			interpreter_global_table.put_variable_value(*i, variable_name);
	}

}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
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
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	if(node_data_type == float_data_type){
		//cout<<"flaot hai ye ...... \n ";
		file_buffer << "Num : " << fixed << std::setprecision(2) << constant;
	}
	else{
		//cout<<"int hai ye.......\n ";
		file_buffer << "Num : " << fixed << std::setprecision(0) << constant;
	}
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
	else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(constant);

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(Ast * temp_lhs)
{
	lhs=temp_lhs;
}

Return_Ast::~Return_Ast()
{
	delete lhs;
}

void Return_Ast::print_ast(ostream & file_buffer)
{
	if(lhs==NULL){
		file_buffer<< "\n" << AST_SPACE << "RETURN <NOTHING>\n";	
	}else{
		file_buffer << "\n" << AST_SPACE << "RETURN ";
		lhs->print_ast(file_buffer);
		file_buffer<<"\n";
	}
	
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	
	print_ast(file_buffer);
	Eval_Result & result = *new Eval_Result_Value_Int();
	result.set_value(-1);
	return result;
}

template class Number_Ast<int>;
template class Number_Ast<float>;

////////////////////////////////////////////////////////////////////////////////

Goto_Ast::Goto_Ast(int num)
{
	basic_block_number=num;
}

Goto_Ast::~Goto_Ast()
{
	
}

int Goto_Ast::get_number(){
	return basic_block_number;
}

void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n" << AST_SPACE << "Goto statement:\n";
	file_buffer << AST_NODE_SPACE<< "Successor: " <<basic_block_number;
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	print_ast(file_buffer);
	file_buffer << AST_SPACE << "GOTO (BB "<<basic_block_number<<")\n";
	Eval_Result & result = * new Eval_Result_Value_Int();
	// result.set_value( (int) basic_block_number);
	result.set_value( basic_block_number);
	return result;
}
/////////////////////////////////////////////////////////////////////////////////

If_Else_Loop_Ast::If_Else_Loop_Ast(Ast * relational_ast_temp,Ast * if_ast_temp,Ast * else_ast_temp)
{
	relational_ast = relational_ast_temp;
	if_ast = if_ast_temp;
	else_ast = else_ast_temp;
}

If_Else_Loop_Ast::~If_Else_Loop_Ast()
{
	delete relational_ast;
	delete if_ast;
	delete else_ast;
}

void If_Else_Loop_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "If_Else statement:";	
	relational_ast->print_ast(file_buffer);	
	file_buffer <<"\n";
	file_buffer << AST_NODE_SPACE<<"True Successor: "<<if_ast->get_number()<<"\n";
	file_buffer << AST_NODE_SPACE<<"False Successor: "<<else_ast->get_number();
	
	
}

Eval_Result & If_Else_Loop_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	print_ast(file_buffer);

	Eval_Result & result = * new Eval_Result_Value_Int();	
	

	Eval_Result & eval_relation = relational_ast->evaluate(eval_env, file_buffer);

	if(eval_relation.get_value()){
		file_buffer << AST_SPACE << "Condition True : Goto (BB "<< if_ast->get_number() <<")\n";
		//if_ast->print_value(eval_env, file_buffer);
		result.set_value(if_ast->get_number());
		return result;
	}
	else{
		file_buffer<< AST_SPACE << "Condition False : Goto (BB "<< else_ast->get_number() <<")\n";
		//else_ast->print_value(eval_env, file_buffer);
		result.set_value(else_ast->get_number());
		return result;
	}
}

/////////////////////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{		
		node_data_type = lhs->get_data_type();
		return true;
	}
	

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n" << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE<<"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	//cout<<"assign eval 1\n";

	//rhs->print_ast(file_buffer);
	Eval_Result & final = *new Eval_Result_Value_Int();

	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	//cout<<"assign eval 2\n";

	if (result.is_variable_defined() == false){
		report_error("Variable should be defined to be on rhs", NOLINE);
	}

	//cout<<"assign eval 3\n";

	lhs->set_value_of_evaluation(eval_env, result);

	//cout<<"assign eval 4\n";
	// Print the result


	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);


	final.set_value(0);

	return final;
}
/////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	op= temp_op;
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

bool Relational_Expr_Ast::check_ast(int line)
{
	if(rhs==NULL)
	{
		node_data_type = lhs->get_data_type();
		return true;
	}
	//if (lhs->get_data_type() == rhs->get_data_type())
	
	node_data_type = lhs->get_data_type();
	return true;
	

	//report_error("Relational statement data type not compatible", line);
}

void Relational_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_NODE_SPACE<<"Condition: "<<op <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	if(rhs!=NULL)
	{
		file_buffer << AST_NODE_SPACE << "   RHS (";
		rhs->print_ast(file_buffer);
		file_buffer << ")";
	}
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & result_rhs = rhs->evaluate(eval_env, file_buffer);

	Eval_Result & result = * new Eval_Result_Value_Int();

	if(op == "LT")
	{
		if(result_lhs.get_value() < result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 
	else if(op == "GT") 
	{
		if(result_lhs.get_value() > result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 
	else if(op == "LE")
	{
		if(result_lhs.get_value() <= result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 
	else if(op == "GE")
	{
		if(result_lhs.get_value() >= result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 
	else if(op == "EQ")
	{
		if(result_lhs.get_value() == result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 
	else if(op == "NE")
	{
		if(result_lhs.get_value() != result_rhs.get_value())result.set_value(1);			
		else result.set_value(0);
	} 

	return result;
		
}

////////////////////////////////////////////////////////////////////////////////////////////

Arith_Expr_Ast::Arith_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs , string temp_op)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	op= temp_op;
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

bool Arith_Expr_Ast::check_ast(int line)
{
	
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic statement data type not compatible", line);
}

void Arith_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_NODE_SPACE<<"Arith: "<<op <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	if(rhs!=NULL)
	{
		file_buffer << AST_NODE_SPACE << "   RHS (";
		rhs->print_ast(file_buffer);
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
			int_result.set_value(  (result_lhs.get_value()+result_rhs.get_value()) ) ;
		} 
		else if(op == "MINUS") 
		{
			int_result.set_value(  (result_lhs.get_value()-result_rhs.get_value()) ) ;
		} 
		else if(op == "MULT")
		{
			int_result.set_value(  (result_lhs.get_value() * result_rhs.get_value()) ) ;
		}
		else if(op == "DIV")
		{
			int_result.set_value(  (result_lhs.get_value() / result_rhs.get_value()) ) ;
		} 
		return int_result;
	}
	else
	{
		if(op == "PLUS")
		{
			float_result.set_value( result_lhs.get_value()+result_rhs.get_value() ) ;
		} 
		else if(op == "MINUS") 
		{
			float_result.set_value( result_lhs.get_value()-result_rhs.get_value() ) ;
		} 
		else if(op == "MULT")
		{
			float_result.set_value( result_lhs.get_value() * result_rhs.get_value() ) ;
		}
		else if(op == "DIV")
		{
			float_result.set_value( result_lhs.get_value() / result_rhs.get_value() ) ;
		} 
		return float_result;
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////

Typecast_Expr_Ast::Typecast_Expr_Ast(Ast * temp_lhs , string temp_op)
{
	lhs = temp_lhs;
	type=temp_op;
}

Typecast_Expr_Ast::~Typecast_Expr_Ast()
{
	delete lhs;
}

Data_Type Typecast_Expr_Ast::get_data_type()
{
	if(type=="integer")return int_data_type;
	else return float_data_type;
}

void Typecast_Expr_Ast::print_ast(ostream & file_buffer)
{
	//file_buffer<< "\n" << AST_NODE_SPACE<<"Arith: "<<op <<"\n";

	//file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print_ast(file_buffer);
	//file_buffer << ")\n";

}

Eval_Result & Typecast_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);

	Eval_Result & int_result = * new Eval_Result_Value_Int();
	Eval_Result & float_result = * new Eval_Result_Value_Float();

	if(this->get_data_type() == int_data_type) 
	{
//		int_result.set_value( (int) result_lhs.get_value() );
		int_result.set_value( result_lhs.get_value() );
		return int_result;
	}

	else if(this->get_data_type() == float_data_type)
	{
		float_result.set_value( result_lhs.get_value() );
		return float_result;
	} 
	
		
}

////////////////////////////////////////////////////////////////////////////////////////////

Unary_Expr_Ast::Unary_Expr_Ast(Ast * temp_lhs)
{
	lhs = temp_lhs;
}

Unary_Expr_Ast::~Unary_Expr_Ast()
{
	delete lhs;
}

Data_Type Unary_Expr_Ast::get_data_type()
{
	return lhs->get_data_type();
}

void Unary_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<< "\n" << AST_NODE_SPACE<<"Arith: UMINUS" <<"\n";

	file_buffer << AST_NODE_SPACE << "   LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Unary_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);

	Eval_Result & int_result = * new Eval_Result_Value_Int();
	Eval_Result & float_result = * new Eval_Result_Value_Float();

	if(lhs->get_data_type() == int_data_type){		
		// int_result.set_value( (int) (-1 * result_lhs.get_value()) );
		int_result.set_value(-1 * result_lhs.get_value());
		return int_result;
	}

	else if(lhs->get_data_type() == float_data_type)
	{
		float_result.set_value( -1 * result_lhs.get_value() );
		return float_result;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////

Fn_Call_Ast::Fn_Call_Ast(Procedure * temp_proc, list<Ast *> * ast_list)
{
	proc = temp_proc;
	var_list  = ast_list;
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

void Fn_Call_Ast::print_ast(ostream & file_buffer)
{

	file_buffer << "\n" << AST_SPACE << "FN CALL: ";
	file_buffer << proc->get_proc_name() <<"(";
	
	if(var_list!=NULL){
		list<Ast *>::iterator i;
		for (i = var_list->begin(); i != var_list->end(); i++)
		{
			file_buffer <<"\n";
			file_buffer << AST_NODE_SPACE;
			(*i)->print_ast(file_buffer);
		}
	}

	// while(var_list->size()){
	// 	file_buffer <<"\n";
	// 	file_buffer << AST_NODE_SPACE;
	// 	(var_list->front())->print_ast(file_buffer);
	// 	var_list->pop_front();
	// }
	file_buffer << ")";
}

Eval_Result & Fn_Call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	// Eval_Result & result_lhs = lhs->evaluate(eval_env, file_buffer);

	// Eval_Result & int_result = * new Eval_Result_Value_Int();
	// Eval_Result & float_result = * new Eval_Result_Value_Float();

	// if(lhs->get_data_type() == int_data_type){		
	// 	// int_result.set_value( (int) (-1 * result_lhs.get_value()) );
	// 	int_result.set_value(-1 * result_lhs.get_value());
	// 	return int_result;
	// }

	// else if(lhs->get_data_type() == float_data_type)
	// {
	// 	float_result.set_value( -1 * result_lhs.get_value() );
	// 	return float_result;
	// }
}
