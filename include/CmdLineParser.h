//
// Created by Nadrino on 03/05/2021.
//


#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H

#ifndef DEFAULT_FASCIST_MODE
#define DEFAULT_FASCIST_MODE 1
#endif

#include "string"
#include "vector"

#include "implementation/OptionHolder.h"

class CmdLineParser {

public:
  CmdLineParser();
  virtual ~CmdLineParser();

  void reset();

  //! Pre-parser
  void addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  void addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "", int nbExpectedVars_ = 1);
  static void setIsFascist(bool isFascistParsing_); // if an extra/unrecognised arg is provided, you'll be punished with a logic error!

  //! Parser
  void parseCmdLine(int argc, char** argv);

  //! Pre/Post-parser
  bool isOptionDefined(const std::string& name_);
  const CmdLineParserUtils::OptionHolder& getOption(const std::string& optionName_);
  std::string getConfigSummary();
  std::string getValueSummary(bool showNonCalledVars_ = false);

  //! Post-parser
  bool isOptionTriggered(const std::string &optionName_);
  bool isOptionSet(const std::string &optionName_, size_t index_ = 0);
  size_t getNbValueSet(const std::string &optionName_);

  // Fetching Values
  template<class T> auto getOptionVal(const std::string& optionName_, int index_ = -1) -> T;
  template<class T> auto getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_ = -1) -> T;
  template<class T> auto getOptionValList(const std::string &optionName_) -> std::vector<T>;

protected:
  int getOptionIndex(const std::string& name_);

private:
  bool _isInitialized_{false};
  std::string _commandName_;
  std::vector<std::string> _commandLineArgs_;
  std::vector<CmdLineParserUtils::OptionHolder> _optionsList_;

};

namespace CmdLineParserGlobals{
  static bool _fascistMode_ = DEFAULT_FASCIST_MODE;
}

#include "implementation/CmdLineParser.impl.h"

#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
