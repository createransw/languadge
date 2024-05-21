#include "syn.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool SynAnalyser::getc() {
	if (BEG_FLG) { /* first lexeme */
		BEG_FLG = false;
		if (c == lst.end()) /* list can be empty */
			return false;
		return true;
	}
	c++;
	if (c == lst.end()) /* end of list */
		return false;
	return true;
}

void SynAnalyser::ungetc() {
	if (c != lst.begin()) /* avoid going out of range */
		c--;
}

void SynAnalyser::S() {
	if (!getc()) { /* empty file is not a program */
		throw SynError("'end.' expected", 0);
	}
	while  (*c == FUNCTION) {
		auto i = find(functions.begin(), functions.end(), (*c).get_name());
		if (i != functions.end())
			throw OpError("redefinition of function", (*c).get_line());
		string tmp = (*c).get_name();
		if (!getc())
			throw SynError("'(' expected", 0);
		functions.push_back(*(new FuncCus(tmp, lst, c)));
	}
	if ((*c == MARK) && (*c == "begin")) { /* main body beginning mark */
		L();
		if (!getc()) /* end of file before end of program */
			throw SynError("'end.' expected", 0);
		if (*c != "end.") /* unexpected lexeme after a program end */
			throw SynError("'end.' expected", (*c).get_line());
		return;
	}
	throw SynError("'begin:' expected", (*c).get_line()); /* no main body */
}

void SynAnalyser::S_f() {
	if (!getc()) { /* empty file is not a program */
		throw SynError("'end.' expected", 0);
	}
	if (*c == "begin") { /* function body beginning */
		L();
		if (!getc()) /* end of file before end of program */
			throw SynError("'end.' expected", 0);
		if (*c != "end") /* unexpected lexeme after a program end */
			throw SynError("'end' expected", (*c).get_line());
		if (!getc()) /* end of file before end of program */
			throw SynError("';' expected", 0);
		if (*c != ";")
			throw SynError("';' expected", (*c).get_line());
		if (!getc()) /* end of file before end of program */
			throw SynError("'begin:' expected", 0);
		return;
	}
	throw SynError("'begin' expected", (*c).get_line()); /* no main body */
}

void SynAnalyser::L() {
	if (!getc())
		return;
	if ((*c == "end.") || (*c == "end")) { /* empty line */
		ungetc();
		return;
	}
	while (*c == MARK) { /* several marks can point same operator */
		IdentMark m((*c).get_name(), --poliz.end()); /* mark to next operator */
		if (find(marks.begin(), marks.end(), m) != marks.end())
			throw OpError("mark redefinition", (*c).get_line());
		marks.push_back(m); /* store mark in global table */
		if (!getc())
			return;
	}
	ungetc();
	O();
	if (!getc())
		return;
	if (*c == ";") {
		PolizOp& p = *(new PolizOp(";"));
		poliz.push_back(p);
		//poliz.push_back(*(new PolizOp(";")));
		L();
	} else
		ungetc();
}

