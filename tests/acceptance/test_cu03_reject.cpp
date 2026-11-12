// Tarefa #20: CU-03 (rejectDueLackOfStock). Cenarios CT-03.1 a CT-03.4.
#include "Fixtures.h"
#include "ServiceOrderService.h"
#include "TestFramework.h"

using namespace fx;

TEST("CT-03.1 Recusa valida: Recusada por falta de estoque, sem orcamento, com historico") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);

    const std::time_t before = std::time(nullptr);
    auto r = service.rejectDueLackOfStock(stockroom(5), id);
    const std::time_t after = std::time(nullptr);
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
    CHECK(h.instant() >= before && h.instant() <= after);
}

TEST("CT-03.2 Recusa sem permissao (mecanico) e negada e a OS nao muda") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    const auto before = snapOf(repo, id);
    ServiceOrderService service(repo);
    auto r = service.rejectDueLackOfStock(mechanic(7), id);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::AcessDenied);
    CHECK(snapOf(repo, id) == before);
}

TEST("CT-03.2 Demais perfis tambem sao negados") {
    const User users[] = {customer(), manager()};
    for (const auto& u : users) {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        ServiceOrderService service(repo);
        auto r = service.rejectDueLackOfStock(u, id);
        REQUIRE(!r.has_value());
        CHECK(r.error() == OrderError::AcessDenied);
        CHECK(snapOf(repo, id).historySize == 0);
    }
}

TEST("CT-03.3 Transicao proibida: OS ja enviada ou ja recusada nao recebe novo historico") {
    const OrderStatus states[] = {OrderStatus::PendingAcceptance,
                                  OrderStatus::RejectedLackOfStock};
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

TEST("CT-03.4 OS inexistente retorna ServiceOrderNotFound sem alterar registros") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    const auto before = snapOf(repo, id);
    ServiceOrderService service(repo);
    auto r = service.rejectDueLackOfStock(stockroom(), 999);
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::ServiceOrderNotFound);
    CHECK(repo.listAll().size() == 1);
    CHECK(snapOf(repo, id) == before);
}
