///// Includes /////

#include "main.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <zlib.h>

///// Functions /////

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

int main(int argc, char** argv)
{
  //tODO std::ifstream input("test.txt");

  try
  {
    auto const host = "www.amazon.co.uk";
    //TODO auto const host = "www.monoclesecurity.com";
    auto const port = "443";
    auto const target = "/s?k=hdmi&crid=109S0HEPG2L7Z&sprefix=hdmi%2Caps%2C93&ref=nb_sb_noss_1";
    int version = 11;

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx(ssl::context::tlsv12_client);

    // This holds the root certificate used for verification
    //TODO load_root_certificates(ctx);

    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_none);

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    // Look up the domain name
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(stream).connect(results);

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{ http::verb::get, target, version };
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36");




    std::cout << req << std::endl << "====================" << std::endl;//TODO

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    //TODO http::response<http::dynamic_body> res;
    http::response<http::string_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);


    //decompress with boost's interface
    boost::iostreams::array_source src{ res.body().data(), res.body().size()};
    boost::iostreams::filtering_istream is;

    //boost::iostreams::zlib_params zparams{};
    if (res["Content-Encoding"] == "deflate")
    {
      std::cout << "decompressing " << res["Content-Encoding"] << std::endl;
      std::cout << "-------------------------------------------------" << '\n';
      is.push(boost::iostreams::zlib_decompressor{ -MAX_WBITS });			// deflate
    }
    else if (res["Content-Encoding"] == "gzip")
    {
      std::cout << "decompressing " << res["Content-Encoding"] << std::endl;
      std::cout << "-------------------------------------------------" << '\n';
      is.push(boost::iostreams::gzip_decompressor{});						// gzip
    }
    else if (res["Content-Encoding"] == "")
    {
      std::cout << "uncompressed " << res["Content-Encoding"] << std::endl;
      std::cout << "-------------------------------------------------" << '\n';
    }
    is.push(src);

    std::ofstream test("test.txt");//TODO
    boost::iostreams::copy(is, test);//TODO read it into a std::stringstream first, then a std::string() return, just abort on failure

    // Gracefully close the stream
    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == net::error::eof)
    {
      ec.clear();
    }
    if (ec)
    {
      throw beast::system_error(ec);
    }

    // If we get here then the connection is closed gracefully
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