void SynAnalyser::O() {
	if (!getc())
		return;

	/* branching operator */
	if (*c == "if") {
		P();
		if (sem_st.top() != BOOL) /* only boolian condition */
			throw OpError("logical sentence expected", (*c).get_line());
		sem_st.pop();
		ConstMark& ref = *(new ConstMark()); /* mark to end of operation */
		poliz.push_back(ref);
		poliz.push_back(*(new PolizOp("JF"))); /* conditional jump */
		if (!getc())
			return;
		if (*c != "then")
			throw SynError("'then' expected", (*c).get_line());
		O();
		ref = ConstMark(--poliz.end());

	/* jump to operator */
	} else if (*c == "goto") {
		if (!getc())
			return;
		if (*c != IDENTIFIER) /* requires mark */
			throw OpError("mark expected", (*c).get_line());
		auto i = find(marks.begin(), marks.end(), (*c).get_name()); /*check mark*/
		if (i == marks.end())
			throw OpError("undeclared mark", (*c).get_line());
		poliz.push_back(*(new ConstMark(*i)));
		poliz.push_back(*(new PolizOp("J"))); /* unconditional jump */

	/* print string */
	} else if (*c == "print") {
		if (!getc())
			return;
		if (*c != STRING) /* requires string */
			throw OpError("string expected", (*c).get_line());
		poliz.push_back(*(new ConstStr((*c).get_name())));
		poliz.push_back(*(new PolizKey("print")));

	/* buy key word */
	} else if (*c == "buy") {
		P();
		if (sem_st.top() != INT) /* check first parameter type */
			throw OpError("integer sentence expected", (*c).get_line());
		sem_st.pop();
		if (!getc())
			return;
		if (*c != ",") /* parameters separated by ',' */
			throw SynError("',' expected", (*c).get_line());
		P();
		if (sem_st.top() != INT) /* check second parameter type */
			throw OpError("integer sentence expected", (*c).get_line());
		sem_st.pop();
		poliz.push_back(*(new PolizKey("buy")));

	/* sell key word */
	} else if (*c == "sell") {
		P();
		if (sem_st.top() != INT) /* check first parameter type */
			throw OpError("integer sentence expected", (*c).get_line());
		sem_st.pop();
		if (!getc())
			return;
		if (*c != ",") /* parameters separated by ',' */
			throw SynError("',' expected", (*c).get_line());
		P();
		if (sem_st.top() != INT) /* check second parameter type */
			throw OpError("integer sentence expected", (*c).get_line());
		sem_st.pop();
		poliz.push_back(*(new PolizKey("sell")));

	/* prod key word */
	} else if (*c == "prod") {
		P();
		if (sem_st.top() != INT) /* check parameter type */
			throw OpError("integer sentence expected", (*c).get_line());
		sem_st.pop();
		poliz.push_back(*(new PolizKey("prod")));

	/* build key word */
	} else if (*c == "build") {
		poliz.push_back(*(new PolizKey("build")));

	/* endturn key word */
	} else if (*c == "endturn") {
		poliz.push_back(*(new PolizKey("endturn")));

	/* while key word */
	} else if (*c == "while") {
		/* mark to condition check */
		auto ref2_v = --poliz.end();
		P();
		if (sem_st.top() != BOOL) /* check condition type */
			throw OpError("logical sentence expected", (*c).get_line());
		sem_st.pop();
		ConstMark& ref1 = *(new ConstMark()); /* mark to end of loop */
		poliz.push_back(ref1);
		poliz.push_back(*(new PolizOp("JF"))); /* condition jump to end */
		if (!getc())
			return;
		if (*c != "do")
			throw SynError("'do' expected", (*c).get_line());
		O();
		poliz.push_back(*(new PolizOp(";")));
		poliz.push_back(*(new ConstMark(ref2_v)));
		/* unconditional jump to condition check */
		poliz.push_back(*(new PolizOp("J")));
		ref1 = ConstMark(--poliz.end());

	/* begin key word */
	} else if (*c == "begin") {
		L();
		if (!getc())
			return;
		if (*c != "end")
			throw SynError("'end' expected", (*c).get_line());

	/* rvalue identifier */
	} else if (*c == IDENTIFIER) {
		ConstRef& r = *(new ConstRef()); /* reference to changing variable */
		poliz.push_back(r);
		string tmp = (*c).get_name();
		if (!getc())
			return;
		if (*c != "=")
			throw SynError("'=' expected", (*c).get_line());
		P();
		auto i = find(vars.begin(), vars.end(), tmp); /* check variable */
		if (i == vars.end())
			vars.push_back(IdentNumber(tmp, sem_st.top())); /* add new variable */
		i = find(vars.begin(), vars.end(), tmp);
		if (sem_st.top() != (*i).get_type()) /* check types */
			throw OpError("wrong sentence type", (*c).get_line());
		sem_st.pop();
		r = ConstRef(*i);
		poliz.push_back(*(new PolizOp("=")));

	/* something unexpected */
	} else
		throw SynError("unexpected lexeme", (*c).get_line());
}

