#include "cb_wrapper_server.h"
#include "../common/comm_macro.h"

cb_wrapper_server::cb_wrapper_server(	
	const std::string& server_ip, unsigned short server_port,
	UTI_ON_SVR_CONN_CB		in_conn_cb,
	UTI_ON_SVR_MSG_CB		in_msg_cb,
	UTI_ON_SVR_CLOSED_CB	in_closed_cb,
	UTI_ON_SVR_SENDED_CB	in_sended_cb,
	long					usr_data)
: base_server(server_ip, server_port)
, server_ip_(server_ip)
, server_port_(server_port)
, on_connection_cb_(in_conn_cb)
, on_msg_cb_(in_msg_cb)
, on_closed_cb_(in_closed_cb)
, on_sended_cb_(in_sended_cb)
, usr_data_(usr_data)
{
	
}

void cb_wrapper_server::start_svc()
{
	int test = 0;
	this->sp_thrd_svc_ = std::shared_ptr<std::thread>(
		new std::thread(std::bind(&cb_wrapper_server::svc_thread_entry, this, test)) );
}

void cb_wrapper_server::stop_svc()
{
	stop_io_svc();

	if (this->sp_thrd_svc_.get())
	{
		this->sp_thrd_svc_->join();
		this->sp_thrd_svc_.reset();
	}
}

void cb_wrapper_server::svc_thread_entry(int test)
{
	start_next_async_accept();
	run_io_svc();
}

int cb_wrapper_server::on_connection_cb(std::shared_ptr<server_session> p_session, error_code ec){
	//printf("on_connection_cb \n");
	if (NULL == on_connection_cb_)	return 0;

	std::string client_ip = p_session->get_socket().remote_endpoint().address().to_string();
	unsigned short client_port = p_session->get_socket().remote_endpoint().port();
	int	 send_len = 0;
	char* p_send_data = NULL;	
	int tmp_ret = on_connection_cb_(p_session->get_session_id(), server_port_, client_port, 
									client_ip.c_str(), client_ip.length(), 
									&p_send_data, &send_len, usr_data_);
	if (0 != tmp_ret)	return tmp_ret;

	return filter_send(p_session, &p_send_data, send_len);
}

int cb_wrapper_server::on_msg_cb(std::shared_ptr<server_session> p_session, error_code ec){
	//printf("on_msg_cb data[%s] len[%d] \n", p_session->read_buf_str_.c_str(), p_session->read_buf_str_.length());
	if (NULL == on_msg_cb_)	return 0;

	int	 send_len = 0;
	char* p_send_data = NULL;
	int tmp_ret = on_msg_cb_(	p_session->get_session_id(), 
								p_session->read_buf_str_.c_str(), p_session->read_buf_str_.length(), 
								&p_send_data, &send_len, usr_data_);
	if (0 != tmp_ret)	return tmp_ret;

	return filter_send(p_session, &p_send_data, send_len);
}

void cb_wrapper_server::on_closed_cb(std::shared_ptr<server_session> p_session, error_code ec){
	//printf("on_close_cb \n");
	if (NULL == on_closed_cb_)	return;

	on_closed_cb_(p_session->get_session_id(), usr_data_);
}

int cb_wrapper_server::on_sended_cb(std::shared_ptr<server_session> p_session, error_code ec){
	//printf("on_send_cb \n");
	if (NULL == on_sended_cb_)	return 0;

	return on_sended_cb_(p_session->get_session_id(), usr_data_);
}

int cb_wrapper_server::filter_send(std::shared_ptr<server_session> p_session, char** pp_in_data, int in_len)
{
	std::string data_to_send;
	if (0 == filter_data(pp_in_data, in_len, data_to_send))
		return async_send(p_session, data_to_send);

	return 0;
}

int cb_wrapper_server::filter_data(char** pp_in_data, int in_len, std::string& out_data)
{
	if (in_len <= 0 || in_len >= MAX_A_FRAME_LENGTH)	return -1;

	char* p_data = (char*)*pp_in_data;
	if (NULL == p_data)	return -1;

	out_data.assign(p_data, in_len);
	delete[] p_data;	*pp_in_data = NULL;
	return 0;
}