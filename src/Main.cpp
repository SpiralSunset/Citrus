#include "citrus/uci/UCI.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	if (argc >= 2) {
		if (std::string(argv[1]) == "uci") {
			UCI uci;
			uci.run();
		} else {
			std::cout << "Invalid argument" << std::endl;
		}
	} else {
		std::cout << "No arguments found" << std::endl;
	}
}
