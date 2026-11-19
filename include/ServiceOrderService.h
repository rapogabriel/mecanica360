/**
 * @file ServiceOrderService.h
 * @brief Define o serviço de regras de negócio para gerenciamento de ordens de serviço.
 */

#pragma once

#include <expected>
#include <vector>

#include "OpeningData.h"
#include "ServiceOrder.h"
#include "ServiceOrderRepository.h"
#include "Types.h"
#include "ValueClasses.h"

namespace mecanica360 {

/**
 * @brief Serviço responsável pela lógica de negócio das ordens de serviço.
 *
 * Depende da abstração `ServiceOrderRepository`, injetada por referência.
 * O repositório fornecido deve permanecer válido durante todo o ciclo de vida
 * da instância deste serviço.
 *
 * Erros de negócio (`OrderError`) são retornados via `std::expected` sem
 * alterar nenhum registro persistido. As verificações seguem a ordem:
 * permissão, validade dos dados, existência da ordem, visibilidade e estado.
 * Exceções são reservadas a violações de contrato do domínio ou do repositório.
 */
class ServiceOrderService {
public:
    /**
     * @brief Constrói o serviço associado a um repositório de ordens de serviço.
     *
     * @param repository Referência ao repositório concreto que armazenará as ordens.
     */
    explicit ServiceOrderService(ServiceOrderRepository& repository);

    /**
     * @brief Abre uma nova ordem de serviço no estado `PendingBudget`.
     *
     * Operação restrita ao perfil de Mecânico.
     *
     * @param user Usuário que solicita a abertura da ordem.
     * @param data Dados iniciais necessários para criação da ordem de serviço.
     * @return Identificador único da ordem criada ou `OrderError`
     *         (`AcessDenied`, `InvalidData`).
     */
    std::expected<int, OrderError> openSO(const User& user, const OpeningData& data);

    /**
     * @brief Lista todas as ordens de serviço no estado `PendingBudget`.
     *
     * Operação restrita ao perfil de Almoxarife.
     *
     * @param user Usuário que solicita a consulta.
     * @return Vetor com as ordens pendentes de orçamento ou `OrderError`
     *         (`AcessDenied`).
     */
    std::expected<std::vector<ServiceOrder>, OrderError> searchNewSO(const User& user);

    /**
     * @brief Anexa um orçamento a uma ordem de serviço e altera seu estado para `PendingAcceptance`.
     *
     * Operação restrita ao perfil de Almoxarife.
     *
     * @param user Usuário que envia o orçamento.
     * @param soId Identificador único da ordem de serviço.
     * @param budget Orçamento a ser vinculado à ordem.
     * @return `void` em caso de sucesso ou `OrderError`
     *         (`AcessDenied`, `InvalidData`, `ServiceOrderNotFound`, `InvalidTransition`).
     */
    std::expected<void, OrderError> sendBudget(const User& user, int soId,
                                               const Budget& budget);

    /**
     * @brief Recusa uma ordem de serviço por falta de estoque, alterando seu estado para `RejectedLackOfStock`.
     *
     * Operação restrita ao perfil de Almoxarife.
     *
     * @param user Usuário que registra a recusa.
     * @param soId Identificador único da ordem de serviço.
     * @return `void` em caso de sucesso ou `OrderError`
     *         (`AcessDenied`, `ServiceOrderNotFound`, `InvalidTransition`).
     */
    std::expected<void, OrderError> rejectDueLackOfStock(const User& user, int soId);

    /**
     * @brief Consulta uma ordem de serviço específica conforme a visibilidade do usuário.
     *
     * Equivalente a `checkSOById()`.
     *
     * @param user Usuário que realiza a consulta.
     * @param soId Identificador único da ordem de serviço.
     * @return A ordem de serviço encontrada ou `OrderError`
     *         (`AcessDenied`, `ServiceOrderNotFound`).
     */
    std::expected<ServiceOrder, OrderError> checkSO(const User& user, int soId);

    /**
     * @brief Lista as ordens de serviço visíveis para o perfil do usuário.
     *
     * Regras de visibilidade por perfil:
     * - Gerente: todas as ordens cadastradas.
     * - Cliente: próprias ordens que não foram recusadas.
     * - Mecânico: ordens abertas por ele.
     * - Almoxarife: acesso negado (`AcessDenied`).
     *
     * @param user Usuário que realiza a consulta.
     * @return Vetor com as ordens acessíveis ao usuário ou `OrderError`
     *         (`AcessDenied`).
     */
    std::expected<std::vector<ServiceOrder>, OrderError> checkSO(const User& user);

    /**
     * @brief Consulta uma ordem de serviço pelo seu identificador, respeitando a visibilidade do perfil.
     *
     * @param user Usuário que realiza a consulta.
     * @param soId Identificador único da ordem de serviço.
     * @return A ordem de serviço encontrada ou `OrderError`
     *         (`AcessDenied`, `ServiceOrderNotFound`).
     */
    std::expected<ServiceOrder, OrderError> checkSOById(const User& user, int soId);

    /**
     * @brief Lista todas as ordens de serviço no estado `RejectedLackOfStock`.
     *
     * Operação restrita ao perfil de Gerente.
     *
     * @param user Usuário que solicita a listagem.
     * @return Vetor com as ordens recusadas por falta de estoque ou `OrderError`
     *         (`AcessDenied`).
     */
    std::expected<std::vector<ServiceOrder>, OrderError> listRejectedDueLackOfStock(
        const User& user);

private:
    ServiceOrderRepository& repository_;
};

}  // namespace mecanica360