// CU-04 (checkSO, checkSOById e listRejectedDueLackOfStock): Cenarios CT-04.1 a CT-04.11.
#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Fixtures.h"
#include "ServiceOrderService.h"

using namespace fx;

SCENARIO("CT-04.1 - Lista do gerente", "[func][cu04]") {
    GIVEN("um gerente autenticado e ordens de clientes e mecanicos diferentes nos estados "
          "pendente, enviada e recusada") {
        FakeRepository repo;
        const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);
        const int b = seed(repo, 11, 8, OrderStatus::PendingAcceptance);
        const int c = seed(repo, 12, 9, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);

        WHEN("o gerente solicita a listagem geral") {
            auto r = service.checkSO(manager());

            THEN("todas as ordens sao apresentadas, inclusive as recusadas") {
                REQUIRE(r.has_value());
                CHECK((ids(*r) == std::vector<int>{a, b, c}));
            }
        }
    }
}

SCENARIO("CT-04.2 - Lista do cliente", "[func][cu04]") {
    GIVEN("o cliente 10 com duas OS proprias nao recusadas, uma recusada e uma OS de outro "
          "cliente") {
        FakeRepository repo;
        const int own1 = seed(repo, 10, 7, OrderStatus::PendingBudget);
        const int own2 = seed(repo, 10, 8, OrderStatus::PendingAcceptance);
        const int ownRejected = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        const int other = seed(repo, 11, 7, OrderStatus::PendingBudget);
        ServiceOrderService service(repo);

        WHEN("o cliente 10 solicita sua lista") {
            auto r = service.checkSO(customer(10));

            THEN("so aparecem as duas OS proprias nao recusadas; nem a recusada nem as alheias "
                 "sao reveladas") {
                REQUIRE(r.has_value());
                const auto got = ids(*r);
                CHECK((got == std::vector<int>{own1, own2}));
                CHECK(!std::ranges::contains(got, ownRejected));
                CHECK(!std::ranges::contains(got, other));
            }
        }
    }
}

SCENARIO("CT-04.3 - Lista do mecanico", "[func][cu04]") {
    GIVEN("o mecanico 7 que abriu ordens em estados diferentes e outro mecanico que abriu outras "
          "OS") {
        FakeRepository repo;
        const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);
        const int b = seed(repo, 11, 7, OrderStatus::PendingAcceptance);
        const int c = seed(repo, 12, 7, OrderStatus::RejectedLackOfStock);
        seed(repo, 10, 8, OrderStatus::PendingBudget);
        ServiceOrderService service(repo);

        WHEN("o mecanico 7 solicita sua lista") {
            auto r = service.checkSO(mechanic(7));

            THEN("a lista traz somente as OS abertas pelo mecanico 7, inclusive as recusadas") {
                REQUIRE(r.has_value());
                CHECK((ids(*r) == std::vector<int>{a, b, c}));
            }
        }
    }
}

SCENARIO("CT-04.4 - Almoxarife na lista geral", "[func][cu04]") {
    GIVEN("um almoxarife autenticado e ordens registradas") {
        FakeRepository repo;
        seed(repo, 10, 7);
        ServiceOrderService service(repo);

        WHEN("ele solicita a lista geral de acompanhamento") {
            auto r = service.checkSO(stockroom());

            THEN("o sistema nega o acesso a listagem geral") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::AcessDenied);
            }
        }
    }
}

SCENARIO("CT-04.5 - Numero visivel", "[func][cu04]") {
    GIVEN("uma OS do cliente 10, nao recusada") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
        ServiceOrderService service(repo);

        WHEN("o cliente 10 consulta seu numero") {
            auto r = service.checkSOById(customer(10), id);

            THEN("o sistema apresenta dados e estado dessa OS") {
                REQUIRE(r.has_value());
                CHECK(r->id() == id);
                CHECK(r->clientId() == 10);
                CHECK(r->state() == OrderStatus::PendingAcceptance);
                CHECK(r->vehicle().plate() == "ABC1D23");
                CHECK(r->diagnosis() == "freios desgastados");
            }
        }
    }
}

