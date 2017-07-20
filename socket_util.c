#include "socket_utility.h"

int is_init = 0;

static int is_socket_util_init() {
    return is_init;
}

int init_socket_util() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        is_init = 0;
    }
    is_init = 1;
#endif
}

void clean_socket_util() {
#ifdef _WIN32
    WSACleanup();
    is_init = 0;
#endif
}

int create_udp_socket(int ip_addr, int port) {
    SOCKET s;
    struct sockaddr_in s_ip_addr;

    if (!is_socket_util_init()) {
		return SU_ERROR;
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		return SU_ERROR;
	}

	memset(&s_ip_addr, 0, sizeof(struct sockaddr_in));
	s_ip_addr.sin_family = AF_INET;
	s_ip_addr.sin_port = htons(port);
	/*
     * If IP addr is 0 or 127.0.0.1 use INADDR_ANY
     */
	if ((ip_addr != 0) && (ip_addr != 0x7f000001)) {
		s_ip_addr.sin_addr.s_addr = htonl(ip_addr);
	}
	else {
		s_ip_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	if (bind(s, (struct sockaddr *)&s_ip_addr, sizeof(s_ip_addr)) == SOCKET_ERROR) {
		close_udp_socket(s);
		return SU_ERROR;
	}

    /*
     * TODO: non blocking mode
     */
#if 0
	bool w_opt = true;
	if (-1 == ioctlsocket(s, FIONBIO, (u_long *)&w_opt)) {
		close_udp_socket(s);
		return SU_ERROR;
	}
#endif

	return s;
}

int create_udp_socket_nobind(int port) {
    SOCKET s;

    if (!is_socket_util_init()) {
		return SU_ERROR;
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		return SU_ERROR;
	}

    /*
     * TODO: non blocking mode
     */
#if 0
	bool w_opt = true;
	if (-1 == ioctlsocket(s, FIONBIO, (u_long *)&w_opt)) {
		close_udp_socket(s);
		return SU_ERROR;
	}
#endif

	return s;
}

int send_udp_message(int sock_fd, void * message, int message_len, int destination_ip, int destination_port) {
    struct sockaddr_in s_destination_addr;
    char * send_buff = (char *)message;
    int ret = 0;

    if (sock_fd < 0) {
		return SU_ERROR;
	}

	if (message == 0 || message_len <= 0) {
		return SU_ERROR;
	}

	if (destination_ip == 0 || destination_port == 0) {
		return SU_ERROR;
	}

	memset(&s_destination_addr, 0, sizeof(s_destination_addr));
	s_destination_addr.sin_family = AF_INET;
	s_destination_addr.sin_addr.s_addr = htonl(destination_ip);
	s_destination_addr.sin_port = htons(destination_port);

	while (message_len > 0) {
		ret = sendto(sock_fd, send_buff, message_len, 0, (struct sockaddr*)&s_destination_addr, sizeof(s_destination_addr));
		if (ret == SOCKET_ERROR) {
			return SU_ERROR;
		}
		message_len -= ret;
		send_buff += ret;
	}

	return SU_SUCCESS;
}

int receive_udp_message(int sock_fd, void * message, int message_len, int & source_ip, int & source_port) {
    struct sockaddr_in s_source_addr = {0};
    char * recv_buff = (char *)message;
    int s_source_addr_len = sizeof(s_source_addr);
    int ret = 0;

    if (sock_fd < 0) {
		return SU_ERROR;
	}

	if (message == 0) {
		return SU_ERROR;
	}

	memset(message, 0, message_len);

	/*
     * TODO: make it non blocking
     */
	ret = recvfrom(sock_fd, recv_buff, message_len, 0, (struct sockaddr*)&s_source_addr, &s_source_addr_len);
	if (ret == SOCKET_ERROR) {
		return SU_ERROR;
	}

	source_ip = ntohl(s_source_addr.sin_addr.s_addr);
	source_port = ntohs(s_source_addr.sin_port);

	return ret;
}

int close_udp_socket(int sock_fd) {
    closesocket(sock_fd);
    return SU_SUCCESS;
}
