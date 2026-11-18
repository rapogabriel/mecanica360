#pragma once
#include <functional>
#include <string>
#include <vector>
#include <optional>
#include "ServiceOrderRepository.h"
#include "ServiceOrder.h"

namespace mecanica360{
    class FileRepository : public ServiceOrderRepository {
        private:
        std::string dataPath_;  // Caminho do arquivo binário principal (ex: "service_orders.dat")
        std::string indexPath_; // Caminho do arquivo de índice primário (ex: "index_id.idx")

        public:
        FileRepository(const std::string& dataPath, const std::string& indexPath);

        
        int insert(ServiceOrder order) override;
        void update(const ServiceOrder& order) override;

        std::vector<ServiceOrder> FileRepository::findBy(std::function<bool(const ServiceOrder&)> predicate,size_t batch = 20) const;
        std::optional<ServiceOrder> findById(int id) const override;
        std::vector<ServiceOrder> listByState(OrderStatus status) const override;
        std::vector<ServiceOrder> listByCustomer(int clientId) const override;
        std::vector<ServiceOrder> listByMechanic(int mechanicId ) const override;
        std::vector<ServiceOrder> listAll() const override;

        struct BatchResult{
            std::vector<ServiceOrder> orders = {};
            std::streampos nextIdPos = -1;
            bool reachEnd= false;
        };
        #pragma pack(push, 1)
        struct IndexTuple {
            int id;
            long long offset;
        };
        #pragma pack(pop)

        BatchResult readBatch(std::streampos startOffset, size_t batchSize)const;

        // Método auxiliar interno para calcular o próximo ID sequencial baseado no índice
        int creatNextId() const;
    };




}

