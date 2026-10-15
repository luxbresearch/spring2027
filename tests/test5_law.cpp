#include "/workspaces/spring2027/margaret1/system.h"
#include "/workspaces/spring2027/margaret1/law.h"
#include <iostream>

using namespace std;
using namespace margaret1;

Property a({"a"}), b({"b"}), c({"c"}), d({"d"});
Law law("l", {{"a", 1}, {"b", 1}, {"c", -1}, {"d", 2}});
vector<Relation> rels = law.deriveAllRelations();
System sys({a, b, c, d});

int main() {
    sys.setManual({"a", "b", "c"});
    sys.solveAll(rels);
    cout << sys.getVal("d") << endl;
}