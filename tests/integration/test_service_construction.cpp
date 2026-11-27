// Tarefa #13: ServiceOrderService (construtor e injecao do repositorio).
#include <catch2/catch_test_macros.hpp>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

TEST_CASE("Service: usa o repositorio injetado (le dados ja existentes)", "[service]") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);
    auto r = service.checkSOById(manager(), id);
    REQUIRE(r.has_value());
    CHECK(r->id() == id);
}

TEST_CASE("Service: grava no repositorio injetado", "[service]") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto id = service.openSO(mechanic(), validOpening());
    REQUIRE(id.has_value());
    CHECK(repo.findById(*id).has_value());
    CHECK(repo.listAll().size() == 1);
}

TEST_CASE("Service: dois servicos sobre o mesmo repositorio compartilham os dados", "[service]") {
    FakeRepository repo;
    ServiceOrderService a(repo);
    ServiceOrderService b(repo);
    auto id = a.openSO(mechanic(), validOpening());
    REQUIRE(id.has_value());
    CHECK(b.checkSOById(manager(), *id).has_value());
}

TEST_CASE("Service: servicos com repositorios distintos sao isolados", "[service]") {
    FakeRepository repo1;
    FakeRepository repo2;
    ServiceOrderService a(repo1);
    ServiceOrderService b(repo2);
    auto id = a.openSO(mechanic(), validOpening());
    REQUIRE(id.has_value());
    CHECK(repo2.listAll().empty());
    auto r = b.checkSOById(manager(), *id);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::ServiceOrderNotFound);
}