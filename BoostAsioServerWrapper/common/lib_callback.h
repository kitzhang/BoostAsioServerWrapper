#ifndef lib_callback_h__
#define lib_callback_h__

typedef int(*UTI_ON_CONNECTION_CB)(int session_id, unsigned short server_port, unsigned short client_port,
	const char* p_client_ip, int ip_len,
	char** pp_send_data, int* send_len, long usr_data);

typedef int(*UTI_ON_MSG_CB)(int session_id, const char* p_recv_data, int recv_len,
	char** pp_send_data, int* send_len, long usr_data);

typedef void(*UTI_ON_CLOSED_CB)(int session_id, long usr_data);

typedef int(*UTI_ON_SENDED_CB)(int session_id, char** pp_send_data, int* send_len, long usr_data);

#endif

