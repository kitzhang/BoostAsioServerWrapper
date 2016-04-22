#ifndef boost_asio_server_wrapper_h__
#define boost_asio_server_wrapper_h__

#include "../common/lib_export.h"
#include "../common/lib_server_callback.h"

#ifdef __cplusplus
extern "C" 
{
#endif

LIB_API int asio_server_get_value();

LIB_API int asio_server_start_service(
	const char* server_ip, int ip_len, unsigned short server_port,
	UTI_ON_SVR_CONN_CB		in_conn_cb = 0,
	UTI_ON_SVR_MSG_CB		in_msg_cb = 0,
	UTI_ON_SVR_CLOSED_CB	in_closed_cb = 0,
	UTI_ON_SVR_SENDED_CB	in_sended_cb = 0,
	long					usr_data = 0);

LIB_API void asio_server_stop_service(const char* server_ip, int ip_len, unsigned short server_port);

#ifdef __cplusplus
}
#endif

#endif


