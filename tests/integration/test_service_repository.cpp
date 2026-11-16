// Integracao ServiceOrderService <-> ServiceOrderRepository (abstracao) e fluxo completo.
#include <functional>

#include "Fixtures.h"
#include "ServiceOrderService.h"
#include "TestFramework.h"

using namespace fx;

namespace {

/// Outra implementacao da abstracao: delega ao FakeRepository e conta as escritas.
class SpyRepository : public ServiceOrderRepository {
public:
    int insert(ServiceOrder order) override { ++inserts; return inner.insert(std::move(order)); }
    void update(const ServiceOrder& order) override { ++updates; inner.update(order); }
    std::optional<ServiceOrder> findById(int id) const override { return inner.findById(id); }
    std::vector<ServiceOrder> listByState(OrderStatus s) const override { return inner.listByState(s); }
    std::vector<ServiceOrder> listByCustomer(int c) const override { return inner.listByCustomer(c); }
    std::vector<ServiceOrder> listByMechanic(int m) const override { return inner.listByMechanic(m); }
    std::vector<ServiceOrder> listAll() const override { return inner.listAll(); }

    FakeRepository inner;
    int inserts = 0;
    int updates = 0;
};

}  // namespace

TEST("Integracao: servico funciona com qualquer implementacao de ServiceOrderRepository") {
    SpyRepository repo;
    ServiceOrderService service(repo);
    auto id = service.openSO(mechanic(), validOpening());
    REQUIRE(id.has_value());
    CHECK(repo.inserts == 1);
    REQUIRE(service.sendBudget(stockroom(), *id, Budget(800.0, 300.0, 3)).has_value());
    CHECK(repo.updates == 1);
    CHECK(repo.inner.findById(*id)->state() == OrderStatus::PendingAcceptance);
}

TEST("Integracao: erros de negocio nao chamam insert nem update") {
    SpyRepository repo;
    const int pending = seed(repo, 10, 7);
    const int sent = seed(repo, 10, 7, OrderStatus::PendingAcceptance);
    const int ins0 = repo.inserts, upd0 = repo.updates;
    ServiceOrderService service(repo);

    OpeningData bad = validOpening();
    bad.deadline = 0;
    CHECK(!service.openSO(mechanic(), bad).has_value());
    CHECK(!service.openSO(manager(), validOpening()).has_value());
    CHECK(!service.sendBudget(manager(), pending, Budget(1, 1, 1)).has_value());
    CHECK(!service.sendBudget(stockroom(), pending, Budget(-1, 1, 1)).has_value());
    CHECK(!service.sendBudget(stockroom(), 999, Budget(1, 1, 1)).has_value());
    CHECK(!service.sendBudget(stockroom(), sent, Budget(1, 1, 1)).has_value());
    CHECK(!service.rejectDueLackOfStock(mechanic(), pending).has_value());
    CHECK(!service.rejectDueLackOfStock(stockroom(), 999).has_value());
    CHECK(!service.rejectDueLackOfStock(stockroom(), sent).has_value());

    CHECK(repo.inserts == ins0);
    CHECK(repo.updates == upd0);
}

TEST("Integracao: consultas nao escrevem no repositorio") {
    SpyRepository repo;
    const int id = seed(repo, 10, 7);
    const int ins0 = repo.inserts, upd0 = repo.updates;
    ServiceOrderService service(repo);
    service.checkSO(manager());
    service.checkSO(customer(10));
    service.checkSO(mechanic(7));
    service.checkSOById(manager(), id);
    service.listRejectedDueLackOfStock(manager());
    service.searchNewSO(stockroom());
    CHECK(repo.inserts == ins0);
    CHECK(repo.updates == upd0);
}

TEST("Integracao: fluxo completo - abrir, listar, enviar orcamento e acompanhar") {
    FakeRepository repo;
    ServiceOrderService service(repo);

    auto id = service.openSO(mechanic(7), validOpening(10));
    REQUIRE(id.has_value());

    auto novas = service.searchNewSO(stockroom());
    REQUIRE(novas.has_value());
    CHECK((ids(*novas) == std::vector<int>{*id}));

    REQUIRE(service.sendBudget(stockroom(5), *id, Budget(800.0, 300.0, 3)).has_value());
    CHECK(service.searchNewSO(stockroom()).value().empty());

    auto cliente = service.checkSOById(customer(10), *id);
    REQUIRE(cliente.has_value());
    CHECK(cliente->state() == OrderStatus::PendingAcceptance);
    REQUIRE(cliente->budget().has_value());
    CHECK_NEAR(cliente->budget()->total(), 1100.0);
    CHECK(cliente->history().size() == 1);

    // Almoxarife deixa de enxergar a OS apos o envio
    CHECK(service.checkSOById(stockroom(), *id).error() == OrderError::AcessDenied);
}

TEST("Integracao: fluxo de recusa aparece para gerente e mecanico, nao para o cliente") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto id = service.openSO(mechanic(7), validOpening(10));
    REQUIRE(id.has_value());
    REQUIRE(service.rejectDueLackOfStock(stockroom(5), *id).has_value());

    CHECK((ids(service.listRejectedDueLackOfStock(manager()).value()) == std::vector<int>{*id}));
    CHECK((ids(service.checkSO(mechanic(7)).value()) == std::vector<int>{*id}));
    CHECK(service.checkSO(customer(10)).value().empty());
    CHECK(service.checkSOById(customer(10), *id).error() == OrderError::AcessDenied);
    // sem orcamento e com 1 registro de historico
    auto so = service.checkSOById(manager(), *id);
    REQUIRE(so.has_value());
    CHECK(!so->budget().has_value());
    CHECK(so->history().size() == 1);
}

TEST("Integracao: identificadores gerados sao unicos em varias aberturas") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    std::vector<int> got;
    for (int i = 0; i < 20; ++i) {
        auto id = service.openSO(mechanic(7), validOpening(10 + i));
        REQUIRE(id.has_value());
        got.push_back(*id);
    }
    for (std::size_t i = 0; i < got.size(); ++i)
        for (std::size_t j = i + 1; j < got.size(); ++j) CHECK(got[i] != got[j]);
}

TEST("Integracao: OS aberta guarda o mecanico do usuario autenticado") {
    FakeRepository repo;
    ServiceOrderService service(repo);
    auto a = service.openSO(mechanic(7), validOpening());
    auto b = service.openSO(mechanic(8), validOpening());
    REQUIRE(a.has_value());
    REQUIRE(b.has_value());
    CHECK(repo.findById(*a)->mechanicId() == 7);
    CHECK(repo.findById(*b)->mechanicId() == 8);
}
