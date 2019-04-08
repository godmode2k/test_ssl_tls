/*
SSL/TLS Test
 - hjkim, 2019.03.07


Source-based:
	https://stackoverflow.com/questions/2504356/transmission-and-verification-of-certificate-openssl-with-socket-in-c
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>


#define PORT		9734



int main(int argc, char* argv[]) {
	int sockfd = 0;
	struct sockaddr_in address;
    struct addrinfo hints, *res;
	char* host = NULL;

	SSL_CTX* ctx = NULL;
	SSL* ssl = NULL;
	BIO* sbio = NULL;


	if ( argc < 2 ) {
		printf( "Usage: %s [host]\n", argv[0] );
		return -1;
	}
	host = argv[1];


	SSL_library_init();
	SSL_load_error_strings();
	ctx = SSL_CTX_new(TLSv1_2_client_method());    
	if ( !ctx ) {
		printf( "SSL_CTX_new(): error\n");
		//exit(1);
		goto __RELEASE;
	}

	//SSL_CTX_load_verify_locations(ctx, "./pub/client/privkey_x509.pem", 0);
	//SSL_CTX_use_PrivateKey_file(ctx, "./pub/private.pem", SSL_FILETYPE_PEM);

	printf("SSL initialized\n");


	///*
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	//address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_addr.s_addr = inet_addr(argv[1]);
	address.sin_port = htons(PORT);


	if ( connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0 ) {
		printf("connect(): error\n");
		//exit(1);
		goto __RELEASE;

	}
	printf("connect()\n");
	//*/

	/*
    if (getaddrinfo(host, "https", &hints, &res) != 0) {
        fprintf(stderr, "Fail to resolve ip address\n");
        //return EXIT_FAILURE;
		goto __RELEASE;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        fprintf(stderr, "Fail to create a socket.\n");
        //return EXIT_FAILURE;
		goto __RELEASE;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        printf("Connection error.\n");
        //return EXIT_FAILURE;
		goto __RELEASE;
    }
	*/



	ssl = SSL_new(ctx);
	sbio = BIO_new(BIO_s_socket());
	if ( !ssl || !sbio ) {
		goto __RELEASE;
	}

	BIO_set_fd(sbio, sockfd, BIO_NOCLOSE);
	SSL_set_bio(ssl, sbio, sbio);
	//SSL_CTX_set_verify_depth(ctx, 1);
	//SSL_set_fd(ssl, sockfd);
	if ( SSL_connect(ssl) <= 0 ) {
		printf( "SSL_connect(): error\n" );
		//exit(1);
		goto __RELEASE;
	}
	printf("SSL_connect()\n");


	if (SSL_get_peer_certificate(ssl) != NULL) {
		/*
		//_ssl = SSL_new(ctx);
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

		int res = SSL_get_verify_result(ssl);
		printf("Certificate Check Result: %d\n", res);
		if (SSL_get_verify_result(ssl) != X509_V_OK) {
			printf("Certiticate Verification Failed\n");
			return 0;
			//exit(1);
		}
		else {
			printf("Certiticate Verification Succeeded\n");
		}
		*/

		{
			///*
			char ch = 'A';
			//printf("\nSay Char: \n");
			//scanf("%c", &ch);

			SSL_write(ssl, &ch, 1);
			SSL_read(ssl, &ch, 1);
			printf("char from server = %c\n", ch);
			//*/


			/*
			const char* host = "daum.net";
			char msg[100] = {0,};
			sprintf(msg, "GET / HTTP/1.0\r\nHost: %s\r\n\r\n", host);
			SSL_write(ssl, msg, strlen(msg));

			int buf_size = 256;
			char buf[buf_size];
			size_t read_size;

			do {
				read_size = SSL_read(ssl, buf, buf_size);
				write(1, buf, read_size);
			} while(read_size > 0);
			*/
		}
	}



__RELEASE:
	if ( ssl ) {
		SSL_shutdown(ssl);  
    	SSL_free(ssl);
		ssl = NULL;
	}

	if ( ctx ) {
    	SSL_CTX_free(ctx);
		ctx = NULL;
	}

	//if ( sbio ) {
	//	//BIO_free_all(sbio);
	//	BIO_free(sbio);
	//	sbio = NULL;
	//}

    ERR_free_strings();
	close(sockfd);


	return 0;
}


