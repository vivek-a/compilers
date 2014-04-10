
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

#ifndef PROCEDURE_HH
#define PROCEDURE_HH

#include<string>
#include<map>
#include<list>

#define PROC_SPACE "   "
#define LOC_VAR_SPACE "      "

using namespace std;

class Procedure;

class Procedure
{
	Data_Type return_type;
	string name;
	Symbol_Table local_symbol_table;
	Symbol_Table params_list;
	list<Basic_Block *> basic_block_list;

	int lineno;
	vector<int> goto_list;

public:
	int return_check;
	Procedure(Data_Type proc_return_type, string proc_name, int line);
	~Procedure();

	void append_local_list(Symbol_Table & new_list);
	
	vector<int> get_goto_list();
	void add_to_goto_list(int num);

	Symbol_Table get_params_list();
	void set_params_list(Symbol_Table & params_list);

	string get_proc_name();
	void set_basic_block_list(list<Basic_Block *> & bb_list);
	void set_local_list(Symbol_Table & new_list);
	Data_Type get_return_type();
	Symbol_Table_Entry & get_symbol_table_entry(string variable_name);

	void print(ostream & file_buffer);

	Basic_Block * get_next_bb(Basic_Block & current_bb);
	Basic_Block & get_start_basic_block();

	Eval_Result & evaluate( ostream & file_buffer , list<Eval_Result_Value *> eval_result_list);

	bool variable_in_symbol_list_check(string variable);
	bool variable_in_param_list_check(string variable);

	// compile
	void compile();
	void print_icode(ostream & file_buffer);
	void print_assembly(ostream & file_buffer);

private:
	void print_prologue(ostream & file_buffer);
	void print_epilogue(ostream & file_buffer);
};

#endif
