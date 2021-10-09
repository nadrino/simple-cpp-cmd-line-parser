//
// Created by Nadrino on 03/05/2021.
//


#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H

#ifndef CMDLINEPARSER_DEFAULT_FASCIST_MODE
#define CMDLINEPARSER_DEFAULT_FASCIST_MODE 1
#endif

#ifndef CMDLINEPARSER_DEFAULT_UNIXGNU_MODE
#define CMDLINEPARSER_DEFAULT_UNIXGNU_MODE false
#endif

#include "string"
#include "vector"
#include "map"

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
#include "yaml-cpp/yaml.h"
#endif

#include "implementation/OptionHolder.h"

class CmdLineParser {

public:
  CmdLineParser();
  CmdLineParser(int argc, char** argv);
  virtual ~CmdLineParser();

  void reset();
  void resetCmdLineArgs();

  //! Pre-parser
  void addCmdLineArgs(int argc, char** argv);
  void addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  void addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "", int nbExpectedVars_ = 1);
  static void setIsFascist(bool isFascistParsing_); // if an extra/unrecognised arg is provided, you'll be punished with a logic error!
  static void setIsUnixGnuMode(bool isUnixGnuMode_);

  //! Parser / Init
  void parseCmdLine(int argc = -1, char** argv = nullptr);
  void parseGNUcmdLine(int argc = -1, char** argv = nullptr);

  //! Pre/Post-parser
  bool isOptionDefined(const std::string& name_);
  const CmdLineParserUtils::OptionHolder& getOption(const std::string& optionName_);
  std::string getConfigSummary();
  std::string getValueSummary(bool showNonCalledVars_ = false);

  //! Post-parser
  bool isOptionTriggered(const std::string &optionName_);
  bool isOptionSet(const std::string &optionName_, size_t index_ = 0);
  size_t getNbValueSet(const std::string &optionName_);
  const std::string &getCommandName() const;

  // Fetching Values
  template<class T> auto getOptionVal(const std::string& optionName_, int index_ = -1) -> T;
  template<class T> auto getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_ = -1) -> T;
  template<class T> auto getOptionValList(const std::string &optionName_) -> std::vector<T>;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  // Setup
  void addYamlOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  void setEnableYamlOptionAdding(bool enableYamlOptionAdding_); // allow non-set options to be defined by YAML file while reading it

  // Init
  void parseYamlFile(const std::string &yamlFilePath_);

  // Misc
  YAML::Node dumpConfigAsYamlNode();
  std::string dumpConfigAsYamlStr();
  std::string dumpConfigAsJsonStr();
#endif

  /**
   * Check the legality of the arguments with the common Unix / GNU style.
   * Options are expected to start with dash '-'.
   * One character option starts with 1 dash, long options start with two dashes
   * @param optionName_ the option itself including dashes
   * @param nbExpectedVars_ number of expected vars
   * @return Whether the option is suitable with Unix GNU standard
   */
  static bool checkOptionGNU(const std::vector<std::string>& optionName_, const int& nbExpectedVars_);

protected:
  int getOptionIndex(const std::string& name_);
  CmdLineParserUtils::OptionHolder* fetchOptionPtr(const std::string& optionCallStr_);
  CmdLineParserUtils::OptionHolder* fetchOptionByNamePtr(const std::string& optionName_);

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  void parseYamlConfigFiles();
#endif

private:
  bool _isInitialized_{false};
  std::string _commandName_;
  std::vector<std::string> _commandLineArgs_;
  std::vector<CmdLineParserUtils::OptionHolder> _optionsList_;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  bool _enableYamlOptionAdding_{false};
  std::vector<std::string> _yamlConfigs_;
#endif

};

namespace CmdLineParserGlobals{
  static bool _fascistMode_ = CMDLINEPARSER_DEFAULT_FASCIST_MODE;
  static bool _unixGnuMode_ = CMDLINEPARSER_DEFAULT_UNIXGNU_MODE;
}

#include "implementation/CmdLineParser.impl.h"

#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
