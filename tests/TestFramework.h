#pragma once

// Mini-framework de testes (sem dependências externas).
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace tf {

struct TestCase {
    std::string name;
    void (*fn)();
};

struct FatalFailure {};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& currentFailures() {
    static int n = 0;
    return n;
}

struct Registrar {
    Registrar(const char* name, void (*fn)()) { registry().push_back({name, fn}); }
};

inline void report(const char* kind, const char* expr, const char* file, int line) {
    ++currentFailures();
    std::cerr << "    " << kind << " falhou: " << expr << "  (" << file << ":" << line
              << ")\n";
}

}  // namespace tf

#define TF_CAT2(a, b) a##b
#define TF_CAT(a, b) TF_CAT2(a, b)

#define TEST(label)                                                                  \
    static void TF_CAT(tf_fn_, __LINE__)();                                          \
    static ::tf::Registrar TF_CAT(tf_reg_, __LINE__)(label, &TF_CAT(tf_fn_, __LINE__)); \
    static void TF_CAT(tf_fn_, __LINE__)()

/// Verificação não fatal.
#define CHECK(expr) \
    do { if (!(expr)) ::tf::report("CHECK", #expr, __FILE__, __LINE__); } while (0)

/// Verificação fatal: interrompe o teste corrente.
#define REQUIRE(expr)                                                      \
    do {                                                                   \
        if (!(expr)) {                                                     \
            ::tf::report("REQUIRE", #expr, __FILE__, __LINE__);            \
            throw ::tf::FatalFailure{};                                    \
        }                                                                  \
    } while (0)

#define CHECK_NEAR(a, b) CHECK(std::fabs((a) - (b)) < 1e-9)

/// Verifica que a expressao lanca a excecao indicada.
#define CHECK_THROWS(expr, ExcType)                                          \
    do {                                                                     \
        bool tf_threw = false;                                               \
        try { (void)(expr); } catch (const ExcType&) { tf_threw = true; }    \
        if (!tf_threw) ::tf::report("CHECK_THROWS", #expr, __FILE__, __LINE__); \
    } while (0)
