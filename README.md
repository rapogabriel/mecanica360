# 🔧 Mecânica360

**Sistema de gerenciamento para oficinas mecânicas**

Projeto acadêmico desenvolvido na disciplina **Engenharia de Software 1**, do curso de Ciência da Computação da Universidade Federal de Ouro Preto (UFOP).

## 📌 Sobre o projeto

O **Mecânica360** é um sistema de informação voltado ao gerenciamento de oficinas mecânicas. Seu objetivo é otimizar a rotina do estabelecimento, centralizando informações e facilitando o controle das operações, desde a abertura de uma ordem de serviço até o acompanhamento dos resultados da oficina.

A solução busca organizar os atendimentos, coordenar a disponibilidade de mecânicos e recursos, acompanhar orçamentos e autorizações e fornecer indicadores para apoiar a tomada de decisões.

## 🎯 Objetivos

- Organizar os atendimentos e as ordens de serviço.
- Coordenar a disponibilidade de mecânicos, peças e ferramentas.
- Registrar diagnósticos, prazos e custos dos serviços.
- Gerenciar orçamentos, revisões e autorizações dos clientes.
- Controlar o estoque e a utilização de recursos.
- Acompanhar impedimentos e reduzir atrasos evitáveis.
- Disponibilizar indicadores operacionais e financeiros para a gestão.

## ✨ Funcionalidades principais

### 📝 Organização de atendimentos e ordens de serviço

- Abertura e acompanhamento de ordens de serviço (OS).
- Registro de diagnósticos, serviços necessários e estimativas de prazo.
- Acompanhamento do estado de cada item de serviço.
- Registro de atrasos, tempos de espera e impedimentos.
- Encerramento e cancelamento de ordens de serviço.

### 💰 Orçamentos e autorizações

- Elaboração e envio de orçamentos aos clientes.
- Registro de aprovação ou recusa de serviços.
- Solicitação e acompanhamento de revisões de orçamento.
- Registro de alterações de custos e prazos.

### 📦 Gestão de estoque e recursos

- Cadastro e consulta de peças e ferramentas.
- Controle de entradas, saídas e disponibilidade.
- Acompanhamento dos níveis de estoque.
- Reserva e liberação de recursos para os serviços.
- Registro de necessidades de reposição.

### 👨‍🔧 Gestão da equipe

- Cadastro de mecânicos e suas especialidades.
- Registro de horários, ausências e disponibilidade.
- Coordenação da alocação de profissionais.
- Reorganização de atividades diante de impedimentos operacionais.

### 📊 Indicadores operacionais e financeiros

- Acompanhamento do tempo médio de espera por item de serviço.
- Monitoramento da ociosidade dos mecânicos por impedimentos.
- Análise do tempo gasto pelo gerente com reorganizações.
- Acompanhamento do percentual de ordens concluídas em atraso.
- Análise financeira e acompanhamento do lucro líquido.

### 🔐 Administração e segurança

- Autenticação de usuários.
- Gerenciamento de contas, perfis e permissões.
- Atualização dos dados de perfil.
- Registro de falhas e ocorrências.
- Gerenciamento de backups e recuperação de dados.

## 👥 Perfis de usuários

O sistema contempla cinco perfis principais:

| Perfil | Responsabilidades |
|---|---|
| **Cliente** | Acompanhar suas ordens de serviço e autorizar ou recusar orçamentos. |
| **Mecânico** | Registrar diagnósticos, abrir ordens de serviço e informar a execução e os impedimentos. |
| **Almoxarifado** | Controlar peças e ferramentas, verificar disponibilidades e elaborar orçamentos. |
| **Gerente** | Definir prioridades, alocar profissionais, acompanhar prazos e analisar indicadores. |
| **Administrador** | Gerenciar usuários, permissões, manutenção técnica e backups. |

Cada perfil possui responsabilidades e permissões específicas.

## 🏗️ Organização do sistema

O projeto prevê uma arquitetura baseada no padrão **MVC (Model–View–Controller)**, com módulos organizados por responsabilidade, buscando coesão e baixo acoplamento.

A organização contempla serviços de autenticação, controle de acesso, ordens de serviço, almoxarifado, gestão de equipe, análise gerencial e alertas operacionais.

A implementação deve seguir as regras de negócio e os requisitos definidos na documentação do projeto.

## 🤝 Colaboração entre grupos — BCC322

No contexto acadêmico, o Mecânica360 e o Azimute participam de uma relação de cliente e fornecedor, com papéis que se invertem conforme o projeto.

| Projeto | Cliente | Fornecedor |
|---|---|---|
| **Mecânica360** | Grupo Azul | Grupo Laranja — Azimute |
| **Azimute** | Grupo Laranja — Azimute | Grupo Azul — Mecânica360 |

No desenvolvimento do Mecânica360, o Grupo Azul atua como cliente e o Grupo Laranja como fornecedor. Já no Azimute, o Grupo Laranja assume o papel de cliente e o Grupo Azul, de fornecedor.

Essa colaboração envolve o alinhamento de responsabilidades, requisitos, entregas e critérios de aceitação entre as equipes.

## 🧪 Qualidade e testes

A validação do sistema deve considerar os requisitos funcionais e não funcionais, incluindo:

- Integridade das ordens de serviço e de seus itens.
- Consistência das movimentações de estoque.
- Controle de acesso conforme o perfil do usuário.
- Correção dos orçamentos e indicadores.
- Registro do histórico de execução e impedimentos.
- Tratamento de erros e recuperação de dados.

Os procedimentos de execução dos testes serão documentados conforme a implementação do projeto.

## 🛠️ Tecnologias

**Em definição.**

As tecnologias, dependências e ferramentas serão documentadas conforme a configuração efetivamente adotada no projeto.

## 🗺️ Roadmap

*Roadmap em desenvolvimento — novas etapas serão documentadas conforme a evolução do projeto.*

## 📚 Documentação

A documentação de requisitos está registrada no documento **ERS-M360-1.4.1 — Especificação de Requisitos**, versão 1.4.1, de 13 de outubro de 2026.

O documento apresenta o escopo, os perfis de usuários, os serviços, as interfaces, o backlog e os requisitos não funcionais do sistema.

## 👩‍💻 Equipe

Projeto desenvolvido por estudantes de Ciência da Computação da **Universidade Federal de Ouro Preto (UFOP)**:

- Ana Gabriela
- André Luiz
- Gabriel
- Rayssa

---

*Mecânica360 — organização, acompanhamento e informação para uma gestão mais eficiente de oficinas mecânicas.*