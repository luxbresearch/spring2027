#include "system.h"
#include "err.h"
#include <iostream>
#include <cmath>

// --- Implementation for margaret1::Property class --- //
//
// Methods:
//      double getVal();                (tested)
//      void setVal(double newVal);     (tested)
//      void setUnknown();              (tested)
//      bool hasName(std::string name); (tested)

double margaret1::Property::getVal() {
    if (!known) {
        margaret1::margaretError("a margaret1 property", "Cannot get value of property (value is unknown): " + names[0], "Ensure property is solved first.");
    }
    return value;
}

void margaret1::Property::setVal(double newVal) {
    known = true;
    value = newVal;
}

void margaret1::Property::setUnknown() {
    known = false;
}

bool margaret1::Property::hasName(std::string name) {
    for (std::string possible: names) {
        if (name == possible) {
            return true;
        }
    }
    return false;
}

// --- Implementation for margaret1::System class --- //
//
// Methods:
//      bool isKnown(std::string requirement);          (tested)
//      bool isSolvable(Relation relation);             (tested)
//      void solve(Relation relation);                  (tested)
//      void solveAll(std::vector<Relation> relations); (tested)
//      void evolve(Evolution evolution, double evoStep, double evoTime);
//      double getVal(std::string name);                (tested)
//      void setVal(std::string name, double newVal);   (tested)
//      void setUnknown(std::string name);              (tested)

bool margaret1::System::isKnown(std::string requirement) {
    for (Property prop: properties) if (prop.hasName(requirement)) return prop.known;
    margaret1::margaretError("a margaret1 System", "No such property with name for isKnown: " + requirement, "Check requirement name.");
    return false; // to appease the compiler so that control doesn't reach end of non-void func
}

bool margaret1::System::isSolvable(margaret1::Relation* relation) {
    for (std::string requirement: relation->requires) if (!isKnown(requirement)) return false;
    return true;
}

void margaret1::System::solve(margaret1::Relation relation) {
    setVal(relation.solves, relation.relation(this));
}

void margaret1::System::solveAll(std::vector<margaret1::Relation> relations) {
    std::vector<std::string> alreadySolved;
    int solveCount = 0;
    int previousSolveCount = 0;
    bool isAlreadySolved = false;
    while (true) {

        for (margaret1::Relation relation: relations) {
            if (isSolvable(&relation)) {
                for (std::string name: alreadySolved) if (relation.name == name) {isAlreadySolved = true; break; }
                if (isAlreadySolved) continue;

                solve(relation);
                alreadySolved.push_back(relation.name);
                solveCount++;
            }
        }

        // No new relations were solved in this loop
        // and as such the same trivial result will be
        // had in the next one. We've done all the work
        // we can do with the given information.
        if (previousSolveCount == solveCount) break;

        // At least one relation was solved and there's
        // a chance we could solve another one on the
        // next go-around with new information, so update
        // the number of solves and keep going
        else previousSolveCount = solveCount;
        
    }
}

void margaret1::System::evolve(margaret1::Evolution evolution, double evoStep, double evoTime) {
    int steps = (int)(evoTime / evoStep);
    std::vector<std::string> updateNames;
    std::vector<double> updateValues;

    for (margaret1::TimeDerivRelation tdr: evolution.derivatives) {
        updateNames.push_back(tdr.solves);
        updateValues.push_back(0);
    }

    for (int step = 0; step < steps; step++) {
        // Figure out the time derivatives as specified in
        // the evolution object, and add the dx/dt dt 
        // elements to their respective properties.

        for (margaret1::TimeDerivRelation tdr: evolution.derivatives) {
            if (!isSolvable(&tdr)) margaret1::margaretError("a margaret1 System", "Cannot solve evolution derivative", "Add more information before evolution attempt");
            for (int index = 0; index < updateNames.size(); index++) {
                if (updateNames[index] == tdr.solves) {
                    updateValues[index] = tdr.relation(this) * std::pow(evoStep, tdr.order) / (std::tgamma(tdr.order + 1));
                }
            }
        }

        for (int index = 0; index < updateNames.size(); index++) {
            setVal(updateNames[index], getVal(updateNames[index]) + updateValues[index]);
        }

        // Solve all of the associated relations to update
        // other system properties
        for (margaret1::Relation relation: evolution.associated) {
            solve(relation);
        }
    }
}

double margaret1::System::getVal(std::string name) {
    for (Property prop: properties) if (prop.hasName(name)) return prop.getVal();
    margaret1::margaretError("a margaret1 System", "No such property with name for getVal: " + name, "Check target property name.");
    return 0.0; // to appease the compiler so that control doesn't reach end of non-void func
}

void margaret1::System::setVal(std::string name, double newVal) {
    for (Property& prop: properties) if (prop.hasName(name)) { prop.setVal(newVal); return; }
    margaret1::margaretError("a margaret1 System", "No such property with name for setVal: " + name, "Check target property name.");
}

void margaret1::System::setUnknown(std::string name) {
    for (Property& prop: properties) if (prop.hasName(name)) { prop.setUnknown(); return; }
    margaret1::margaretError("a margaret1 System", "No such property with name for setUnknown: " + name, "Check target property name.");
}