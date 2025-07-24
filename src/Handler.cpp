#include "Handler.h"

#include <boost/algorithm/string.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

// docRoot - root of the project
Handler::Handler(std::string docRoot) : core(docRoot) {
  this->docRoot = docRoot;
  functionsMap["recordings"] =
      std::bind(&Handler::handleRecordings, this, std::placeholders::_1,
                std::placeholders::_2);
}

// Обработка ошибок будет здесь и передевать lambda в эту функцию и деать send
// Hanlde request with first part "recordings" (docRoot/recordings/id)
std::string
Handler::handleRecordings(std::vector<std::string> vecReqPart,
                          const http::request<http::string_body> &req) {

  std::string handlePath = req.target().to_string();

  if (req.method() == http::verb::get) {
    pt::ptree root = core.getListRecorders();
    std::ostringstream oss;
    write_json(oss, root);
    return oss.str();
  } else if (req.method() == http::verb::delete_) {

    pt::ptree root = core.deleteFile(stoi(vecReqPart[1]));
    std::ostringstream oss;
    write_json(oss, root);
    return oss.str();
  }
  return "";
}

// This function produces an HTTP response for the given request.
// The type the response object depends on the contents of the request.
template <class Body, class Allocator, class Send>
void Handler::handleRequest(
    http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
  // Returns a good request
  auto const successResponse = [&req](boost::beast::string_view why) {
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = why.to_string() + "\n";
    res.prepare_payload();
    return res;
  };

  // Returns a bad request response
  auto const badRequest = [&req](boost::beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = why.to_string() + "\n";
    res.prepare_payload();
    return res;
  };

  // Returns a not found response
  auto const notFound = [&req](boost::beast::string_view target) {
    http::response<http::string_body> res{http::status::not_found,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resorce '" + target.to_string() + "' was not found \n";
    res.prepare_payload();
    return res;
  };

  // Split request

  std::vector<std::string> vecReqPart;

  std::string relativePath = req.target().to_string().substr(1);
  boost::split(vecReqPart, relativePath, boost::is_any_of("/"));

  auto it = functionsMap.find(vecReqPart[0]);

  if (it != functionsMap.end()) {
    auto result = it->second(vecReqPart, req);
    if (result != "") {
      return send(std::move(successResponse(result)));
    }
  }
  return send(std::move(badRequest("unknown request")));
}

// Handles an HTTP server connection void
void Handler::doSession(tcp::socket &socket) {
  bool close = false;
  boost::system::error_code ec;

  // This buffer is required to persist across reads
  boost::beast::flat_buffer buffer;

  // This lambda is used to send messages
  sendLambda<tcp::socket> lambda{socket, close, ec};

  // Read a request
  http::request<http::string_body> req;
  http::read(socket, buffer, req, ec);

  // Send the response
  handleRequest(std::move(req), lambda);

  // Send a TCP shutdown
  socket.shutdown(tcp::socket::shutdown_send, ec);

  // At this point the connection is closed gracefully
}
