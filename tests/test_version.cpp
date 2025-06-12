#include <iostream>
#include <string>
#include <variant>
#include <vector>

int main() {
    std::variant<int, std::string> v = "Hello C++17";

    // 使用 std::holds_alternative
    if (std::holds_alternative<std::string>(v)) {
        std::cout << "Variant holds a string." << std::endl;
    }

    // 使用 std::get_if
    if (auto* s = std::get_if<std::string>(&v)) {
        std::cout << "The string is: " << *s << std::endl;
    }

    // 测试 if-initializer 和 auto
    if (auto it = std::vector<int>{1, 2, 3}.begin(); it != std::vector<int>{}.end()) {
         std::cout << "Auto and if-initializer work!" << std::endl;
    }

    std::cout << "C++17 test successful!" << std::endl;
    return 0;
}