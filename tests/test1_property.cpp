#include "/workspaces/spring2027/margaret1/system.h"
#include <iostream>

margaret1::Property myProp({"a", "b"});

int main() {
    myProp.setVal(1);
    std::cout << myProp.getVal() << std::endl;
    std::cout << myProp.hasName("a") << " " << myProp.hasName("x") << std::endl;
    myProp.setUnknown();
    std::cout << myProp.getVal() << std::endl;
}