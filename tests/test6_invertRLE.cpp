#include "../margaret1/system.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace margaret1;

// Test to make sure that the Relation::invertRLE function works as intended.

Property a({"a"}), b({"b"}), c({"c"}), d({"d"});
Relation rel("r", "a", SEQ(P(b) - pow(P(c), 2) * P(d)), {"b", "c", "d"}); // a = b - d*c^2

int main() {
    // Given b=2 and d=-4 (so a = 2 + 4c^2), solve for c when a=3.
    //
    // 3 = 2 + 4c^2  ==>  c^2 = 1/4  ==>  c = +/- 0.5
    cout << rel.invertRLE("c", 3, {{"b", 2}, {"d", -4}}, -10, 10, 0.01).first << endl << endl;

    // Do the same, but use invertREM
    vector<pair<double, double>> results = rel.invertREM("c", 3, {{"b", 2}, {"d", -4}}, -10, 10, 0.01, 1e-6);
    for (pair<double, double> p: results) {
        cout << p.first << " " << p.second << endl;
    }
}