#include "poliz_types.h"
#include <list>

extern std::list<IdentNumber> vars_global; /* global numeric variables */
extern std::list<IdentMark> marks_global; /* global marks */
extern std::list<PolizFunc_ref> functions; /* all defined functions */

class generate_tables {
	static int num;
public:
	generate_tables(); /* fills table with predefined functions */
	~generate_tables();
};
