#pragma once

#include <optional>
#include <string>
#include <vector>

#include "OpeningData.h"
#include "Types.h"
#include "ValueClasses.h"

namespace mecanica360 {

// Ordem de servico.
// Decisao sobre o ID: a ordem nasce sem ID (id() == 0) e o repositorio
// atribui o identificador unico em insert(), devolvendo-o ao chamador.
// Assim a unicidade (CT-01.2) fica a cargo de quem guarda as ordens.
class ServiceOrder {
public:
    // Cria a ordem em PendingBudget, sem orcamento e sem historico.
    static ServiceOrder create(const OpeningData& data, int mechanicId);

    int id() const;
    int clientId() const;
    int mechanicId() const;
    const Vehicle& vehicle() const;
    const std::string& diagnosis() const;
    const std::vector<NecessaryPart>& parts() const;
    double labor() const;
    int deadline() const;
    OrderStatus state() const;
    std::optional<Budget> budget() const;
    const std::vector<StatusHistory>& history() const;

    // Usado somente pelo repositorio ao inserir.
    void assignId(int id);

    bool canTransition(OrderStatus target) const;
    // Muda o estado e acrescenta uma entrada no historico (responsavel + instante).
    void transition(OrderStatus target, int responsibleId);
    void attachBudget(const Budget& budget);

private:
    ServiceOrder() = default;

    int id_ = 0;
    int clientId_ = 0;
    int mechanicId_ = 0;
    Vehicle vehicle_;
    std::string diagnosis_;
    std::vector<NecessaryPart> parts_;
    double labor_ = 0.0;
    int deadline_ = 0;
    OrderStatus state_ = OrderStatus::PendingBudget;
    std::optional<Budget> budget_;
    std::vector<StatusHistory> history_;
};

}  // namespace mecanica360