SCENARIO("CT-04.6 - Numero nao visivel", "[func][cu04]") {
    GIVEN("um cliente que conhece o numero de uma OS de terceiro e da propria OS recusada por "
          "estoque") {
        FakeRepository repo;
        const int third = seed(repo, 11, 7, OrderStatus::PendingBudget);
        const int ownRejected = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);

        WHEN("ele consulta cada numero em tentativas separadas") {
            auto r1 = service.checkSOById(customer(10), third);
            auto r2 = service.checkSOById(customer(10), ownRejected);

            THEN("o sistema nega acesso aos detalhes em ambas as situacoes") {
                REQUIRE(!r1.has_value());
                CHECK(r1.error() == OrderError::AcessDenied);
                REQUIRE(!r2.has_value());
                CHECK(r2.error() == OrderError::AcessDenied);
            }
        }
    }
}

SCENARIO("CT-04.7 - Numero inexistente", "[func][cu04]") {
    GIVEN("um gerente autenticado e nenhuma OS de numero 999") {
        FakeRepository repo;
        seed(repo, 10, 7);
        ServiceOrderService service(repo);

        WHEN("o gerente consulta o numero 999") {
            auto r = service.checkSOById(manager(), 999);

            THEN("o sistema informa OS nao encontrada") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::ServiceOrderNotFound);
            }
        }
    }
}

SCENARIO("CT-04.8 - Relacao de recusas", "[func][cu04]") {
    GIVEN("um gerente autenticado e ordens recusadas e em outros estados") {
        FakeRepository repo;
        seed(repo, 10, 7, OrderStatus::PendingBudget);
        seed(repo, 10, 7, OrderStatus::PendingAcceptance);
        const int r1 = seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        const int r2 = seed(repo, 11, 8, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);

        WHEN("o gerente solicita a lista especifica de recusas por estoque") {
            auto r = service.listRejectedDueLackOfStock(manager());

            THEN("somente ordens no estado Recusada por falta de estoque sao apresentadas") {
                REQUIRE(r.has_value());
                CHECK((ids(*r) == std::vector<int>{r1, r2}));
                for (const auto& so : *r) CHECK(so.state() == OrderStatus::RejectedLackOfStock);
            }
        }
    }
}

SCENARIO("CT-04.9 - Recusas sem permissao", "[func][cu04]") {
    GIVEN("um cliente autenticado") {
        FakeRepository repo;
        seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
        ServiceOrderService service(repo);

        WHEN("o cliente realiza a consulta especifica de ordens recusadas") {
            auto r = service.listRejectedDueLackOfStock(customer());

            THEN("o sistema nega a consulta por falta de permissao") {
                REQUIRE(!r.has_value());
                CHECK(r.error() == OrderError::AcessDenied);
            }
        }
    }
}

SCENARIO("CT-04.10 - Somente leitura", "[func][cu04]") {
    GIVEN("uma OS que ja possui estado, orcamento e historico previamente registrados") {
        FakeRepository repo;
        const int id = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
        const auto before = snapOf(repo, id);
        const std::time_t instant = repo.findById(id)->history()[0].instant();
        ServiceOrderService service(repo);

        WHEN("um usuario autorizado consulta listas e detalhes dessa OS") {
            (void)service.checkSO(manager());
            (void)service.checkSOById(manager(), id);
            (void)service.checkSO(customer(10));
            (void)service.checkSOById(customer(10), id);

            THEN("todos os dados permanecem iguais apos a consulta") {
                CHECK(repo.listAll().size() == 1);
                CHECK(snapOf(repo, id) == before);
                CHECK(repo.findById(id)->history()[0].instant() == instant);
                REQUIRE(repo.findById(id)->budget().has_value());
                CHECK(repo.findById(id)->budget()->total() == Catch::Approx(1100.0));
            }
        }
    }
}

SCENARIO("CT-04.11 - Numero pelo almoxarife", "[func][cu04]") {
    GIVEN("um almoxarife autenticado e uma OS em Orcamento pendente e outra em Pendente "
          "aceitacao") {
        FakeRepository repo;
        const int pendingBudget = seed(repo, 10, 7, OrderStatus::PendingBudget);
        const int pendingAcceptance = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
        ServiceOrderService service(repo);

        WHEN("o almoxarife tenta consultar cada identificador") {
            auto ok = service.checkSOById(stockroom(), pendingBudget);
            auto denied = service.checkSOById(stockroom(), pendingAcceptance);

            THEN("o sistema exibe apenas a OS em Orcamento pendente e nega acesso a OS em "
                 "Pendente aceitacao") {
                REQUIRE(ok.has_value());
                CHECK(ok->id() == pendingBudget);
                REQUIRE(!denied.has_value());
                CHECK(denied.error() == OrderError::AcessDenied);
            }
        }
    }
}