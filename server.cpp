/*
SSL/TLS Test
 - hjkim, 2019.03.07


Cert
//$ openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
$ openssl req -x509 -nodes -newkey rsa:1024 -keyout mycert.pem -out mycert.pem


Source-based:
	https://stackoverflow.com/questions/2504356/transmission-and-verification-of-certificate-openssl-with-socket-in-c
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>


#define PORT		9734


int main(void) {
	int server_sockfd, client_sockfd;
	int server_len;
	socklen_t client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	int result = 0;
	//ssl initiation
	SSL_library_init();
	SSL_load_error_strings();
	SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_server_method());

	SSL_CTX_use_certificate_file(ctx, "./pub/server/cert/mycert.pem", SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ctx, "./pub/server/cert/mycert.pem", SSL_FILETYPE_PEM);


	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	server_len = sizeof(server_address);

	result = bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	if (result < 0) {
		printf("bind(): %d\n", result);
	}

	listen(server_sockfd, 5);

	while (1) {
		char ch;
		printf("server waiting\n");

		//accept connection

		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

		printf("Connected\n");

		SSL* ssl;
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client_sockfd);

		//handshake
		SSL_accept(ssl);
		printf("Handshake\n");

		//exchage message
		result = SSL_read(ssl, &ch, sizeof(ch));

		if (result < 0) {
			printf("SSL_read(): %d\n", result);
		}
		++ch;
		result = SSL_write(ssl, &ch, sizeof(ch));


		if (result < 0) {
			printf("SSL_write(): %d\n", result);
		}

		close(client_sockfd);
	}
}
