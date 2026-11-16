// Tarefa #13 (repositorio): FakeRepository como implementacao de ServiceOrderRepository.
#include <stdexcept>

#include "Fixtures.h"
#include "TestFramework.h"

using namespace fx;

TEST("FakeRepository: insert atribui ids unicos, sequenciais a partir de 1") {
    FakeRepository repo;
    const int a = repo.insert(ServiceOrder::create(validOpening(), 7));
    const int b = repo.insert(ServiceOrder::create(validOpening(), 7));
    CHECK(a == 1);
    CHECK(b == 2);
    CHECK(repo.findById(a)->id() == a);
    CHECK(repo.findById(b)->id() == b);
    CHECK(repo.listAll().size() == 2);
}

TEST("FakeRepository: usado pela abstracao ServiceOrderRepository") {
    FakeRepository fake;
    ServiceOrderRepository& repo = fake;
    const int id = repo.insert(ServiceOrder::create(validOpening(), 7));
    CHECK(repo.findById(id).has_value());
}

TEST("FakeRepository: findById inexistente devolve nullopt") {
    FakeRepository repo;
    seed(repo, 10, 7);
    CHECK(!repo.findById(999).has_value());
    CHECK(!repo.findById(0).has_value());
}

TEST("FakeRepository: update substitui a OS de mesmo id") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    const int other = seed(repo, 11, 8);
    ServiceOrder so = *repo.findById(id);
    so.transition(OrderStatus::RejectedLackOfStock, 5);
    repo.update(so);
    CHECK(repo.findById(id)->state() == OrderStatus::RejectedLackOfStock);
    CHECK(repo.findById(id)->history().size() == 1);
    CHECK(repo.findById(other)->state() == OrderStatus::PendingBudget);
    CHECK(repo.listAll().size() == 2);
}

TEST("FakeRepository: update de id inexistente lanca out_of_range e nao insere") {
    FakeRepository repo;
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    so.assignId(42);
    CHECK_THROWS(repo.update(so), std::out_of_range);
    CHECK(repo.listAll().empty());
}

TEST("FakeRepository: findById devolve copia (alterar a copia nao altera o repositorio)") {
    FakeRepository repo;
    const int id = seed(repo, 10, 7);
    ServiceOrder copy = *repo.findById(id);
    copy.transition(OrderStatus::PendingAcceptance, 5);
    CHECK(repo.findById(id)->state() == OrderStatus::PendingBudget);
}

TEST("FakeRepository: listas por estado, cliente, mecanico e todas, em ordem de insercao") {
    FakeRepository repo;
    const int a = seed(repo, 10, 7, OrderStatus::PendingBudget);
    const int b = seed(repo, 10, 8, OrderStatus::PendingAcceptance);
    const int c = seed(repo, 11, 7, OrderStatus::RejectedLackOfStock);
    CHECK(ids(repo.listByState(OrderStatus::PendingBudget)) == std::vector<int>{a});
    CHECK(ids(repo.listByState(OrderStatus::PendingAcceptance)) == std::vector<int>{b});
    CHECK(ids(repo.listByState(OrderStatus::RejectedLackOfStock)) == std::vector<int>{c});
    CHECK((ids(repo.listByCustomer(10)) == std::vector<int>{a, b}));
    CHECK((ids(repo.listByMechanic(7)) == std::vector<int>{a, c}));
    CHECK((ids(repo.listAll()) == std::vector<int>{a, b, c}));
    CHECK(repo.listByCustomer(99).empty());
    CHECK(repo.listByMechanic(99).empty());
}
