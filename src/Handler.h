#ifndef HANDLER_H
#define HANDLER_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Core.h"

namespace http = boost::beast::http;

class Handler {
public:
  Handler(std::string docRoot);

  void doSession(boost::asio::ip::tcp::socket &socket);

private:
  Core core;

  std::string docRoot;

  template <class Body, class Allocator, class Send>
  void handleRequest(http::request<Body, http::basic_fields<Allocator>> &&req,
                     Send &&send);

  std::unordered_map<std::string, std::function<std::string(
                                      std::vector<std::string> vecReqPart,
                                      http::request<http::string_body> &)>>
      functionsMap;

  std::string handleRecordings(std::vector<std::string> vecReqPart,
                               const http::request<http::string_body> &req);

  // This is the C++11 equivalent of a generic lambda.
  // The function object is used to send an HTTP message.
  template <class Stream> struct sendLambda {
    Stream &stream_;
    bool &close_;
    boost::system::error_code &ec_;

    explicit sendLambda(Stream &stream, bool &close,
                        boost::system::error_code &ec)
        : stream_(stream), close_(close), ec_(ec) {}

    template <bool isRequest, class Body, class Fields>
    void operator()(http::message<isRequest, Body, Fields> &&msg) const {
      // Determine if we shuld close the connection after
      close_ = msg.need_eof();

      // We need the servializer here because the serializer requires
      // a non-const file_body, and teh message oriented version of
      // http::write only workswith const messages.
      http::serializer<isRequest, Body, Fields> sr{msg};
      http::write(stream_, sr, ec_);
    }
  };
};

#endif // HANDLER_H
