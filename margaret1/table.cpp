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

#include "table.h"
#include "system.h"

// Implementation for DataColumn
//
// Methods:
//      double getVal(int index);                               (test!)
//      double setVal(int index, double newVal);                (test!)
//      int closest(double target, int dmin, int dmax = -1);    (test!)
//      std::vector<int> within(double target, double tolerableError, int dmin, int dmax=-1);
//          (test!)
//      DataColumn manipulate(double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1);
//          (test!)
//      hasName(std::string name);                              (test!)

double margaret1::DataColumn::getVal(int index) {
    return data[index];
}

void margaret1::DataColumn::setVal(int index, double newVal) {
    data[index] = newVal;
}

int margaret1::DataColumn::closest(double target, int dmin, int dmax = -1) {
    int minIndex, currentIndex;
    double minError = 999999, error;

    // if dmax = -1, go to the end of the column
    if (dmax == -1) dmax = data.size() - 1;
    if (dmin < 0 || dmin >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmin out of range", "Use 0 < dmin < col.data.size()");
    if (dmax < 0 || dmax >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmax out of range", "Use 0 < dmax < col.data.size()");

    for (currentIndex = dmin; currentIndex <= dmax; currentIndex++) {
        error = std::abs(getVal(currentIndex) - target);
        if (error < minError) {
            minError = error;
            minIndex = currentIndex;
        }
    }

    return currentIndex;
}

std::vector<int> within(double target, double tolerableError, int dmin, int dmax=-1) {
    int currentIndex;
    std::vector<int> results;
    double error;

    // if dmax = -1, go to the end of the column
    if (dmax == -1) dmax = data.size() - 1;
    if (dmin < 0 || dmin >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmin out of range", "Use 0 < dmin < col.data.size()");
    if (dmax < 0 || dmax >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmax out of range", "Use 0 < dmax < col.data.size()");

    for (currentIndex = dmin; currentIndex <= dmax; currentIndex++) {
        error = std::abs(getVal(currentIndex) - target);
        if (error < tolerableError) results.push_back(currentIndex);
    }

    return results;
}

margaret1::DataColumn margaret1::DataColumn::manipulate(double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1) {
    int currentIndex;
    DataColumn newCol;

    // if dmax = -1, go to the end of the column
    if (dmax == -1) dmax = data.size() - 1;
    if (dmin < 0 || dmin >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmin out of range", "Use 0 < dmin < col.data.size()");
    if (dmax < 0 || dmax >= data.size()) margaret1::margaretError("a margaret1 DataColumn", "dmax out of range", "Use 0 < dmax < col.data.size()");

    for (currentIndex = dmin; currentIndex <= dmax; currentIndex++) {
        newCol.data.push_back(manip(this, currentIndex));
    }

    return newCol;
}

bool margaret1::DataColumn::hasName(std::string name) {
    for (std::string possible: names) {
        if (name == possible) {
            return true;
        }
    }
    return false;
}

// Implementation for margaret1::DataTable
//
// Methods:
//      DataColumn getCol(std::string name);
//      double getVal(std::string name, int index);
//      int closest(std::string name, double target, int dmin, int dmax = -1);
//      std::vector<int> within(std::string name, double target, double tolerableError, int dmin, int dmax=-1);
//      double getValWhereClosest(std::string nameToGet, std::string nameToSearch, double target, int dmin, int dmax=-1);
//      double getValWhereWithin(std::string nameToGet, std::string nameToSearch, double target, double tolerableError, int dmin, int dmax=-1);
//      void manipulate(std::string name, double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1);
//      void addCol(DataColumn col);
//      void removeCol(std::string name);

margaret1::DataColumn margaret1::DataTable::getCol(std::string name) {
    for (margaret1::DataColumn col: cols) {
        if (col.hasName(name)) return col;
    }
    margaret1::margaretError("a margaret1 DataTable", "No such data column with that name: " + name, "Check the column name and make sure it was added.");
    return margaret1::DataColumn();
}

double margaret1::DataTable::getVal(std::string name, int index) { 
    return getCol(name).getVal(index); 
}

int margaret1::DataTable::closest(std::string name, double target, int dmin, int dmax = -1) { 
    return getCol(name).closest(target, dmin, dmax); 
}

std::vector<int> margaret1::DataTable::within(std::string name, double target, double tolerableError, int dmin, int dmax=-1) { 
    return getCol(name).within(target, tolerableError, dmin, dmax); 
}

double margaret1::DataTable::getValWhereClosest(std::string nameToGet, std::string nameToSearch, double target, int dmin, int dmax=-1) {
    return getVal(nameToGet, getCol(nameToSearch).closest(target, dmin, dmax));
}

std::vector<double> margaret1::DataTable::getValWhereWithin(std::string nameToGet, std::string nameToSearch, double target, double tolerableError, int dmin, int dmax=-1) {
    std::vector<int> indices = getCol(nameToSearch).within(target, tolerableError, dmin, dmax);
    std::vector<double> results;
    margaret1::DataColumn readCol = getCol(nameToGet);
    for (int index: indices) results.push_back(readCol.getVal(index));
    return results;
}

void margaret1::DataTable::manipulate(std::string name, double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1) {
    margaret1::DataColumn col = getCol(nameToGet);
    removeCol(col.names[0]);
    addCol(col.manipulate(manip, dmin, dmax));
}

void margaret1::DataTable::addCol(DataColumn col) {
    cols.push_back(col);
}

void margaret1::DataTable::removeCol(std::string name) {
    for (auto it = cols.begin(); it != cols.end(); ++it) {
        if (it->hasName(name)) {
            cols.erase(it);
            break;
        }
    }
}