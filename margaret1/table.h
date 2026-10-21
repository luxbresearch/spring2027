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

#include "err.h"
#include "system.h"
#include <cmath>
#include <fstream>
#include <vector>

// Macros used for briefly describing System equations; very convenient!
#define DCEQ(x) [](margaret1::DataColumn* dcptr, int index) { return x; }
#define X dcptr->getVal(index);

namespace margaret1 {

    // --- margaret1::DataColumn --- //
    //
    // The DataColumn class is just a dataclass to hold the
    // information stored in a column of a DataTable (as
    // the name suggests). It has a few basic methods incl.
    // finding rows where the given value is close enough
    // to a particular target value, finding all such rows,
    // etc.

    class DataColumn {

        public:

            std::vector<double> data;
            std::vector<std::string> names;

            DataColumn() {}
            DataColumn(std::vector<std::string> names): names(names) {}
            DataColumn(std::vector<std::string> names, std::vector<double> data): names(names), data(data) {}

            double getVal(int index);
            void setVal(int index, double newVal);

            int closest(double target, int dmin, int dmax = -1);
            std::vector<int> within(double target, double tolerableError, int dmin, int dmax=-1);
            DataColumn manipulate(double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1);
            bool hasName(std::string name);

    };

    class DataTable {

        public:

            std::vector<DataColumn> cols;

            DataTable() {}
            DataTable(std::vector<DataColumn> cols): cols(cols) {}

            DataColumn getCol(std::string name);
            double getVal(std::string name, int index);
            int closest(std::string name, double target, int dmin, int dmax = -1);
            std::vector<int> within(std::string name, double target, double tolerableError, int dmin, int dmax=-1);
            double getValWhereClosest(std::string nameToGet, std::string nameToSearch, double target, int dmin, int dmax=-1);
            std::vector<double> getValWhereWithin(std::string nameToGet, std::string nameToSearch, double target, double tolerableError, int dmin, int dmax=-1);
            void manipulate(std::string name, double std::function<double(DataColumn*, int)> manip, int dmin, int dmax=-1);
            void addCol(DataColumn col);
            void removeCol(std::string name);

    };

};