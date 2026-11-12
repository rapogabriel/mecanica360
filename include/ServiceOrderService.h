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
 * @brief Servico de logica de negocio das ordens de servico (Sprint 2, API-M360-1.9).
 *
 * Depende da abstracao ServiceOrderRepository, injetada por referencia (agregacao do UML);
 * o repositorio deve sobreviver ao servico.
 *
 * Estrategia de erros: erros de negocio (OrderError) sao retornados via std::expected;
 * em qualquer erro nenhum registro e alterado. Ordem das verificacoes: permissao,
 * validade dos dados, existencia da OS, visibilidade e estado.
 * Excecoes ficam reservadas a violacoes de contrato do dominio (ServiceOrder, repositorio).
 */
class ServiceOrderService {
public:
    /** @brief Constroi o servico sobre o repositorio injetado. */
    explicit ServiceOrderService(ServiceOrderRepository& repository);

    /** @brief CU-01 (Mecanico): abre uma OS em PendingBudget e devolve seu identificador.
     *  Erros: AcessDenied, InvalidData. */
    std::expected<int, OrderError> openSO(const User& user, const OpeningData& data);

    /** @brief CU-02 (Almoxarife): lista as OS em PendingBudget. Erro: AcessDenied. */
    std::expected<std::vector<ServiceOrder>, OrderError> searchNewSO(const User& user);

    /** @brief CU-02 (Almoxarife): anexa o orcamento e leva a OS a PendingAcceptance.
     *  Erros: AcessDenied, InvalidData, ServiceOrderNotFound, InvalidTransition. */
    std::expected<void, OrderError> sendBudget(const User& user, int soId,
                                               const Budget& budget);

    /** @brief CU-03 (Almoxarife): leva a OS a RejectedLackOfStock.
     *  Erros: AcessDenied, ServiceOrderNotFound, InvalidTransition. */
    std::expected<void, OrderError> rejectDueLackOfStock(const User& user, int soId);

    /** @brief CU-04: consulta individual (assinatura do UML); equivale a checkSOById. */
    std::expected<ServiceOrder, OrderError> checkSO(const User& user, int soId);

    /** @brief CU-04: lista de acompanhamento do perfil. Gerente: todas; Cliente: proprias
     *  nao recusadas; Mecanico: as que abriu; Almoxarife: AcessDenied. */
    std::expected<std::vector<ServiceOrder>, OrderError> checkSO(const User& user);

    /** @brief CU-04: consulta por identificador, conforme a visibilidade do perfil.
     *  Erros: AcessDenied, ServiceOrderNotFound. */
    std::expected<ServiceOrder, OrderError> checkSOById(const User& user, int soId);

    /** @brief CU-04 (Gerente): lista as OS em RejectedLackOfStock. Erro: AcessDenied. */
    std::expected<std::vector<ServiceOrder>, OrderError> listRejectedDueLackOfStock(
        const User& user);

private:
    ServiceOrderRepository& repository_;
};

}  // namespace mecanica360
