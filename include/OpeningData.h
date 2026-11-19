/**
 * @file OpeningData.h
 * @brief Define a estrutura de dados utilizada na abertura de uma ordem de serviço.
 */

#pragma once

#include <vector>

#include "ValueClasses.h"

namespace mecanica360 {

/**
 * @brief Dados iniciais fornecidos pelo mecânico para abrir uma ordem de serviço.
 */
struct OpeningData {
    int clientId = 0;                  ///< Identificador único do cliente proprietário do veículo.
    Vehicle vehicle;                   ///< Dados do veículo atendido.
    std::string diagnosis;             ///< Descrição do diagnóstico inicial realizado pelo mecânico.
    std::vector<NecessaryPart> parts;  ///< Lista de peças necessárias para o reparo.
    double labor = 0.0;                ///< Valor estimado de mão de obra.
    int deadline = 0;                  ///< Prazo estimado para conclusão do serviço (em dias).
};

}  // namespace mecanica360