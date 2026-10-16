#include "/workspaces/spring2027/margaret1/system.h"
#include "/workspaces/spring2027/margaret1/law.h"
#include <iostream>

using namespace std;
using namespace margaret1;

// In this example we define a law relating a, b, c, and d:
//
//      a * b * d^2 / c = 1.
//
// Test with a=1, b=2, c=3; we have
//
//      1 * 2 * d^2 / 3 = 1;
//
//      d^2 = 3/2;
//
//      d = sqrt(3/2) = 1.22474487139.

Property a({"a"}), b({"b"}), c({"c"}), d({"d"});
MultLaw law("l", {{"a", 1}, {"b", 1}, {"c", -1}, {"d", 2}});
vector<Relation> rels = law.deriveAllRelations();
System sys({a, b, c, d});

int main() {
    sys.setManual({"a", "b", "c"});
    sys.solveAll(rels);
    cout << sys.getVal("d") << endl;
}