#include "poliz_types.h"

using namespace std;

class SynAnalyser;

PolizItem::PolizItem() {}
PolizItem::~PolizItem() {}

PolizItem_ref::PolizItem_ref(PolizItem& P): p(P) {};
void PolizItem_ref::operator delete(void *r) {
	delete &(((PolizItem_ref*)r)->p);
}
PolizItem& PolizItem_ref::get_ref() {
	return p;
}
void PolizItem_ref::run(POLIZ::iterator&i,std::stack<PolizConst_ref>&s) {
	p.run(i, s);
}

PolizIdent::PolizIdent(const std::string& n, ConstTypes t): name(n), type(t) {}
const std::string& PolizIdent::get_name() const {
	return name;
}
bool PolizIdent::operator==(const PolizIdent& p) const {
	return name == p.name;
}
bool PolizIdent::operator!=(const PolizIdent& p) const {
	return name != p.name;
}
bool PolizIdent::operator==(const std::string& s) const {
	return name == s;
}
bool PolizIdent::operator!=(const std::string& s) const {
	return name != s;
}
ConstTypes PolizIdent::get_type() const {
	return type;
}
void PolizIdent::set_type(ConstTypes t) {
	type = t;
};

IdentMark::IdentMark(const std::string& n, POLIZ::iterator v):
	PolizIdent(n, MRK), value(v) {}
IdentMark::IdentMark(const std::string& n): PolizIdent(n, MRK) {}
POLIZ::iterator IdentMark::get_i() const {
	return value;
}
void IdentMark::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstMark(*this)));
	++i;
}

IdentNumber::IdentNumber(const std::string& n, ConstTypes t, int v):
	IdentNumber::PolizIdent(n, t), value(v) {}
void IdentNumber::set(const ConstNum& c) {
	value = c;
}
void IdentNumber::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstNum(*this)));
	++i;
}
IdentNumber::operator int() const {
	return value;
}

PolizAction::PolizAction() {};

PolizFunc::PolizFunc(const std::string& n):name(n), res(n), argc(0) {}
bool PolizFunc::check(std::stack<ConstTypes>& s) const {
	auto i = par.begin();
	for (int j = 0; j < argc; ++i, ++j) {
	/* in stack order is reversed */
		if ((*i).get_type() != s.top())
			return false;
		s.pop();
	}
	s.push(res.get_type());
	return true;
}
int PolizFunc::par_num() const {
	return argc;
}
bool PolizFunc::operator==(const PolizFunc& p) const {
	return name == p.name;
}
bool PolizFunc::operator!=(const PolizFunc& p) const {
	return name != p.name;
}
bool PolizFunc::operator==(const std::string& s) const {
	return name == s;
}
bool PolizFunc::operator!=(const std::string& s) const {
	return name != s;
}

PolizFunc_ref::PolizFunc_ref(PolizFunc& P): p(P) {};
void PolizFunc_ref::operator delete(void *r) {
	delete &(((PolizFunc_ref*)r)->p);
}
int PolizFunc_ref::par_num() const {
	return p.par_num();
}
bool PolizFunc_ref::check(std::stack<ConstTypes>& s) {
	return p.check(s);
}
bool PolizFunc_ref::operator==(const PolizFunc_ref& a) {
	return p == a.p;
}
bool PolizFunc_ref::operator==(const PolizFunc& a) {
	return p == a;
}
bool PolizFunc_ref::operator==(const std::string& a) {
	return p == a;
}
PolizFunc& PolizFunc_ref::get_ref() const {
	return p;
}
void PolizFunc_ref::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) { 
	p.run(i, s);
}

