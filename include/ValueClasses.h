/**
 * @file ValueClasses.h
 * @brief Definição das classes de valor e Usuário do sistema Mecânica360.
 */

#pragma once

#include <ctime>
#include <string>

#include "Types.h"

namespace mecanica360 {

/**
 * @brief Representa um usuário do sistema.
 * 
 * Armazena os dados básicos de identificação e o nível de acesso (Role).
 */
class User {
public:
    /**
     * @brief Constrói um novo usuário.
     * 
     * @param[in] id Identificador único do usuário no sistema.
     * @param[in] name Nome completo ou de exibição do usuário.
     * @param[in] role Papel/Nível de permissão (ex: Mecânico).
     */
    User(int id, std::string name, Role role);

    /// @brief Obtém o identificador do usuário.
    /// @return ID numérico.
    int id() const;

    /// @brief Obtém o nome do usuário.
    /// @return String constante com o nome.
    const std::string& name() const;

    /// @brief Obtém o nível de acesso do usuário.
    /// @return O valor do enum Role correspondente.
    Role role() const;

private:
    int id_;
    std::string name_;
    Role role_;
};

/**
 * @brief Representa um veículo de um cliente.
 * 
 * Classe de valor que encapsula os dados básicos de um automóvel 
 * para registro nas ordens de serviço.
 */
class Vehicle {
public:
    /**
     * @brief Construtor padrão.
     * @note Necessário para inicialização de instâncias vazias, especialmente 
     * no struct OpeningData e durante a execução de testes automatizados.
     */
    Vehicle();

    /**
     * @brief Constrói um veículo com dados preenchidos.
     * 
     * @param[in] plate Placa de identificação do veículo.
     * @param[in] model Modelo e/ou fabricante (ex: "Fiat Uno").
     * @param[in] year Ano de fabricação ou modelo.
     */
    Vehicle(std::string plate, std::string model, int year);

    /// @brief Obtém a placa do veículo.
    /// @return String contendo a placa.
    const std::string& plate() const;

    /// @brief Obtém o modelo do veículo.
    /// @return String contendo o modelo.
    const std::string& model() const;

    /// @brief Obtém o ano do veículo.
    /// @return Inteiro representando o ano.
    int year() const;

private:
    std::string plate_;
    std::string model_;
    int year_;
};

/**
 * @brief Representa uma peça necessária para a execução de um serviço.
 */
class NecessaryPart {
public:
    /**
     * @brief Constrói o registro de uma peça necessária.
     * 
     * @param[in] description Descrição ou nome da peça.
     * @param[in] quantity Quantidade de unidades necessárias.
     */
    NecessaryPart(std::string description, int quantity);

    /// @brief Obtém a descrição da peça.
    /// @return String com o nome/detalhe da peça.
    const std::string& description() const;

    /// @brief Obtém a quantidade exigida.
    /// @return Inteiro representando a quantidade.
    int quantity() const;

private:
    std::string description_;
    int quantity_;
};

/**
 * @brief Representa o orçamento financeiro e de tempo de uma Ordem de Serviço.
 */
class Budget {
public:
    /**
     * @brief Constrói um novo orçamento.
     * 
     * @param[in] partsValue Custo total calculado para as peças.
     * @param[in] laborValue Custo total calculado para a mão de obra.
     * @param[in] deadline Prazo estimado em dias para a entrega do veículo.
     */
    Budget(double partsValue, double laborValue, int deadline);

    /// @brief Obtém o valor isolado das peças.
    /// @return Valor monetário bruto em formato decimal.
    double partsValue() const;

    /// @brief Obtém o valor isolado da mão de obra.
    /// @return Valor monetário bruto em formato decimal.
    double laborValue() const;

    /// @brief Obtém o prazo estimado para conclusão.
    /// @return Prazo em dias corridos.
    int deadline() const;

    /**
     * @brief Calcula o valor final total do orçamento.
     * @return A soma do valor das peças com o valor da mão de obra.
     */
    double total() const;

private:
    double partsValue_;
    double laborValue_;
    int deadline_;
};

/**
 * @brief Representa um registro histórico de mudança de estado de uma Ordem.
 * 
 * Utilizado para rastrear quando uma ordem avançou ou retrocedeu no fluxo, 
 * quem foi o responsável e exatamente quando isso ocorreu.
 */
class StatusHistory {
public:
    /**
     * @brief Constrói um novo registro de transição de estado.
     * 
     * @param[in] previousState O estado no qual a ordem se encontrava antes da mudança.
     * @param[in] newState O novo estado assumido pela ordem.
     * @param[in] responsibleId ID do usuário (funcionário) que efetuou a alteração.
     * @param[in] instant Timestamp exato do momento da transição.
     */
    StatusHistory(OrderStatus previousState, OrderStatus newState,
                  int responsibleId, std::time_t instant);

    /// @brief Obtém o estado anterior à transição.
    /// @return Valor do enum OrderStatus.
    OrderStatus previousState() const;

    /// @brief Obtém o novo estado consolidado.
    /// @return Valor do enum OrderStatus.
    OrderStatus newState() const;

    /// @brief Obtém o identificador do funcionário que realizou a mudança.
    /// @return ID numérico do responsável.
    int responsibleId() const;

    /// @brief Obtém o timestamp do momento exato da mudança.
    /// @return Objeto std::time_t (representação de tempo Unix).
    std::time_t instant() const;

private:
    OrderStatus previousState_;
    OrderStatus newState_;
    int responsibleId_;
    std::time_t instant_;
};

}  // namespace mecanica360