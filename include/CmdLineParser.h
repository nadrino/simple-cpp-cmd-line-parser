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
#include "sstream"

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
#include "yaml-cpp/yaml.h"
#endif

#include "implementation/OptionHolder.h"

class CmdLineParser {

public:
  inline CmdLineParser();
  inline CmdLineParser(int argc, char** argv);
  inline virtual ~CmdLineParser();

  inline void resetCmdLineArgs();

  //! Pre-parser
  inline void addCmdLineArgs(int argc, char** argv);
  inline void addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  inline void addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_,
                        const std::string &description_ = "", int nbExpectedVars_ = 1, bool allowEmpty_ = false);
  inline void addDummyOption(const std::string &dummyTitle_ = ""); // only display a separation bar when printing getConfigSummary()
  inline static void setIsFascist(bool isFascistParsing_); // if an extra/unrecognised arg is provided, you'll be punished with a logic error!
  inline static void setIsUnixGnuMode(bool isUnixGnuMode_);

  //! Parser / Init
  inline void parseCmdLine(int argc = -1, char** argv = nullptr);
  inline void parseGNUcmdLine(int argc = -1, char** argv = nullptr);

  //! Pre/Post-parser
  [[nodiscard]] inline bool isNoOptionTriggered() const;
  [[nodiscard]] inline bool isOptionDefined(const std::string& name_) const;
  [[nodiscard]] inline std::string getConfigSummary() const;
  [[nodiscard]] inline std::string getCommandLineString() const;
  [[nodiscard]] inline std::string getValueSummary(bool showNonCalledVars_ = false) const;
  [[nodiscard]] inline const std::stringstream& getDescription() const;
  [[nodiscard]] inline const CmdLineParserUtils::OptionHolder& getOption(const std::string& optionName_) const;
  [[nodiscard]] inline const CmdLineParserUtils::OptionHolder* getOptionPtr(const std::string& optionName_) const;

  inline std::stringstream& getDescription();
  inline CmdLineParserUtils::OptionHolder* getOptionPtr(const std::string& optionName_);

  //! Post-parser
  [[nodiscard]] inline bool isOptionTriggered(const std::string &optionName_) const;
  [[nodiscard]] inline bool isOptionSet(const std::string &optionName_, size_t index_ = 0) const;
  [[nodiscard]] inline size_t getNbValueSet(const std::string &optionName_) const;
  [[nodiscard]] inline const std::string &getCommandName() const;

  // Fetching Values
  template<typename T> inline auto getOptionVal(const std::string& optionName_, int index_ = -1) const -> T;
  template<typename T> inline auto getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_ = -1) const -> T;
  template<typename T> inline auto getOptionValList(const std::string &optionName_) const -> std::vector<T>;

  // template specialization
  inline std::string getOptionVal(const std::string& optionName_, const char* defaultVal_, int index_ = -1) const;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  // Setup
  inline void addYamlOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_ = "");
  inline void setEnableYamlOptionAdding(bool enableYamlOptionAdding_); // allow non-set options to be defined by YAML file while reading it

  // Init
  inline void parseYamlFile(const std::string &yamlFilePath_);

  // Misc
  inline YAML::Node dumpConfigAsYamlNode();
  inline std::string dumpConfigAsYamlStr();
  inline std::string dumpConfigAsJsonStr();
#endif

  /**
   * Check the legality of the arguments with the common Unix / GNU style.
   * Options are expected to start with dash '-'.
   * One character option starts with 1 dash, long options start with two dashes
   * @param optionName_ the option itself including dashes
   * @param nbExpectedVars_ number of expected vars
   * @return Whether the option is suitable with Unix GNU standard
   */
  inline static bool checkOptionGNU(const std::vector<std::string>& optionName_, const int& nbExpectedVars_);

protected:
  inline int getOptionIndex(const std::string& name_) const;
  inline CmdLineParserUtils::OptionHolder* fetchOptionPtr(const std::string& optionCallStr_);
  inline CmdLineParserUtils::OptionHolder* fetchOptionByNamePtr(const std::string& optionName_);

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  void parseYamlConfigFiles();
#endif

private:
  bool _isInitialized_{false};
  std::string _commandName_{};
  std::stringstream _descriptionStringStream_{};
  std::vector<std::string> _commandLineArgs_{};
  std::vector<CmdLineParserUtils::OptionHolder> _optionsList_{};

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  bool _enableYamlOptionAdding_{false};
  std::vector<std::string> _yamlConfigs_{};
#endif

};

namespace CmdLineParserGlobals{
  static bool _fascistMode_{CMDLINEPARSER_DEFAULT_FASCIST_MODE};
  static bool _unixGnuMode_{CMDLINEPARSER_DEFAULT_UNIXGNU_MODE};
}

#include "implementation/CmdLineParser.impl.h"

#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_H
