//
// Created by Nadrino on 03/05/2021.
//

#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <functional>

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
#include "yaml-cpp/yaml.h"
#endif

#include <CmdLineParser.h>


//#include <CmdLineParser.h>

inline CmdLineParser::CmdLineParser() = default;
inline CmdLineParser::CmdLineParser(int argc, char** argv){ this->addCmdLineArgs(argc, argv); }
inline CmdLineParser::~CmdLineParser() = default;

inline void CmdLineParser::resetCmdLineArgs(){
  _commandLineArgs_.clear();
  _commandName_ = "";
}

//! Pre-parser
inline void CmdLineParser::addCmdLineArgs(int argc, char** argv){
  for( int iArg = 0 ; iArg < argc ; iArg++ ){
    if( _commandName_.empty() and iArg == 0 ) _commandName_ = argv[iArg];
    else _commandLineArgs_.emplace_back(argv[iArg]);
  }
}
inline void CmdLineParser::addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_) {
  this->addOption(optionName_, commandLineCallStrList_, description_, 0);
}
inline void CmdLineParser::addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_,
                              const std::string &description_, int nbExpectedVars_, bool allowEmpty_) {
  if( _isInitialized_ ){ throw std::logic_error("Can't add options while parseCmdLine has already been called"); }

  if (CmdLineParserGlobals::_unixGnuMode_){
    checkOptionGNU(commandLineCallStrList_, nbExpectedVars_);
  }

  if( this->isOptionDefined(optionName_) ){
    throw std::logic_error("Option \"" + optionName_ + "\" has already been defined.");
  }
  _optionsList_.emplace_back();
  _optionsList_.back().setName(optionName_);
  for( const auto& commandLineCallStr : commandLineCallStrList_){
    _optionsList_.back().addCmdLineCallStr(commandLineCallStr);
  }
  _optionsList_.back().setDescription(description_);
  _optionsList_.back().setNbExpectedVars(nbExpectedVars_);
  _optionsList_.back().setAllowEmptyValue(allowEmpty_);
}
inline void CmdLineParser::addDummyOption(const std::string &dummyTitle_){
  _optionsList_.emplace_back();
  _optionsList_.back().setDescription(dummyTitle_);
}
inline void CmdLineParser::setIsFascist(bool isFascistParsing_){
  CmdLineParserGlobals::_fascistMode_ = isFascistParsing_;
}
inline void CmdLineParser::setIsUnixGnuMode(bool isUnixGnuMode_){
    CmdLineParserGlobals::_unixGnuMode_ = isUnixGnuMode_;
}


