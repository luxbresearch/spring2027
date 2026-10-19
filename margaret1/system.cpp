// Copyright 2026 Lux Bodell
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include "system.h"
#include "err.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <chrono>
#include <thread>

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

// --- Implementation for margaret1::Relation class --- //
//
// Methods:
//      double suppose(std::vector<std::pair<std::string, double>> values)  (tested)
//      std::pair<double, double> invert_rangeLE(
//          std::string for, double output,
//          std::vector<std::pair<std::string, double>> knownValues, 
//          double dmin, double dmax, double precision)                     (write!)


double margaret1::Relation::suppose(std::vector<std::pair<std::string, double>> values) {
    std::vector<margaret1::Property> props;
    margaret1::Property prop;
    for (std::pair<std::string, double> p: values) {
        prop = margaret1::Property({p.first});
        prop.setVal(p.second);
        props.push_back(prop);
    }
    props.push_back(margaret1::Property({solves}));
    System* test = new System(props);
    test->solve(*this);
    return test->getVal(solves);
}

// invertRLE: Sweep a range for a value for a particular
// variable which produces the least error for a target output
// and return that target value and the associated error.
//
// That is, find the value x_0 in the given domain for the input
// which best approximates the solution to f(x)=y for the given
// output y and the Relation f(x).
std::pair<double, double> margaret1::Relation::invertRLE(
        std::string input, double output, 
        std::vector<std::pair<std::string, double>> knownValues, 
        double dmin, double dmax, double precision) {

    std::vector<margaret1::Property> props;
    
    // Init other constant properties
    margaret1::Property prop;
    for (std::pair<std::string, double> p: knownValues) {
        prop = margaret1::Property({p.first});
        prop.setVal(p.second);
        props.push_back(prop);
    }

    // Init output property 
    margaret1::Property solvesProperty({solves});
    solvesProperty.setVal(output);
    props.push_back(solvesProperty);

    // Init input property
    margaret1::Property inputProperty({input});
    props.push_back(inputProperty);

    // Init system
    System* test = new System(props);

    // Sweep through the target domain
    double currentInput, currentOutput, error;
    double leastErrorInput = -dmin, leastError = 999999;
    for (currentInput = dmin; currentInput <= dmax; currentInput += precision) {
        // Set the inverted property to the current value and
        // solve what the output property is under those conditions
        test->setVal(input, currentInput);
        test->solve(*this);
        currentOutput = test->getVal(solves);
        // Compute the error from the target value and possibly
        // update the least-error input
        error = std::abs(currentOutput - output);

        if (error < leastError) {
            leastError = error;
            leastErrorInput = currentInput;
        }
    }

    // The least-error input is as close as we got so that's
    // good enough
    return std::pair<double, double>(leastErrorInput, leastError);
}

// invertREM: Sweep a range for values of a particular
// variable which produces error margins against a known
// output which are satisfactory to some degree, and
// return each value and its error margin.
// 
// That is, find values x_0 in the given domain for the input
// which are good approximations to the solution to f(x)=y
// for the given output y and the Relation f(x).
std::vector<std::pair<double, double>> margaret1::Relation::invertREM(
        std::string input, double output, 
        std::vector<std::pair<std::string, double>> knownValues, 
        double dmin, double dmax, double precision, double tolerableError) {

    std::vector<margaret1::Property> props;
    
    // Init other constant properties
    margaret1::Property prop;
    for (std::pair<std::string, double> p: knownValues) {
        prop = margaret1::Property({p.first});
        prop.setVal(p.second);
        props.push_back(prop);
    }

    // Init output property 
    margaret1::Property solvesProperty({solves});
    solvesProperty.setVal(output);
    props.push_back(solvesProperty);

    // Init input property
    margaret1::Property inputProperty({input});
    props.push_back(inputProperty);

    // Init system
    System* test = new System(props);

    // Sweep through the target domain
    double currentInput, currentOutput, error;
    std::vector<std::pair<double, double>> result;
    for (currentInput = dmin; currentInput <= dmax; currentInput += precision) {
        // Set the inverted property to the current value and
        // solve what the output property is under those conditions
        test->setVal(input, currentInput);
        test->solve(*this);
        currentOutput = test->getVal(solves);

        // Compute the error from the target value and possibly
        // update the least-error input
        error = std::abs(currentOutput - output);
        if (error < tolerableError) result.push_back(std::pair(currentInput, error));
    }

    return result;
}

// invertREMHD: Essentially, produce points in the given 
// domain that are very close to the level set f(x)=y.
//
// Since we already have one function that'll do this in
// one dimension, we can actually use it as a base case
// for this function and then have this one be recursive,
// taking each n-dimensional problem and reducing it to
// an (n-1)-dimensional problem until eventually we get
// to the 1-dimensional problem, and then solving that with
// invertREM and concatenating the results together.

