// Tarefa #23: CU-04 (checkSO, checkSOById e listRejectedDueLackOfStock).
// Cenarios CT-04.1 a CT-04.11.
#include <algorithm>

#include "Fixtures.h"
#include "ServiceOrderService.h"
#include "TestFramework.h"

using namespace fx;

namespace {
bool contains(const std::vector<int>& v, int id) {
    return std::find(v.begin(), v.end(), id) != v.end();
}
}  // namespace

TEST("CT-04.1 Lista do gerente: todas as ordens, inclusive recusadas") {
    FakeRepository repo;
    const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);
    const int b = seed(repo, 11, 8, OrderStatus::PendingAcceptance);
    const int c = seed(repo, 12, 9, OrderStatus::RejectedLackOfStock);
    ServiceOrderService service(repo);
    auto r = service.checkSO(manager());
    REQUIRE(r.has_value());
    CHECK((ids(*r) == std::vector<int>{a, b, c}));
}

TEST("CT-04.2 Lista do cliente: so as proprias nao recusadas") {
    FakeRepository repo;
    const int own1 = seed(repo, 10, 7, OrderStatus::PendingBudget);
    const int own2 = seed(repo, 10, 8, OrderStatus::PendingAcceptance);
    const int ownRejected = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    const int other = seed(repo, 11, 7, OrderStatus::PendingBudget);
    ServiceOrderService service(repo);
    auto r = service.checkSO(customer(10));
    REQUIRE(r.has_value());
    const auto got = ids(*r);
    CHECK((got == std::vector<int>{own1, own2}));
    CHECK(!contains(got, ownRejected));
    CHECK(!contains(got, other));
}

TEST("CT-04.3 Lista do mecanico: so as que abriu, inclusive recusadas") {
    FakeRepository repo;
    const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);
    const int b = seed(repo, 11, 7, OrderStatus::PendingAcceptance);
    const int c = seed(repo, 12, 7, OrderStatus::RejectedLackOfStock);
    seed(repo, 10, 8, OrderStatus::PendingBudget);
    ServiceOrderService service(repo);
    auto r = service.checkSO(mechanic(7));
    REQUIRE(r.has_value());
    CHECK((ids(*r) == std::vector<int>{a, b, c}));
}

TEST("CT-04.4 Almoxarife na lista geral: acesso negado") {
    FakeRepository repo;
    seed(repo, 10, 7);
    ServiceOrderService service(repo);
    auto r = service.checkSO(stockroom());
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::AcessDenied);
}

TEST("CT-04.5 Numero visivel: cliente 10 consulta a propria OS nao recusada") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    ServiceOrderService service(repo);
    auto r = service.checkSOById(customer(10), id);
    REQUIRE(r.has_value());
    CHECK(r->id() == id);
    CHECK(r->clientId() == 10);
    CHECK(r->state() == OrderStatus::PendingAcceptance);
    CHECK(r->vehicle().plate() == "ABC1D23");
    CHECK(r->diagnosis() == "freios desgastados");
}

TEST("CT-04.5 checkSO(user, id) equivale a checkSOById") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);
    auto r = service.checkSO(customer(10), id);
    REQUIRE(r.has_value());
    CHECK(r->id() == id);
    CHECK(service.checkSO(customer(11), id).error() == OrderError::AcessDenied);
    CHECK(service.checkSO(manager(), 999).error() == OrderError::ServiceOrderNotFound);
}

TEST("CT-04.6 Numero nao visivel: OS de terceiro e OS propria recusada sao negadas") {
    FakeRepository repo;
    const int third = seed(repo, 11, 7, OrderStatus::PendingBudget);
    const int ownRejected = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    ServiceOrderService service(repo);

    auto r1 = service.checkSOById(customer(10), third);
    REQUIRE(!r1.has_value());
    CHECK(r1.error() == OrderError::AcessDenied);

    auto r2 = service.checkSOById(customer(10), ownRejected);
    REQUIRE(!r2.has_value());
    CHECK(r2.error() == OrderError::AcessDenied);
}

