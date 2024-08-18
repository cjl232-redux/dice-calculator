#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <unordered_map>
#include <vector>

void load_sum_counts(
    const std::vector<std::vector<int>>& faces,
    std::unordered_map<int, int>& sum_counts,
    int current_depth,
    int cumulative_value
) {
    if (current_depth + 1 == faces.size()) {
        for (const int value : faces[current_depth]) {
            sum_counts[cumulative_value + value]++;
        }
    } else {
        for (const int value : faces[current_depth]) {
            load_sum_counts(
                faces, 
                sum_counts, 
                current_depth + 1, 
                cumulative_value + value
            );
        }
    }
}

int main(const int argc, const char* const argv[]) {
    // Return an error if no arguments are supplied:
    if (argc == 1) {
        std::cout << "At least one argument is required." << std::endl;
        return EXIT_FAILURE;
    }
    // Otherwise check each argument and store the possible numbers:
    std::vector<std::vector<int>> faces;
    size_t maximum_roll = 0;
    for (int i = 0; i < argc - 1; i++) {
        std::string value(argv[i + 1]);
        std::smatch matches;
        std::regex pattern("^(0*[1-9]\\d*)?[dD](0*[1-9]\\d*)$");
        if (std::regex_match(value, matches, pattern)) {
            std::string count_string = std::string(matches[1]);
            std::string sides_string = std::string(matches[2]);
            int count = !count_string.empty() ? std::stoi(count_string) : 1;
            int sides = std::stoi(sides_string);
            for (int i = 0; i < count; i++) {
                faces.push_back(std::vector<int>(sides));
                std::iota(faces.back().begin(), faces.back().end(), 1);
            }
            maximum_roll += sides * count;
        } else {
            std::cout << "Error processing argument '" << value << "': ";
            std::cout << "arguments must be dice notations." << std::endl;
            return EXIT_FAILURE;
        }
    }
    // Work out how many times each possible sum occurs:
    std::unordered_map<int, int> sum_counts;
    load_sum_counts(faces, sum_counts, 0, 0);
    size_t total_combinations = 0;
    for (size_t i = faces.size(); i <= maximum_roll; i++) {
        total_combinations += sum_counts.at(i);
    }
    // Output the results to the console:
    for (size_t i = faces.size(); i <= maximum_roll; i++) {
        std::cout << i << ": " << sum_counts.at(i);
        char buffer[100];
        double proportion = 100.0 * sum_counts.at(i) / total_combinations;
        std::sprintf(buffer, " (%.1f\%)", proportion);
        std::cout << buffer << std::endl;
    }
    //Output the results to a file:
    std::string file_name = "output";
    for (int i = 0; i < argc - 1; i++) {
        file_name += "_" + std::string(argv[i + 1]);
    }
    file_name += ".csv";
    std::ofstream out(file_name);
    out << "Value,Frequency" << std::endl;
    for (size_t i = faces.size(); i <= maximum_roll; i++) {
        out << i << "," << sum_counts.at(i) << std::endl;
    }
    out.close();
    return EXIT_SUCCESS;
}