// CU-01 (openSO): Cenarios CT-01.1 a CT-01.3.
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

SCENARIO("CT-01.1 - Abertura valida", "[func][cu01]") {
    GIVEN("mecanico autenticado, cliente 10, veiculo Gol (placa ABC1D23), diagnostico freios "
          "desgastados, quatro pastilhas, mao de obra R$ 300 e prazo de tres dias") {
        FakeRepository repo;
        ServiceOrderService service(repo);

        WHEN("o mecanico confirma a abertura") {
            auto id = service.openSO(mechanic(7), validOpening(10));

            THEN("a OS recebe identificador unico, registra dados e responsavel, fica em "
                 "Orcamento pendente, sem orcamento e com historico vazio") {
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
                CHECK(so->labor() == Catch::Approx(300.0));
                CHECK(so->deadline() == 3);
                CHECK(so->state() == OrderStatus::PendingBudget);
                CHECK(!so->budget().has_value());
                CHECK(so->history().empty());
            }
        }
    }
}

SCENARIO("CT-01.2 - Unicidade", "[func][cu01]") {
    GIVEN("dois conjuntos independentes de informacoes validas, ainda que iguais") {
        FakeRepository repo;
        ServiceOrderService service(repo);

        WHEN("o mecanico abre uma OS para cada atendimento") {
            auto a = service.openSO(mechanic(), validOpening());
            auto b = service.openSO(mechanic(), validOpening());

            THEN("o sistema registra duas ordens com identificadores diferentes") {
                REQUIRE(a.has_value());
                REQUIRE(b.has_value());
                CHECK(*a != *b);
                CHECK(repo.listAll().size() == 2);
            }
        }
    }
}

SCENARIO("CT-01.3 - Dados invalidos", "[func][cu01]") {
    GIVEN("tentativas independentes com cliente nao positivo, placa ou diagnostico vazios, "
          "ausencia de pecas, peca sem descricao, quantidade zero, mao de obra negativa ou prazo "
          "zero") {
        struct Case {
            const char* name;
            std::function<void(OpeningData&)> mutate;
        };
        const Case cases[] = {
            {"cliente zero", [](OpeningData& d) { d.clientId = 0; }},
            {"cliente negativo", [](OpeningData& d) { d.clientId = -3; }},
            {"placa vazia", [](OpeningData& d) { d.vehicle = Vehicle("", "Gol", 2015); }},
            {"diagnostico vazio", [](OpeningData& d) { d.diagnosis = ""; }},
            {"ausencia de pecas", [](OpeningData& d) { d.parts.clear(); }},
            {"peca sem descricao", [](OpeningData& d) { d.parts = {NecessaryPart("", 4)}; }},
            {"quantidade zero", [](OpeningData& d) { d.parts = {NecessaryPart("pastilha", 0)}; }},
            {"mao de obra negativa", [](OpeningData& d) { d.labor = -1.0; }},
            {"prazo zero", [](OpeningData& d) { d.deadline = 0; }},
        };

        WHEN("o mecanico confirma cada solicitacao") {
            THEN("cada solicitacao invalida e recusada e nao produz nova ordem") {
                for (const auto& c : cases) {
                    INFO("Caso testado: " << c.name);
                    FakeRepository repo;
                    ServiceOrderService service(repo);
                    OpeningData d = validOpening();
                    c.mutate(d);
                    auto r = service.openSO(mechanic(), d);
                    REQUIRE(!r.has_value());
                    CHECK(r.error() == OrderError::InvalidData);
                    CHECK(repo.listAll().empty());
                }
            }
        }
    }
}