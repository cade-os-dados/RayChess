/*
    Script criado pelo copilot
*/

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <cstdlib>

int main() {
    std::set<std::string> headers;
    std::regex includeRegex(R"(import\s*<([^>]+)>;)");

    for (auto& p : std::filesystem::recursive_directory_iterator(".")) {
        if (p.path().extension() == ".cpp" || p.path().extension() == ".cppm") {
            std::ifstream in(p.path());
            std::string line;
            while (std::getline(in, line)) {
                std::smatch m;
                if (std::regex_search(line, m, includeRegex)) {
                    headers.insert(m[1]);
                }
            }
        }
    }

    for (auto& h : headers) {
        std::cout << "Precompilando <" << h << ">...\n";
        std::string cmd = "g++ -std=c++20 -fmodules-ts -xc++-system-header " + h;
        std::system(cmd.c_str());
    }
}