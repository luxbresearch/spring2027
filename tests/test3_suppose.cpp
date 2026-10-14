#include "/workspaces/spring2027/margaret1/system.h"
#include <iostream>

using namespace std;
using namespace margaret1;

// Some example relations between them
Relation r("ex", "a", SEQ(P(b) * P(c)), {"b", "c"});

int main() {
    cout << r.suppose({{"b", 2}, {"c", 4}}) << endl;
}