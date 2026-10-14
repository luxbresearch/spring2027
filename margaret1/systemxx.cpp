#include "systemxx.h"
#include "err.h"
#include <sstream>
#include <cmath>

// --- Implementation for margaret1::Law class --- //
//
// Methods:
//      bool relates(std::string prop)                  (test!)
//      double exponent(std::string prop)               (test!)
//      Relation deriveRelationFor(std::string prop)    (test!)
//      std::vector<Relation> allDerivedRelations()     (test!)

bool margaret1::Law::relates(std::string prop) {
    for (std::pair<std::string, double> p: pairs) if (p.first == prop) return true; return false;
}

double margaret1::Law::exponent(std::string prop) {
    for (std::pair<std::string, double> p: pairs) if (p.first == prop) return p.second;
    margaret1::margaretError("a margaret1 Law", "No such property related by law " + name + " when getting exponent: " + prop, "Check property name and the law that you're asking it from.");
    return 0.0;
}

margaret1::Relation margaret1::Law::deriveRelationFor(std::string prop) {
    if (!relates(prop)) margaret1::margaretError("a margaret1 law", "No such property related by law " + name + " when deriving relation: " + prop, "Check property name and the law that you're asking it from.");

    // The name is a little arbitrary but we'll use this 
    // convention.

    std::stringstream ss;
    ss << name << " solved for " << prop;
    std::string relationName = ss.str();

    // The requirements for the Relation is everything *except*
    // `prop` that's in the Law's list of properties, and the
    // Relation solves for `prop`.

    std::string solvesFor = prop;

    std::vector<std::string> requires;
    for (std::string possibleRequire: allProperties) if (prop != possibleRequire) requires.push_back(possibleRequire);

    // In general, if everything else in the law is Y and our
    // target property is x, so the law is 1 = x^k * Y, the
    // resultant relation for x is obviously x = Y^k (since 
    // the derivation goes x^-k = Y to start). Build and return
    // a lambda function that gives us that Relation.

    double k = exponent(prop);

    auto func = [k, requires](margaret1::System* sysptr) {
        double prod = 1.0;
        for (std::string requirement: requires) {
            prod *= sysptr->getVal(requirement);
        }
        return std::pow(prod, k);
    };

    return margaret1::Relation(relationName, solvesFor, func, requires);
}

std::vector<margaret1::Relation> margaret1::Law::allDerivedRelations() {
    std::vector<margaret1::Relation> res;
    for (std::string prop: allProperties) res.push_back(deriveRelationFor(prop));
    return res;
}