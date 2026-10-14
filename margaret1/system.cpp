#include "system.h"
#include "err.h"
#include <iostream>
#include <cmath>
#include <cassert>

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

// --- Implementation for margaret1::EvolutionRecord class --- //
//
// Methods:
//      std::vector<double> series(std::string name)                (written)
//      int closestStepForward(double time);                        (written)
//      int closestStepBackward(double time);                       (written)
//      double deriv(std::string name, double time);                (written)
//      double deriv2(std::string name, double time);               (written)
//      double getVal(std::string name, double time);               (written)

std::vector<double> margaret1::EvolutionRecord::series(std::string name) {
    for (int index = 0; index < variableNames.size(); index++) if (variableNames[index] == name) return variableSeries[index];
    margaret1::margaretError("a margaret1 EvolutionRecord", "No such variable domain: " + name, "Check the variable name and the existing domains of this EvolutionRecord.");
    return {};
}

int margaret1::EvolutionRecord::closestStepForward(double time) {
    if (time < 0 || time > timeDomain[timeDomain.size() - 1]) margaret1::margaretError("a margaret1 EvolutionRecord", "Selected time is out of range for closestStepForward.", "Check the time domain of the EvolutionRecord and your requested input");
    
    int res;
    for (int index = 0; index < timeDomain.size(); index++) if (timeDomain[index] >= time) { res = index; break; }
    assert(timeDomain[res] >= time);
    return res;

    margaret1::margaretError("a margaret1 EvolutionRecord", "Unknown error in closestStepForward", "Check the time domain of the EvolutionRecord and your requested input.");
    return -1;
}

int margaret1::EvolutionRecord::closestStepBackward(double time) {
    if (time < 0 || time > timeDomain[timeDomain.size() - 1]) margaret1::margaretError("a margaret1 EvolutionRecord", "Selected time is out of range for closestStepBackward.", "Check the time domain of the EvolutionRecord and your requested input");

    int res;
    for (int index = 0; index < timeDomain.size(); index++) if (timeDomain[index] >= time) { res = index; break; }
    assert(timeDomain[res] >= time);
    return res;

    margaret1::margaretError("a margaret1 EvolutionRecord", "Unknown error in closestStepBackward", "Check the time domain of the EvolutionRecord and your requested input.");
    return -1;
}

double margaret1::EvolutionRecord::deriv(std::string name, double time) {
    std::vector<double> s = series(name);
    int x = closestStepBackward(time);
    return (s[x+1] - s[x]) / stepSize; // f'(x) is approx (f(x+h) - f(x)) / h, accuracy O(h)
}

double margaret1::EvolutionRecord::deriv2(std::string name, double time) {
    std::vector<double> s = series(name);
    int x = closestStepBackward(time);
    return (s[x+1] - (2 * s[x]) + s[x-1]) / (stepSize * stepSize); // f''(x) is approx (f(x+h) - 2f(x) + f(x-h)) / h^2, accuracy O(h^2)
}

double margaret1::EvolutionRecord::getVal(std::string name, double time) {
    int anchor = closestStepBackward(time);
    std::vector<double> s = series(name);

    // If the selected time is actually a data point in the time
    // domain, just return the value directly to save some compute
    if (timeDomain[anchor] == time) return s[anchor];

    // Otherwise, calculate dt and do a quadratic approximation
    double deltaT = time - timeDomain[anchor];

    // We skip out on deriv() and deriv2() here since they go get
    // the series again, and we already have that information locally
    double d1 = (s[anchor+1] - s[anchor]) / stepSize,
        d2 = (s[anchor+1] - (2 * s[anchor]) + s[anchor-1]) / (stepSize * stepSize);
    
    return s[anchor] + (d1 * deltaT) + ((d2 * deltaT * deltaT) / 2); // quadratic approximation with first and second derivatives
}

// --- Implementation for margaret1::System class --- //
//
// Methods:
//      bool isKnown(std::string requirement);                              (tested)
//      bool isSolvable(Relation relation);                                 (tested)
//      void solve(Relation relation);                                      (tested)
//      void solveAll(std::vector<Relation> relations);                     (tested)
//      void evolve(Evolution evolution, double evoStep, double evoTime);   (written)
//      double getVal(std::string name);                                    (tested)
//      void setVal(std::string name, double newVal);                       (tested)
//      void setUnknown(std::string name);                                  (tested)
//      void setManual();                                                   (tested)

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

margaret1::EvolutionRecord margaret1::System::evolve(margaret1::Evolution evolution, double evoStep, double evoTime, std::vector<std::string> record) {
    int steps = (int)(evoTime / evoStep);
    std::vector<std::string> updateNames;
    std::vector<double> updateValues;
    margaret1::EvolutionRecord evoRecord;
    double currentTime = 0.0;

    evoRecord.filled = (record.size() != 0);

    // Setup: get all the updating variables' names in place
    for (margaret1::TimeDerivRelation tdr: evolution.derivatives) {
        updateNames.push_back(tdr.solves);
        updateValues.push_back(0);
    }

    if (record.size() != 0) {
        // Setup: get all the recording variables' names and series in place
        // and set up the EvolutionRecord's other data
        evoRecord.maxTime = evoTime; evoRecord.stepSize = evoStep;
        for (std::string name: record) {
            evoRecord.variableNames.push_back(name); // add the name to the list
            evoRecord.variableSeries.push_back(std::vector<double>({})); // add the empty series to the list
        }
    }

    // Record initial data if desired
    if (record.size() != 0) {
        // Record the values of the specified variables at this juncture
        evoRecord.timeDomain.push_back(currentTime); // currentTime == 0 right now
        for (int index = 0; index < evoRecord.variableSeries.size(); index++) {
            evoRecord.variableSeries[index].push_back(getVal(record[index]));
        }
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

        if (record.size() != 0) {
            // Record the values of the specified variables at this juncture
            evoRecord.timeDomain.push_back(currentTime);
            for (int index = 0; index < evoRecord.variableSeries.size(); index++) {
                evoRecord.variableSeries[index].push_back(getVal(record[index]));
            }
        }
    }

    return evoRecord; // Return the record generated whether or not it actually has data
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

void margaret1::System::setManual(std::vector<std::string> names) {
    // If no names were provided, default to everything
    double user;
    int totalNew = 0;
    int total = 0;
    std::cout << "[System] Please enter values for the given system properties:" << std::endl;
    if (names.size() == 0) {
        for (margaret1::Property prop: properties) {
            std::cout << "\tValue for \"" << prop.names[0] << "\":\t";
            std::cin >> user;
            if (!prop.known) totalNew++;
            total++;
            setVal(prop.names[0], user); 
        }
    } else {
        for (std::string name: names) {
            std::cout << "\tValue for \"" << name << "\":\t";
            std::cin >> user;
            if (!isKnown(name)) totalNew++;
            total++;
            setVal(name, user);
        }
    }
    std::cout << "[System] Done; " << total << " new properties set (" << total << " total)." << std::endl;
}