#include "ident_tables.h"

using namespace std;

list<IdentNumber> vars_global;
list<IdentMark> marks_global;
list<PolizFunc_ref> functions;

int generate_tables::num = 0;

generate_tables::generate_tables() {
	if (!functions.empty())
		return;
	FuncStd* f = new FuncStd("my_id");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("players");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("supply");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("raw_price");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("demand");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("production_price");
	(*f).set_res_type(INT);
	functions.push_back(*f);

	f = new FuncStd("money");
	(*f).set_res_type(INT);
	(*f).add_par(IdentNumber("1", INT));
	functions.push_back(*f);

	f = new FuncStd("raw");
	(*f).set_res_type(INT);
	(*f).add_par(IdentNumber("1", INT));
	functions.push_back(*f);

	f = new FuncStd("production");
	(*f).set_res_type(INT);
	(*f).add_par(IdentNumber("1", INT));
	functions.push_back(*f);

	f = new FuncStd("factories");
	(*f).set_res_type(INT);
	(*f).add_par(IdentNumber("1", INT));
	functions.push_back(*f);

	vars_global.push_back(IdentNumber("ProdPrice", INT, 2000));
	vars_global.push_back(IdentNumber("BuildPrice", INT, 2500));
	vars_global.push_back(IdentNumber("KeepRawPrice", INT, 300));
	vars_global.push_back(IdentNumber("KeepProdPrice", INT, 500));
	vars_global.push_back(IdentNumber("PlantRent", INT, 1000));
}

generate_tables::~generate_tables() {
	if (num > 0) {
		return;
	}
	num++;
	for (auto i = functions.begin(); i != functions.end(); i++)
		delete &(*i);
}
