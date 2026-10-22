#include "../margaret1/system.h"
#include "../margaret1/table.h"

using namespace margaret1;
using namespace std;

int main() {
    DataColumn a({"a"}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    DataColumn b({"b"}, {0, 2, 3, 4, 5, 10, 12, 14, 16, 18});
    DataTable table({a, b});
    table.exportCSV("test.csv");

    auto table2 = DataTable::fromCSV("test.csv", {"a", "b"}, 0, -1);
    margaretData("Closest to 1.2", {table2.closest("b", 1.2, 0, -1)});

    return 0;
}