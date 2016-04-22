#include "BoostAsioServerWrapper.h"
#include "cb_wrapper_manager.h"
//#include <memory>

int asio_server_get_value()
{
	//std::shared_ptr<int> tmp_p(new int);
	//int* p = tmp_p.get();
	//test_cast((std::shared_ptr<int>)p);
	return 20160418;
}

int asio_server_start_service(
	const char* server_ip, int ip_len, unsigned short server_port,
	UTI_ON_SVR_CONN_CB		in_conn_cb,
	UTI_ON_SVR_MSG_CB		in_msg_cb,
	UTI_ON_SVR_CLOSED_CB	in_closed_cb,
	UTI_ON_SVR_SENDED_CB	in_sended_cb,
	long					usr_data)
{
	if (NULL == server_ip || ip_len <= 0 || 0 == server_port)	return -1;

	return cb_wrapper_manager::instance()->start_svc(std::string(server_ip, ip_len), server_port,
		in_conn_cb, in_msg_cb, in_closed_cb, in_sended_cb, usr_data);
}

void asio_server_stop_service(const char* server_ip, int ip_len, unsigned short server_port)
{
	cb_wrapper_manager::instance()->stop_svc(std::string(server_ip, ip_len), server_port);
}


