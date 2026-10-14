#include "/workspaces/spring2027/margaret1/system.h"
#include <iostream>

using namespace std;
using namespace margaret1;

// A few example properties
Property a({"a"});
Property b({"b"});
Property c({"c"});
Property d({"d"});

// Some example relations between them
Relation r1("ex1", "a", SEQ(P(b) * P(c)), {"b", "c"});
Relation r2("ex2", "d", SEQ(P(b) * P(a)), {"b", "a"});

// A system incorporating all these things
System sys({a, b, c, d});

int main() {
    sys.setManual({"b", "c"});
    cout << "done, solving ..." << endl;
    sys.solveAll({r1, r2});
    cout << "done: " << endl;
    cout << sys.getVal("a") << " " << sys.getVal("d") << endl;
}