#include "cb_wrapper_manager.h"

cb_wrapper_manager* cb_wrapper_manager::inst_ = NULL;
cb_wrapper_manager* cb_wrapper_manager::instance()
{
	if (NULL == inst_)	inst_ = new cb_wrapper_manager;

	return inst_;
}

cb_wrapper_manager::cb_wrapper_manager()
{
	
}

int cb_wrapper_manager::start_svc(	
	const std::string& server_ip, unsigned short server_port,
	UTI_ON_SVR_CONN_CB		in_conn_cb,
	UTI_ON_SVR_MSG_CB		in_msg_cb,
	UTI_ON_SVR_CLOSED_CB	in_closed_cb,
	UTI_ON_SVR_SENDED_CB	in_sended_cb,
	long					usr_data)

{
	if (server_ip.empty() || 0 == server_port)
		return -1;

	if (validate_svc(server_ip, server_port, in_conn_cb, in_msg_cb,
		in_closed_cb, in_sended_cb, usr_data))
		return 0;

	CB_WRAPPER_SERVER_SP	sp_cb_wrapper_server;
	try{
		sp_cb_wrapper_server = CB_WRAPPER_SERVER_SP(
			new cb_wrapper_server(	server_ip, server_port,
									in_conn_cb, in_msg_cb, 
									in_closed_cb, in_sended_cb, 
									usr_data) );
		sp_cb_wrapper_server->start_svc();
	}
	catch (std::exception& e){
		printf("ip[%s] port[%d] failed to start listen! [%s] \n", server_ip.c_str(), server_port, e.what());
		return -1;
	}

	save_svc(server_ip, server_port, sp_cb_wrapper_server);	
	return 0;
}

void cb_wrapper_manager::stop_svc(const std::string& server_ip, unsigned short server_port)
{
	if (server_ip.empty() || 0 == server_port)	return;

	std::unique_lock<std::mutex> g(std_mutex_);
	CB_WRAPPER_SERVER_TUPLE_LIST::iterator iter = tuple_list_cb_wrapper_server_.begin();
	for (; iter != tuple_list_cb_wrapper_server_.end(); ++iter){
		CB_WRAPPER_SERVER_TUPLE_LIST::reference ref = *iter;
		auto tmp_server_ip = std::get<0>(ref);
		auto tmp_server_port = std::get<1>(ref);
		if (!(server_ip == tmp_server_ip && server_port == tmp_server_port))	continue;

		CB_WRAPPER_SERVER_SP sp_cb_wrapper_server = std::get<2>(ref);
		sp_cb_wrapper_server->stop_svc();
		sp_cb_wrapper_server.reset();
		tuple_list_cb_wrapper_server_.remove(ref);
		break;
	}
}

bool cb_wrapper_manager::validate_svc(
	const std::string& server_ip, unsigned short server_port,
	UTI_ON_SVR_CONN_CB		in_conn_cb,
	UTI_ON_SVR_MSG_CB		in_msg_cb,
	UTI_ON_SVR_CLOSED_CB	in_closed_cb,
	UTI_ON_SVR_SENDED_CB	in_sended_cb,
	long					usr_data)
{
	std::unique_lock<std::mutex> g(std_mutex_);
	if (tuple_list_cb_wrapper_server_.empty())	return false;

	CB_WRAPPER_SERVER_TUPLE_LIST::iterator iter = tuple_list_cb_wrapper_server_.begin();
	for (; iter != tuple_list_cb_wrapper_server_.end(); ++iter){
		 CB_WRAPPER_SERVER_TUPLE_LIST::reference ref = *iter;
		 auto tmp_server_ip = std::get<0>(ref);
		 auto tmp_server_port = std::get<1>(ref);
		 if (!(server_ip == tmp_server_ip && server_port == tmp_server_port))	continue;

		 CB_WRAPPER_SERVER_SP sp_cb_wrapper_server = std::get<2>(ref);
		 if (!sp_cb_wrapper_server->flag_svc())	sp_cb_wrapper_server->start_svc();
		 return true;
	}
	return false;
}

void cb_wrapper_manager::save_svc(
	const std::string& server_ip, unsigned short server_port,
	CB_WRAPPER_SERVER_SP sp_cb_wrapper_server)
{
	std::unique_lock<std::mutex> g(std_mutex_);
	tuple_list_cb_wrapper_server_.push_back(std::make_tuple(server_ip, server_port, sp_cb_wrapper_server));
}
