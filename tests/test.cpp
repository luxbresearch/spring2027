#include "/workspaces/spring2027/margaret1/system.h"
#include <iostream>

margaret1::Property myProp({"a", "b"});

int main() {
    myProp.setVal(1);
    std::cout << myProp.getVal() << std::endl;
}