void SynAnalyser::P() {
	T();
	if (!getc())
		return;
	if ((*c != COMPARE) || ((*c).get_name() == "=")) { /* no more operators */
		ungetc();
		return;
	}
	if (sem_st.top() != INT) /* check operand type */
		throw OpError("integer expression expected", (*c).get_line());
	sem_st.pop();
	string tmp = (*c).get_name();
	T();
	if (sem_st.top() != INT) /* check operand type */
		throw OpError("integer expression expected", (*c).get_line());
	sem_st.pop();
	sem_st.push(BOOL); /* add returning type to stack */
	poliz.push_back(*(new PolizOp(tmp)));
}

void SynAnalyser::T() {
	M();
	if (!getc())
		return;
	string tmp;
	while (*c == ADDITION) { /* more operators */
		if (sem_st.top() != INT) /* check operand type */
			throw OpError("integer expression expected", (*c).get_line());
		sem_st.pop();
		tmp = (*c).get_name();
		M();
		poliz.push_back(*(new PolizOp(tmp)));
		sem_st.push(INT); /* add returning type */
		if (!getc())
			return;
		if (*c != ADDITION) { /* it was the last operand */
			if (sem_st.top() != INT)
				throw OpError("integer expression expected", (*c).get_line());
			sem_st.pop();
		}
	}
	ungetc();
}

void SynAnalyser::M() {
	C();
	if (!getc())
		return;
	string tmp;
	while (*c == MULTIPLICATION) { /* more operators */
		if (sem_st.top() != INT) /* check operand type */
			throw OpError("integer expression expected", (*c).get_line());
		sem_st.pop();
		tmp = (*c).get_name();
		C();
		poliz.push_back(*(new PolizOp(tmp)));
		sem_st.push(INT); /* add returning type */
		if (!getc())
			return;
		if (*c != ADDITION) { /* it was the last operand */
			if (sem_st.top() != INT)
				throw OpError("integer expression expected", (*c).get_line());
			sem_st.pop();
		}
	}
	ungetc();
}

void SynAnalyser::C() {
	if (!getc())
		return;

	/*identifier */
	if (*c == IDENTIFIER) {
		if ((*c == "true") || (*c == "false")) { /* can be logical constant */
			sem_st.push(BOOL);
			poliz.push_back(*(new ConstNum((*c).get_name(), BOOL)));
		} else {
			/* check identifier */
			auto i = find(vars.begin(), vars.end(), (*c).get_name());
			if (i == vars.end())
				throw OpError("undeclared variable", (*c).get_line());
			sem_st.push((*i).get_type());
			poliz.push_back(*i);
		}

	/* integer constant */
	} else if (*c == NUMBER) {
		sem_st.push(INT);
		poliz.push_back(*(new ConstNum((*c).get_name(), INT)));

	/* function */
	} else if (*c == FUNCTION) {
		/* check name */
		auto i = find(functions.begin(), functions.end(), (*c).get_name());
		if (i == functions.end())
			throw OpError("undeclared function", (*c).get_line());
		if (!getc())
			return;
		if (*c != "(")
			throw SynError("'(' expected", (*c).get_line());
		if ((*i).par_num() == 0) /* if 0 parameters move to ')' */
			if (!getc())
				return;
		for (int k = 0; k < (*i).par_num(); ++k) { /* get all parameters */
			P();
			if (!getc())
				return;
			if ((*c != ",") && (k != (*i).par_num() - 1))
				throw SynError("',' expected", (*c).get_line());
		}
		if (*c != ")")
			throw SynError("')' expected", (*c).get_line());
		if(!((*i).check(sem_st))) /* function checks types by itself */
			throw OpError("wrong parameter type", (*c).get_line());
		/* find out, is this function standard or custom */
		FuncStd* r = dynamic_cast<FuncStd*>(&((*i).get_ref()));
		if (r)
			poliz.push_back(*r);
		else {
			FuncCus* r =
				dynamic_cast<FuncCus*>(&((*i).get_ref()));
			//poliz.push_back(*(new FuncCus(*r)));
			poliz.push_back(*r);
		}

	/* sub expression */
	} else if (*c == "(") {
		P();
		if (!getc())
			return;
		if (*c != ")")
			throw SynError("')' expected", (*c).get_line());

	/* logical not operator */
	} else if (*c == "!") {
		C();
		if (sem_st.top() != BOOL)
			throw OpError("logical expression expected", (*c).get_line());
		sem_st.pop();
		sem_st.push(BOOL);
		poliz.push_back(*(new PolizOp("!")));

	/* something unexpected */
	} else
		throw SynError("unexpected lexeme", (*c).get_line());
}