//! Parser / Init
inline void CmdLineParser::parseCmdLine(int argc, char** argv){

  if( _isInitialized_ ){
    throw std::logic_error("Can't parse cmd line args since it has already been called. Please do reset() before.");
  }

  if (CmdLineParserGlobals::_unixGnuMode_){
    return CmdLineParser::parseGNUcmdLine(argc, argv);
  }

  if( argv != nullptr ){
    this->resetCmdLineArgs();
    this->addCmdLineArgs(argc, argv);
  }

  CmdLineParserUtils::OptionHolder* optionPtr = nullptr;
  for( const auto & argument : _commandLineArgs_){

    CmdLineParserUtils::OptionHolder* nextOpt = this->fetchOptionPtr(argument);
    if( nextOpt != nullptr ){

      // Check if all the required values have been specified in the last option
      if( optionPtr != nullptr and not optionPtr->isFullyFilled() and not optionPtr->isAllowEmptyValue() ){
        throw std::logic_error(optionPtr->getName() + ": missing values before starting " + nextOpt->getName()
                                + " (" + std::to_string(optionPtr->getNbExpectedVars()) + " values were expected)");
      }

      optionPtr = nextOpt;
      optionPtr->setIsTriggered(true);
      if( optionPtr->getNbExpectedVars() == 0 ){
        // nothing more is expected here
        optionPtr = nullptr;
      }

      continue; // next entry of the loop
    }
    if( optionPtr != nullptr ){
      // if optionPtr has been previously set, then we are expected to do something
      if( optionPtr->getNbExpectedVars() == 0 ){
        // This is an option trigger: no extra arg is expected
        optionPtr->setIsTriggered(true);
        optionPtr = nullptr; // that's all we need to do
      }
      else{
        optionPtr->setNextVariableValue(argument);
      }

      if( not CmdLineParserGlobals::_fascistMode_ and optionPtr->getNbExpectedVars() == optionPtr->getNbValues() ){
        // if an extra argument is provided but is not recognized, it will simply be ignored.
        // For a _fascistMode_: this is unacceptable!
        optionPtr = nullptr;
      }

      continue;
    }
    if( CmdLineParserGlobals::_fascistMode_ ){
      // WHAT? HOW DID YOU GET THERE? YOU HAVE NOTHING TO DO HERE!
      throw std::logic_error("Unrecognised option or value: " + argument + " (CmdLineParser is in _fascistMode_)");
    }

  }

  if( optionPtr != nullptr and not optionPtr->isFullyFilled() and not optionPtr->isAllowEmptyValue() ){
    throw std::logic_error(optionPtr->getName() + ": missing values (" + std::to_string(optionPtr->getNbExpectedVars()) + " values were expected)");
  }

  _isInitialized_ = true;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  // will fill the options specified in the yaml file that were not provided in cmdline: i.e. cmdline options overrides yaml config
  parseYamlConfigFiles();
#endif

}
inline void CmdLineParser::parseGNUcmdLine(int argc, char** argv) {
  if( _isInitialized_ ){
    throw std::logic_error("Can't parse cmd line args since it has already been called. Please do reset() before.");
  }

  if( argv != nullptr ){
    this->resetCmdLineArgs();
    this->addCmdLineArgs(argc, argv);
  }

  for (auto argIt = _commandLineArgs_.begin(); argIt != _commandLineArgs_.end(); ++argIt) {
    if ((*argIt).length() < 2 || (*argIt)[0] != '-')
      throw std::logic_error("Unix GNU standard doesn't allow more then 1 option. Option: " + (*argIt));

    if ((*argIt)[1] != '-') {
      // [list of] short option was found
      // loop over each option in the argument
      std::string optionWord = (*argIt).substr(1);
      for (const auto& shortArg :  optionWord) {
        auto* nextOpt = this->fetchOptionPtr("-" + std::string(&shortArg).substr(0, 1));
        if (nextOpt == nullptr) {
          if (CmdLineParserGlobals::_fascistMode_)
            throw std::logic_error(std::string("Unrecognised option or value: ") + shortArg);
          continue;
        }

        nextOpt->setIsTriggered(true);
        if (nextOpt->getNbExpectedVars() == 0)
          continue;

        // if the word is not over treat the rest of the word as an option value
        if (optionWord.find(shortArg) != optionWord.length() - 1) {
          nextOpt->setNextVariableValue(optionWord.substr(optionWord.find(shortArg)+1));
          // done with the short option since no more chars to parse. Continue to next argument
          break;
        } else {
          // end of line
          if (argIt == _commandLineArgs_.end() - 1) {
            throw std::logic_error("Option" + (*argIt).substr(0, 2) + "required a parameter, but nothing was found");
          }
          // take the next command line argument as a value
          nextOpt->setNextVariableValue(*(argIt + 1));
          ++argIt;
          continue;
        }
      }
    } else {
      // long option was found
      auto eqPos = (*argIt).find('=');
      auto searchPattern = (*argIt);
      if (eqPos != std::string::npos)
        searchPattern = searchPattern.substr(0, eqPos);
      auto nextOpt = this->fetchOptionPtr(searchPattern);
      if (nextOpt == nullptr) {
        if (CmdLineParserGlobals::_fascistMode_)
          throw std::logic_error("Unrecognised option or value: " + (*argIt));
        continue;
      }
      nextOpt->setIsTriggered(true);
      if (nextOpt->getNbExpectedVars() == 0)
        continue;
      // separated with '='
      if (eqPos != std::string::npos) {
        nextOpt->setNextVariableValue((*argIt).substr(eqPos+1));
        continue;
      }
      if (argIt == _commandLineArgs_.end() - 1) {
        throw std::logic_error("Option" + (*argIt) + "required a parameter, but nothing was found");
      }
      nextOpt->setNextVariableValue(*(argIt + 1));
      ++argIt;
    }
  }

  _isInitialized_ = true;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  // will fill the options specified in the yaml file that were not provided in cmdline: i.e. cmdline options overrides yaml config
  parseYamlConfigFiles();
#endif
}

