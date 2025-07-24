#include "Core.h"
#include <iostream>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

Core::Core(std::string docRoot) : docRoot{docRoot} {

  this->recordingsPath = docRoot + "/recordings";

  for (const auto &entry : fs::directory_iterator(recordingsPath)) {
    vecListRecorders.push_back(entry.path().filename().string());
  }
}

pt::ptree Core::getListRecorders() {

  pt::ptree listFiles;
  for (size_t i = 0; i < vecListRecorders.size(); i++) {
    listFiles.put(std::to_string(i), vecListRecorders[i]);
  }

  return pt::ptree().add_child("files", listFiles);
}

pt::ptree Core::deleteFile(size_t id) {

  pt::ptree root;
  boost::system::error_code ec;

  root.put("deleted",
           fs::remove(recordingsPath + '/' + vecListRecorders[id], ec));
  root.put("erro code", ec.message());
  if (vecListRecorders.size() > id) {
    vecListRecorders.erase(vecListRecorders.begin() + id);
  }

  return root;
}
