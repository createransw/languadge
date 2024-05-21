#include "ident_tables.h"
#include <stack>

class SynAnalyser {
	std::list<Lexeme>::const_iterator c; /* for current lexeme */
    const std::list<Lexeme>& lst;
	bool BEG_FLG;
	std::stack<ConstTypes> sem_st;
	generate_tables g;
	std::list<IdentNumber>& vars;
	std::list<IdentMark>& marks;
	/*---- service functions ----*/
	bool getc(); /* read next lexeme */
	void ungetc(); /* return lexeme to stream */

	/*--- grammar conditions ---*/
	void S(); /* whole program */
	void S_f(); /* whole function */
	void L(); /* program line */
	void O(); /* operation */
	void P(); /* expression with value */
	void T(); /* compare operands */
	void M(); /* addition operands */
	void C(); /* multiplication operands */

public:
	POLIZ poliz;
	SynAnalyser(const std::list<Lexeme>&, std::list<IdentNumber>&,
			std::list<IdentMark>&);
	void set_c(std::list<Lexeme>::const_iterator);
	std::list<Lexeme>::const_iterator run_f();
	void run();
	~SynAnalyser();
};

class FuncCus: public PolizFunc { /* class for custom functions */
	SynAnalyser S;
	std::list<IdentNumber> par_l;
public:
	FuncCus(const std::string&, const std::list<Lexeme>&,
			std::list<Lexeme>::const_iterator&);
	void run(POLIZ::iterator&, std::stack<PolizConst_ref>&);
};
