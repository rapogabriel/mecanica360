// Tarefa #18: CU-02 (searchNewSO e sendBudget). Cenarios CT-02.1 a CT-02.6.
#include "Fixtures.h"
#include "ServiceOrderService.h"
#include "TestFramework.h"

using namespace fx;

TEST("CT-02.1 Pesquisa de OS pendentes retorna so Orcamento pendente") {
    FakeRepository repo;
    const int pending = seed(repo, 10, 7, OrderStatus::PendingBudget);
    seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    seed(repo, 10, 7, OrderStatus::RejectedLackOfStock);
    ServiceOrderService service(repo);

    auto r = service.searchNewSO(stockroom());
    REQUIRE(r.has_value());
    REQUIRE(r->size() == 1);
    CHECK((*r)[0].id() == pending);
    CHECK((*r)[0].state() == OrderStatus::PendingBudget);
}

TEST("CT-02.1 Sem OS pendentes retorna lista vazia") {
    FakeRepository repo;
    seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    ServiceOrderService service(repo);
    auto r = service.searchNewSO(stockroom());
    REQUIRE(r.has_value());
    CHECK(r->empty());
}

TEST("CT-02.2 Orcamento valido: total 1100, Pendente aceitacao e historico do almoxarife") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);

    auto r = service.sendBudget(stockroom(5), id, Budget(800.0, 300.0, 3));
    REQUIRE(r.has_value());

    auto so = repo.findById(id);
    REQUIRE(so.has_value());
    CHECK(so->state() == OrderStatus::PendingAcceptance);
    REQUIRE(so->budget().has_value());
    CHECK_NEAR(so->budget()->total(), 1100.0);
    CHECK(so->budget()->deadline() == 3);
    REQUIRE(so->history().size() == 1);
    CHECK(so->history()[0].responsibleId() == 5);
    CHECK(so->history()[0].previousState() == OrderStatus::PendingBudget);
    CHECK(so->history()[0].newState() == OrderStatus::PendingAcceptance);
}

TEST("CT-02.3 Perfil indevido (gerente) e negado nas duas acoes e a OS nao muda") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    const auto before = snapOf(repo, id);
    ServiceOrderService service(repo);

    auto search = service.searchNewSO(manager());
    REQUIRE(!search.has_value());
    CHECK(search.error() == OrderError::AcessDenied);

    auto send = service.sendBudget(manager(), id, Budget(800.0, 300.0, 3));
    REQUIRE(!send.has_value());
    CHECK(send.error() == OrderError::AcessDenied);
    CHECK(snapOf(repo, id) == before);
}

TEST("CT-02.3 Outros perfis tambem sao negados") {
    const User users[] = {customer(), mechanic()};
    for (const auto& u : users) {
        FakeRepository repo;
        const int id = seed(repo, 10, 7);
        ServiceOrderService service(repo);
        CHECK(service.searchNewSO(u).error() == OrderError::AcessDenied);
        CHECK(service.sendBudget(u, id, Budget(1, 1, 1)).error() == OrderError::AcessDenied);
        CHECK(snapOf(repo, id).state == OrderStatus::PendingBudget);
    }
}

TEST("CT-02.4 Dados do orcamento invalidos sao recusados sem alterar a OS") {
    const Budget invalid[] = {Budget(-1.0, 300.0, 3), Budget(800.0, -1.0, 3),
                              Budget(800.0, 300.0, 0), Budget(800.0, 300.0, -1)};
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

TEST("CT-02.4 Valores zero para pecas e mao de obra sao aceitos") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrderService service(repo);
    CHECK(service.sendBudget(stockroom(), id, Budget(0.0, 0.0, 1)).has_value());
}

TEST("CT-02.5 Numero inexistente retorna ServiceOrderNotFound sem modificar OS") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    const auto before = snapOf(repo, id);
    ServiceOrderService service(repo);
    auto r = service.sendBudget(stockroom(), 999, Budget(800.0, 300.0, 3));
    REQUIRE(!r.has_value());
    CHECK(r.error() == OrderError::ServiceOrderNotFound);
    CHECK(repo.listAll().size() == 1);
    CHECK(snapOf(repo, id) == before);
}

TEST("CT-02.6 Estado incompativel: Pendente aceitacao ou Recusada impedem novo orcamento") {
    const OrderStatus states[] = {OrderStatus::PendingAcceptance,
                                  OrderStatus::RejectedLackOfStock};
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
