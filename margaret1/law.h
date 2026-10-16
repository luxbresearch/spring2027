// laws.h
//
// Laws!

#pragma once

#include "system.h"
#include "err.h"
#include <utility>
#include <stdexcept>
#include <tuple>

namespace margaret1 {

    // --- margaret1::Law and subclasses --- //
    //
    // In practical circumstances, we have a few equations
    // that govern the behavior of the system, more or less
    // (like the ideal gas law, energy conservation law, 
    // energy conservation law, etc. for quasi-1D aerodynamics)
    // and then all the relations are derived from those
    // few equations.
    //
    // In these cases, putting together lots of individual
    // Relation objects is just tedious. It requires algebraically
    // manipulating the equation over and over and over to 
    // produce various explicit statements for each variable,
    // which introduces opportunities for errors and makes it
    // harder to group all the relevant relations together.
    //
    // To remedy this, we have the Law classes. There are three
    // subclasses for three primary cases, all deriving from the
    // Law base class which has general methods for initialization
    // + generating a list of all available relations.
    //
    // The first case is MultLaw, where the law can be
    // expressed as a product of several variables equalling 1:
    //
    //      x_0 * x_1 * x_2 * ... * x_(n-1) == 1.
    //
    // The second case is AddLaw, where the law can be
    // expressed as a sum of several variables equalling 0:
    //
    //      x_0 + x_1 + x_2 + ... + x_(n-1) == 0.
    //
    // The third case is PolyLaw, where the law can be
    // expressed as the root of a polynomial (possibly of
    // non-integral exponents) with arbitrary coefficients:
    //
    //      a_0*x_0^(n_0) + a_1*x_1^(n_1) ... == 0.
    //
    // These cases cover essentially everything, and in all
    // cases the deriveAllRelations() method will produce
    // a valid set of self-consistent relations.

    class Law {

        public:
          
            Law() {}
            Law(std::string name, std::vector<std::pair<std::string, double>> pairs): name(name), pairs(pairs) {
                for (std::pair<std::string, double> pair: pairs) props.push_back(pair.first);
            }

            std::vector<std::string> props;
            std::vector<std::pair<std::string, double>> pairs;
            std::string name;
            
            double getFactor(std::string prop);

            virtual Relation deriveRelationFor(std::string prop) { throw std::runtime_error("virtual method called"); return Relation(); };      
            std::vector<Relation> deriveAllRelations();

    };

    class MultLaw: public Law {

        public:

            MultLaw(std::string name, std::vector<std::pair<std::string, double>> pairs): Law(name, pairs) {}

            double exponent(std::string prop);              // map to getFactor
            Relation deriveRelationFor(std::string prop) override;

    };

    class AddLaw: public Law {

        public:

            AddLaw(std::string name, std::vector<std::pair<std::string, double>> pairs): Law(name, pairs) {}

            double coefficient(std::string prop);   // map to getFactor
            Relation deriveRelationFor(std::string prop) override;

    };

    class PolyLaw: public Law {

        public:

            PolyLaw(std::string name, std::vector<std::tuple<std::string, double, double>> tuples): name(name), tuples(tuples) {
                for (std::tuple<std::string, double, double> tuple: tuples) props.push_back(std::get<0>(tuple));
            }

            std::vector<std::string> props;
            std::string name;
            std::vector<std::tuple<std::string, double, double>> tuples;

            double exponent(std::string prop);
            double coefficient(std::string prop);
            Relation deriveRelationFor(std::string prop) override;

    };

}