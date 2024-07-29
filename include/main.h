#include "servidorHTTP.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "paginasPadrao.h"
#include "rotas.h"
#include <pthread.h>
#include <locale.h>

struct respostaServidor;

void iniciaThreadConexao(int servidorSocket);
