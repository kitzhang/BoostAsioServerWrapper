#ifndef cb_wrapper_manager_h__
#define cb_wrapper_manager_h__

#include "cb_wrapper_server.h"
#include <tuple>
#include <mutex>

class cb_wrapper_manager {
private:
	static cb_wrapper_manager* inst_;
public:
	static cb_wrapper_manager* instance();

public:
	cb_wrapper_manager();
	int start_svc(	const std::string& server_ip, unsigned short server_port,
		UTI_ON_CONNECTION_CB	in_conn_cb = NULL,
		UTI_ON_MSG_CB			in_msg_cb = NULL,
		UTI_ON_CLOSED_CB		in_closed_cb = NULL,
		UTI_ON_SENDED_CB		in_sended_cb = NULL,
		long					usr_data = 0);

	void stop_svc(const std::string& server_ip, unsigned short server_port);

	bool validate_svc(const std::string& server_ip, unsigned short server_port,
		UTI_ON_CONNECTION_CB	in_conn_cb = NULL,
		UTI_ON_MSG_CB			in_msg_cb = NULL,
		UTI_ON_CLOSED_CB		in_closed_cb = NULL,
		UTI_ON_SENDED_CB		in_sended_cb = NULL,
		long					usr_data = 0);

protected:
	typedef std::shared_ptr<cb_wrapper_server> CB_WRAPPER_SERVER_SP;

	void save_svc(const std::string& server_ip, unsigned short server_port,
		CB_WRAPPER_SERVER_SP sp_cb_wrapper_server);

private:
	typedef std::list<std::tuple<std::string, unsigned short, CB_WRAPPER_SERVER_SP>> CB_WRAPPER_SERVER_TUPLE_LIST;
	CB_WRAPPER_SERVER_TUPLE_LIST	tuple_list_cb_wrapper_server_;

	std::mutex						std_mutex_;
};

#endif

