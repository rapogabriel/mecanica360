/**
 * @file ServiceOrder.h
 * @brief Definição da classe mecanica360::ServiceOrder.
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "OpeningData.h"
#include "Types.h"
#include "ValueClasses.h"

#include "Serializable.hpp"

namespace mecanica360 {

/**
 * @brief Representa uma Ordem de Serviço na oficina.
 * 
 * Esta classe gerencia os dados e as transições de estado de um serviço prestado, 
 * desde a sua abertura até a sua conclusão.
 * 
 * **Decisão sobre o ID:** 
 * A ordem nasce sem ID (`id() == 0`). O repositório é responsável por atribuir o 
 * identificador único chamando o método assignId() durante a inserção no banco de 
 * dados. Dessa forma, a garantia de unicidade (CT-01.2) fica a cargo da persistência.
 */
class ServiceOrder: public ISerializable {
public:
    /**
     * @brief Constrói uma nova Ordem de Serviço.
     * 
     * A ordem é inicializada no estado `PendingBudget` (Aguardando Orçamento),
     * sem orçamento atrelado e com o histórico de transições vazio.
     * 
     * @param[in] data Estrutura de dados contendo as informações iniciais de abertura.
     * @param[in] mechanicId Identificador do mecânico responsável pela execução.
     */
    ServiceOrder(const OpeningData& data, int mechanicId);
    ServiceOrder();
    /// @brief Obtém o identificador único da ordem.
    /// @return ID da ordem (retorna 0 se a ordem ainda não foi persistida).
    int id() const;

    /// @brief Obtém o identificador do cliente associado.
    /// @return ID do cliente.
    int clientId() const;

    /// @brief Obtém o identificador do mecânico responsável.
    /// @return ID do mecânico.
    int mechanicId() const;

    /// @brief Obtém os dados do veículo que passará pelo serviço.
    /// @return Referência constante para o objeto Vehicle.
    const Vehicle& vehicle() const;

    /// @brief Obtém o texto detalhando o diagnóstico inicial.
    /// @return String contendo o diagnóstico do problema.
    const std::string& diagnosis() const;

    /// @brief Obtém a lista de peças inicialmente estimadas para o reparo.
    /// @return Vetor de objetos NecessaryPart.
    const std::vector<NecessaryPart>& parts() const;

    /// @brief Obtém o valor estimado da mão de obra para a ordem.
    /// @return Valor bruto em formato decimal.
    double labor() const;

    /// @brief Obtém o prazo previsto para a conclusão do serviço.
    /// @return Prazo em dias corridos.
    int deadline() const;

    /// @brief Obtém o estado atual da ordem no fluxo da oficina.
    /// @return O valor do enum OrderStatus correspondente ao estado.
    OrderStatus state() const;

    /// @brief Obtém o orçamento fechado, se existir.
    /// @return Objeto std::optional contendo o Budget associado, ou vazio.
    std::optional<Budget> budget() const;

    /// @brief Obtém o histórico completo das transições de estado.
    /// @return Vetor de StatusHistory ordenado cronologicamente.
    const std::vector<StatusHistory>& history() const;

    /**
     * @brief Injeta o identificador único gerado pelo banco de dados.
     * 
     * @warning Este método deve ser invocado **exclusivamente pelo repositório** 
     * logo após a persistência da entidade (`insert()`).
     * 
     * @param[in] id O identificador numérico gerado.
     */
    void assignId(int id);

    /**
     * @brief Valida se a ordem pode avançar para o estado especificado.
     * 
     * @param[in] target O estado de destino que se deseja alcançar.
     * @return true Se a transição respeita a máquina de estados do negócio.
     * @return false Caso a transição seja inválida (ex: pular etapas).
     */
    bool canTransition(OrderStatus target) const;

    /**
     * @brief Efetua a mudança de estado da ordem e atualiza o histórico.
     * 
     * Além de alterar o atributo interno `state_`, este método automaticamente 
     * registra o evento em `history_`, salvando o instante da mudança e quem a executou.
     * 
     * @param[in] target O estado de destino da transição.
     * @param[in] responsibleId Identificador do funcionário que solicitou a mudança.
     * 
     * @pre O método `canTransition(target)` deve retornar `true`.
     */
    void transition(OrderStatus target, int responsibleId);

    /**
     * @brief Anexa um orçamento finalizado e aprovado à ordem.
     * 
     * @param[in] budget O objeto contendo os valores e condições aprovadas.
     */
    void attachBudget(const Budget& budget);

    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;



private:
    /// @brief Identificador único da ordem de serviço.
    int id_ = 0;
    
    /// @brief Identificador do mecânico alocado.
    int mechanicId_ = 0;
    
    /// @brief Estrutura que agrupa os dados de abertura extraídos (DTO).
    OpeningData data;
    
    /// @brief Estado atual na máquina de estados.
    OrderStatus state_;
    
    /// @brief Orçamento aprovado (pode estar vazio antes do aceite do cliente).
    std::optional<Budget> budget_;
    
    /// @brief Registro cronológico das mudanças de estado.
    std::vector<StatusHistory> history_;



};

}  // namespace mecanica360