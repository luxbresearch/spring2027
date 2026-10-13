#include "system.h"
#include "err.h"

#define SEQ(x) [](margaret1::System sys) { return x; }
#define P(x) sys.getVal(#x)

double margaret1::Property::getVal() {
    if (!known) {
        margaret1::margaretError("a margaret1 property", "Cannot get value of property (value is unknown): " + names[0], "Ensure property is solved first.");
    }
    return value;
}

void margaret1::Property::setVal(double newVal) {
    known = true;
    value = newVal;
}

void margaret1::Property::setUnknown() {
    known = false;
}

bool margaret1::Property::hasName(std::string name) {
    for (std::string possible: names) {
        if (name == possible) {
            return true;
        }
    }
    return false;
}

bool margaret1::Relation::isSolvable(margaret1::System system) {
    for (std::string requirement: requires) {
        if (!(system.isKnown(requirement))) {
            return false;
        }
    }
    return true;
}

double margaret1::Relation::solveFor(margaret1::System system) {
    if (!isSolvable(system)) {
        margaret1::margaretError("a margaret1 Relation", "Not enough information to solve relation: " + name, "Check with isSolvable before attempting.");
    }
    return relation(system);
}

void margaret1::Relation::solve(margaret1::System )