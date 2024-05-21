#include "lex.h"
#include <set>
#include <iostream>

using namespace std;

set<string> key_words {"if", "then", "goto", "print", "buy", "sell", "prod",
		"build", "endturn", "begin", "end", "end."}; /* all possible key words */

Lexeme::Lexeme(LexType t, const string& s, int i): type(t), name(s), line(i) {}

int Lexeme::get_line() const {
	return line;
}

const string& Lexeme::get_name() const {
	return name;
}

bool Lexeme::operator==(const Lexeme& l) const {
	if (name != l.name)
		return false;
	if (type != l.type)
		return false;
    return true; /* don't compare lines */
}

bool Lexeme::operator!=(const Lexeme& l) const {
	return !(*this == l);
}

bool Lexeme::operator==(LexType l) const {
	return type == l;
}

bool Lexeme::operator!=(LexType l) const {
	return type != l;
}

bool Lexeme::operator==(const string& s) const {
	return name == s;
}

bool Lexeme::operator!=(const string& s) const {
	return name != s;
}

ostream& operator<<(ostream& out, const Lexeme& l) {
	switch (l.type) {
		case (NUMBER): out << "NUMBER "; break;
		case (MARK): out << "MARK "; break;
		case (FUNCTION): out << "FUNCTION "; break;
		case (KEY): out << "KEY "; break;
		case (IDENTIFIER): out << "IDENTIFIER "; break;
		case (STRING): out << "STRING "; break;
		case (SEPARATOR): out << "SEPARATOR "; break;
		case (COMPARE): out << "COMPARE "; break;
		case (ADDITION): out << "ADDITION "; break;
		case (MULTIPLICATION): out << "MULTIPLICATION "; break;
		case (NOT): out << "NOT "; break;
	}
	out << l.name << ' ' << l.line;
	return out;
}

bool LexAnalyser::getc() {
	input.get(c);
	if (input.eof()) /* check for file end */
		return false;
	return true;
}

void LexAnalyser::ungetc() {
	input.putback(c); /* put symbol back to stream */
}

bool LexAnalyser::sep(char s) {
/* all symbols that may not be highlighted by spaces */
	if ((s == ' ') || (s == '\t') || (s == '\n'))
		return true;
	if ((s == '+') || (s == '-') || (s == '*') || (s == '/') || (s == '%'))
		return true;
	if ((s == '<') || (s == '>') || (s == '=') || (s == '!'))
		return true;
	if ((s == ';') || (s == ','))
		return true;
	if ((s == '(') || (s == ')'))
		return true;
	return false;
}

void LexAnalyser::H() {
	if (!buffer.empty()) /* other functions save lexemes by themself */
		buffer.clear();
	if (!getc()) /* check for file end */
		return;
	if ((c == ' ') || (c == '\t') || (c == '\n')) { /* skip spaces */
		if (c == '\n') /* new line in origin */
			line++;
		H();
		return;
	}

	buffer.push_back(c); /* beginning of new lexeme */
	if ((c >= '0') && (c <= '9')) /* beginning of integer */
		N();

	else if (((c >='a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
	/* beginning of identifier */
		I();

	else if (c == '"') /* beginning of string */
		S();

	/* all single operators and separators */
	else if ((c == '+') || (c == '-')) {
		text.push_back(Lexeme(ADDITION, buffer, line));
		H();
	} else if ((c == '*') || (c == '/') || (c == '%')) {
		text.push_back(Lexeme(MULTIPLICATION, buffer, line));
		H();
	} else if ((c == '(') || (c == ')') || (c == ';') || (c == ',')) {
		text.push_back(Lexeme(SEPARATOR, buffer, line));
		H();

	/* compare operators (may be double) */
	} else if ((c == '<') || (c == '>') || (c == '=') || (c == '!'))
		C1();

	else /* unknown symbol */
		throw LexError(c, line); /* unexpected symbol */
}

void LexAnalyser::N() {
	if (!getc()) /* check for file end */
		return;
	if ((c >= '0') && (c <= '9')) { /* integer continues */
		buffer.push_back(c);
		N();
	} else if (sep(c)) { /* end of integer */
		ungetc(); /* last symbol is the beginning of next lexeme */
		text.push_back(Lexeme(NUMBER, buffer, line)); /* save lexeme */
		H(); /* return to starting vertex */
	} else /* unexpected symbol */
		throw LexError(c, line);
}

void LexAnalyser::I() {
	if (!getc()) /* check for file end */
		return;
	if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) ||
				((c >= 'A') && (c >= 'Z'))) { /* identifier continues */
		buffer.push_back(c);
		I();
	} else if (c == ':') { /* mark */
		text.push_back(Lexeme(MARK, buffer, line));
		H();
	} else if (c == '(') { /* function */
        ungetc(); /* '(' is separate lexeme */
		text.push_back(Lexeme(FUNCTION, buffer, line));
		H();
	} else if (c == '.') { /* end. */
		if (buffer == "end")
			buffer.push_back(c);
		text.push_back(Lexeme(KEY, buffer, line));
		H();
	} else if (sep(c)) { /* end of identifier */
		ungetc(); /* last symbol is the beginning of next lexeme */
		if (key_words.find(buffer) != key_words.end()) /* check for key word */
			text.push_back(Lexeme(KEY, buffer, line));
		else
			text.push_back(Lexeme(IDENTIFIER, buffer, line)); /*usual identifier*/
		H();
	} else /* unexpected symbol */
		throw LexError(c, line);
}

void LexAnalyser::S() {
	if (!getc()) /* check for file end */
		return;
	if (c != '"') { /* string continues */
		buffer.push_back(c);
		S();
	} else {
		buffer.push_back(c);
		text.push_back(Lexeme(STRING, buffer, line));
		H();
	}
}

void LexAnalyser::C1() {
	char tmp = c;
	if (!getc()) /* check for file end */
		return;
	if (c != '=') { /* it's single operator */
		ungetc(); /* last symbol is the beginning of next lexeme */
		if (tmp == '!') {
		text.push_back(Lexeme(NOT, buffer, line));
		H();
		} else {
			text.push_back(Lexeme(COMPARE, buffer, line));
			H();
		}
	} else { /* it's double operator */
		buffer.push_back(c);
		text.push_back(Lexeme(COMPARE, buffer, line));
		H();
	}
}

LexAnalyser::LexAnalyser(istream& i): input(i) {
	line = 1;
}

bool LexAnalyser::run() {
	/* clean given containers before filling them */
	buffer.clear();
	text.clear();
	H();
	return true;
}
