#pragma once

#include <vector>
#include <functional>
#include <string>
#include "err.h"

// Macros used for briefly describing System equations; very convenient!
#define SEQ(x) [](margaret1::System* sysptr) { return x; }
#define P(x) sysptr->getVal(#x)

namespace margaret1 {

    // --- margaret1::Property --- //
    //
    // The Property class is essentially a dataclass for storing
    // a particular property of the system (e.g. Mach number,
    // temperature, etc.) with an arbitrary set of legal names
    // (e.g. {"M", "Mach", "Mach number"}) and a statement about
    // whether or not the value is currently known.

    class Property {

        public:

            bool known;
            double value;
            std::vector<std::string> names;

            Property(std::vector<std::string> names): names(names) {}

            double getVal();                // Get the value and error if it is unknown
            void setVal(double newVal);     // Set the value accordingly and make the value known
            void setUnknown();              // Make the value unknown again
            bool hasName(std::string name); // Check if this is a valid name for the property

    };

    class System; // forward-declaration to make stuff work out at compile time

    // --- margaret1::Relation --- // 
    //
    // The Relation class is a dataclass storing information about 
    // a relation of some number of properties. A Relation specifically
    // has a function that takes in a pointer to a System (more
    // on that later), an indication as to what property is solved
    // for by that function, the list of properties that have to be
    // known to use that function, along with a name for itself.
    //
    // The function can be constructed using the system equation (SEQ(x))
    // and property (P(x)) macros defined at the top of the file. For
    // example, a valid function is built by
    //
    //      SEQ(P(x) * pow(P(y), 2))
    //
    // which is a lambda function of one argument of type System*
    // returning a double which needs properties with names "x" and "y"
    // to be defined and returns x * y^2.

    class Relation {

        public:

            Relation(std::string name, std::string solves, std::function<double(System*)> relation, std::vector<std::string> requires): name(name), solves(solves), relation(relation), requires(requires) {}

            std::string name;
            std::function<double(System*)> relation;
            std::string solves;
            std::vector<std::string> requires;

    };

    // --- margaret1::TimeDerivRelation --- //
    //
    // The TimeDerivRelation is a subclass of the Relation class
    // which includes an additional statement about the order of
    // the relation, describing what order derivative of the given
    // property is being solved for. The same thing as a Relation,
    // but instead of solving for the property itself, solves for
    // a time derivative of a particular order.
    //
    // This allows adding multiple terms in a derivative equation
    // (e.g., when solving Navier-Stokes, each term can be put in
    // a separate TimeDerivRelation object rather than as a single
    // blackboard-destroying equation), as well as adding derivatives
    // of multiple orders to increase accuracy.

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

    // --- margaret1::Evolution --- //
    //
    // The Evolution class provides all the data necessary to evolve
    // a system with respect to time, given time derivatives of
    // various properties. See TimeDerivRelation for more info on the
    // construction of the time derivatives.
    //
    // Provided is the name of the Evolution, the set of the relevant
    // time derivatives (e.g. in an Evolution of a 2-node System's
    // temperature, it would include a derivative for T1 and a
    // derivative for T2), and the set of relevant relations that solve
    // for things affected by the quantities which are evolving in
    // time (e.g. the total energy of one of the nodes in the
    // aforementioned 2-node System).

    class Evolution {

        public:

            Evolution(std::string name, std::vector<TimeDerivRelation> derivatives, std::vector<Relation> associated): name(name), derivatives(derivatives), associated(associated) {}

            std::string name;
            std::vector<TimeDerivRelation> derivatives;
            std::vector<Relation> associated;

    };

    // --- margaret1::EvolutionRecord --- //
    //
    // A dataclass resulting from the time evolution of a System,
    // the EvolutionRecord stores data about things of interest as
    // they change over time. 
    //
    // Records all the data points directly encountered by the system
    // in the course of evolution, and allows quadratic interpolation
    // of values at any time between start and finish to smooth out
    // the dataset. This way, after an Evolution has been run, the
    // interesting parameters that have changed can be examined as
    // roughly-smooth functions of time using the returned 
    // EvolutionRecord object.

    class EvolutionRecord {

        public:

            EvolutionRecord() {}

            bool filled; // false if the record was just returned to fill space with no
                         // recorded variables passed to System.evolve(), true if the
                         // record actually contains data
            std::vector<std::string> variableNames;
            std::vector<double> timeDomain = {};
            double maxTime; // minTime is zero by convention; time domain ranges from 0 to maxTime
            double stepSize; // calculable from timeDomain although it shouldn't be necessary
            std::vector<std::vector<double>> variableSeries;

            std::vector<double> series(std::string name);   // the domain of the variable with the given
                                                            // name

            int closestStepForward(double time);            // determine the index in timeDomain of the 
                                                            // timestep nearest the given time, rounding
                                                            // up to nearest timestep

            int closestStepBackward(double time);           // determine the index in timeDomain of the
                                                            // timestep nearest the given time, rounding
                                                            // down to nearest timestep

            double getVal(std::string name, double time);   // directly get or interpolate the value of
                                                            // the given variable at the given time

            double deriv(std::string name, double time);    // get the approximate first time derivative
                                                            // of the given variable at the given time

            double deriv2(std::string name, double time);   // get the approximate second time derivative
                                                            // of the given variable at the given time

    };

    // --- margaret1::System --- //
    //
    // The System class actually contains all of the interesting
    // methods. It is initialized with a list of Property objects
    // that indicate the arbitrary properties the system has, and
    // then the various methods can be used to interrogate the
    // properties of the system, solve properties using Relation
    // objects (including solving many relations simultaneously),
    // evolving properties with time as prescribed by Evolution
    // objects, and setting property values manually.

    class System {

        public:

            std::vector<Property> properties;

            System(std::vector<Property> props): properties(props) {}

            bool isKnown(std::string requirement);                              // check if the given property is known
            
            bool isSolvable(Relation* relationptr);                             // check if the given Relation/TimeDerivRelation
                                                                                // can be solved based on what Property objects
                                                                                // are listed as known

            void solve(Relation relation);                                      // solve the given relation, errors if the
                                                                                // Relation doesn't satisfy !isSolvable(&relation).

            void solveAll(std::vector<Relation> relations);                     // solve many relationships simultaneously, using
                                                                                // whatever information is initially available to
                                                                                // solve as many as possible and repeating until
                                                                                // no more information can be gleaned from the
                                                                                // currently-known properties and the relations
                                                                                // between them

            EvolutionRecord evolve(Evolution evolution, double evoStep, double evoTime, std::vector<std::string> record);   
                                                                                // evolve the system as prescribed in the Evolution
                                                                                // object, with a timestep of evoStep and for a total
                                                                                // length of time evoTime. Return an EvolutionRecord
                                                                                // containing series for the specified variables.

            double getVal(std::string name);                                    // get the value of a Property with that name

            void setVal(std::string name, double newVal);                       // set the value of a Property with that name

            void setUnknown(std::string name);                                  // make a Property with that name unknown

            void setManual(std::vector<std::string> names={});                  // manually set properties with the specified
                                                                                // names with an interface in the console at
                                                                                // run-time. 

    };

}