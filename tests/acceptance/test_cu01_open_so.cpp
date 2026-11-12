// Tarefa #16: CU-01 (openSO e validacoes). Cenarios CT-01.1 a CT-01.3.
#include <functional>
#include <ctime>

#include "Fixtures.h"
#include "ServiceOrderService.h"
#include "TestFramework.h"

using namespace fx;

TEST("CT-01.1 Abertura valida") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto id = service.openSO(mechanic(7), validOpening(10));
    REQUIRE(id.has_value());

    auto so = repo.findById(*id);
    REQUIRE(so.has_value());
    CHECK(so->id() == *id);
    CHECK(so->clientId() == 10);
    CHECK(so->mechanicId() == 7);
    CHECK(so->vehicle().plate() == "ABC1D23");
    CHECK(so->vehicle().model() == "Gol");
    CHECK(so->diagnosis() == "freios desgastados");
    REQUIRE(so->parts().size() == 1);
    CHECK(so->parts()[0].quantity() == 4);
    CHECK_NEAR(so->labor(), 300.0);
    CHECK(so->deadline() == 3);
    CHECK(so->state() == OrderStatus::PendingBudget);
    CHECK(!so->budget().has_value());
    CHECK(so->history().empty());
}

TEST("CT-01.2 Unicidade: dados iguais geram ordens com identificadores diferentes") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto a = service.openSO(mechanic(), validOpening());
    auto b = service.openSO(mechanic(), validOpening());
    REQUIRE(a.has_value());
    REQUIRE(b.has_value());
    CHECK(*a != *b);
    CHECK(repo.listAll().size() == 2);
}

TEST("CT-01.3 Dados invalidos sao recusados e nao produzem OS") {
    struct Case {
        const char* name;
        std::function<void(OpeningData&)> mutate;
    };
    const Case cases[] = {
        {"cliente zero", [](OpeningData& d) { d.clientId = 0; }},
        {"cliente negativo", [](OpeningData& d) { d.clientId = -3; }},
        {"placa vazia", [](OpeningData& d) { d.vehicle = Vehicle("", "Gol", 2015); }},
        {"placa so com espacos", [](OpeningData& d) { d.vehicle = Vehicle("  ", "Gol", 2015); }},
        {"diagnostico vazio", [](OpeningData& d) { d.diagnosis = ""; }},
        {"diagnostico so com espacos", [](OpeningData& d) { d.diagnosis = " \t"; }},
        {"sem pecas", [](OpeningData& d) { d.parts.clear(); }},
        {"peca sem descricao", [](OpeningData& d) { d.parts = {NecessaryPart("", 4)}; }},
        {"quantidade zero", [](OpeningData& d) { d.parts = {NecessaryPart("pastilha", 0)}; }},
        {"quantidade negativa", [](OpeningData& d) { d.parts = {NecessaryPart("pastilha", -1)}; }},
        {"segunda peca invalida",
         [](OpeningData& d) { d.parts.push_back(NecessaryPart("disco", 0)); }},
        {"mao de obra negativa", [](OpeningData& d) { d.labor = -1.0; }},
        {"prazo zero", [](OpeningData& d) { d.deadline = 0; }},
        {"prazo negativo", [](OpeningData& d) { d.deadline = -2; }},
    };
    for (const auto& c : cases) {
        FakeRepository repo;
        ServiceOrderService service(repo);
        OpeningData d = validOpening();
        c.mutate(d);
        auto r = service.openSO(mechanic(), d);
        if (r.has_value()) std::cerr << "    caso aceito indevidamente: " << c.name << "\n";
        REQUIRE(!r.has_value());
        CHECK(r.error() == OrderError::InvalidData);
        CHECK(repo.listAll().empty());
    }
}

TEST("CT-01.3 Mao de obra zero e aceita (nao negativa)") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    OpeningData d = validOpening();
    d.labor = 0.0;
    CHECK(service.openSO(mechanic(), d).has_value());
}

TEST("CU-01 Usuario sem papel de mecanico recebe AcessDenied e nenhuma OS e criada") {
    const User users[] = {customer(), stockroom(), manager()};
    for (const auto& u : users) {
        FakeRepository repo;
        ServiceOrderService service(repo);
        auto r = service.openSO(u, validOpening());
        REQUIRE(!r.has_value());
        CHECK(r.error() == OrderError::AcessDenied);
        CHECK(repo.listAll().empty());
    }
}

TEST("CU-01 Permissao e verificada antes dos dados") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    OpeningData d = validOpening();
    d.clientId = 0;
    auto r = service.openSO(manager(), d);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::AcessDenied);
}
