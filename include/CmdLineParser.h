//
// Created by Adrien BLANCHET on 03/05/2021.
//


#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H

#include "string"
#include "vector"

#include "implementation/OptionHolder.h"

class CmdLineParser {

public:
  CmdLineParser();
  virtual ~CmdLineParser();

  void addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  void addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "", int nbExpectedVars_ = -1);

  void parseCmdLine(int argc, char** argv);

  // Probing options
  bool isOptionDefined(const std::string& name_);
  const OptionHolder& getOption(const std::string& optionName_);
  bool isOptionTriggered(const std::string &optionName_);
  bool isOptionSet(const std::string &optionName_, size_t index_ = 0);
  size_t getNbValueSet(const std::string &optionName_);

  // Fectching Values
  template<class T> auto getOptionVal(const std::string& optionName_, int index_ = -1) -> T;
  template<class T> auto getOptionValList(const std::string &optionName_) -> std::vector<T>;
  template<class T> auto getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_ = -1) -> T;

  //
  std::string getConfigSummary();
  std::string getValueSummary();


protected:
  int getOptionIndex(const std::string& name_);

private:
  std::vector<std::string> _commandLineArgs_;
  std::vector<OptionHolder> _optionsList_;

};

#include "implementation/CmdLineParser.impl.h"

#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
