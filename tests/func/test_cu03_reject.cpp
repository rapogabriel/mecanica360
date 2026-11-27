// CU-03 (rejectDueLackOfStock): Cenarios CT-03.1 a CT-03.4.
#include <catch2/catch_test_macros.hpp>
#include <ctime>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

SCENARIO("CT-03.1 - Recusa valida", "[func][cu03]") {
    GIVEN("uma OS em Orcamento pendente, sem orcamento, e o almoxarife constatou falta de pecas") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        ServiceOrderService service(repo);

        WHEN("o almoxarife confirma a recusa") {
            const std::time_t before = std::time(nullptr);
            auto r = service.rejectDueLackOfStock(stockroom(5), id);
            const std::time_t after = std::time(nullptr);

            THEN("a OS fica Recusada por falta de estoque, sem orcamento, e seu historico recebe "
                 "uma mudanca com responsavel e instante") {
                REQUIRE(r.has_value());

                auto so = repo.findById(id);
                REQUIRE(so.has_value());
                CHECK(so->state() == OrderStatus::RejectedLackOfStock);
                CHECK(!so->budget().has_value());
                REQUIRE(so->history().size() == 1);
                const auto& h = so->history()[0];
                CHECK(h.previousState() == OrderStatus::PendingBudget);
                CHECK(h.newState() == OrderStatus::RejectedLackOfStock);
                CHECK(h.responsibleId() == 5);
                CHECK(h.instant() >= before);
                CHECK(h.instant() <= after);
            }
        }
    }
}

SCENARIO("CT-03.2 - Recusa sem permissao", "[func][cu03]") {
    GIVEN("um mecanico autenticado que conhece uma OS em Orcamento pendente") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        const auto before = snapOf(repo, id);
        ServiceOrderService service(repo);

        WHEN("o mecanico tenta recusa-la por falta de estoque") {
            auto r = service.rejectDueLackOfStock(mechanic(7), id);

            THEN("a operacao e negada e estado e historico da OS nao mudam") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::AcessDenied);
                CHECK(snapOf(repo, id) == before);
            }
        }
    }
}

SCENARIO("CT-03.3 - Transicao proibida", "[func][cu03]") {
    GIVEN("um almoxarife autenticado e uma OS inicialmente em Pendente aceitacao ou ja Recusada "
          "por falta de estoque") {
        const OrderStatus states[] = {OrderStatus::PendingAcceptance,
                                      OrderStatus::RejectedLackOfStock};

        WHEN("o almoxarife solicita nova recusa") {
            THEN("a transicao e rejeitada e nao se acrescenta registro de historico") {
                for (OrderStatus s : states) {
                    FakeRepository repo;
                    const int id = seed(repo, 10, 7, s);
                    const auto before = snapOf(repo, id);
                    ServiceOrderService service(repo);
                    auto r = service.rejectDueLackOfStock(stockroom(), id);
                    REQUIRE(!r.has_value());
                    CHECK(r.error() == OrderError::InvalidTransition);
                    CHECK(snapOf(repo, id) == before);
                    CHECK(snapOf(repo, id).historySize == 1);
                }
            }
        }
    }
}

SCENARIO("CT-03.4 - OS inexistente", "[func][cu03]") {
    GIVEN("um almoxarife autenticado que indica numero que nao corresponde a qualquer OS") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        const auto before = snapOf(repo, id);
        ServiceOrderService service(repo);

        WHEN("ele solicita a recusa por falta de estoque") {
            auto r = service.rejectDueLackOfStock(stockroom(), 999);

            THEN("o sistema informa que a OS nao existe e nao altera qualquer registro") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::ServiceOrderNotFound);
                CHECK(repo.listAll().size() == 1);
                CHECK(snapOf(repo, id) == before);
            }
        }
    }
}