#ifndef cb_wrapper_server_h__
#define cb_wrapper_server_h__

#include "../common/lib_callback.h"
#include "base_server.h"
#include <thread>

class cb_wrapper_server : public base_server{
public:
	cb_wrapper_server(	
		const std::string& server_ip, unsigned short server_port,
		UTI_ON_CONNECTION_CB	in_conn_cb = NULL,
		UTI_ON_MSG_CB			in_msg_cb = NULL,
		UTI_ON_CLOSED_CB		in_closed_cb = NULL,
		UTI_ON_SENDED_CB		in_sended_cb = NULL,
		long					usr_data = 0);

	void start_svc();
	void stop_svc();
	bool flag_svc();

	virtual int on_connection_cb(std::shared_ptr<server_session> p_session, error_code ec);

	virtual int on_msg_cb(std::shared_ptr<server_session> p_session, error_code ec);

	virtual void on_closed_cb(std::shared_ptr<server_session> p_session, error_code ec);

	virtual int on_sended_cb(std::shared_ptr<server_session> p_session, error_code ec);

protected:
	void svc_thread_entry(int test);

	inline int filter_send(std::shared_ptr<server_session> p_session, char** pp_in_data, int in_len);
	inline int filter_data(char** pp_in_data, int in_len, std::string& out_data);

private:
	std::string				server_ip_;
	unsigned short			server_port_;
	UTI_ON_CONNECTION_CB	on_connection_cb_;
	UTI_ON_MSG_CB			on_msg_cb_;
	UTI_ON_CLOSED_CB		on_closed_cb_;
	UTI_ON_SENDED_CB		on_sended_cb_;
	long					usr_data_;
	volatile bool			flag_svc_;

	std::shared_ptr<std::thread>	sp_thrd_svc_;
};

#endif

