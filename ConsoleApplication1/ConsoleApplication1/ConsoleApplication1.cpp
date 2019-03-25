// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <cstdlib>
#include <string>

using namespace std;
namespace beast = boost::beast;       // from <boost/asio/ip/tcp.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

void sendToThingsboard(string key, string value, string token)
{
	try
	{
		string host = "demo.thingsboard.io";
		string port = "80";
		string target = "/api/v1/" + token + "/telemetry";
		string json = "{\"" + key + "\": " + value + "}";//пример "{\"temperature\": 25}"
		int version = 11;
		std::cout << json << std::endl;

		auto start = std::chrono::steady_clock::now();
		// The io_context is required for all I/O
		boost::asio::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver{ ioc };
		tcp::socket socket{ ioc };

		// Look up the domain name
		auto const results = resolver.resolve(host, port);

		// Make the connection on the IP address we get from a lookup
		boost::asio::connect(socket, results.begin(), results.end());

		//create POST request
		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		request_stream << "POST " << target << " HTTP/1.1\r\n";
		request_stream << "Host:" << host << "\r\n";
		request_stream << "User-Agent: curl/7.55.1 \r\n";
		request_stream << "Content-Type:application/json \r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Content-Length: " << json.length() << "\r\n\r\n";
		request_stream << json;

		// Send the HTTP request to the remote host
		boost::asio::write(socket, request);
		
		//ТО ЧТО ДАЛЬШЕ ПОИДЕЕ ТЕБЕ НЕ НУЖНО

		// This buffer is used for reading and must be persisted
		boost::beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::dynamic_body> res;

		// Receive the HTTP response
		http::read(socket, buffer, res);

		// Write the message to standard out
		std::cout << res << std::endl;

		// Gracefully close the socket
		boost::system::error_code ec;
		socket.shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != boost::system::errc::not_connected)
			throw boost::system::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		//return EXIT_FAILURE;
	}
}

int main()
{
	string key = "temperature";//ключ параметра датчика - у тебя там будет temperature или light
	string value = "53";//значение ключа - у тебя там буде то что показывает датчик
	string token = "rhOUfSGn2nZbolDYRi61"; //api token - берется из личного кабинета thingsboard у устройства
	sendToThingsboard(key, value, token);
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
