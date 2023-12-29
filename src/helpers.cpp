
#include "./headers/helpers.h"

void
remove_trailing_whitespaces(std::string& str) {
	while (std::isspace(str.back())) str.pop_back();
}

std::size_t 
get_ranged_input_inclusive(std::size_t low, std::size_t high) {

	#define DEFAULT_VALUE (low - 1)

	assert(high >= low && "INVALID RANGE");

	std::size_t answer{};
	do {
		std::cin >> std::ws >> answer;
		if (std::cin.fail()) {
			std::cerr << "Invalid input! Try again: ";
			std::cin.clear();
			/*
				Assign answer an "impossible" value to prevent uncorrect comparsion.
				In above, answer is initizlized with value by default. If our range
				contains 0 (default value for answer) and we try to intialize it with uncorrect input,
				like char, or string, it's value left the same, but condition evaluates to true 
				what is unocorrect.
			*/
			answer = DEFAULT_VALUE;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	} while (answer < low || answer > high);

	return answer;
}

void
print_programm_name() {
	std::cout << "             _            \n";
	std::cout << "            | |           \n";
	std::cout << " _ __   ___ | |_ ___  ___   \n";
	std::cout << "| '_ \\ / _ \\| __/ _ \\/ __|\n";
	std::cout << "| | | | (_) | ||  __/\\__ \\\n";
	std::cout << "|_| |_|\\___/ \\__\\___||___/\n";
}