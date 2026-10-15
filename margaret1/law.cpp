#include "law.h"
#include <cmath>
#include <iostream>

// --- Implementation for margaret1::Law class --- //

double margaret1::Law::exponent(std::string prop) {
    for (std::pair<std::string, double> pair: pairs) if (pair.first == prop) return pair.second;
    margaret1::margaretError("a margaret1 Law", "No such product term in the law: " + name, "Check to see if the name is right.");
    return 0.0; 
}

// Suppose the law is 1 = x^k * y, where y is the product of the
// rest of the elements of the Law, x is the thing we're solving
// for, and k is its exponent (e.g. Law::exponent(x)).
//
// We have x^-k = y and therefore x = y^k, so knowing the exponent
// and the other terms, we can establish the function that gives
// x in terms of the other things.
margaret1::Relation margaret1::Law::deriveRelationFor(std::string prop) {
    std::string derivedName = name + " (derived: " + prop + ")";
    std::string solvesFor = prop;
    std::vector<std::string> requires;
    for (std::pair<std::string, double> pair: pairs) if (pair.first != prop) requires.push_back(pair.first);

    double k = exponent(prop);
    std::function<double(System*)> relation = [this, prop, k](System* sysptr) {
        double result = 1.0;
        std::cout << "\t\tLDR evaluation: do not multiply by " << prop << std::endl;
        for (std::pair<std::string, double> pair: this->pairs) {
            if (pair.first != prop) {
                std::cout << "\t\tLDR evaluation: multiplied " << pair.first << std::endl;
                result *= std::pow(sysptr->getVal(pair.first), pair.second);
            } else {
                std::cout << "\t\tLDR evaluation: skipped " << pair.first << std::endl;
            }
        }
        return pow(result, k);
    };

    return margaret1::Relation(derivedName, solvesFor, relation, requires);
}

std::vector<margaret1::Relation> margaret1::Law::deriveAllRelations() {
    std::vector<margaret1::Relation> res;
    for (std::pair<std::string, double> pair: pairs) res.push_back(deriveRelationFor(pair.first));
    return res;
}