//! Pre/Post-parser
inline bool CmdLineParser::isNoOptionTriggered() const{
  for( const auto& option : _optionsList_ ){
    if( option.isTriggered() ){ return false; }
  }
  return true;
}
inline bool CmdLineParser::isOptionDefined(const std::string& name_) const {
  return ( this->getOptionIndex(name_) != -1 );
}
inline std::string CmdLineParser::getCommandLineString() const{
  std::string cl = _commandName_;
  for( const auto& arg : _commandLineArgs_ ){ cl += " " + arg; }
  return cl;
}
inline std::string CmdLineParser::getConfigSummary() const{
  std::stringstream ss;
  std::string bufStr;

  for( const auto& option : _optionsList_ ){
    if( not ss.str().empty() ) ss << std::endl;
    bufStr = option.getSummary();
    if( bufStr.empty() and not option.getDescription().empty() ){
      // dummy option -> print a separator with the provided description
      ss << "────────────";
      ss << " " << option.getDescription() << " ";
      ss << "────────────";
    }
    else{
      ss << option.getSummary();
#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
      for( const auto& yamlOption : _yamlConfigs_ ){
        if( option.getName() == yamlOption ){
          ss << " ** YAML config option";
        }
      }
#endif
    }
  }
  if( not _commandLineArgs_.empty() ){
    ss << std::endl << "Command Line Args: { ";
    for(auto it = _commandLineArgs_.begin(); it != _commandLineArgs_.end(); ++it) {
      ss << "\"" << *it << "\"";
      if(std::next(it) != _commandLineArgs_.end()) {
        ss << ", ";
      }
    }
    ss << " }";
  }

  return ss.str();
}
inline std::string CmdLineParser::getValueSummary(bool showNonCalledVars_) const {
  std::stringstream ss;
  if( _isInitialized_ ){
    if(this->isNoOptionTriggered() ){
      ss << "No options were set.";
    }
    for( const auto& option : _optionsList_ ){
      if( option.isTriggered() ){
        if( not ss.str().empty() ) ss << std::endl;
        ss << option.getName() << ": ";

        if( option.getNbExpectedVars() == 0 ){
          ss << "true";
        }
        else{
          ss << "{ ";
          for(auto it = option.getValuesList().begin(); it != option.getValuesList().end(); ++it) {
            ss << "\"" << *it << "\"";
            if(std::next(it) != option.getValuesList().end()) {
              ss << ", ";
            }
          }
          ss << " }";
        }
        if( showNonCalledVars_ ) ss << " -> called.";
      }
      else if( showNonCalledVars_ ){
        if( not ss.str().empty() ) ss << std::endl;
        ss << option.getName() << ": ";

        if( option.getNbExpectedVars() == 0 ){
          ss << "false";
        }
        else{
          ss << "{}";
        }
        ss << " -> not called.";
      }
    }
  }
  else{
    ss << "CmdLineParser not initialized.";
  }
  return ss.str();
}
inline const std::stringstream& CmdLineParser::getDescription() const {
  return _descriptionStringStream_;
}
inline const CmdLineParserUtils::OptionHolder &CmdLineParser::getOption(const std::string &optionName_) const {
  int optionIndex = this->getOptionIndex(optionName_);
  if( optionIndex == -1 ){
    throw std::logic_error("Option " + optionName_ + " is not defined");
  }
  return _optionsList_.at(optionIndex);
}
inline const CmdLineParserUtils::OptionHolder* CmdLineParser::getOptionPtr(const std::string& optionName_) const{
  int optionIndex = this->getOptionIndex(optionName_);
  if( optionIndex == -1 ){
    throw std::logic_error("Option " + optionName_ + " is not defined");
  }
  return &_optionsList_.at(optionIndex);
}

inline std::stringstream& CmdLineParser::getDescription() {
  return _descriptionStringStream_;
}
inline CmdLineParserUtils::OptionHolder* CmdLineParser::getOptionPtr(const std::string& optionName_){
  // Call the const version by const_casting the object to remove const-ness
  return const_cast<CmdLineParserUtils::OptionHolder*>(const_cast<const CmdLineParser*>(this)->getOptionPtr(optionName_));
}


//! Post-parser
inline bool CmdLineParser::isOptionTriggered(const std::string &optionName_) const {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  return this->getOption(optionName_).isTriggered();
}
inline bool CmdLineParser::isOptionSet(const std::string &optionName_, size_t index_) const {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  if( optionPtr->getNbExpectedVars() == 0 ) return isOptionTriggered(optionName_);
  if(optionPtr->getNbValues() > index_ ) return true;
  return false;
}
inline size_t CmdLineParser::getNbValueSet(const std::string &optionName_) const {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  return this->getOption(optionName_).getNbValues();
}
inline const std::string &CmdLineParser::getCommandName() const {
  return _commandName_;
}

