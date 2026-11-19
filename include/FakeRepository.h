/**
 * @file FakeRepository.h
 * @brief Define a implementação em memória do repositório de ordens de serviço para testes.
 */

#pragma once

#include <vector>

#include "ServiceOrderRepository.h"

namespace mecanica360 {

/**
 * @brief Repositório em memória utilizado na execução de testes automatizados.
 *
 * Atribui identificadores sequenciais a partir de 1 e preserva a ordem de
 * inserção nas listagens (comportamento esperado pelos casos de teste CT-04.x).
 */
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
    std::vector<ServiceOrder> orders_;  ///< Armazenamento em memória das ordens de serviço.
    int nextId_ = 1;                    ///< Próximo identificador sequencial a ser atribuído.
};

}  // namespace mecanica360