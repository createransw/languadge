#include <cstring>
#include <ostream>
#include <stack>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include "lex.h"

enum ConstTypes {
	INT, BOOL, STR, MRK, REF
};

class PolizItem_ref;
class PolizConst_ref;
class ConstNum;

typedef std::list<PolizItem_ref> POLIZ;

class PolizItem { /* abstract class to store elements */
/*protected:
	std::string name;*/
public:
	PolizItem();
	/*PolizItem(const std::string& n): name(n) {}
	friend std::ostream& operator<<(std::ostream& out, const PolizItem& p) {
		out << p.name;
		return out;
	}
	void set(const std::string& s) {
		name = s;
	}
	bool operator==(const PolizItem& i) const {
		return name == i.name;
	}*/
	virtual void run(POLIZ::iterator&, std::stack<PolizConst_ref>&) = 0;
	virtual ~PolizItem();
};

class PolizItem_ref { /* wrapper for containers */
	PolizItem& p;
public:
	PolizItem_ref(PolizItem&);
	/*friend std::ostream& operator<<(std::ostream& out, const PolizItem_ref& p) {
		out << p.p;
		return out;
	}*/
	void operator delete(void *);
	PolizItem& get_ref();
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class PolizIdent: public PolizItem { /* abstract class for variables */
	std::string name;
	ConstTypes type;
public:
	PolizIdent(const std::string&, ConstTypes);
	const std::string& get_name() const;
	bool operator==(const PolizIdent&) const;
	bool operator!=(const PolizIdent&) const;
	bool operator==(const std::string&) const;
	bool operator!=(const std::string&) const;
	ConstTypes get_type() const;
	void set_type(ConstTypes);
};

class IdentMark: public PolizIdent { /* class for marks */
	POLIZ::iterator value;
public:
	IdentMark(const std::string&, POLIZ::iterator);
	IdentMark(const std::string&);
	POLIZ::iterator get_i() const;
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class IdentNumber: public PolizIdent { /* class for numeric variables */
	int value;
public:
	IdentNumber(const std::string& n, ConstTypes t = BOOL, int v = 0);
	void set(const ConstNum&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
	operator int() const;
};

class PolizAction: public PolizItem { /* class for some activities */
public:
	PolizAction();
};

class PolizFunc: public PolizAction { /* class for functions */
protected:
	std::string name;
	std::vector<IdentNumber> par; /* parameter, and local variable list */
	IdentNumber res; /* returning value */
	int argc; /* number of parameters */
public:
	PolizFunc(const std::string&);
	bool check(std::stack<ConstTypes>&) const;
	int par_num() const;
	bool operator==(const PolizFunc&) const;
	bool operator!=(const PolizFunc&) const;
	bool operator==(const std::string&) const;
	bool operator!=(const std::string&) const;
};

class PolizFunc_ref { /* wrapper for containers */
	PolizFunc& p;
public:
	PolizFunc_ref(PolizFunc& P);
	void operator delete(void *);
	/*friend std::ostream& operator<<(std::ostream& out, const PolizFunc_ref& p) {
		out << p.p;
		return out;
	}*/
	int par_num() const;
	bool check(std::stack<ConstTypes>&);
	bool operator==(const PolizFunc_ref&);
	bool operator==(const PolizFunc&);
	bool operator==(const std::string&);
	PolizFunc& get_ref() const;
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class FuncStd: public PolizFunc { /* class for predefined functions */
public:
	FuncStd(const std::string&);
	void add_par(const IdentNumber&);
	void set_res_type(ConstTypes);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class PolizOp: public PolizAction { /* class for operators */
	enum OpTypes {
		PLUS, MIN, MUL, DIV, MOD, LESS, LESSEQ, MORE, MOREEQ, EQ, NEQ,
		NOT, JMP, JMPF, CLR, SV };
	OpTypes type;
public:
	PolizOp(const std::string&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class PolizKey: public PolizAction { /* class for key words */
	enum KeyTypes {
		PRNT, BUY, SELL, PROD, BLD, ENDT };
	KeyTypes type;
public:
	PolizKey(const std::string&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};

class PolizConst: public PolizItem { /* abstract class for constants */
	ConstTypes type;
public:
	PolizConst(ConstTypes t);
};

class PolizConst_ref { 
	PolizConst& p;
public:
	PolizConst_ref(PolizConst&);
	void operator delete(void*);
	PolizConst& get_ref();
};

class ConstNum: public PolizConst { /* class for numeric constants */
	int value;
public:
	ConstNum(const std::string&, ConstTypes);
	//ConstNum(const IdentNumber& n): PolizConst(n.get_name()),
		//type(n.get_type()) {}
	ConstNum(int);
	ConstNum(const IdentNumber&);
	explicit ConstNum(bool);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
	operator int() const;
	explicit operator bool() const;
};

class ConstStr: public PolizConst { /* class for string constants */
	std::string value;
public:
	ConstStr(const std::string&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
	friend std::ostream& operator<<(std::ostream&, const ConstStr&);
};

class ConstMark: public PolizConst { /* class for mark constants */
	POLIZ::iterator value;
public:
	ConstMark(POLIZ::iterator);
	ConstMark();
	void set_v(POLIZ::iterator);
	ConstMark(const IdentMark&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
	operator POLIZ::iterator() const;
};

class ConstRef: public PolizConst { /* class for reference constants */
	IdentNumber* value;
public:
	ConstRef(IdentNumber&);
	ConstRef();
	IdentNumber& get_ref();
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};
