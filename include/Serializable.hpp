/**
 * @file Serializable.h
 * @brief Define a interface de serialização e funções utilitárias de leitura e escrita.
 */

#pragma once

#include <fstream>
#include <string>

namespace mecanica360 {

/**
 * @brief Interface (contrato) para classes que suportam serialização binária.
 *
 * Obriga as classes derivadas a implementarem a escrita e a leitura de seu
 * estado em fluxos de arquivo binário.
 */
class ISerializable {
public:
    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~ISerializable() = default;

    /**
     * @brief Serializa o estado atual do objeto em um fluxo de saída binário.
     *
     * @param out Fluxo de arquivo de saída aberto em modo binário.
     */
    virtual void serialize(std::ofstream& out) const = 0;

    /**
     * @brief Desserializa e restaura o estado do objeto a partir de um fluxo de entrada binário.
     *
     * @param in Fluxo de arquivo de entrada aberto em modo binário.
     */
    virtual void deserialize(std::ifstream& in) = 0;
};

/**
 * @brief Funções utilitárias para leitura e escrita binária de tipos primitivos e strings.
 */
namespace serialization_utils {

    /**
     * @brief Escreve um valor de tipo primitivo (ou trivialmente copiável) em formato binário.
     *
     * @tparam T Tipo do dado a ser escrito (ex.: `int`, `double`, `enum`).
     * @param out Fluxo de arquivo de saída aberto em modo binário.
     * @param value Valor a ser gravado no fluxo.
     */
    template <typename T>
    inline void writePrimitive(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    /**
     * @brief Lê um valor de tipo primitivo (ou trivialmente copiável) em formato binário.
     *
     * @tparam T Tipo do dado a ser lido (ex.: `int`, `double`, `enum`).
     * @param in Fluxo de arquivo de entrada aberto em modo binário.
     * @param value Referência onde o valor lido será armazenado.
     */
    template <typename T>
    inline void readPrimitive(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    /**
     * @brief Escreve uma string de tamanho variável em formato binário.
     *
     * Grava primeiramente o tamanho da string (`size_t`) seguido pelos seus
     * caracteres brutos.
     *
     * @param out Fluxo de arquivo de saída aberto em modo binário.
     * @param str String a ser gravada no fluxo.
     */
    inline void writeString(std::ofstream& out, const std::string& str) {
        size_t size = str.size();
        writePrimitive(out, size);
        out.write(str.data(), size);
    }

    /**
     * @brief Lê uma string de tamanho variável em formato binário.
     *
     * Lê o tamanho gravado (`size_t`), redimensiona a string de destino e
     * carrega os caracteres correspondentes do fluxo.
     *
     * @param in Fluxo de arquivo de entrada aberto em modo binário.
     * @param str Referência onde a string lida será armazenada.
     */
    inline void readString(std::ifstream& in, std::string& str) {
        size_t size;
        readPrimitive(in, size);
        str.resize(size);
        in.read(&str[0], size);
    }

}  // namespace serialization_utils

}  // namespace mecanica360