#pragma once

#include <vector>

#include "ServiceOrderRepository.h"

namespace mecanica360 {

// Repositorio em memoria usado nos testes. Os IDs sao sequenciais a
// partir de 1 e as listas mantem a ordem de insercao (os testes CT-04.x
// dependem dessa ordem).
class FakeRepository : public ServiceOrderRepository {
public:
    int insert(ServiceOrder order) override;
    void update(const ServiceOrder& order) override;
    std::optional<ServiceOrder> findById(int id) const override;
    std::vector<ServiceOrder> listByState(OrderStatus status) const override;
    std::vector<ServiceOrder> listByCustomer(int clientId) const override;
    std::vector<ServiceOrder> listByMechanic(int mechanicId) const override;
    std::vector<ServiceOrder> listAll() const override;

private:
    std::vector<ServiceOrder> orders_;
    int nextId_ = 1;
};

}  // namespace mecanica360