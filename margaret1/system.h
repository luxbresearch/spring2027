#pragma once

#include <vector>
#include <functional>
#include <string>
#include "err.h"

#define SEQ(x) [](margaret1::System* sysptr) { return x; }
#define P(x) sysptr->getVal(#x)

namespace margaret1 {

    class Property {

        public:

            bool known;
            double value;
            std::vector<std::string> names;

            Property(std::vector<std::string> names): names(names) {}

            double getVal();
            void setVal(double newVal);
            void setUnknown();
            bool hasName(std::string name);

    };

    class System; // forward-declare to make this work right
    class Relation {

        public:

            Relation(std::string name, std::string solves, std::function<double(System*)> relation, std::vector<std::string> requires): name(name), solves(solves), relation(relation), requires(requires) {}

            std::string name;
            std::function<double(System*)> relation;
            std::string solves;
            std::vector<std::string> requires;

    };

    class System {

        public:

            std::vector<Property> properties;

            System(std::vector<Property> props): properties(props) {}

            bool isKnown(std::string requirement);
            bool isSolvable(Relation relation);
            void solve(Relation relation);
            void solveAll(std::vector<Relation> relations);
            double getVal(std::string name);
            void setVal(std::string name, double newVal);
            void setUnknown(std::string name);

    };

}