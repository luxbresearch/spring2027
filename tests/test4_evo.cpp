#include "/workspaces/spring2027/margaret1/system.h" // include system++
#include <iostream>

using namespace std;
using namespace margaret1;

Property a({"a"}), b({"b"}), c({"c"}), d({"d"});

Relation r1("r1", "a", SEQ(P(b) * P(c)), {"b", "c"});
Relation r2("r2", "c", SEQ(P(d) * P(d)), {"d"});

TimeDerivRelation td("td", "d", SEQ(1), {}, 1); // d/dt (d) = 1
Evolution evo("evo", {td}, {r1, r2});

System sys({a, b, c, d});

int main() {
    sys.setManual({}); // default to all!
    cout << r1.relation(&sys) << endl;
    EvolutionRecord rec = sys.evolve(evo, 0.01, 1, {"a", "b", "c", "d"});
    for (double t = 0; t < 0.5; t += 0.005) {
        cout << "t=" << t << "\ta=" << rec.getVal("a", t) << "\tb=" << rec.getVal("b", t) << "\tc=" << rec.getVal("c", t) << "\td=" << rec.getVal("d", t) << endl;
    }
}