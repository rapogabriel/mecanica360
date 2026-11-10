#pragma once

#include <expected>
#include <vector>

#include "OpeningData.h"
#include "ServiceOrder.h"
#include "ServiceOrderRepository.h"
#include "Types.h"
#include "ValueClasses.h"

namespace mecanica360 {

// Servico com as 7 operacoes da API (somente declaracoes).
class ServiceOrderService {
public:
    explicit ServiceOrderService(ServiceOrderRepository& repository);

    // F-01 (Mecanico)
    std::expected<int, OrderError> openSO(User user, OpeningData data);

    // F-02 (Almoxarife)
    std::expected<std::vector<ServiceOrder>, OrderError> searchNewSO(User user);
    std::expected<void, OrderError> sendBudget(User user, int id, Budget budget);

    // F-03 (Almoxarife)
    std::expected<void, OrderError> rejectDueLackOfStock(User user, int id);

    // F-04 (Gerente, Cliente, Mecanico; Almoxarife so por id)
    std::expected<std::vector<ServiceOrder>, OrderError> checkSO(User user);
    std::expected<ServiceOrder, OrderError> checkSOById(User user, int id);
    std::expected<std::vector<ServiceOrder>, OrderError>
        listRejectedDueLackOfStock(User user);

private:
    ServiceOrderRepository& repository_;
};

}  // namespace mecanica360