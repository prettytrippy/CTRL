#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <algorithm>

std::unordered_map<std::string, int> environment = {{"ctrl", 0}};

std::string binary_function(const std::string &func, const std::string &a, const std::string &b, std::vector<std::string> &cells) {
    int val_b = std::stoi(b);
    
    if (func == "Assign") {
        environment[a] = val_b;
        return b; 
    } 
    else if (func == "Input") {
        std::cout << "Input " << a << ": ";
        std::cin >> val_b;
        environment[a] = val_b;
        return "";
    }
    else if (func == "Print") {
        std::cout << a << " " << val_b << std::endl;
        return "";
    } 
    else if (func == "Read") {
        int index = val_b;
        if (index >= cells.size()) throw std::out_of_range("Index out of range in Read");
        environment[a] = std::stoi(cells[index]);
        return cells[index];
    } 
    else if (func == "Write") {
        int index = val_b;
        if (index >= cells.size()) cells.resize(index + 1, "0");
        cells[index] = a;
        return a;
    }

    // Arithmetic and logical operations
    int val_a = std::stoi(a);

    if (func == "Add") return std::to_string(val_a + val_b);
    if (func == "Sub") return std::to_string(val_a - val_b);
    if (func == "Mul") return std::to_string(val_a * val_b);
    if (func == "Div") return std::to_string(val_a / val_b);
    if (func == "Mod") return std::to_string(val_a % val_b);
    if (func == "Or") return std::to_string(val_a || val_b);
    if (func == "And") return std::to_string(val_a && val_b);
    if (func == "Xor") return std::to_string((val_a || val_b) && !(val_a && val_b));
    if (func == "Eq") return std::to_string(val_a == val_b);
    if (func == "Ne") return std::to_string(val_a != val_b);
    if (func == "Gt") return std::to_string(val_a > val_b);
    if (func == "Lt") return std::to_string(val_a < val_b);

    throw std::runtime_error("Unknown function: " + func);
}

std::pair<std::string, std::string> retrieve_arguments(const std::string &inner_expr) {
    int depth = 0;
    for (size_t i = 0; i < inner_expr.size(); ++i) {
        if (inner_expr[i] == '(') {
            depth++;
        } else if (inner_expr[i] == ')') {
            depth--;
        } else if (inner_expr[i] == ',' && depth == 0) {
            // Split into two parts
            std::string left = inner_expr.substr(0, i);
            std::string right = inner_expr.substr(i + 1);

            // Trim both parts
            left.erase(left.find_last_not_of(" \t") + 1);
            left.erase(0, left.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));

            return {left, right};
        }
    }

    throw std::runtime_error("Invalid argument structure: " + inner_expr);
}

std::string evaluate(const std::string &expr, std::vector<std::string> &cells) {
    std::string trimmed = expr;
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), ' '), trimmed.end());

    if (trimmed.empty() || trimmed[0] == '#') {
        return ""; // Skip comments or empty lines
    }

    // If the expression is not a function call
    if (trimmed.find('(') == std::string::npos) {
        if (trimmed[0] == '$') { // Variable reference
            return std::to_string(environment[trimmed.substr(1)]);
        } else {
            return trimmed; // Return as-is (e.g., a number or literal)
        }
    }

    // Handle function calls
    size_t open_paren_index = trimmed.find('(');
    std::string func_name = trimmed.substr(0, open_paren_index);
    std::string inner_expr = trimmed.substr(open_paren_index + 1, trimmed.size() - open_paren_index - 2);

    auto args = retrieve_arguments(inner_expr);

    std::string left = evaluate(args.first, cells);
    std::string right = evaluate(args.second, cells);

    return binary_function(func_name, left, right, cells);
}

void loop(std::vector<std::string> &cells) {
    while (environment["ctrl"] < cells.size()) {
        evaluate(cells[environment["ctrl"]], cells);
        environment["ctrl"]++;
    }
}

void interpret(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<std::string> cells;
    std::string line;
    while (std::getline(file, line)) {
        cells.push_back(line);
    }
    file.close();

    loop(cells);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    try {
        interpret(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
