// CU-02 (searchNewSO e sendBudget): Cenarios CT-02.1 a CT-02.6.
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

SCENARIO("CT-02.1 - Pesquisa de OS pendentes", "[func][cu02]") {
    GIVEN("um almoxarife autenticado e tres ordens: Orcamento pendente, Pendente aceitacao e "
          "Recusada por falta de estoque") {
        FakeRepository repo;
        const int pending = seed(repo, 10, 7, OrderStatus::PendingBudget);
        seed(repo, 10, 7, OrderStatus::PendingAcceptance);
        seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);

        WHEN("o almoxarife consulta novas OS") {
            auto r = service.searchNewSO(stockroom());

            THEN("somente a OS em Orcamento pendente aparece") {
                REQUIRE(r.has_value());
                REQUIRE(r->size() == 1);
                CHECK((*r)[0].id() == pending);
                CHECK((*r)[0].state() == OrderStatus::PendingBudget);
            }
        }
    }
}

SCENARIO("CT-02.2 - Orcamento valido", "[func][cu02]") {
    GIVEN("um almoxarife autenticado e uma OS em Orcamento pendente com pecas R$ 800, mao de obra "
          "R$ 300 e prazo de tres dias") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        ServiceOrderService service(repo);

        WHEN("o almoxarife confirma o envio desse orcamento") {
            auto r = service.sendBudget(stockroom(5), id, Budget(800.0, 300.0, 3));

            THEN("a OS recebe orcamento total de R$ 1.100, passa a Pendente aceitacao e ganha um "
                 "registro de historico atribuido ao almoxarife") {
                REQUIRE(r.has_value());

                auto so = repo.findById(id);
                REQUIRE(so.has_value());
                CHECK(so->state() == OrderStatus::PendingAcceptance);
                REQUIRE(so->budget().has_value());
                CHECK(so->budget()->total() == Catch::Approx(1100.0));
                CHECK(so->budget()->deadline() == 3);
                REQUIRE(so->history().size() == 1);
                CHECK(so->history()[0].responsibleId() == 5);
                CHECK(so->history()[0].previousState() == OrderStatus::PendingBudget);
                CHECK(so->history()[0].newState() == OrderStatus::PendingAcceptance);
            }
        }
    }
}

SCENARIO("CT-02.3 - Perfil indevido", "[func][cu02]") {
    GIVEN("um gerente autenticado e uma OS pendente") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        const auto before = snapOf(repo, id);
        ServiceOrderService service(repo);

        WHEN("o gerente tenta consultar novas OS e, separadamente, enviar um orcamento") {
            auto search = service.searchNewSO(manager());
            auto send = service.sendBudget(manager(), id, Budget(800.0, 300.0, 3));

            THEN("as duas acoes sao negadas por falta de permissao e a OS permanece inalterada") {
                REQUIRE(!search.has_value());
                CHECK(search.error() == OrderError::AcessDenied);
                REQUIRE(!send.has_value());
                CHECK(send.error() == OrderError::AcessDenied);
                CHECK(snapOf(repo, id) == before);
            }
        }
    }
}

SCENARIO("CT-02.4 - Dados do orcamento invalidos", "[func][cu02]") {
    GIVEN("um almoxarife autenticado e uma OS em Orcamento pendente, com valor de pecas negativo, "
          "mao de obra negativa ou prazo zero") {
        const Budget invalid[] = {
            Budget(-1.0, 300.0, 3),
            Budget(800.0, -1.0, 3),
            Budget(800.0, 300.0, 0),
        };

        WHEN("o almoxarife tenta enviar cada orcamento") {
            THEN("cada envio e recusado e estado, orcamento e historico ficam iguais") {
                for (const auto& b : invalid) {
                    FakeRepository repo;
                    const int id = seed(repo, 10, 7);
                    const auto before = snapOf(repo, id);
                    ServiceOrderService service(repo);
                    auto r = service.sendBudget(stockroom(), id, b);
                    REQUIRE(!r.has_value());
                    CHECK(r.error() == OrderError::InvalidData);
                    CHECK(snapOf(repo, id) == before);
                }
            }
        }
    }
}

SCENARIO("CT-02.5 - Numero inexistente", "[func][cu02]") {
    GIVEN("um almoxarife autorizado e nenhuma OS de numero 999") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        const auto before = snapOf(repo, id);
        ServiceOrderService service(repo);

        WHEN("ele envia orcamento para a OS 999") {
            auto r = service.sendBudget(stockroom(), 999, Budget(800.0, 300.0, 3));

            THEN("o sistema informa ordem nao encontrada, sem criar ou modificar OS") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::ServiceOrderNotFound);
                CHECK(repo.listAll().size() == 1);
                CHECK(snapOf(repo, id) == before);
            }
        }
    }
}

SCENARIO("CT-02.6 - Estado incompativel", "[func][cu02]") {
    GIVEN("um almoxarife autenticado e uma OS ja em Pendente aceitacao ou Recusada por falta de "
          "estoque") {
        const OrderStatus states[] = {OrderStatus::PendingAcceptance,
                                      OrderStatus::RejectedLackOfStock};

        WHEN("o almoxarife tenta enviar-lhe outro orcamento") {
            THEN("o sistema impede a transicao e preserva os dados da OS") {
                for (OrderStatus s : states) {
                    FakeRepository repo;
                    const int id = seed(repo, 10, 7, s);
                    const auto before = snapOf(repo, id);
                    ServiceOrderService service(repo);
                    auto r = service.sendBudget(stockroom(), id, Budget(500.0, 100.0, 2));
                    REQUIRE(!r.has_value());
                    CHECK(r.error() == OrderError::InvalidTransition);
                    CHECK(snapOf(repo, id) == before);
                }
            }
        }
    }
}