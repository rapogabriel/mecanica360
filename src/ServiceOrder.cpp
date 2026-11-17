/**
 * @file ServiceOrder.cpp
 * @brief Implementação dos métodos da classe mecanica360::ServiceOrder.
 */

#include "ServiceOrder.h"

#include <ctime>
#include <optional>
#include <stdexcept>

namespace mecanica360 {

/**
 * @details Inicializa a ordem garantindo que o ciclo de vida comece estritamente 
 * no estado `PendingBudget`. O uso de `std::nullopt` sinaliza a ausência explícita 
 * de valores orçamentários nesta fase.
 */
ServiceOrder::ServiceOrder(const OpeningData& data, int mechanicId) {
    this->data = data;
    mechanicId_ = mechanicId;
    state_ = OrderStatus::PendingBudget;
    budget_ = std::nullopt;
}

int ServiceOrder::id() const { return id_; }
int ServiceOrder::clientId() const { return data.clientId; }
int ServiceOrder::mechanicId() const { return mechanicId_; }
const Vehicle& ServiceOrder::vehicle() const { return data.vehicle; }
const std::string& ServiceOrder::diagnosis() const { return data.diagnosis; }
const std::vector<NecessaryPart>& ServiceOrder::parts() const { return data.parts; }
double ServiceOrder::labor() const { return data.labor; }
int ServiceOrder::deadline() const { return data.deadline; }
OrderStatus ServiceOrder::state() const { return state_; }
std::optional<Budget> ServiceOrder::budget() const { return budget_; }
const std::vector<StatusHistory>& ServiceOrder::history() const { return history_; }

void ServiceOrder::assignId(int id) { id_ = id; }

/**
 * @details Verifica as regras da máquina de estados do sistema. 
 * Atualmente, uma ordem recém-aberta (`PendingBudget`) só possui duas saídas válidas:
 * 1. Avançar para `PendingAcceptance` (Orçamento montado e aguardando aprovação do cliente).
 * 2. Ser abortada precocemente como `RejectedLackOfStock` (Falta de peças no estoque).
 */
bool ServiceOrder::canTransition(OrderStatus target) const {
    return state_ == OrderStatus::PendingBudget &&
           (target == OrderStatus::PendingAcceptance ||
            target == OrderStatus::RejectedLackOfStock);
}

/**
 * @details O timestamp é capturado no instante exato da execução.
 * 
 * @throws std::logic_error Se a transição solicitada quebrar o fluxo definido em `canTransition()`.
 */
void ServiceOrder::transition(OrderStatus target, int responsibleId) {
    if (!canTransition(target)) {
        throw std::logic_error("ServiceOrder::transition: transicao nao permitida");
    }
    history_.emplace_back(state_, target, responsibleId, std::time(nullptr));
    state_ = target;
}

/**
 * @details Valida a integridade do ciclo de vida: um orçamento financeiro só pode 
 * ser anexado se a ordem estiver ativamente na fase de orçamentação.
 * 
 * @throws std::logic_error Se o estado atual for diferente de `PendingBudget`.
 */
void ServiceOrder::attachBudget(const Budget& budget) {
    if (state_ != OrderStatus::PendingBudget) {
        throw std::logic_error("ServiceOrder::attachBudget: OS fora de PendingBudget");
    }
    budget_ = budget;
}

}  // namespace mecanica360