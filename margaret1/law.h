// laws.h
//
// Laws!

#pragma once

#include "system.h"
#include "err.h"
#include <utility>

namespace margaret1 {

    class Law {

        public:

            Law(std::string name, std::vector<std::pair<std::string, double>> pairs): name(name), pairs(pairs) {}

            std::vector<std::pair<std::string, double>> pairs;

            std::string name;
            double exponent(std::string prop);
            Relation deriveRelationFor(std::string prop);
            std::vector<Relation> deriveAllRelations();

    };

}