#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
using namespace std;

unordered_map<string, int> environment = {{"ctrl", 0}};

string binary_function(const string &func, const string &a, const string &b, vector<string> &cells) {
    // cout << "b: " << b << endl;
    int val_b = stoi(b);
    
    if (func == "Assign") {
        environment[a] = val_b;
        return b; 
    } 
    else if (func == "Input") {
        cout << "Input " << a << ": ";
        cin >> val_b;
        environment[a] = val_b;
        return "";
    }
    else if (func == "Print") {
        cout << a << " " << val_b << endl;
        return "";
    } 
    else if (func == "Read") {
        int index = val_b;
        if (index >= cells.size()) throw out_of_range("Index out of range in Read");
        // for (int i = 0; i < cells.size(); i++) {
        //     cout << "Cells: " << cells[i] << endl;
        // }
        // cout << "Environment" << endl;
        // for (const auto & [ key, value ] : environment) {
        //     cout << key << ": " << value << endl;
        // }
        // cout << "Read cells index: " << cells[index] << endl;
        environment[a] = stoi(cells[index]);
        return cells[index];
    } 
    else if (func == "Write") {
        // cout << "a: " << a << endl;
        int val_a = stoi(a);
        int index = val_b;
        if (index >= cells.size()) cells.resize(index + 10, "0");
        cells[index] = to_string(val_a);
        return a;
    }

    // cout << "a: " << a << endl;
    int val_a = stoi(a);

    if (func == "Add") return to_string(val_a + val_b);
    if (func == "Sub") return to_string(val_a - val_b);
    if (func == "Mul") return to_string(val_a * val_b);
    if (func == "Div") return to_string(val_a / val_b);
    if (func == "Mod") return to_string(val_a % val_b);
    if (func == "Or") return to_string(val_a || val_b);
    if (func == "And") return to_string(val_a && val_b);
    if (func == "Xor") return to_string((val_a || val_b) && !(val_a && val_b));
    if (func == "Eq") return to_string(val_a == val_b);
    if (func == "Ne") return to_string(val_a != val_b);
    if (func == "Gt") return to_string(val_a > val_b);
    if (func == "Lt") return to_string(val_a < val_b);

    throw runtime_error("Unknown function: " + func);
}

pair<string, string> retrieve_arguments(const string &inner_expr) {
    int depth = 0;
    for (size_t i = 0; i < inner_expr.size(); ++i) {
        if (inner_expr[i] == '(') {
            depth++;
        } else if (inner_expr[i] == ')') {
            depth--;
        } else if (inner_expr[i] == ',' && depth == 0) {

            string left = inner_expr.substr(0, i);
            string right = inner_expr.substr(i + 1);

            left.erase(left.find_last_not_of(" \t") + 1);
            left.erase(0, left.find_first_not_of(" \t"));
            right.erase(right.find_last_not_of(" \t") + 1);
            right.erase(0, right.find_first_not_of(" \t"));

            return {left, right};
        }
    }

    throw runtime_error("Invalid argument structure: " + inner_expr);
}

string evaluate(const string &expr, vector<string> &cells) {
    string trimmed = expr;
    trimmed.erase(remove(trimmed.begin(), trimmed.end(), ' '), trimmed.end());

    if (trimmed.empty() || trimmed[0] == '#') {
        return "";
    }

    if (trimmed.find('(') == string::npos) {
        if (trimmed[0] == '$') { 
            return to_string(environment[trimmed.substr(1)]);
        } else {
            return trimmed; 
        }
    }

    size_t open_paren_index = trimmed.find('(');
    string func_name = trimmed.substr(0, open_paren_index);
    string inner_expr = trimmed.substr(open_paren_index + 1, trimmed.size() - open_paren_index - 2);

    auto args = retrieve_arguments(inner_expr);

    string left = evaluate(args.first, cells);
    string right = evaluate(args.second, cells);

    return binary_function(func_name, left, right, cells);
}

void loop(vector<string> &cells) {
    while (environment["ctrl"] < cells.size()) {
        string result = evaluate(cells[environment["ctrl"]], cells);
        // cout << environment["ctrl"] << ":" << result << endl; 
        cout << environment["ctrl"] << " " << cells[environment["ctrl"]] << ": " << result << endl;
        environment["ctrl"]++;
    }
}

void interpret(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    vector<string> cells;
    string line;
    while (getline(file, line)) {
        cells.push_back(line);
    }
    file.close();

    loop(cells);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];

    try {
        interpret(filename);
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