FuncStd::FuncStd(const std::string& n): PolizFunc(n) {}
void FuncStd::add_par(const IdentNumber& n) {
	par.push_back(n);
	argc++;
}
void FuncStd::set_res_type(ConstTypes t) {
	res.set_type(t);
}
void FuncStd::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	for (int j = argc - 1; j >= 0; j--) {
		PolizConst_ref op = s.top();
		s.pop();
		ConstNum& p = dynamic_cast<ConstNum&>(op.get_ref());
		par[j].set(p);
		delete &op;
	}
	if (name == "my_id")
		res.set(server.my_id());
	else if (name == "players")
		res.set(server.players());
	else if (name == "supply")
		res.set(server.supply());
	else if (name == "raw_price")
		res.set(server.raw_price());
	else if (name == "demand")
		res.set(server.demand());
	else if (name == "production_price")
		res.set(server.production_price());
	else if (name == "money")
		res.set(server.money(par[0]));
	else if (name == "raw")
		res.set(server.raw(par[0]));
	else if (name == "production")
		res.set(server.production(par[0]));
	else if (name == "factories")
		res.set(server.factories(par[0]));
	s.push(*(new ConstNum(res)));
	++i;
}

PolizOp::PolizOp(const std::string& n) {
	if (n == "+")
		type = PLUS;
	else if (n == "-")
		type = MIN;
	else if (n == "*")
		type = MUL;
	else if (n == "/")
		type = DIV;
	else if (n == "%")
		type = MOD;
	else if (n == "<")
		type = LESS;
	else if (n == "<=")
		type = LESSEQ;
	else if (n == ">")
		type = MORE;
	else if (n == ">=")
		type = MOREEQ;
	else if (n == "==")
		type = EQ;
	else if (n == "!=")
		type = NEQ;
	else if (n == "!")
		type = NOT;
	else if (n == "J")
		type = JMP;
	else if (n == "JF")
		type = JMPF;
	else if (n == ";")
		type = CLR;
	else if (n == "=")
		type = SV;
	else {
		throw SynError("unexpected lexeme", 0);
	}
}
void PolizOp::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	switch (type) {
		case PLUS: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			int res = one + two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case MIN: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			int res = one - two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case MUL: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			int res = one * two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case DIV: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			int res = one / two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case MOD: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			int res = one % two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case LESS: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = one < two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case LESSEQ: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = one <= two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case MORE: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = one > two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case MOREEQ: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = one >= two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case EQ: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = (one == two);
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case NEQ: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			bool res = one != two;
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op1;
			delete &op2;
			break;
		}
		case NOT: {
			PolizConst_ref op = s.top();
			s.pop();
			ConstNum& one = dynamic_cast<ConstNum&>(op.get_ref());
			bool res = !bool(one);
			ConstNum& Res = *(new ConstNum(res));
			s.push(Res);
			delete &op;
			break;
		}
		case JMP: {
			PolizConst_ref op = s.top();
			s.pop();
			ConstMark& one = dynamic_cast<ConstMark&>(op.get_ref());
			i = one;
			delete &op;
			return;
			break;
		}
		case JMPF: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstMark& two = dynamic_cast<ConstMark&>(op2.get_ref());
			ConstNum& one = dynamic_cast<ConstNum&>(op1.get_ref());
			if (bool(one) == false)
				i = two;
			else
				++i;
			delete &op1;
			delete &op2;
			return;
			break;
		}
		case CLR: {
			while (!s.empty()) {
				PolizConst_ref op = s.top();
				s.pop();
				delete &op;
			}
			break;
		}
		case SV: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstRef& one = dynamic_cast<ConstRef&>(op1.get_ref());
			ConstNum& two = dynamic_cast<ConstNum&>(op2.get_ref());
			(one.get_ref()).set(two);
			delete &op1;
			delete &op2;
			break;
		}
	}
	++i;
}

