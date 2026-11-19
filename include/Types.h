/**
 * @file Types.h
 * @brief Define as enumerações de papéis de usuário, estados e erros das ordens de serviço.
 */

#pragma once

namespace mecanica360 {

/**
 * @brief Papéis (perfis de acesso) dos usuários no sistema.
 */
enum class Role {
    Customer,   ///< Cliente proprietário do veículo.
    Mechanic,   ///< Mecânico responsável pelo diagnóstico e abertura de ordens.
    Stockroom,  ///< Almoxarife responsável por orçamento e verificação de estoque.
    Manager     ///< Gerente com visão administrativa das ordens de serviço.
};

/**
 * @brief Estados possíveis no ciclo de vida de uma ordem de serviço.
 */
enum class OrderStatus {
    PendingBudget,       ///< Aguardando elaboração de orçamento pelo almoxarifado.
    PendingAcceptance,   ///< Orçamento enviado, aguardando aprovação do cliente.
    RejectedLackOfStock  ///< Recusada pelo almoxarifado por falta de peças em estoque.
};

/**
 * @brief Códigos de erro de negócio retornados pelas operações de ordem de serviço.
 */
enum class OrderError {
    AcessDenied,           ///< Usuário não possui permissão para executar a operação.
    InvalidData,           ///< Dados fornecidos são inválidos ou incompletos.
    ServiceOrderNotFound,  ///< Nenhuma ordem de serviço encontrada com o identificador informado.
    InvalidTransition      ///< Transição de estado não permitida a partir do estado atual da ordem.
};

}  // namespace mecanica360