std::vector<std::pair<std::vector<double>, double>> margaret1::Relation::invertREMHD(
        double output,
        std::vector<std::pair<std::string, double>> knownValues,
        std::vector<std::tuple<std::string, double, double>> targetDomains,
        double precision, double tolerableError) {
    
    std::vector<std::pair<std::vector<double>, double>> results;

    if (targetDomains.size() == 1) {

        // In the one-dimensional case, get the results
        // for the equivalently-stated one-dimensional
        // probem

        std::string input = std::get<0>(targetDomains[0]);
        double dmin = std::get<1>(targetDomains[0]);
        double dmax = std::get<2>(targetDomains[0]);
        std::vector<std::pair<double, double>> results1d = invertREM(
            input, output, knownValues,
            dmin, dmax, precision, tolerableError
        );

        // Properly format the results
        //
        // Essentially, the results from invertREM (the
        // one-dimensional base case of this function)
        // output the results as individual doubles,
        // instead of as vectors of doubles. Just fix
        // that by going through and appending a vector
        // containing just the output double for every
        // output produced by invertREM.
        for (std::pair<double, double> p: results1d) results.push_back(std::pair<std::vector<double>, double>({p.first}, p.second));

        // Output them
        return results;

    } else if (targetDomains.size() > 1) {

        // In the n-dimensional case, sweep through a
        // range of values for the FIRST of the variables
        // still unknown and for each such case get the
        // set of solutions to the (n-1)-dimensional case
        // and add them to the results.

        std::tuple<std::string, double, double> sweepingDomainData = targetDomains[0];
        std::string domainToSweep = std::get<0>(sweepingDomainData);
        double dmin = std::get<1>(sweepingDomainData);
        double dmax = std::get<2>(sweepingDomainData);

        std::vector<std::pair<std::string, double>> newKnownsBase = knownValues;
        std::vector<std::pair<std::string, double>> newKnowns;

        std::vector<std::tuple<std::string, double, double>> newTargetDomains = targetDomains;
        newTargetDomains.erase(newTargetDomains.begin()); // drop the first one, we're sweeping it

        std::vector<std::pair<std::vector<double>, double>> sectionResults;
        std::vector<double> sectionVector; double sectionError;
        std::vector<double> newPositionVector;

        double currentInput, currentOutput, error;
        for (currentInput = dmin; currentInput <= dmax; currentInput += precision) {
            // Set up the new knowns to have the first of 
            // the variables (domainToSweep) set to the
            // particular value
            newKnowns = newKnownsBase;
            newKnowns.push_back({domainToSweep, currentInput});

            // Recurse to get the data for this section, with
            // the 
            sectionResults = invertREMHD(output, newKnowns, newTargetDomains, precision, tolerableError);

            // Copy the data to results correctly formatted
            // (all these data happened with domainToSweep=currentInput,
            // so add that piece of information)
            for (std::pair<std::vector<double>, double> p: sectionResults) {
                sectionVector = p.first; sectionError = p.second;
                newPositionVector = {currentInput};
                for (int i = 0; i < sectionVector.size(); i++) newPositionVector.push_back(sectionVector[i]);
                results.push_back({newPositionVector, sectionError});
            }
        }

        // Return the stack of results
        return results;

    } else {
        margaret1::margaretError("a margaret1 Relation", "Invalid number of target domains in invertREMHD.", "Pass a nonzero number of target domains to this function.");
    }

    return {};
}


// --- Implementation for margaret1::EvolutionRecord class --- //
//
// Methods:
//      std::vector<double> series(std::string name)                (tested)
//      int closestStepForward(double time);                        (tested)
//      int closestStepBackward(double time);                       (tested)
//      double deriv(std::string name, double time);                (tested)
//      double deriv2(std::string name, double time);               (tested)
//      double getVal(std::string name, double time);               (tested)
//      void exportCSV(std::string filename)                        (tested)
//      void exportPRN(std::string filename)                        (tested)

std::vector<double> margaret1::EvolutionRecord::series(std::string name) {
    for (int index = 0; index < variableNames.size(); index++) if (variableNames[index] == name) return variableSeries[index];
    margaret1::margaretError("a margaret1 EvolutionRecord", "No such variable domain: " + name, "Check the variable name and the existing domains of this EvolutionRecord.");
    return {};
}

int margaret1::EvolutionRecord::closestStepForward(double time) {
    if (timeDomain.empty() || time < timeDomain.front() || time > timeDomain.back()) margaret1::margaretError("a margaret1 EvolutionRecord", "Selected time is out of range for closestStepForward.", "Check the time domain of the EvolutionRecord and your requested input");
    
    return static_cast<int>(std::lower_bound(timeDomain.begin(), timeDomain.end(), time) - timeDomain.begin());
}