PolizKey::PolizKey(const std::string& n) {	
	if (n == "print")
		type = PRNT;
	else if (n == "buy")
		type = BUY;
	else if (n == "sell")
		type = SELL;
	else if (n == "prod")
		type = PROD;
	else if (n == "build")
		type = BLD;
	else if (n == "endturn")
		type = ENDT;
	else {
		throw SynError("unexpected lexeme", 0);
	}
}
void PolizKey::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	switch (type) {
		case PRNT: {
			PolizConst_ref op = s.top();
			s.pop();
			ConstStr& str = dynamic_cast<ConstStr&>(op.get_ref());
			cout << str;
			delete &op;
			break;
		}
		case BUY: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& num = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& price = dynamic_cast<ConstNum&>(op2.get_ref());
			try {
				server.buy(num, price);
			}
			catch (RunError& e) {
				delete &op1;
				delete &op2;
				throw e;
			}
			delete &op1;
			delete &op2;
			break;
		}
		case SELL: {
			PolizConst_ref op2 = s.top();
			s.pop();
			PolizConst_ref op1 = s.top();
			s.pop();
			ConstNum& num = dynamic_cast<ConstNum&>(op1.get_ref());
			ConstNum& price = dynamic_cast<ConstNum&>(op2.get_ref());
			try {
				server.sell(num, price);
			}
			catch (RunError& e) {
				delete &op1;
				delete &op2;
				throw e;
			}
			delete &op1;
			delete &op2;
			break;
		}
		case PROD: {
			PolizConst_ref op = s.top();
			s.pop();
			ConstNum& num = dynamic_cast<ConstNum&>(op.get_ref());
			try {
				server.prod(num);
			}
			catch (RunError& e) {
				delete &op;
				throw e;
			}
			delete &op;
			break;
		}
		case BLD: {
			server.build();
			break;
		}
		case ENDT: {
			server.endturn();
			break;
		}
	}
	++i;
}

PolizConst::PolizConst(ConstTypes t): type(t) {}

PolizConst_ref::PolizConst_ref(PolizConst& P): p(P) {}
void PolizConst_ref::operator delete(void* r) {
	PolizConst* ref = &(((PolizConst_ref*) r)->p);
	ConstNum* ref1 = dynamic_cast<ConstNum*>(ref);
	if (ref1) {
		delete ref1;
		return;
	}
	ConstMark* ref2 = dynamic_cast<ConstMark*>(ref);
	if (ref2) {
		delete ref2;
		return;
	}
	ConstRef* ref3 = dynamic_cast<ConstRef*>(ref);
	if (ref3) {
		delete ref3;
		return;
	}
	ConstStr* ref4 = dynamic_cast<ConstStr*>(ref);
	if (ref4) {
		delete ref4;
		return;
	}
}
PolizConst& PolizConst_ref::get_ref() {
	return p;
}

ConstNum::ConstNum(const std::string& n, ConstTypes t): PolizConst(t) {
	value = std::atoi(n.c_str());
}
ConstNum::ConstNum(int v): PolizConst(INT), value(v) {}
ConstNum::ConstNum(const IdentNumber& v): PolizConst(v.get_type()), value(v) {}
ConstNum::ConstNum(bool v): PolizConst(BOOL) {
	if (v)
		value = 1;
	else
		value = 0;
}
void ConstNum::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstNum(*this)));
	++i;
}
ConstNum::operator int() const {
	return value;
}
ConstNum::operator bool() const {
	return value != 0;
}

ConstStr::ConstStr(const std::string& n): PolizConst(STR) {
	value = n;
	value.erase(value.begin());
	value.erase(value.end() - 1);
}
void ConstStr::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstStr(*this)));
	++i;
}
ostream& operator<<(std::ostream& out, const ConstStr& c) {
	out << c.value << endl;
	return out;
}

ConstMark::ConstMark(POLIZ::iterator v): value(v), PolizConst(MRK) {}
ConstMark::ConstMark(): PolizConst(MRK) {};
void ConstMark::set_v(POLIZ::iterator v) {
	value = v;
}
ConstMark::ConstMark(const IdentMark& m): value(m.get_i()), PolizConst(MRK) {}
void ConstMark::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstMark(*this)));
	++i;
}
ConstMark::operator POLIZ::iterator() const {
	POLIZ::iterator res = value;
	++res;
	return res;
}

ConstRef::ConstRef(IdentNumber& v): value(&v), PolizConst(REF) {}
ConstRef::ConstRef(): value(nullptr), PolizConst(REF) {}
IdentNumber& ConstRef::get_ref() {
	return *value;
}
void ConstRef::run(POLIZ::iterator& i, std::stack<PolizConst_ref>& s) {
	s.push(*(new ConstRef(*this)));
	++i;
}
