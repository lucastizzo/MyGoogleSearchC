# MyGoogleSearchC
Trabalho de Sistemas Distribuidos - Aplicação distribuida em C

Tutorial para rodar o projeto no Linux - no Windows deve ser parecido com exceção da instalação das bibliotecas
Para compilar o projeto - Compilar os arquivos includes e depois os arquivos .c
    
    1 - Instalar a biblioteca json-c
    
        sudo apt-get install libjson-c-dev
    
    2 - Olhar o arquivo para as configurações de C em ./.vscode no projeto, adaptar os diretorios 
    
        c_cpp_properties.json
        launch.json
        settings.json
        tasks.json
    
    3 - Executar com F5 no VScode dentro de qaulquer arquivo em /src/Servidor-C/ o codigo de compilação deve ser algo como:
        
        gcc -I/home/tizzo/projeto-SD/MygoogleSearchC/include /home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/*.c -o /home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/output/program -ljson-c 

Há um pequeno aviso ao compilar servidorHTTP.c que acusa uma declaração implicita da função 'pagina404' mas que não afeta o funcionamento do programa



Para compilar o teste - Executar com o servidor em execução
    
    1- Instalar a biblioteca curl, caso no linux o comando é esse
        sudo apt-get install libcurl4-openssl-dev
    2- Compilar o código com o comando
        gcc -o teste teste.c -lcurl -lm
    3- Executar
        ./teste

Deixei um arquivo CSV na pasta teste com os resultados dos teste local em minha maquina. 

