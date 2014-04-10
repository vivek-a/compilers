
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

#include<string>
#include<fstream>
#include<iostream>
#include<iomanip>           

using namespace std;

#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name)
{
	return_type = proc_return_type;
	name = proc_name;
	return_check=0;
}

void Procedure::check_with_arg_list(string var_name,int line)
{
	list<Symbol_Table_Entry *>::iterator i;
	list<Symbol_Table_Entry *> v1 = (this->get_params_list()).get_symbol_table();
	for (i = v1.begin(); i != v1.end(); i++)
	{
		if( (*i)->get_variable_name() == var_name)
			report_error(" Formal parameter and local variable name cannot be same",line);
	}	
	return;
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

Symbol_Table Procedure::get_params_list()
{
	return this->params_list;
}

void Procedure::set_params_list(Symbol_Table & params_list)
{
	this->params_list = params_list;
	local_symbol_table.set_table_scope(local);
}

vector<int> Procedure::get_goto_list()
{
	return goto_list;
}

void Procedure::add_to_goto_list(int num){
	(this->goto_list).push_back(num);
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}

void Procedure::set_local_list(Symbol_Table & new_list)
{
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);

}

void Procedure::append_local_list(Symbol_Table & new_list)
{
	list<Symbol_Table_Entry *> list = new_list.get_symbol_table();
	while (!list.empty())
	{
		local_symbol_table.push_symbol(list.front());
		list.pop_front();
	}
	local_symbol_table.set_table_scope(local);
}

Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

bool Procedure::variable_in_param_list_check(string variable)
{
	return params_list.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: "<< name << ", Return Type: ";
	if(return_type == void_data_type)file_buffer<< "VOID";
	else if(return_type == int_data_type)file_buffer<< "INT";
	else if(return_type == float_data_type)file_buffer<< "FLOAT";
	file_buffer<< "\n";

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_bb(file_buffer);
}
	
Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}

Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == current_bb.get_bb_number())
		{
			flag = true;
			continue;
		}
		if (flag)
			return (*i);
	}
	
	return NULL;
}

Eval_Result & Procedure::evaluate(ostream & file_buffer,list<Eval_Result_Value *> eval_result_list)
{

	
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);
	
	list<Eval_Result_Value *>::iterator j;

	list<Symbol_Table_Entry *>::iterator i;
	

	for(j = eval_result_list.begin() , i = (params_list.get_symbol_table()).begin(); i != params_list.get_symbol_table().end();j++)
	{				
		eval_env.put_variable_value( *(*j), (*i)->get_variable_name());
		i++;
	}

	Eval_Result * result = NULL;

	file_buffer<< PROC_SPACE << "Evaluating Procedure " << "<< "<<name<<" >>" << "\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):";
	eval_env.print(file_buffer);
	file_buffer<< endl;
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(return_check == 1)current_bb = NULL;
		else
		{
			if(result->get_value() == 0)
				current_bb = get_next_bb(*current_bb);
			else{				
				for(list<Basic_Block *>::iterator i = basic_block_list.begin(); i != basic_block_list.end(); i++)
				{
					if( (*i)->get_bb_number() == result->get_value())
						current_bb = *i;		
				}
			}
		}	
	}

	// Eval_Result_Value put_result = *result;
	// put_result.set_value(result->get_value());

	file_buffer<<endl<< endl << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << "<<name<<" >>";	

	// cout<<"return : "<<this->get_return_type()<<endl;

	if( result->get_result_enum() == int_result && result->is_variable_defined() )
	{
		// cout<<endl<<LOC_VAR_SPACE<<"   return : "<< fixed <<setprecision(0)<<result->get_value()<<endl;
		Eval_Result_Value * put_result =  new Eval_Result_Value_Int();
		put_result->set_value(result->get_value());
		put_result->set_variable_status(result->is_variable_defined());
		eval_env.put_variable_value( *put_result , "return" );
	}
	else if(result->get_result_enum()  == float_result &&  result->is_variable_defined() )
	{
		// cout<<endl<<LOC_VAR_SPACE<<"   return : "<< fixed <<setprecision(2)<<result->get_value()<<endl;
		Eval_Result_Value * put_result =  new Eval_Result_Value_Float();
		put_result->set_value(result->get_value());
		put_result->set_variable_status(result->is_variable_defined());
		eval_env.put_variable_value( *put_result , "return" );
	}

	eval_env.print(file_buffer);

	if(this->get_proc_name() != "main")file_buffer<<endl;

	return *result;
}