int margaret1::EvolutionRecord::closestStepBackward(double time) {
    if (timeDomain.empty() || time < timeDomain.front() || time > timeDomain.back()) margaret1::margaretError("a margaret1 EvolutionRecord", "Selected time is out of range for closestStepBackward.", "Check the time domain of the EvolutionRecord and your requested input");

    return static_cast<int>(std::upper_bound(timeDomain.begin(), timeDomain.end(), time) - timeDomain.begin()) - 1;
}

double margaret1::EvolutionRecord::deriv(std::string name, double time) {
    std::vector<double> s = series(name);
    if (s.size() < 2) margaret1::margaretError("a margaret1 EvolutionRecord", "Cannot calculate a derivative with fewer than two data points.", "Record at least two time points.");
    int x = closestStepBackward(time);
    if (x == static_cast<int>(s.size()) - 1) x--;
    return (s[x+1] - s[x]) / (timeDomain[x+1] - timeDomain[x]);
}

double margaret1::EvolutionRecord::deriv2(std::string name, double time) {
    std::vector<double> s = series(name);
    if (s.size() < 3) margaret1::margaretError("a margaret1 EvolutionRecord", "Cannot calculate a second derivative with fewer than three data points.", "Record at least three time points.");
    int x = closestStepBackward(time);
    x = std::max(1, std::min(x, static_cast<int>(s.size()) - 2));
    double leftStep = timeDomain[x] - timeDomain[x-1];
    double rightStep = timeDomain[x+1] - timeDomain[x];
    return 2 * ((s[x+1] - s[x]) / rightStep - (s[x] - s[x-1]) / leftStep) / (leftStep + rightStep);
}

double margaret1::EvolutionRecord::getVal(std::string name, double time, bool doNotInterpolate) {
    std::vector<double> s = series(name);
    int lower = closestStepBackward(time);

    // If no interpolation is requested just return the nearest
    // data point that's available (default to rounding down)
    if (doNotInterpolate) {
        return s[lower];
    }

    if (timeDomain[lower] == time || lower == static_cast<int>(s.size()) - 1) return s[lower];

    if (s.size() == 1) return s[0];

    if (s.size() == 2) {
        double fraction = (time - timeDomain[lower]) / (timeDomain[lower + 1] - timeDomain[lower]);
        return s[lower] + fraction * (s[lower + 1] - s[lower]);
    }

    int first = std::max(0, std::min(lower - 1, static_cast<int>(s.size()) - 3));
    double result = 0;
    for (int i = first; i < first + 3; i++) {
        double weight = 1;
        for (int j = first; j < first + 3; j++) {
            if (i != j) weight *= (time - timeDomain[j]) / (timeDomain[i] - timeDomain[j]);
        }
        result += weight * s[i];
    }
    return result;
}

bool margaret1::EvolutionRecord::exportCSV(std::string filename) {
    std::ofstream csv;
    csv.open(filename);

    if (!csv.is_open()) return false;

    csv << "t,";
    for (int i = 0; i < variableNames.size(); i++) {
        csv << variableNames[i];
        if (i != variableNames.size() - 1) csv << ",";
    }
    csv << std::endl << std::fixed << std::setprecision(5);

    for (int i = 0; i < timeDomain.size(); i++) {
        csv << timeDomain[i] << ",";
        for (int j = 0; j < variableNames.size(); j++) {
            csv << variableSeries[j][i];
            if (j != variableNames.size() - 1) csv << ",";
        }
        csv << std::endl;
    }

    return true; 
}

bool margaret1::EvolutionRecord::exportPRN(std::string filename) {
    std::ofstream prn;
    prn.open(filename);

    if (!prn.is_open()) return false;

    prn << "t\t\t";
    for (int i = 0; i < variableNames.size(); i++) {
        prn << variableNames[i] << "\t\t";
    }
    prn << std::endl << std::fixed << std::setprecision(5);

    for (int i = 0; i < timeDomain.size(); i++) {
        prn << timeDomain[i] << "\t\t";
        for (int j = 0; j < variableNames.size(); j++) {
            prn << variableSeries[j][i] << "\t\t";
        }
        prn << std::endl;
    }

    return true;
}

// --- Implementation for margaret1::System class --- //
//
// Methods:
//      bool isKnown(std::string requirement);                              (tested)
//      bool isSolvable(Relation relation);                                 (tested)
//      void solve(Relation relation);                                      (tested)
//      void solveAll(std::vector<Relation> relations);                     (tested)
//      void evolve(Evolution evolution, double evoStep, double evoTime);   (tested)
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
    double val = relation.relation(this);
    setVal(relation.solves, val);
}

