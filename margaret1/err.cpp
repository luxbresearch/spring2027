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
#include <iostream>

void margaret1::margaretError(std::string source, std::string message, std::string solve) {
    std::cout << "[!!!] Margaret 1 encountered an unrecoverable error" << std::endl;
    std::cout << " -->  From:\t" << source << std::endl;
    std::cout << " -->  Error:\t" << message << std::endl;
    std::cout << " -->  Try:\t" << solve << std::endl;
    std::cout << "[!!!] Program cannot progress :(" << std::endl;
    throw std::runtime_error("Unrecoverable Margaret error");
}