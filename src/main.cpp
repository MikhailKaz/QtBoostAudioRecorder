#include <Handler.h>

#include <iostream>
#include <thread>

using tcp = boost::asio::ip::tcp;

// using C++ 14 | Boost 1.67

int main(int argc, char *argv[]) {
  try {
    // Check command line arguments.
    if (argc != 4) {
      std::cerr << "Usage: http-server-sync <address> <port> <doc_root>\n"
                << "Example:\n"
                << "   http-server-sync 0.0.0.0 8080 .\n";
      return EXIT_FAILURE;
    }
    auto const address = boost::asio::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    std::string const docRoot = argv[3];

    // The io_context is required for all I/O
    boost::asio::io_context ioc{1};

    // The acceptor receives incoming connections
    Handler handler(docRoot);
    tcp::acceptor acceptor{ioc, {address, port}};
    for (;;) {
      // This will receive the new connections
      tcp::socket socket{ioc};

      // Block until we get a connection
      acceptor.accept(socket);

      // Launch the session, transferring ownership of the socket
      // Start do_session with socket do_session(socket)
      std::thread{std::bind(&Handler::doSession, &handler, std::move(socket))}
          .detach();
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