SynAnalyser::SynAnalyser(const list<Lexeme>& l, list<IdentNumber>& n,
		list<IdentMark>& m): lst(l), vars(n), marks(m), BEG_FLG(true) {
		c = lst.begin();
}

void SynAnalyser::set_c(list<Lexeme>::const_iterator i) {
	c = i;
}

list<Lexeme>::const_iterator SynAnalyser::run_f() {
	S_f();
	return c;
}

void SynAnalyser:: run() {
	S();
}

SynAnalyser::~SynAnalyser() {
	const PolizIdent* r;
	const PolizFunc* t;
	for (auto i = poliz.begin(); i != poliz.end(); ++i) {
		/* leave global tables */
		r = dynamic_cast<const PolizIdent*>(&(*i).get_ref());
		if (!r) {
			t = dynamic_cast<const PolizFunc*>(&(*i).get_ref());
			if (!t)
				delete &(*i);
		}
	}
}

ostream& operator<<(ostream& out, const list<Lexeme>& l) {
	for (auto i = l.begin(); i != l.end(); ++i)
		out << (*i) << endl;
	return out;
}

/* ostream& operator<<(ostream& out, const POLIZ& l) {
	for (auto i = l.begin(); i != l.end(); ++i)
		out << (*i) << ' ';
	return out;
}*/

/*int main() {
	try {
		ifstream input("program");
		LexAnalyser L(input);
		if (L.run())
			cout << L.text << endl;
		SynAnalyser S(L.text, vars_global, marks_global);
		S.run();
		//cout << S.poliz << endl;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
	}
}*/

FuncCus::FuncCus(const string& n, const list<Lexeme>& l,
	list<Lexeme>::const_iterator& i): PolizFunc(n),
	S(l, par_l, marks_global) {
	if ((i == l.end()) || ((*i).get_name() != "("))
		throw SynError("( expected", (i == l.end()) ? 0: (*i).get_line());
	++i;
	if (i == l.end())
		throw SynError("unexpected end",  0);
	while ((*i).get_name() != ")") {
		ConstTypes t;
		if ((*i).get_name() == "bool")
			t = BOOL;
		else if ((*i).get_name() == "int")
			t = INT;
		else
			throw SynError("type specifier expected", (*i).get_line());
		++i;
		if (i == l.end())
			throw SynError("unexpected end",  0);
		if (*i != IDENTIFIER)
			throw SynError("identifier expected", (*i).get_line());
		par.push_back(IdentNumber((*i).get_name(), t));
		argc++;
		++i;
		if (i == l.end())
			throw SynError("unexpected end",  0);
		if (((*i).get_name() != ",") && ((*i).get_name() != ")"))
			throw SynError("',' expected", (*i).get_line());
		++i;
	}
	res = IdentNumber(n, INT);
	par.push_back(IdentNumber(n, INT));
	for (auto j = par.begin(); j != par.end(); ++j)
		par_l.push_back((*j));
	++i;
	S.set_c(i);
	i = S.run_f();
}

void FuncCus::run(POLIZ::iterator& i, stack<PolizConst_ref>& s) {
	for (int j = argc - 1; j >= 0; j--) {
		PolizConst_ref op = s.top();
		s.pop();
		ConstNum& p = dynamic_cast<ConstNum&>(op.get_ref());
		par[j].set(p);
		delete &op;
	}
	auto j = par_l.begin();
	for (int k = 0; k < argc; ++k) {
		(*j) = par[k];
	}
	stack<PolizConst_ref> st;
	try {
		for (auto t = S.poliz.begin(); t != S.poliz.end();)
			(*t).run(t, st);
	}
	catch (RunError &e) {
		while (st.empty()) {
			PolizConst_ref op = s.top();
			s.pop();
			delete &op;
		}
		throw e;
	}
	auto t = find(par_l.begin(), par_l.end(), name);
	res = *t;
	s.push(*(new ConstNum(res)));
	++i;
}
