#ifndef CORE_H
#define CORE_H

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

class Core {
public:
  Core(std::string docRoot);

  boost::property_tree::ptree getListRecorders();

  boost::property_tree::ptree deleteFile(size_t id);

private:
  std::string docRoot;

  std::string recordingsPath;

  std::vector<std::string> vecListRecorders;
};

#endif // CORE_H
