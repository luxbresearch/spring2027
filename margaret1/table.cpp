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
#include <sstream>

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

int margaret1::DataColumn::closest(double target, int dmin, int dmax) {
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

std::vector<int> margaret1::DataColumn::within(double target, double tolerableError, int dmin, int dmax) {
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

margaret1::DataColumn margaret1::DataColumn::manipulate(std::function<double(DataColumn*, int)> manip, int dmin, int dmax) {
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

int margaret1::DataTable::closest(std::string name, double target, int dmin, int dmax) { 
    return getCol(name).closest(target, dmin, dmax); 
}

std::vector<int> margaret1::DataTable::within(std::string name, double target, double tolerableError, int dmin, int dmax) { 
    return getCol(name).within(target, tolerableError, dmin, dmax); 
}

double margaret1::DataTable::getValWhereClosest(std::string nameToGet, std::string nameToSearch, double target, int dmin, int dmax) {
    return getVal(nameToGet, getCol(nameToSearch).closest(target, dmin, dmax));
}

std::vector<double> margaret1::DataTable::getValWhereWithin(std::string nameToGet, std::string nameToSearch, double target, double tolerableError, int dmin, int dmax) {
    std::vector<int> indices = getCol(nameToSearch).within(target, tolerableError, dmin, dmax);
    std::vector<double> results;
    margaret1::DataColumn readCol = getCol(nameToGet);
    for (int index: indices) results.push_back(readCol.getVal(index));
    return results;
}

void margaret1::DataTable::manipulate(std::string name, std::function<double(DataColumn*, int)> manip, int dmin, int dmax) {
    margaret1::DataColumn col = getCol(name);
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

margaret1::DataTable margaret1::DataTable::fromCSV(std::string filename, std::vector<std::string> names, int dmin, int dmax) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        margaret1::margaretError("a margaret1 DataTable", "Could not open CSV file: " + filename, "Check the file path and ensure it exists.");
    }

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<double> row;
        size_t start = 0;
        while (true) {
            size_t comma = line.find(',', start);
            std::string field = line.substr(start, comma == std::string::npos ? comma : comma - start);
            size_t parsed = 0;
            double value;
            try {
                value = std::stod(field, &parsed);
            } catch (...) {
                margaret1::margaretError("a margaret1 DataTable", "Invalid numeric value in CSV file: " + field, "Ensure the CSV contains numeric values.");
                return margaret1::DataTable();
            }
            if (field.find_first_not_of(" \t\r\n", parsed) != std::string::npos) {
                margaret1::margaretError("a margaret1 DataTable", "Invalid numeric value in CSV file: " + field, "Ensure the CSV contains numeric values.");
            }
            row.push_back(value);
            if (comma == std::string::npos) break;
            start = comma + 1;
        }
        if (row.size() != names.size()) {
            margaret1::margaretError("a margaret1 DataTable", "CSV column count does not match the supplied names", "Provide one name for each CSV column.");
        }
        rows.push_back(row);
    }

    if (rows.empty()) {
        margaret1::DataTable table;
        for (const std::string& name : names) {
            margaret1::DataColumn column;
            column.names.push_back(name);
            table.addCol(column);
        }
        return table;
    }

    if (dmax == -1) dmax = static_cast<int>(rows.size()) - 1;
    if (dmin < 0 || dmin >= static_cast<int>(rows.size()) || dmax < dmin || dmax >= static_cast<int>(rows.size())) {
        margaret1::margaretError("a margaret1 DataTable", "CSV row range out of range", "Use valid zero-based row indices for dmin and dmax.");
    }

    margaret1::DataTable table;
    for (size_t columnIndex = 0; columnIndex < names.size(); ++columnIndex) {
        margaret1::DataColumn column;
        column.names.push_back(names[columnIndex]);
        for (int rowIndex = dmin; rowIndex <= dmax; ++rowIndex) {
            column.data.push_back(rows[rowIndex][columnIndex]);
        }
        table.addCol(column);
    }
    return table;
}

void margaret1::DataTable::exportCSV(std::string filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        margaret1::margaretError("a margaret1 DataTable", "Could not open CSV file for writing: " + filename, "Check the file path and ensure it is writable.");
    }

    for (size_t i = 0; i < cols.size(); ++i) {
        file << cols[i].names[0];
        if (i < cols.size() - 1) file << ",";
    }
    file << "\n";

    size_t numRows = cols.empty() ? 0 : cols[0].data.size();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        for (size_t colIndex = 0; colIndex < cols.size(); ++colIndex) {
            file << cols[colIndex].data[rowIndex];
            if (colIndex < cols.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

margaret1::DataTable margaret1::DataTable::fromPRN(std::string filename, std::vector<std::string> names, int dmin, int dmax) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        margaret1::margaretError("a margaret1 DataTable", "Could not open PRN file: " + filename, "Check the file path and ensure it exists.");
    }

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::vector<double> row;
        std::string field;
        while (lineStream >> field) {
            size_t parsed = 0;
            double value;
            try {
                value = std::stod(field, &parsed);
            } catch (...) {
                margaret1::margaretError("a margaret1 DataTable", "Invalid numeric value in PRN file: " + field, "Ensure the PRN contains numeric values.");
                return margaret1::DataTable();
            }
            if (parsed != field.size()) {
                margaret1::margaretError("a margaret1 DataTable", "Invalid numeric value in PRN file: " + field, "Ensure the PRN contains numeric values.");
            }
            row.push_back(value);
        }
        if (row.empty()) continue;
        if (row.size() != names.size()) {
            margaret1::margaretError("a margaret1 DataTable", "PRN column count does not match the supplied names", "Provide one name for each PRN column.");
        }
        rows.push_back(row);
    }

    if (rows.empty()) {
        margaret1::DataTable table;
        for (const std::string& name : names) {
            margaret1::DataColumn column;
            column.names.push_back(name);
            table.addCol(column);
        }
        return table;
    }

    if (dmax == -1) dmax = static_cast<int>(rows.size()) - 1;
    if (dmin < 0 || dmin >= static_cast<int>(rows.size()) || dmax < dmin || dmax >= static_cast<int>(rows.size())) {
        margaret1::margaretError("a margaret1 DataTable", "PRN row range out of range", "Use valid zero-based row indices for dmin and dmax.");
    }

    margaret1::DataTable table;
    for (size_t columnIndex = 0; columnIndex < names.size(); ++columnIndex) {
        margaret1::DataColumn column;
        column.names.push_back(names[columnIndex]);
        for (int rowIndex = dmin; rowIndex <= dmax; ++rowIndex) {
            column.data.push_back(rows[rowIndex][columnIndex]);
        }
        table.addCol(column);
    }
    return table;

}

