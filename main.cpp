#include "syn.h"
#include "fstream"

using namespace std;

void prt() {
	cout << server.my_id() << ' ';
	cout << server.players() << ' ';
	cout << server.supply() << ' ';
	cout << server.raw_price() << ' ';
	cout << server.demand() << ' ';
	cout << server.production_price() << ' ';
	cout << server.money(1) << ' ';
	cout << server.raw(1) << ' ';
	cout << server.production(4) << ' ';
	cout << server.factories(1) << endl;
}

int main(int argc, char** argv) {
	stack<PolizConst_ref> s;
	try {
		if (argc < 4)
			throw SynError("adress not given", 0);
		ifstream input(argv[1]);
		LexAnalyser L(input);
		L.run();
		SynAnalyser S(L.text, vars_global, marks_global);
		S.run();
		server = connection(atoi(argv[3]), argv[2]);
		for (auto i = S.poliz.begin(); i != S.poliz.end();) {
			(*i).run(i, s);
		}
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		while (!s.empty()) {
			PolizConst_ref op = s.top();
			s.pop();
			delete &op;
		}
	}
}
