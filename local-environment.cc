
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors (theory and lab)  at IIT
           Bombay.

           Release  date Jan  15, 2013.  Copyrights reserved  by Uday
           Khedker. This implemenation has been made available purely
           for academic purposes without any warranty of any kind.

           Documentation  (functionality,   manual, and  design)  and
           related tools are at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include<string>
#include<fstream>
#include<iomanip>
using namespace std;

#include"local-environment.hh"
#include"error-display.hh"
#include"user-options.hh"

double Eval_Result::get_value()
{
	report_internal_error("Should not reach, Eval_Result : get_value");
}

// void Eval_Result::set_value(int number)
// {
// 	report_internal_error("Should not reach, Eval_Result : set_value");
// }

void Eval_Result::set_value(double number)
{
	report_internal_error("Should not reach, Eval_Result : set_value");
}


bool Eval_Result::is_variable_defined()
{
	report_internal_error("Should not reach, Eval_Result : is_variable_defined");
}

void Eval_Result::set_variable_status(bool def)
{
	report_internal_error("Should not reach, Eval_Result : set_variable_status");
}
///////////////////////////////////////////////////////////////////////////////

// void Eval_Result_Value::set_value(int number)
// {
// 	report_internal_error("Should not reach, Eval_Result : set_value");
// }

void Eval_Result_Value::set_value(double number)
{
	//cout<<"akajshdkj  :: "<<number<<endl;
	report_internal_error("Should not reach, Eval_Result : set_value");
}

double Eval_Result_Value::get_value()
{
	report_internal_error("Should not reach, Eval_Result : get_value");
}

///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Int::Eval_Result_Value_Int()
{
	value = 0;
	defined = false;
	result_type = int_result;
}

Eval_Result_Value_Int::~Eval_Result_Value_Int()
{ }

void Eval_Result_Value_Int::set_value(double number)
{
	value = number;
	defined = true;
}

double Eval_Result_Value_Int::get_value()
{
	return value;
}

void Eval_Result_Value_Int::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Int::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Int::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Int::get_result_enum()
{
	return result_type;
}

///////////////////////////////////////////////////////////////////////////////

Eval_Result_Value_Float::Eval_Result_Value_Float()
{
	value = 0;
	defined = false;
	result_type = float_result;
}

Eval_Result_Value_Float::~Eval_Result_Value_Float()
{ }

void Eval_Result_Value_Float::set_value(double number)
{
	value = number;
	defined = true;
}

double Eval_Result_Value_Float::get_value()
{
	return value;
}

void Eval_Result_Value_Float::set_variable_status(bool def)
{
	defined = def;
}

bool Eval_Result_Value_Float::is_variable_defined()
{
	return defined;
}

void Eval_Result_Value_Float::set_result_enum(Result_Enum res)
{
	result_type = res;
}

Result_Enum Eval_Result_Value_Float::get_result_enum()
{
	return result_type;
}

///////////////////////////////////////////////////////////////////////////////////

Local_Environment::Local_Environment()
{}

Local_Environment::~Local_Environment()
{}

void Local_Environment::print(ostream & file_buffer)
{
	map<string, Eval_Result_Value *>::iterator i;
	for (i = variable_table.begin(); i != variable_table.end(); i++)
	{
		Eval_Result_Value * vi = variable_table[(*i).first];
	
		// cout<<"\n :::::::::::    ::::::  " << (*i).first << vi->get_result_enum() <<endl;

		if (vi != NULL)
		{
			file_buffer<<endl;
			
			if (vi->is_variable_defined() == false)
			{				
				file_buffer << VAR_SPACE << (*i).first << " : undefined";
			}
		
			else
			{
				if(vi->get_result_enum()==float_result){
					file_buffer << VAR_SPACE << (*i).first << " : " <<fixed<<setprecision(2)<< vi->get_value();
				}
				else if(vi->get_result_enum()==int_result){
					file_buffer << VAR_SPACE << (*i).first << " : " <<fixed<<setprecision(0)<<vi->get_value();
				}
			}
		}
	}

}

bool Local_Environment::is_variable_defined(string name)
{
	Eval_Result_Value * i = variable_table[name];
	if (i != NULL)
		return i->is_variable_defined();
	else
		return false;
}

Eval_Result_Value * Local_Environment::get_variable_value(string name)
{
	Eval_Result_Value * i = variable_table[name];
	return i;
}

void Local_Environment::put_variable_value(Eval_Result_Value & i, string name)
{
	variable_table[name] = &i;
}

bool Local_Environment::does_variable_exist(string name)
{
	if (variable_table[name] == NULL)
		return false;
	else
		return true;
}
