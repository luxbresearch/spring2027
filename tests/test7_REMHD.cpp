#include "../margaret1/system.h"
#include "../margaret1/err.h"
#include <cmath>
#include <iostream> 

using namespace std;
using namespace margaret1;

// Test to make sure that the Relation::invertREMHD function works as intended.

Property a({"r"}), x({"x"}), y({"y"}), z({"z"});
Relation rel("rel", "r", SEQ(sqrt(pow(P(x), 2) + pow(P(y), 2) + pow(P(z), 2))), {"x", "y", "z"}); // z^2 = x^2 + y^2

int main() {
    // Given z=0, solve for x and y given r=1.
    //
    // Since the relation is r^2 = x^2 + y^2 + z^2, this should
    // result in pairs (x,y) on the unit circle.

    vector<pair<vector<double>, double>> results = rel.invertREMHD(
        1,
        {{"z", 0}},
        {{"x", -3, 3}, {"y", -3, 3}},
        0.01, 1e-3
    );

    for (pair<vector<double>, double> p: results) {
        margaretData("Matching vector: ", p.first);
    }
}