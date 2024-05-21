#include <exception>
#include <string>


class LexError: public std::exception { /* wrong symbols */
	std::string s;
public:
	LexError(char c, int l) {
		s = "unexpected symbol '";
		s += c;
		s += "' in line ";
		s += std::to_string(l);
	}
	const char* what() const throw() {
		return s.c_str();
	}
};

class SynError: public std::exception { /* unexpected lexeme */
	std::string s;
public:
	SynError(const char* w, int l) {
		s = "syntax error";
		s += " in line ";
		s += std::to_string(l);
		s += ": ";
		s += w;
	}
	const char* what() const throw() {
		return s.c_str();
	}
};

class OpError: public std::exception { /* wrong operand */
	std::string s;
public:
	OpError(const char* w, int l) {
		s = "type error";
		s += " in line ";
		s += std::to_string(l);
		s += ": ";
		s += w;
	}
	const char* what() const throw() {
		return s.c_str();
	}
};

class RunError: public std::exception { /* runtime error */
	std::string s;
public:
	RunError(const char* w) {
		s = "runtime error";
		s += ": ";
		s += w;
	}
	const char* what() const throw() {
		return s.c_str();
	}
};
