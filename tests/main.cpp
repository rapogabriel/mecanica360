#include <exception>
#include <iostream>

#include "TestFramework.h"

int main() {
    int failed = 0;
    const auto& tests = tf::registry();
    for (const auto& t : tests) {
        tf::currentFailures() = 0;
        try {
            t.fn();
        } catch (const tf::FatalFailure&) {
        } catch (const std::exception& e) {
            ++tf::currentFailures();
            std::cerr << "    exceção inesperada: " << e.what() << "\n";
        }
        const bool ok = tf::currentFailures() == 0;
        std::cout << (ok ? "[ OK ] " : "[FALHA] ") << t.name << "\n";
        if (!ok) ++failed;
    }
    std::cout << "\n" << (tests.size() - failed) << "/" << tests.size()
              << " testes passaram\n";
    return failed == 0 ? 0 : 1;
}
