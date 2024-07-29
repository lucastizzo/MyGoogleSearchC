# MyGoogleSearchC

Trabalho de Sistemas Distribuídos - Aplicação distribuída em C

## Tutorial para Rodar o Projeto no Linux

> No Windows deve ser parecido, com exceção da instalação das bibliotecas.

### Passos para Compilar o Projeto

1. **Instalar a biblioteca `json-c`**:
    ```sh
    sudo apt-get install libjson-c-dev
    ```

2. **Configurar o Visual Studio Code**:
    - Verifique e adapte os diretórios nos arquivos de configuração em `./.vscode`:
        - `c_cpp_properties.json`
        - `launch.json`
        - `settings.json`
        - `tasks.json`

3. **Compilar o Projeto**:
    - No Visual Studio Code, abra qualquer arquivo em `/src/Servidor-C/` e pressione `F5` para compilar.
    - O comando de compilação deve ser algo como:
        ```sh
        gcc -I/home/tizzo/projeto-SD/MygoogleSearchC/include /home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/*.c -o /home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/output/program -ljson-c
        ```

> **Nota**: Há um pequeno aviso ao compilar `servidorHTTP.c` que acusa uma declaração implícita da função `pagina404`, mas isso não afeta o funcionamento do programa.

### Passos para Compilar o Teste

1. **Instalar a biblioteca `curl`**:
    ```sh
    sudo apt-get install libcurl4-openssl-dev
    ```

2. **Compilar o Código de Teste**:
    ```sh
    gcc -o teste teste.c -lcurl -lm
    ```

3. **Executar o Teste**:
    ```sh
    ./teste
    ```

> **Nota**: Deixei um arquivo CSV na pasta `teste` com os resultados dos testes locais em minha máquina.

---

## Estrutura do Projeto

- **`/include`**: Arquivos de cabeçalho (.h)
- **`/src/Servidor-C`**: Código-fonte do servidor (.c)
- **`/teste`**: Código de teste e resultados

## Dependências

- `json-c`: Biblioteca para manipulação de JSON em C.
- `curl`: Biblioteca para transferências de dados com URLs.

---