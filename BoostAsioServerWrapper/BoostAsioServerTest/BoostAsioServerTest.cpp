// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "../BoostAsioServerWrapper/BoostAsioServerWrapper.h"

int test_on_connection_cb(int session_id, unsigned short server_port, unsigned short client_port,
	const char* p_client_ip, int ip_len,
	char** pp_send_data, int* send_len, long usr_data)
{
	printf("[%s] on_connection session_id[%d] server_port[%d] client_ip[%s] client_port[%d] \n", 
		__FILE__, session_id, server_port, p_client_ip, client_port);

	return 0;
}

int test_on_msg_cb(int session_id, const char* p_recv_data, int recv_len,
	char** pp_send_data, int* send_len, long usr_data)
{
	printf("[%s] on_msg session_id[%d] recv_len[%d] recv_data[%s] \n",
		__FILE__, session_id, recv_len, p_recv_data);

	return 0;
}

void test_on_closed_cb(int session_id, long usr_data)
{
	printf("[%s] on_close session_id[%d] \n", __FILE__, session_id);
}

int test_on_sended_cb(int session_id, char** pp_send_data, int* send_len, long usr_data)
{
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Established [%d] \n", asio_server_get_value());
	std::string server_ip = "192.168.0.40";
	unsigned short server_port = 1220;

	int tmp_ret = asio_server_start_service(server_ip.c_str(), server_ip.length(), server_port,
		test_on_connection_cb, test_on_msg_cb, test_on_closed_cb,  test_on_sended_cb);	
	printf("start_server return[%d] \n", tmp_ret);

	char ch = 0;
	printf("press Enter key to continue ...");
	while ((ch = std::cin.get()) != EOF){
		if ('\n' == ch)	break;
	}

	asio_server_stop_service(server_ip.c_str(), server_ip.length(), server_port);

	printf("press Enter key to terminate ...");
	while ((ch = std::cin.get()) != EOF){
		if ('\n' == ch)	break;
	}
	return 0;
}

