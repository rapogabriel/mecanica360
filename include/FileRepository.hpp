/**
 * @file FileRepository.hpp
 * @brief Define a implementação de persistência em arquivos binários para ordens de serviço.
 */

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "ServiceOrder.h"
#include "ServiceOrderRepository.h"

namespace mecanica360 {

/**
 * @brief Repositório concreto de ordens de serviço baseado em arquivos binários.
 *
 * Persiste os registros serializados em um arquivo principal de dados e mantém
 * um arquivo auxiliar de índice primário (mapeando ID para deslocamento em bytes)
 * para acelerar consultas e atualizações.
 */
class FileRepository : public ServiceOrderRepository {
private:
    std::string dataPath_;   ///< Caminho do arquivo binário principal de dados (ex.: `"service_orders.dat"`).
    std::string indexPath_;  ///< Caminho do arquivo binário de índice primário (ex.: `"index_id.idx"`).

public:
    /**
     * @brief Resultado de uma operação de leitura paginada em lote (*batch*).
     */
    struct BatchResult {
        std::vector<ServiceOrder> orders = {};  ///< Ordens de serviço carregadas no lote atual.
        std::streampos nextIdPos = -1;          ///< Posição no índice para retomar a próxima leitura.
        bool reachEnd = false;                  ///< Indica se o fim dos registros foi alcançado.
    };

#pragma pack(push, 1)
    /**
     * @brief Entrada do arquivo de índice primário sem preenchimento (*padding*) de memória.
     */
    struct IndexTuple {
        int id;            ///< Identificador único da ordem de serviço.
        long long offset;  ///< Deslocamento (*byte offset*) do registro no arquivo de dados.
    };
#pragma pack(pop)

    /**
     * @brief Constrói o repositório associando os arquivos de dados e de índice.
     *
     * @param dataPath Caminho para o arquivo binário de dados das ordens.
     * @param indexPath Caminho para o arquivo binário de índice primário.
     */
    FileRepository(const std::string& dataPath, const std::string& indexPath);

    int insert(ServiceOrder order) override;
    void update(const ServiceOrder& order) override;

    /**
     * @brief Busca ordens de serviço que satisfaçam um predicado, lendo o arquivo em lotes.
     *
     * @param predicate Função de filtragem aplicada a cada ordem carregada.
     * @param batch Quantidade máxima de registros lidos por lote do disco.
     * @return Vetor com todas as ordens que atenderam ao predicado.
     */
    std::vector<ServiceOrder> findBy(std::function<bool(const ServiceOrder&)> predicate,
                                     size_t batch = 20) const;
    std::optional<ServiceOrder> findById(int id) const override;