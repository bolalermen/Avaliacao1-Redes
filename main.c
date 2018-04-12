/*

Avaliação 1 de Redes de Computadores - ES41
Alunos:
Cristiane Harumi Servulo Masuki
Gabriel Barbieri Lermen
Wellington Murilo da Silva Nogueira

*/

#pragma comment(lib, "WS2_32.lib") /* program use socket library */
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <direct.h>
#include <winsock2.h> //biblioteca socket

struct
{
    char   *ext;
    char   *filetype;
}

extensions[] =
{
    {"gif", "image/gif"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"ico", "image/ico"},
    {"zip", "image/zip"},
    {"gz", "image/gz"},
    {"tar", "image/tar"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {0, 0}
};

/* web() handles a single web request, so it's ok to exit on errors */
void web(SOCKET cli_socket, int hit)
{
    FILE   *fp;
    int     j, buflen, len;
    long    idx, nbytes;
    char   *fstr;
    static char buffer[8096 + 1];

    /* ler a requisição do navegador através da conexão TCP */
    nbytes = recv(cli_socket, buffer, 8096, 0);

    /* checar se recebemos uma requisição do navegador */
    if (nbytes > 0 && nbytes < 8096)
    {
        buffer[nbytes] = 0;  /* conclude the buffer */
    }
    else
    {
        buffer[0] = 0; /* empty the buffer */
    }

    for (idx = 4; idx < 8096; idx++)
    {
        if (buffer[idx] == ' ')
        {
            buffer[idx] = 0;
            break;
        }
    }

    if (!strncmp(buffer+5, "/favicon.ico", 12))
    {
        sprintf(buffer, "HTTP/1.0 204 NO CONTENT\r\n\r\n");
        send(cli_socket, buffer, strlen(buffer), 0);
    }
    else
    {
        /* abrir o arquivo para leitura */
        if ((fp = fopen(&buffer[5], "rb")) == NULL)
        {
            fp = fopen("erro.html", "r");
        }

        sprintf(buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
        send(cli_socket, buffer, strlen(buffer), 0);

        /* enviar arquivos em blocos de 8KB - o último pode ser menor */
        while ((nbytes = fread(buffer, 1, 8096, fp)) > 0)
        {
            send(cli_socket, buffer, nbytes, 0);
        }
    }
}

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET  listenfd, socketfd;
    static struct sockaddr_in cli_addr;
    static struct sockaddr_in serv_addr;
    unsigned short port;
    int     idx, hit;
    size_t  length;
    int recv_size;
    char server_reply[2000], *message;

    /* Inicializar a biblioteca do sistema Winsock */
    printf("\Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        return 0;
    }

    printf("============================\n");
    printf("Inicializado.\n");
    printf("============================\n");

    /* Criar um socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Nao foi possivel criar o socket.\n");
    }
    printf("Socket criado.\n");
    printf("============================\n");

    /* Preparar a estrutura sockaddr_in */
    port = 80; //Configurar a porta de conexao
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    /* Ligar o socket */
    if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERRO: Nao e possivel ligar um socket a essa porta.\n");
    }
    printf("Ligacao do socket concluida.\n");
    printf("============================\n");

    /* Ouvir as conexoes recebidas */
    if (listen(listenfd, 64) < 0)
    {
        printf("Erro na conexao");
    }
    printf("Aguardando novas conexoes...\n\n\n");

    /* Loop infinito para processar mais de uma requisição. */
    for (hit = 1; ; hit++)
    {
        length = sizeof(cli_addr);

        /* Aceitar conexao */
        socketfd = accept(listenfd, (struct sockaddr *) &cli_addr, &length);

        //Conectar ao servidor remoto
        /*  if (connect(socketfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0)
            {
                puts("Erro de Conexao");           
            }
            puts("Conectado.");

            message = "GET / HTTP/1.1\r\n\r\n";
            if( send(socketfd , message , strlen(message) , 0) < 0)
            {
                puts("Falha no Envio");
            }
            puts("Dados Enviados.\n");

            recv_size = recv(socketfd, server_reply, 2000, 0);
            server_reply[recv_size] = '\0';
            printf("%s\r\n",server_reply);*/

        /* Chamar a funcao web() para processar uma solicitacao HTTP */
        web(socketfd, hit);
        shutdown(socketfd, SD_BOTH);
    }
}
