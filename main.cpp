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

int main(int argc, char** argv)
{
  //tODO std::ifstream input("test.txt");

  try
  {
    // Connect
    const char* host = "www.amazon.co.uk";
    const char* port = "443";
    const char* target = "/s?k=hdmi&crid=109S0HEPG2L7Z&sprefix=hdmi%2Caps%2C93&ref=nb_sb_noss_1";
    const int version = 11;
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
    boost::asio::ip::tcp::tcp::resolver resolver(ioc);
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ctx);
    const boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> results = resolver.resolve(host, port);
    boost::beast::get_lowest_layer(stream).connect(results);
    stream.handshake(boost::asio::ssl::stream_base::client);
    // Send Request
    boost::beast::http::request<boost::beast::http::string_body> req(boost::beast::http::verb::get, target, version);
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36");
    boost::beast::http::write(stream, req);
    // Read response
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream, buffer, res);
    // Decompress
    boost::iostreams::array_source src(res.body().data(), res.body().size());
    boost::iostreams::filtering_istream is;
    if (res["Content-Encoding"] == "deflate")
    {
      is.push(boost::iostreams::zlib_decompressor{ -MAX_WBITS });
    }
    else if (res["Content-Encoding"] == "gzip")
    {
      is.push(boost::iostreams::gzip_decompressor{});
    }
    else if (res["Content-Encoding"] == "")
    {
      // Ignore
    }
    else
    {
      std::cout << "Bad format: " << res["Content-Encoding"] << std::endl;
    }
    is.push(src);
    // Parse the response


    //TODO

    std::ofstream test("test.txt");//TODO
    boost::iostreams::copy(is, test);//TODO read it into a std::stringstream first, then a std::string() return, just abort on failure

    // Close the stream
    boost::beast::error_code ec;
    stream.shutdown(ec);
    if (ec == boost::asio::error::eof)
    {
      ec.clear();
    }
    if (ec)
    {
      throw boost::beast::system_error(ec);
    }
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
