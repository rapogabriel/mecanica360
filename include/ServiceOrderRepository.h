/**
 * @file ServiceOrderRepository.h
 * @brief Define a interface de persistência para ordens de serviço.
 */

#pragma once

#include <optional>
#include <vector>

#include "ServiceOrder.h"

namespace mecanica360 {

/**
 * @brief Interface (contrato) para armazenamento e consulta de ordens de serviço.
 *
 * Define as operações básicas de persistência que devem ser implementadas por
 * repositórios concretos (ex.: em memória, arquivo ou banco de dados).
 */
class ServiceOrderRepository {
public:
    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~ServiceOrderRepository() = default;

    /**
     * @brief Insere uma nova ordem de serviço no repositório.
     *
     * Atribui um identificador único à ordem, realiza o armazenamento e
     * retorna o ID gerado.
     *
     * @param order Ordem de serviço a ser persistida.
     * @return Identificador único atribuído à ordem inserida.
     */
    virtual int insert(ServiceOrder order) = 0;

    /**
     * @brief Atualiza os dados de uma ordem de serviço existente.
     *
     * @param order Ordem de serviço contendo o ID alvo e os novos dados.
     */
    virtual void update(const ServiceOrder& order) = 0;

    /**
     * @brief Busca uma ordem de serviço pelo seu identificador.
     *
     * @param id Identificador único da ordem de serviço.
     * @return A ordem encontrada ou `std::nullopt` caso não exista.
     */
    virtual std::optional<ServiceOrder> findById(int id) const = 0;

    /**
     * @brief Lista todas as ordens de serviço que possuem um determinado estado.
     *
     * @param status Estado das ordens desejadas.
     * @return Vetor com as ordens que correspondem ao estado informado.
     */
    virtual std::vector<ServiceOrder> listByState(OrderStatus status) const = 0;

    /**
     * @brief Lista todas as ordens de serviço associadas a um cliente.
     *
     * @param clientId Identificador único do cliente.
     * @return Vetor com as ordens vinculadas ao cliente informado.
     */
    virtual std::vector<ServiceOrder> listByCustomer(int clientId) const = 0;

    /**
     * @brief Lista todas as ordens de serviço atribuídas a um mecânico.
     *
     * @param mechanicId Identificador único do mecânico.
     * @return Vetor com as ordens atribuídas ao mecânico informado.
     */
    virtual std::vector<ServiceOrder> listByMechanic(int mechanicId) const = 0;

    /**
     * @brief Retorna todas as ordens de serviço cadastradas no repositório.
     *
     * @return Vetor contendo todas as ordens armazenadas.
     */
    virtual std::vector<ServiceOrder> listAll() const = 0;
};

}  // namespace mecanica360