// Fetching Values
template<typename T> auto CmdLineParser::getOptionVal(const std::string &optionName_, int index_) const -> T {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  if( index_ == -1 ){
    if(optionPtr->getNbValues() == 1 ){
      // there is no ambiguity, index is 0
      index_ = 0;
    }
    else if( optionPtr->getNbValues() == 0 ){
      throw std::runtime_error("\"" + optionName_ + "\" option was not specified.");
    }
    else{
      throw std::logic_error(optionName_ + ": " + std::to_string(optionPtr->getNbValues())
                              + " values are set. You have to provide the index of the one you want.");
    }
  } // index_ == -1
  return optionPtr->getValue<T>(index_);
}
template<typename T> auto CmdLineParser::getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_) const -> T{
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }

  auto* optionPtr = this->getOptionPtr(optionName_);
  if( optionPtr->isTriggered() ){
    if( optionPtr->getNbValues() == 0 and optionPtr->isAllowEmptyValue() ){ return defaultValue_; }
    try{
      return this->getOptionVal<T>(optionName_,index_);
    }
    catch ( std::logic_error& e ){
      // Catch only logic errors. runtime error will still show
      return defaultValue_;
    }
  }

  // if not triggered
  return defaultValue_;
}
template<typename T> auto CmdLineParser::getOptionValList(const std::string &optionName_) const -> std::vector<T> {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  std::vector<T> outputList;
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  for(size_t iIndex = 0 ; iIndex < optionPtr->getNbValues() ; iIndex++ ){
    outputList.emplace_back(optionPtr->getValue<T>(iIndex));
  }
  return outputList;
}

// template specialization
inline std::string CmdLineParser::getOptionVal(const std::string& optionName_, const char* defaultVal_, int index_) const {
  return this->getOptionVal(optionName_, std::string(defaultVal_), index_);
}

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
// Setup
inline void CmdLineParser::addYamlOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_) {
  this->addOption(optionName_, commandLineCallStrList_, description_, 1);
  _yamlConfigs_.emplace_back(optionName_);
}
inline void CmdLineParser::setEnableYamlOptionAdding(bool enableYamlOptionAdding_){
  _enableYamlOptionAdding_ = enableYamlOptionAdding_;
}

// Init
inline void CmdLineParser::parseYamlFile(const std::string &yamlFilePath_){
  if( _isInitialized_ ){
    throw std::logic_error("CmdLineParser::parseYamlFile can't be called while already initialized.");
  }

  // reset
  this->resetCmdLineArgs();
  _optionsList_.clear();
  _enableYamlOptionAdding_ = false;
  _yamlConfigs_.clear();
  _isInitialized_ = false;

  this->setEnableYamlOptionAdding( true );

  this->addOption("CmdLineParser::parseYamlFile", {}, "Provided yaml file path", 1);
  _yamlConfigs_.emplace_back("CmdLineParser::parseYamlFile");
  this->fetchOptionByNamePtr("CmdLineParser::parseYamlFile")->setNextVariableValue(yamlFilePath_);
  this->fetchOptionByNamePtr("CmdLineParser::parseYamlFile")->setIsTriggered(true);

  this->parseCmdLine(0, nullptr);

  _optionsList_.erase(_optionsList_.begin());
  _yamlConfigs_.erase(_yamlConfigs_.begin());
}

// Misc
inline YAML::Node CmdLineParser::dumpConfigAsYamlNode() {
  YAML::Node yamlNode;
  for( const auto& option : _optionsList_ ){
    if( option.isTriggered() ){ // only taking into account explicit options

      if( option.getNbValues() == 0 ){
        yamlNode[option.getName()] = true;
      }
      else if( option.getNbValues() == 1 ){
        yamlNode[option.getName()] = option.getValue<std::string>(0);
      }
      else{
        for( int iVal = 0 ; iVal < option.getNbValues() ; iVal++ ){
          yamlNode[option.getName()].push_back(option.getValue<std::string>(iVal));
        }
      }
    }
  } // option
  return yamlNode;
}
inline std::string CmdLineParser::dumpConfigAsYamlStr() {
  return YAML::Dump(this->dumpConfigAsYamlNode());
}
inline std::string CmdLineParser::dumpConfigAsJsonStr(){
  YAML::Emitter emitter;
  emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq << this->dumpConfigAsYamlNode();
  if( emitter.c_str() == nullptr ) return "";
  return {emitter.c_str() + 1}; // +1: shift the array address to get rid of the heading "["
}
#endif