void margaret1::System::solveAll(std::vector<margaret1::Relation> relations) {
    std::vector<std::string> alreadySolved;
    int solveCount = 0;
    int previousSolveCount = 0;
    bool isAlreadySolved = false;
    while (true) {

        for (margaret1::Relation relation: relations) {
            if (isSolvable(&relation)) {
                for (std::string name: alreadySolved) if (relation.name == name) { isAlreadySolved = true; break; }
                if (isAlreadySolved) continue;

                std::cout << "\tSolvable relation: " << relation.name << "\t(requires ";
                for (std::string r: relation.requires) std::cout << r << ", ";
                std::cout << "solves for " << relation.solves << ")" << std::endl;
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
    std::cout << "[System] Starting system evolution ..." << std::endl << "\tSetting up ..."; std::cout.flush();
    if (!std::isfinite(evoStep) || evoStep <= 0 || !std::isfinite(evoTime) || evoTime < 0) {
        margaret1::margaretError("a margaret1 System", "Invalid evolution time or step size.", "Use a positive finite step size and a non-negative finite evolution time.");
    }
    int steps = (int)(evoTime / evoStep);
    std::vector<std::string> updateNames;
    std::vector<double> updateValues;
    margaret1::EvolutionRecord evoRecord;
    double currentTime = 0.0;
    std::vector<margaret1::Relation> orderedRelations;
    std::vector<bool> relationPending(evolution.associated.size(), true);

    while (orderedRelations.size() < evolution.associated.size()) {
        bool madeProgress = false;
        for (int i = 0; i < evolution.associated.size(); i++) {
            if (!relationPending[i]) continue;
            bool hasPendingRequirement = false;
            for (int j = 0; j < evolution.associated.size(); j++) {
                if (i == j || !relationPending[j]) continue;
                for (std::string requirement: evolution.associated[i].requires) {
                    if (requirement == evolution.associated[j].solves) {
                        hasPendingRequirement = true;
                        break;
                    }
                }
                if (hasPendingRequirement) break;
            }
            if (!hasPendingRequirement) {
                orderedRelations.push_back(evolution.associated[i]);
                relationPending[i] = false;
                madeProgress = true;
            }
        }
        if (!madeProgress) {
            margaret1::margaretError("a margaret1 System", "Associated evolution relations contain a dependency cycle.", "Remove the cycle between the properties they solve.");
        }
    }

    evoRecord.filled = (record.size() != 0);

    // Setup: get all the updating variables' names in place
    for (margaret1::TimeDerivRelation tdr: evolution.derivatives) {
        if (std::find(updateNames.begin(), updateNames.end(), tdr.solves) == updateNames.end()) {
            updateNames.push_back(tdr.solves);
            updateValues.push_back(0);
        }
    }

    // for (margaret1::Relation relation: orderedRelations) {
    //     if (!isSolvable(&relation)) margaret1::margaretError("a margaret1 System", "Cannot solve associated evolution relation", "Set its required properties before evolution.");
    //     solve(relation);
    // }
    solveAll(orderedRelations);

    if (record.size() != 0) {
        // Setup: get all the recording variables' names and series in place
        // and set up the EvolutionRecord's other data
        evoRecord.maxTime = steps * evoStep; evoRecord.stepSize = evoStep;
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

    std::cout << "done." << std::endl;

    for (int step = 0; step < steps; step++) {
        std::cout << "\r\tEvolution step " << step+1 << "/" << steps << "\t(" << (step+1)*100/steps << "%)"; std::cout.flush();
        // Figure out the time derivatives as specified in
        // the evolution object, and add the dx/dt dt 
        // elements to their respective properties.

        std::fill(updateValues.begin(), updateValues.end(), 0);
        for (margaret1::TimeDerivRelation tdr: evolution.derivatives) {
            if (!isSolvable(&tdr)) margaret1::margaretError("a margaret1 System", "Cannot solve evolution derivative", "Add more information before evolution attempt");
            for (int index = 0; index < updateNames.size(); index++) {
                if (updateNames[index] == tdr.solves) {
                    updateValues[index] += tdr.relation(this) * std::pow(evoStep, tdr.order) / (std::tgamma(tdr.order + 1));
                }
            }
        }

        for (int index = 0; index < updateNames.size(); index++) {
            setVal(updateNames[index], getVal(updateNames[index]) + updateValues[index]);
        }

        // Solve all of the associated relations to update
        // other system properties
        for (margaret1::Relation relation: orderedRelations) {
            solve(relation);
        }

        currentTime += evoStep;
        if (record.size() != 0) {
            // Record the values of the specified variables at this juncture
            evoRecord.timeDomain.push_back(currentTime);
            for (int index = 0; index < evoRecord.variableSeries.size(); index++) {
                evoRecord.variableSeries[index].push_back(getVal(record[index]));
            }
        }
    }

    std::cout << std::endl << "[System] System evolution complete." << std::endl;

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