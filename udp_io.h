#include <sys/socket.h>
#include <netinet/in.h>
#define RECVBUF 256
typedef struct{
int c_sockfd;
int s_sockfd;
struct sockaddr_in servaddr;
struct sockaddr_in cliaddr;
}ip_connection_t;

int udpio_init(ip_connection_t *c, const char *port);
ssize_t udpio_recv(ip_connection_t *ctx, char *buf);
ssize_t udpio_send(ip_connection_t *c, void *data, size_t len);
