#ifndef __SOCKET_UTIL__
#define __SOCKET_UTIL

#ifdef _WIN32
#include<winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

/*
 * error codes
 */
#define SU_SUCCESS    0
#define SU_ERROR     -1

/*
 * 16 * 1024
 */
#define MAX_SEND_SIZE 16384

/*
 * public functions
 */
int init_socket_util();
void clean_socket_util();
int create_udp_socket(int ip_addr, int port);
int create_udp_socket_nobind(int port);
int send_udp_message(int sock_fd, void * message, int message_len, int destination_ip, int destination_port);
int receive_udp_message(int sock_fd, void * message, int message_len, int & source_ip, int & source_port);
int close_udp_socket(int sock_fd);

// socket utility class
class socket_utility {
private:
	WSADATA wsa;

	bool is_init = false;
	bool socket_utility_init();
public:
	/*
	 *
	 */
	socket_utility();
	/*
	 *
	 */
	~socket_utility();
	/*
	 * create UPD socket bind to ip and port
	 */
	int create_udp_socket(int ip_addr, int port);
	/*
	 * create UPD socket not bind to any ip
	 */
	int create_udp_socket_nobind(int port);
	/*
	 * send message
	 */
	int send_udp_message(int sock_fd, void * message, int message_len, int destination_ip, int destination_port);
	/*
	 * receive message
	 */
	int receive_udp_message(int sock_fd, void * message, int message_len, int & source_ip, int & source_port);
	/*
	 * close socket
	 */
	int close_udp_socket(int sock_fd);
};

#endif
