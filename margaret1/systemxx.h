// systemxx.h
//
// This header contains some additional utilities for Margaret 1's
// System class that make actual implementation a bit easier.
// None of this is strictly required for the system to run,
// and is mostly just for ease of development later on. The system.h
// header will do fine on its own and not need anything in here.
// This stuff is just for speeding development up.

#pragma once

#include "system.h"
#include "err.h"

namespace margaret1 {

    // --- margaret1::Law --- //
    //
    // In many practical cases, relations will not be given
    // explicitly for every single variable, and instead a single
    // law relating all of them is given; e.g.
    //
    //      dotm = A * rho * u
    //
    // for the mass flow rate dotm, duct area A, fluid density rho,
    // and fluid velocity u in a quasi-1-dimensional flow in a
    // duct. In general these laws can be expressed as the product
    // of some number of properties (which are representable with
    // Property objects) raised to some powers, and the whole
    // product is set equal to unity. In this case, we can rewrite
    // the definition (or "law") as 
    //
    //      1 = dotm^1 * A^-1 * rho^-1 * u^-1
    //
    // and we see that it is all just a single product.
    //
    // It would be somewhat tedious to break this equation into
    // four separate Relation objects for solving for dotm, A, rho,
    // and u separately, so instead we can specify the "product form"
    // of the law with a margaret1::Law object, which can then
    // automatically generate all of the relations that are
    // consequent of the law.
    //
    // We can also use the later Theory object with a combination of
    // Laws and Relations to automatically generate all possible
    // Relations to use.

    class Law {

        public:

            Law(std::string name, std::vector<std::pair<std::string, double>> pairs): name(name), pairs(pairs) {
                for (std::pair<std::string, double> p: pairs) {
                    allProperties.push_back(p.first);
                }
            }

            std::vector<std::string> allProperties;
            std::string name;
            std::vector<std::pair<std::string, double>> pairs;

            bool relates(std::string prop);                 // does that property show up here?

            double exponent(std::string prop);              // what's the exponent of that property in
                                                            // the statement of the law?

            Relation deriveRelationFor(std::string prop);   // derive a Relation object that solves for
                                                            // the given Property

            std::vector<Relation> allDerivedRelations();    // derive all Relations and return them together

    };

}