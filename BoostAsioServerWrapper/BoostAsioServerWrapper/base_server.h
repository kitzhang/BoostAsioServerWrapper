#ifndef base_server_h__
#define base_server_h__

#ifdef _MSC_VER
#define _WIN32_WINNT 0x0501
#endif

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <memory>
#include <atomic>
#include "../common/type_macro.h"

using namespace boost::asio;
using boost::system::error_code;
using boost::asio::ip::tcp;

class server_session
{
public:
	server_session(io_service& iosev)
		: tmp_header_(0)
		, unit_total_len_(0)
		, recv_header_len_(0)
		, m_socket(iosev)
	{
		static std::atomic<int> tmp_session_id = 0;
		session_id_ = ++tmp_session_id;
	}

	inline ip::tcp::socket& get_socket()	{	return m_socket;	}
	inline void reset_unit();
	int get_session_id();

public:
	u_long tmp_header_;
	u_long unit_total_len_;
	u_long recv_header_len_;

	std::string read_buf_str_;
	//multi send
	std::string send_buf_str_;
	std::list<std::string>  send_list_buf_;
	boost::shared_mutex	rw_mutex_;

private:
	ip::tcp::socket m_socket;
	int	   session_id_;
};

class base_server
{
public:
	base_server(const std::string& server_ip, unsigned short server_port);

	//////////////////////////////////////////////////////////////////////////
	//start next connection
	void start_next_async_accept();
	void run_io_svc();	// will block
	void stop_io_svc();
	bool flag_svc();


	//connection callback
	void base_on_connection_cb(std::shared_ptr<server_session> pSession, error_code ec);

	//message callback
	void base_on_msg_header_cb(std::shared_ptr<server_session> pSession, error_code ec, std::size_t bytes_transferred);
	void base_on_msg_content_cb(std::shared_ptr<server_session> pSession, error_code ec, std::size_t bytes_transferred);

	void base_close(std::shared_ptr<server_session> pSession, error_code ec);

	//////////////////////////////////////////////////////////////////////////
	//async send data
	int async_send(std::shared_ptr<server_session> pSession, const std::string& data);
	void base_on_sended_cb(std::shared_ptr<server_session> pSession, error_code ec, std::size_t bytes_transferred);

	//for out callback
	virtual int on_connection_cb(std::shared_ptr<server_session> pSession, error_code ec) = 0;
	virtual int on_msg_cb(std::shared_ptr<server_session> pSession, error_code ec) = 0;
	virtual void on_closed_cb(std::shared_ptr<server_session> pSession, error_code ec) = 0;
	virtual int on_sended_cb(std::shared_ptr<server_session> pSession, error_code ec) = 0;
	
private:
	io_service			m_iosev;
	ip::tcp::acceptor	m_acceptor;
	volatile bool		flag_svc_;
};

#endif


