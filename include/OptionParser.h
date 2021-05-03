//
// Created by Adrien BLANCHET on 03/05/2021.
//

#ifndef SIMPLE_CPP_OPTION_PARSER_OPTIONPARSER_H
#define SIMPLE_CPP_OPTION_PARSER_OPTIONPARSER_H

#include "string"

#include "implementation/OptionHolder.h"

class OptionParser {

public:
  OptionParser(int argc, char** argv);
  virtual ~OptionParser();

  template<class T> void addOption(const std::string& name_, const std::string& commandLineCalls_, const std::string& description_);

  void printConfig();

private:
  int _argc_;
  char** _argv_;

};

#include "implementation/OptionParser.impl.h"


#endif //SIMPLE_CPP_OPTION_PARSER_OPTIONPARSER_H