TEST("CT-04.7 Numero inexistente: ServiceOrderNotFound") {
    FakeRepository repo;
    seed(repo, 10, 7);
    ServiceOrderService service(repo);
    auto r = service.checkSOById(manager(), 999);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::ServiceOrderNotFound);
}

TEST("CT-04.8 Relacao de recusas: somente Recusada por falta de estoque") {
    FakeRepository repo;
    seed(repo, 10, 7, OrderStatus::PendingBudget);
    seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    const int r1 = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    const int r2 = seed(repo, 11, 8, OrderStatus::RejectedLackOfStock);
    ServiceOrderService service(repo);
    auto r = service.listRejectedDueLackOfStock(manager());
    REQUIRE(r.has_value());
    CHECK((ids(*r) == std::vector<int>{r1, r2}));
    for (const auto& so : *r) CHECK(so.state() == OrderStatus::RejectedLackOfStock);
}

TEST("CT-04.9 Recusas sem permissao: cliente e demais perfis sao negados") {
    const User users[] = {customer(), mechanic(), stockroom()};
    for (const auto& u : users) {
        FakeRepository repo;
        seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);
        auto r = service.listRejectedDueLackOfStock(u);
        REQUIRE(!r.has_value());
        CHECK(r.error() == OrderError::AcessDenied);
    }
}

TEST("CT-04.10 Somente leitura: consultas nao alteram estado, orcamento nem historico") {
    FakeRepository repo;
    const int pa = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    const int rj = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    const int pb = seed(repo, 10, 7, OrderStatus::PendingBudget);
    const auto s1 = snapOf(repo, pa);
    const auto s2 = snapOf(repo, rj);
    const auto s3 = snapOf(repo, pb);
    const std::time_t t1 = repo.findById(pa)->history()[0].instant();
    ServiceOrderService service(repo);

    const User users[] = {manager(), customer(10), mechanic(7), stockroom()};
    for (const auto& u : users) {
        service.checkSO(u);
        service.listRejectedDueLackOfStock(u);
        service.searchNewSO(u);
        for (int id : {pa, rj, pb, 999}) {
            service.checkSOById(u, id);
            service.checkSO(u, id);
        }
    }

    CHECK(repo.listAll().size() == 3);
    CHECK(snapOf(repo, pa) == s1);
    CHECK(snapOf(repo, rj) == s2);
    CHECK(snapOf(repo, pb) == s3);
    CHECK(repo.findById(pa)->history()[0].instant() == t1);
    REQUIRE(repo.findById(pa)->budget().has_value());
    CHECK_NEAR(repo.findById(pa)->budget()->total(), 1100.0);
}

TEST("CT-04.11 Numero pelo almoxarife: so OS em Orcamento pendente") {
    FakeRepository repo;
    const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);        // "OS 101" da spec
    const int b = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    const int c = seed(repo, 10, 7, OrderStatus::PendingAcceptance);    // "OS 103" da spec
    ServiceOrderService service(repo);

    auto ok = service.checkSOById(stockroom(), a);
    REQUIRE(ok.has_value());
    CHECK(ok->id() == a);

    auto denied = service.checkSOById(stockroom(), c);
    REQUIRE(!denied.has_value());
    CHECK(denied.error() == OrderError::AcessDenied);
    CHECK(service.checkSOById(stockroom(), b).error() == OrderError::AcessDenied);
}

TEST("CU-04 Mecanico consulta por id so as que abriu (inclusive recusada)") {
    FakeRepository repo;
    const int mine = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    const int others = seed(repo, 10, 8, OrderStatus::PendingBudget);
    ServiceOrderService service(repo);
    CHECK(service.checkSOById(mechanic(7), mine).has_value());
    auto r = service.checkSOById(mechanic(7), others);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::AcessDenied);
}

TEST("CU-04 Gerente consulta por id qualquer OS, inclusive recusada") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    ServiceOrderService service(repo);
    CHECK(service.checkSOById(manager(), id).has_value());
}
