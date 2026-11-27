// Matriz de permissoes por perfil (Customer, Mechanic, Stockroom, Manager).
#include <catch2/catch_test_macros.hpp>
#include <limits>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

namespace {
bool denied(const auto& r) { return !r.has_value() && r.error() == OrderError::AcessDenied; }
}  // namespace

TEST_CASE("Permissoes: openSO somente Mecanico", "[permissions]") {
    for (const User& u : {customer(), stockroom(), manager()}) {
        FakeRepository repo;
        ServiceOrderService service(repo);
        CHECK(denied(service.openSO(u, validOpening())));
        CHECK(repo.listAll().empty());
    }
    FakeRepository repo;
    ServiceOrderService service(repo);
    CHECK(service.openSO(mechanic(), validOpening()).has_value());
}

TEST_CASE("Permissoes: searchNewSO, sendBudget e rejectDueLackOfStock somente Almoxarife",
          "[permissions]") {
    for (const User& u : {customer(), mechanic(), manager()}) {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        ServiceOrderService service(repo);
        CHECK(denied(service.searchNewSO(u)));
        CHECK(denied(service.sendBudget(u, id, Budget(1, 1, 1))));
        CHECK(denied(service.rejectDueLackOfStock(u, id)));
        CHECK(snapOf(repo, id).state == OrderStatus::PendingBudget);
    }
}

TEST_CASE("Permissoes: permissao e verificada antes de dados invalidos e de existencia",
          "[permissions]") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    CHECK(denied(service.sendBudget(manager(), 999, Budget(-1, -1, 0))));
    CHECK(denied(service.rejectDueLackOfStock(customer(), 999)));
    OpeningData bad = validOpening();
    bad.clientId = 0;
    CHECK(denied(service.openSO(manager(), bad)));
}

TEST_CASE("Permissoes: dados invalidos sao verificados antes da existencia da OS",
          "[permissions]") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto r = service.sendBudget(stockroom(), 999, Budget(-1, 300, 3));
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::InvalidData);
}

TEST_CASE("Permissoes: lista geral - Gerente, Cliente e Mecanico sim; Almoxarife nao",
          "[permissions]") {
    FakeRepository repo;
    seed(repo, 10, 7);
    ServiceOrderService service(repo);
    CHECK(service.checkSO(manager()).has_value());
    CHECK(service.checkSO(customer(10)).has_value());
    CHECK(service.checkSO(mechanic(7)).has_value());
    CHECK(denied(service.checkSO(stockroom())));
}

TEST_CASE("Permissoes: lista de recusadas somente Gerente", "[permissions]") {
    for (const User& u : {customer(), mechanic(), stockroom()}) {
        FakeRepository repo;
        ServiceOrderService service(repo);
        CHECK(denied(service.listRejectedDueLackOfStock(u)));
    }
    FakeRepository repo;
    ServiceOrderService service(repo);
    CHECK(service.listRejectedDueLackOfStock(manager()).has_value());
}

TEST_CASE("Permissoes: cliente e mecanico sem OS proprias recebem lista vazia", "[permissions]") {
    FakeRepository repo;
    seed(repo, 10, 7);
    ServiceOrderService service(repo);
    CHECK(service.checkSO(customer(99)).value().empty());
    CHECK(service.checkSO(mechanic(99)).value().empty());
}

TEST_CASE("Permissoes: valores limite do orcamento e da abertura (zero aceito, NaN/negativo recusado)",
          "[permissions]") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    CHECK(service.sendBudget(stockroom(), id, Budget(nan, 1, 1)).error() == OrderError::InvalidData);
    CHECK(service.sendBudget(stockroom(), id, Budget(1, nan, 1)).error() == OrderError::InvalidData);
    CHECK(snapOf(repo, id).state == OrderStatus::PendingBudget);
    OpeningData d = validOpening();
    d.labor = nan;
    CHECK(service.openSO(mechanic(), d).error() == OrderError::InvalidData);
}