inline bool CmdLineParser::checkOptionGNU(const std::vector<std::string>& commandLineCallStrList_, const int& nbExpectedVars_) {
  if (nbExpectedVars_ > 1)
    throw std::logic_error("Unix GNU standard doesn't allow more then 1 option. Option: " + commandLineCallStrList_[0]);
  for (const auto& option : commandLineCallStrList_){
    if (option.length() < 2)
      throw std::logic_error("Option must start with dash and contain at least 1 character. Option: " + option);

    if (option[0] != '-')
      throw std::logic_error("Option must start with dash. Option: " + option);

    if (option.length() > 2 && option[1] != '-')
      throw std::logic_error("Long option must start with two dashs. Option: " + option);
  }

  return true;
}


// Protected:
inline int CmdLineParser::getOptionIndex(const std::string& name_) const {
  for( size_t iOption = 0 ; iOption < _optionsList_.size() ; iOption++ ){
    if( _optionsList_.at(iOption).getName() == name_ ){
      return int(iOption);
    }
  }
  return -1;
}
inline CmdLineParserUtils::OptionHolder* CmdLineParser::fetchOptionPtr(const std::string& optionCallStr_){
  for( auto& option : _optionsList_ ){
    if( option.getName().empty() ){ continue; } // skip dummy options
    // loop over the possible calls
    for( const auto& cmdLineCall : option.getCmdLineCallStrList() ){
      // call found? return the ptr
      if( cmdLineCall == optionCallStr_ ){ return &option; }
    }
  }
  return nullptr; // default return -> no call
}
inline CmdLineParserUtils::OptionHolder* CmdLineParser::fetchOptionByNamePtr(const std::string& optionName_){
  CmdLineParserUtils::OptionHolder* optionPtr = nullptr;
  for( auto& option : _optionsList_ ){
    if(option.getName() == optionName_ ){
      optionPtr = &option;
      break;
    }
  }
  return optionPtr;
}

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
inline void CmdLineParser::parseYamlConfigFiles(){

  for( auto& yamlOptionName : _yamlConfigs_ ){

    if( not this->isOptionTriggered(yamlOptionName) ){
      continue;
    }

    std::string yamlFilePath = this->getOptionVal<std::string>(yamlOptionName);
    YAML::Node yamlNode = YAML::LoadFile(yamlFilePath);

    if( _enableYamlOptionAdding_ ){

      for( const auto& yamlEntry : yamlNode ){

        if( not isOptionDefined( yamlEntry.first.as<std::string>() ) ){

          // first check if this is a trigger option
          bool isTriggerOption = false;
          if( yamlEntry.second.IsScalar() ){
            auto value = yamlEntry.second.as<std::string>();
            if( value == "true" or value == "false" ){
              // Caveat: don't support "int" triggers (bool = 0 or 1) since it could also be a int arg...
              isTriggerOption = true;
            }
          }

          _isInitialized_ = false;
          if( isTriggerOption ){
            this->addTriggerOption(
                yamlEntry.first.as<std::string>(),
                {},
                "trigger option added with yaml file"
            );
          }
          else{
            this->addOption(
                yamlEntry.first.as<std::string>(),
                {},
                "option added with yaml file",
                -1
            );
          }
          _isInitialized_ = true;
        } // option not defined?
      } // loop over yaml nodes
    } // enable add option with yaml?

    for( auto& option: _optionsList_ ){

      if( yamlOptionName == option.getName() ){
        // option is this yaml file
        continue;
      }

      if( yamlNode[option.getName()] ){
        // Option provided in yaml!

        if( option.isTriggered() ){
          // nothing to take from yaml since it's already been triggered in cmd line
          continue;
        }

        if( option.getNbExpectedVars() == 0 ){
          // trigger case
          if( yamlNode[option.getName()].IsScalar() ){
            auto value = yamlNode[option.getName()].as<std::string>();
            if( value == "true" ){
              option.setIsTriggered(true);
            }
          }
          else{
            std::stringstream ss;
            ss << "yaml error: option \"" << option.getName()
                      << "\" has to be either a single value; have: "
                      << yamlNode[option.getName()].Type() << std::endl;
            throw std::runtime_error(ss.str());
          }
        }
        else{
          option.setIsTriggered(true);

          if( yamlNode[option.getName()].IsScalar() ){
            option.setNextVariableValue(yamlNode[option.getName()].as<std::string>());
          }
          else if( yamlNode[option.getName()].IsSequence() ){
            for( const auto& yamlEntry : yamlNode[option.getName()] ){
              option.setNextVariableValue(yamlEntry.as<std::string>());
            }
          }
          else{
            std::stringstream ss;
            ss << "yaml error: option \"" << option.getName()
                      << "\" has to be either a single value or a sequence; have: "
                      << yamlNode[option.getName()].Type() << std::endl;
            throw std::runtime_error(ss.str());
          }
        }

      } // yaml option

    } // option

  }

}
#endif


#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H
