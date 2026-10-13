#pragma once

#include <vector>
#include <functional>
#include <string>
#include "err.h"

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

            Relation(std::string name, std::string solves, std::function<double(System)> relation, std::vector<std::string> requires): name(name), solves(solves), relation(relation), requires(requires) {}

            std::string name;
            std::function<double(System)> relation;
            std::string solves;
            std::vector<std::string> requires;

            bool isSolvable(System system);
            double solveFor(System system);
            void solve(System system);

    };

    class Evolution {

        public:

            Evolution();

            std::function<double(System)> evolution;
            std::vector<std::string> evolves;
            std::vector<Relation> relations;

            void isEvolvable(System system);
            double infinitesimalFor(System system, double step);
            double infinitesimal(System system, double step);
            double evolveFor(System system, double step, double totalDT);
            void evolve(System system, double step, double totalDT);

    };

    class System {

        public:

            System();

            std::vector<Relation> relations;
            std::vector<Property> properties;

            bool isKnown(std::string requirement);
            void solveAll(std::vector<Relation> relations);
            void evolveSimultaneous(std::vector<Evolution> evolutions, double step, double totalDT);
            double getVal(std::string name);

    };

}