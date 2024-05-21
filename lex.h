#include <list>
#include <ostream>
#include <string>
#include "connection.h"

enum LexType {
	NUMBER, MARK, FUNCTION, KEY, IDENTIFIER, STRING, SEPARATOR,
	COMPARE, ADDITION, MULTIPLICATION, NOT
};

class Lexeme {
	std::string name; /* word as it was in file */
	LexType type;
	int line; /* line in origin document */
public:
	Lexeme(LexType, const std::string&, int); /* all 3 fields must be set */
	int get_line() const;
	const std::string& get_name() const;
	/* compare by name, type and whale data */
	bool operator==(const Lexeme&) const;
	bool operator!=(const Lexeme&) const;
	bool operator==(LexType) const;
	bool operator!=(LexType) const;
	bool operator==(const std::string&) const;
	bool operator!=(const std::string&) const;
	/* for debugging */
	friend std::ostream& operator<<(std::ostream&, const Lexeme&);
};

class LexAnalyser {
	char c; /* current analyzing symbol */
	std::string buffer; /* for accumulating lexeme */
	std::istream& input; /* source of symbols */
	int line;

	/*---- service functions ----*/
	bool getc(); /* reads new symbol, returns false if stream ends */
	void ungetc(); /* return symbol to input */
	bool sep(char); /* checks if symbol is separator */

	/*----- L-graph states -----*/
	void H(); /* starting state, skips spaces */
	void N(); /* integers */
	void I(); /* identifiers and service names */
	void S(); /* string constants */
	void C1(); /* compare and assignment operators */

public:
	std::list<Lexeme> text; /* resulting list of lexemes */
	LexAnalyser(std::istream&);
	bool run(); /* starts analysis, processes errors */
};
