// laws.h
//
// Laws!

#pragma once

#include "system.h"
#include "err.h"

namespace margaret1 {

    // --- LAWS --- //
    //
    // The LawTerm and Law classes are used to specify
    // physical laws using products, sums, and exponents.
    // Whereas a Relation allows solving for a single property
    // with a given function of others, a Law allows for the
    // automatic generation of many Relations that allow for
    // solving many if not all of the involved properties.

    class LawTerm {

        public:

            LawTerm(std::vector<std::pair<std::string, double>> pairs): pairs(pairs) {}

            std::vector<std::pair<std::string, double>> pairs;

    };

    class Law {

        public:

            Law(std::vector<std::pair<LawTerm, double>> pairs): pairs(pairs) {}

            std::vector<std::pair<LawTerm, double>> pairs;

    };

}