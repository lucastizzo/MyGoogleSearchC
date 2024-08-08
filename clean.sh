#!/bin/bash

# Apagar os arquivos de output
rm -rf ${workspaceFolder}/src/Servidor-C/output/program
rm -rf ${workspaceFolder}/src/loadbalancer/output/loadbalancer

# Apagar arquivos object (*.o)
find ${workspaceFolder} -name "*.o" -type f -delete

# Apagar diretórios de build
rm -rf ${workspaceFolder}/src/Servidor-C/build
rm -rf ${workspaceFolder}/src/loadbalancer/build

echo "Limpeza completa."
