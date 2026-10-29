#include <cassert>
#include "RepositorioFake.hpp"

int main() {
    RepositorioFake repo;

    // insert / findById / update / listAll
    int a = repo.insert(ServiceOrder(0, 10, 20, OrderStatus::OrcamentoPendente));
    repo.insert(ServiceOrder(0, 10, 21, OrderStatus::PendenteAceitacao));
    repo.insert(ServiceOrder(0, 11, 20, OrderStatus::OrcamentoPendente));
    repo.insert(ServiceOrder(0, 12, 22, OrderStatus::RecusadaFaltaEstoque));

    assert(a == 1);
    assert(repo.listAll().size() == 4);
    assert(repo.findById(a)->clientId() == 10);
    assert(!repo.findById(99).has_value());

    repo.update(ServiceOrder(a, 10, 20, OrderStatus::PendenteAceitacao));
    assert(repo.findById(a)->status() == OrderStatus::PendenteAceitacao);
    repo.update(ServiceOrder(a, 10, 20, OrderStatus::OrcamentoPendente));

    // listByStatus
    assert(repo.listByStatus(OrderStatus::OrcamentoPendente).size() == 2);
    assert(repo.listByStatus(OrderStatus::PendenteAceitacao).size() == 1);
    assert(repo.listByStatus(OrderStatus::PendenteAceitacao)[0].id() == 2);
    assert(repo.listByStatus(OrderStatus::RecusadaFaltaEstoque).size() == 1);

    // listByClient
    assert(repo.listByClient(10).size() == 2);
    assert(repo.listByClient(12).size() == 1);
    assert(repo.listByClient(99).empty());

    // listByMechanic
    assert(repo.listByMechanic(20).size() == 2);
    assert(repo.listByMechanic(22)[0].id() == 4);
    assert(repo.listByMechanic(99).empty());

    return 0;
}