void margaret1::DataTable::exportPRN(std::string filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        margaret1::margaretError("a margaret1 DataTable", "Could not open PRN file for writing: " + filename, "Check the file path and ensure it is writable.");
    }

    size_t numRows = cols.empty() ? 0 : cols[0].data.size();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        for (size_t colIndex = 0; colIndex < cols.size(); ++colIndex) {
            file << cols[colIndex].data[rowIndex];
            if (colIndex < cols.size() - 1) file << " ";
        }
        file << "\n";
    }
    file.close();
}

margaret1::DataTable margaret1::DataTable::fromRelations(std::vector<Relation> relations, std::vector<std::pair<std::string, std::vector<double>>> knowns, std::vector<std::tuple<std::string, double, double>> targetDomains, double precision) {
    margaret1::DataTable table;
    for (const auto& [name, minVal, maxVal] : targetDomains) {
        margaret1::DataColumn column;
        column.names.push_back(name);
        for (double val = minVal; val <= maxVal; val += precision) {
            column.data.push_back(0);
        }
        table.addCol(column);
    }

    std::string name;
    std::vector<double> data;
    margaret1::DataColumn col;
    for (std::pair<std::string, std::vector<double>> p: knowns) {
        name = p.first; data = p.second; col = table.getCol(name);
        for (int i = 0; i < data.size(); i++) col.setVal(i, data[i]);
    }

    // Use Relation::suppose to get the values for each row in
    // the table
    for (size_t rowIndex = 0; rowIndex < table.cols[0].data.size(); ++rowIndex) {
        std::vector<std::pair<std::string, double>> knownValues;
        for (auto col : table.cols) {
            knownValues.push_back({col.names[0], col.data[rowIndex]});
        }

        for (auto relation: relations) {
            double result = relation.suppose(knownValues);
            // Find the corresponding column for the result
            for (auto col : table.cols) {
                if (col.hasName(relation.solves)) {
                    col.data[rowIndex] = result;
                    break;
                }
            }
        }
    }

    return table;
}

void margaret1::relationsToCSV(std::vector<Relation> relations, std::string filename, std::vector<std::pair<std::string, std::vector<double>>> knowns, std::vector<std::tuple<std::string, double, double>> targetDomains, double precision) {
    margaret1::DataTable::fromRelations(relations, knowns, targetDomains, precision).exportCSV(filename);
}