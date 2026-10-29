#include <cassert>
#include "RepositorioFake.hpp"

int main() {
    RepositorioFake repo;

    int a = repo.insert(ServiceOrder(0, 10, 20, OrderStatus::OrcamentoPendente));
    int b = repo.insert(ServiceOrder(0, 11, 21, OrderStatus::PendenteAceitacao));
    assert(a == 1);
    assert(b == 2);
    assert(repo.listAll().size() == 2);

    assert(repo.findById(a)->clientId() == 10);
    assert(!repo.findById(99).has_value());

    repo.update(ServiceOrder(a, 10, 20, OrderStatus::PendenteAceitacao));
    assert(repo.findById(a)->status() == OrderStatus::PendenteAceitacao);

    repo.update(ServiceOrder(99, 1, 1, OrderStatus::OrcamentoPendente));
    assert(repo.listAll().size() == 2);

    return 0;
}
