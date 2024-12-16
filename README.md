# Nosso BNFC

## Comandos para rodar:

``bnfc -m -c codigo.cf``

``make``

``./Testcodigo < test.codigo``

## Regras semânticas

Tabelas de Símbolos:
Criar tabelas de símbolos para armazenar informações sobre variáveis, funções, tipos e identificadores.
Suportar escopos aninhados (ex.: blocos {} e funções).
Verificação de Declarações:
Garantir que todos os identificadores sejam declarados antes de serem usados.
Lidar com declarações repetidas no mesmo escopo.
Verificação de Tipos:
Validar se operações aritméticas e relacionais são realizadas entre operandos compatíveis.
Garantir que tipos atribuídos em declarações e atribuições sejam consistentes.
Verificar tipos em chamadas de funções, parâmetros e retorno.
Análise de Escopo:
Diferenciar variáveis globais, locais e parâmetros de função.
Verificar a acessibilidade de identificadores em diferentes escopos.
Implementar a análise para o tipo Decl:
Validar tipos básicos e tipos compostos (vetores, matrizes e interfaces).
Lidar com ponteiros (Typont) e suas regras de uso.
Expressões Básicas:
Resolução de identificadores (EIdent).
Verificação de tipos para expressões aritméticas (EAdd, ESub, EMul).
Atribuições e Inicializações:
Validação de atribuições (EAss, EAssSimpl).
Verificação de inicializações de vetores e matrizes (EAssArray, EAssMatrix).
Suporte a ponteiros e referências (EPontMem, EPont).
Expressões de Comparação:
Suporte a operadores relacionais e igualdade (Equal, EDiff, ELt).
Chamadas de Função:
Validar existência e assinatura de funções (Call).
Verificar a compatibilidade dos argumentos passados.
Comandos de Controle de Fluxo:
Validar expressões condicionais em SWhile, SIf, SIfelse.
Garantir que loops SFor tenham expressões válidas.
Comandos de Retorno:
Validar que SReturn retorne valores consistentes com o tipo da função.
Rótulos e Goto:
Garantir que rótulos (SLabel) sejam únicos e acessíveis.
Verificar validade de saltos com SGoto.
Comandos de Interface:
Validar estrutura de interfaces (SInterface) e consistência com objetos associados.
