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

    class System;
    class Relation {

        public:

            Relation(std::string name, std::string solves, std::function<double(System*)> relation, std::vector<std::string> requires): name(name), solves(solves), relation(relation), requires(requires) {}

            std::string name;
            std::function<double(System*)> relation;
            std::string solves;
            std::vector<std::string> requires;

    };

    class TimeDerivRelation: public Relation {

        public:

            TimeDerivRelation(std::string name, std::string solves, std::function<double(System*)> relation, std::vector<std::string> requires, int order):
                Relation(name, solves, relation, requires), order(order) {}

            int order;
            std::string name;
            std::function<double(System*)> relation;
            std::string solves;
            std::vector<std::string> requires;

    };

    class Evolution {

        public:

            Evolution(std::string name, std::vector<TimeDerivRelation> derivatives, std::vector<Relation> associated): name(name), derivatives(derivatives), associated(associated) {}

            std::string name;
            std::vector<TimeDerivRelation> derivatives;
            std::vector<Relation> associated;

    };

    class System {

        public:

            std::vector<Property> properties;

            System(std::vector<Property> props): properties(props) {}

            bool isKnown(std::string requirement);
            bool isSolvable(Relation* relationptr);
            void solve(Relation relation);
            void solveAll(std::vector<Relation> relations);
            void evolve(Evolution evolution, double evoStep, double evoTime);
            double getVal(std::string name);
            void setVal(std::string name, double newVal);
            void setUnknown(std::string name);

    };

}