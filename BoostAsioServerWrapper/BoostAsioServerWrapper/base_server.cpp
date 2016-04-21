#include "base_server.h"
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include "../common/comm_macro.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// server_session
void server_session::reset_unit()
{
	tmp_header_ = 0;
	unit_total_len_ = 0;
	recv_header_len_ = 0;
	read_buf_str_.clear();
}

int server_session::get_session_id()
{
	return session_id_;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// base_server

base_server::base_server(const std::string& server_ip, unsigned short server_port)
: m_acceptor(m_iosev, tcp::endpoint(ip::address_v4::from_string(server_ip.c_str()), server_port), false)
{
	std::cout << "ip [" << server_ip.c_str() << "] " 
			  << "tcp port[" << server_port 
			  << "] start listening ok!" << std::endl;
}


void base_server::start_next_async_accept()
{
	std::shared_ptr<server_session> p_session(new server_session(m_iosev));

	m_acceptor.async_accept(p_session->get_socket(),
		boost::bind(&base_server::base_on_connection_cb, this, p_session, boost::asio::placeholders::error));
}

void base_server::run_io_svc()
{
	m_iosev.run();
}

void base_server::stop_io_svc()
{
	m_iosev.stop();
}

void base_server::base_on_connection_cb(std::shared_ptr<server_session> p_session, error_code ec)
{
	//static int index = 1;
	std::cout << "new client " <</* index++ << " "  <<*/p_session->get_socket().remote_endpoint().address()
		<< ":" << p_session->get_socket().remote_endpoint().port() << std::endl;

	if ( 0 != on_connection_cb(p_session, ec) )
	{	std::cout << "session close acitvely on connection" << std::endl;	return;	}

	if (ec) { std::cout << ec.message() << std::endl;	return; }

	//继续等待连接
	start_next_async_accept();

	boost::asio::async_read(
		p_session->get_socket(), boost::asio::buffer((char*)&p_session->tmp_header_, sizeof(p_session->tmp_header_)),
		boost::bind(&base_server::base_on_msg_header_cb, this, p_session,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
		);
}

void base_server::base_on_msg_header_cb(std::shared_ptr<server_session> p_session, error_code ec, std::size_t bytes_transferred)
{
	if (ec)	{ base_server::base_close(p_session, ec);	return; }

	if (NET_DATA_HEADER_LEN != bytes_transferred){
		std::cout << "read header failed len: " << bytes_transferred << std::endl;
		base_server::base_close(p_session, ec);	return;
	}

	//std::cout	<< "[" << p_session->get_socket().remote_endpoint().address() 
	//			<< ":" << p_session->get_socket().remote_endpoint().port() << "] "
	//			<< "len[" << bytes_transferred << "] "
	//			<< p_session->getBuf() << std::endl;


	p_session->unit_total_len_ = ntohl(p_session->tmp_header_);
	if (p_session->unit_total_len_ >= MAX_A_FRAME_LENGTH || 0 == p_session->unit_total_len_){
		std::cout << "read_len[" << p_session->unit_total_len_
			<< "] out of range !!!!!!!!!!!!!!!! " << std::endl;
		base_server::base_close(p_session, ec);	return;
	}

	//alloc buffer memory
	p_session->read_buf_str_.clear();
	p_session->recv_header_len_ = 0;
	p_session->tmp_header_ = 0;
	p_session->read_buf_str_.append(p_session->unit_total_len_, 0);

	boost::asio::async_read(
		p_session->get_socket(), boost::asio::buffer(const_cast<char*>(p_session->read_buf_str_.c_str()), p_session->unit_total_len_),
		boost::bind(&base_server::base_on_msg_content_cb, this, p_session,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
		);
}

void base_server::base_on_msg_content_cb(std::shared_ptr<server_session> p_session, error_code ec, std::size_t bytes_transferred)
{
	if (ec)	{ base_server::base_close(p_session, ec);	return; }

	if (p_session->unit_total_len_ != bytes_transferred)
	{
		std::cout << "read data failed len: " << bytes_transferred << std::endl;
		base_server::base_close(p_session, ec);	return;
	}

	std::string& ref_buf = p_session->read_buf_str_;

	if (0 != on_msg_cb(p_session, ec) )
	{	std::cout << "session close acitvely on msg" << std::endl;	return;	}

	p_session->reset_unit();

	boost::asio::async_read(
		p_session->get_socket(), boost::asio::buffer((char*)&p_session->tmp_header_, sizeof(p_session->tmp_header_)),
		boost::bind(&base_server::base_on_msg_header_cb, this, p_session,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
		);
}

void base_server::base_close(std::shared_ptr<server_session> p_session, error_code ec)
{
	std::cout << boost::system::system_error(ec).what() << std::endl;

	std::cout << "[" << p_session->get_socket().remote_endpoint().address()
		<< ":" << p_session->get_socket().remote_endpoint().port() << "] " << "...... disconnect" << std::endl;

	p_session->get_socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, const_cast<boost::system::error_code&>(ec));
	p_session->get_socket().close(const_cast<boost::system::error_code&>(ec));

	on_closed_cb(p_session, ec);
}

int base_server::async_send(std::shared_ptr<server_session> p_session, const std::string& data)
{
	//////////////////////////////////////////////////////////////////////////
	if (data.empty())			return -1;
	//if (p_session->get_socket().is_open()){	std::cout << "socket is not open" << std::endl;		return -1;	}

	if (data.length() >= MAX_A_FRAME_LENGTH){
		error_code ec;
		std::cout << "send_len[" << data.length() << "] out of range !!!!!!!!!!!!!!!! " << std::endl;
		base_server::base_close(p_session, ec);	return -1;
	}

	u_long net_len = htonl(data.length());
	//////////////////////////////////////////////////////////////////////////
	{
		WRITE_LOCK rd_lock(p_session->rw_mutex_);
		if (p_session->send_buf_str_.empty() && p_session->send_list_buf_.empty()){
			p_session->send_buf_str_.append((char*)&net_len, sizeof(u_long));
			p_session->send_buf_str_.append(data.c_str(), data.length());
		}
		else{
			std::string tmp_data;
			tmp_data.append((char*)&net_len, sizeof(u_long));
			tmp_data.append(data.c_str(), data.length());
			p_session->send_list_buf_.push_back(tmp_data);
			return 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	boost::asio::async_write(
		p_session->get_socket(), boost::asio::buffer(const_cast<char*>(p_session->send_buf_str_.c_str()), p_session->send_buf_str_.length()),
		boost::bind(&base_server::base_on_sended_cb, this, p_session,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
		);

	return 0;
}

void base_server::base_on_sended_cb(std::shared_ptr<server_session> p_session, error_code ec, std::size_t bytes_transferred)
{
	//////////////////////////////////////////////////////////////////////////
	if (ec)	{ base_server::base_close(p_session, ec);	return; }

	if (p_session->send_buf_str_.length() != bytes_transferred){
		std::cout << "send data failed len: " << bytes_transferred << std::endl;
		base_server::base_close(p_session, ec);	return;
	}

	if (0 != on_sended_cb(p_session, ec) )
	{	std::cout << "session close acitvely on sended" << std::endl;	return;	}

	//////////////////////////////////////////////////////////////////////////
	{
		WRITE_LOCK rd_lock(p_session->rw_mutex_);
		p_session->send_buf_str_.clear();
		if (p_session->send_list_buf_.empty())	return;

		std::string& ref_buf = p_session->send_list_buf_.front();
		p_session->send_buf_str_.append(ref_buf.c_str(), ref_buf.length());
		p_session->send_list_buf_.pop_front();
	}

	//////////////////////////////////////////////////////////////////////////
	boost::asio::async_write(
		p_session->get_socket(), boost::asio::buffer(const_cast<char*>(p_session->send_buf_str_.c_str()), p_session->send_buf_str_.length()),
		boost::bind(&base_server::base_on_sended_cb, this, p_session,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
		);
}


