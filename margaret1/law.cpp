// Copyright 2026 Lux Bodell
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include "law.h"
#include <cmath>
#include <iostream>
#include <tuple>

// --- Implementation for margaret1::Law class --- //
// 
// Methods:
//      double exponent(std::string prop);              (tested)
//      std::vector<Relation> deriveAllRelations();     (tested)
//      Relation deriveRelationFor(std::string prop);   (many subclasses; tested MultLaw)

double margaret1::Law::getFactor(std::string prop) {
    for (std::pair<std::string, double> pair: pairs) if (pair.first == prop) return pair.second;
    margaret1::margaretError("a margaret1 Law", "No such product term in the law: " + name, "Check to see if the name is right.");
    return 0.0; 
}

double margaret1::MultLaw::exponent(std::string prop) { return getFactor(prop); }
double margaret1::AddLaw::coefficient(std::string prop) { return getFactor(prop); }

double margaret1::PolyLaw::coefficient(std::string prop) {
    for (std::tuple<std::string, double, double> tuple: tuples) if (std::get<0>(tuple) == prop) return std::get<1>(tuple);
    margaret1::margaretError("a margaret1 PolyLaw", "No such term in the law: " + name, "Check to see if the name is right.");
    return 0.0;
}

double margaret1::PolyLaw::exponent(std::string prop) {
    for (std::tuple<std::string, double, double> tuple: tuples) if (std::get<0>(tuple) == prop) return std::get<2>(tuple);
    margaret1::margaretError("a margaret1 PolyLaw", "No such term in the law: " + name, "Check to see if the name is right.");
    return 0.0;
}

std::vector<margaret1::Relation> margaret1::Law::deriveAllRelations() {
    std::vector<margaret1::Relation> res;
    for (std::string prop: props) res.push_back(deriveRelationFor(prop));
    return res;
}

// Suppose the law is 1 = x^k * y, where y is the product of the
// rest of the elements of the Law, x is the thing we're solving
// for, and k is its exponent (e.g. MultLaw::exponent(x)).
//
// We have x^-k = y and therefore x = y^(-1/k).
margaret1::Relation margaret1::MultLaw::deriveRelationFor(std::string prop) {
    std::string derivedName = name + " (derived: " + prop + ")";
    std::string solvesFor = prop;
    std::vector<std::string> requires;
    for (std::pair<std::string, double> pair: pairs) if (pair.first != prop) requires.push_back(pair.first);

    double k = exponent(prop);
    std::function<double(System*)> relation = [this, prop, k](System* sysptr) {
        double y = 1.0;
        for (std::pair<std::string, double> pair: this->pairs) {
            if (pair.first != prop) {
                y *= std::pow(sysptr->getVal(pair.first), pair.second);
            }
        }
        return pow(y, -1/k);
    };

    return margaret1::Relation(derivedName, solvesFor, relation, requires);
}

// Suppose the law is 0 = x + y, where y is the product of the
// rest of the elements of the Law, x is the thing we're solving
// for, and k is its coefficient (e.g. AddLaw::coefficient(x)).
// 
// We have x = -y, which is nice and easy.
margaret1::Relation margaret1::AddLaw::deriveRelationFor(std::string prop) {
    std::string derivedName = name + " (derived: " + prop + ")";
    std::string solvesFor = prop;
    std::vector<std::string> requires;
    for (std::pair<std::string, double> pair: pairs) if (pair.first != prop) requires.push_back(pair.first);

    double k = coefficient(prop);
    std::function<double(System*)> relation = [this, prop, k](System* sysptr) {
        double result = 0.0;
        for (std::pair<std::string, double> pair: this->pairs) {
            if (pair.first != prop) {
                result -= sysptr->getVal(pair.first) * pair.second;
            }
        }
        return result;
    };

    return margaret1::Relation(derivedName, solvesFor, relation, requires);
}

// Suppose the law is 0 = ax^n + y, where y is the sum of the
// rest of the elements of the PolyLaw, x is the thing we're solving
// for, a is its coefficient (e.g. PolyLaw::coefficient(x)), and
// n is its exponent (e.g. PolyLaw::exponent(x)).
// 
// We have x = -(y/a)^(1/n), which is the most complicated yet
// but obviously still doable.
margaret1::Relation margaret1::PolyLaw::deriveRelationFor(std::string prop) {
    std::string derivedName = name + " (derived: " + prop + ")";
    std::string solvesFor = prop;
    std::vector<std::string> requires;
    for (std::pair<std::string, double> pair: pairs) if (pair.first != prop) requires.push_back(pair.first);

    double a = coefficient(prop);
    double n = exponent(prop);
    std::function<double(System*)> relation = [this, prop, a, n](System* sysptr) {
        double y = 0.0;
        for (std::tuple<std::string, double, double> tuple: this->tuples) {
            if (std::get<0>(tuple) != prop) {
                y -= pow(sysptr->getVal(std::get<0>(tuple)) * std::get<1>(tuple), std::get<2>(tuple));
            }
        }
        return std::pow(y / a, 1/n);
    };

    return margaret1::Relation(derivedName, solvesFor, relation, requires);
}

std::vector<margaret1::Relation> margaret1::LawSet::deriveAllRelations() {
    std::vector<margaret1::Relation> res;
    for (margaret1::Law* lawptr: laws) {
        std::vector<margaret1::Relation> lawRelations = lawptr->deriveAllRelations();
        for (margaret1::Relation relation: lawRelations) res.push_back(relation);
    }
    return res;
}