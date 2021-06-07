//
// Created by Nadrino on 03/05/2021.
//

#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H

#include "iostream"
#include "sstream"
#include <stdexcept>
#include <functional>

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
#include "yaml-cpp/yaml.h"
#endif

#include <CmdLineParser.h>


//#include <CmdLineParser.h>

CmdLineParser::CmdLineParser() = default;
CmdLineParser::~CmdLineParser() = default;

void CmdLineParser::reset(){
  _commandLineArgs_.clear();
  _optionsList_.clear();
#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  _enableYamlOptionAdding_ = false;
  _yamlConfigs_.clear();
#endif
  _isInitialized_ = false;
}

void CmdLineParser::addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_) {
  this->addOption(optionName_, commandLineCallStrList_, description_, 0);
}
void CmdLineParser::addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_, int nbExpectedVars_) {
  if( _isInitialized_ ){ throw std::logic_error("Can't add options while parseCmdLine has already been called"); }
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
}
void CmdLineParser::setIsFascist(bool isFascistParsing_){
  CmdLineParserGlobals::_fascistMode_ = isFascistParsing_;
}

void CmdLineParser::parseCmdLine(int argc, char** argv){

  if( _isInitialized_ ){
    throw std::logic_error("Can't parse cmd line args since it has already been called. Please do reset() before.");
  }

  for( int iArg = 0 ; iArg < argc ; iArg++ ){
    if( iArg == 0 ) _commandName_ = argv[iArg];
    else _commandLineArgs_.emplace_back(argv[iArg]);
  }

  CmdLineParserUtils::OptionHolder* optionPtr = nullptr;
  for( const auto & argument : _commandLineArgs_){

    CmdLineParserUtils::OptionHolder* nextOpt = fetchOptionPtr(argument);

    if( nextOpt != nullptr ){

      // Check if all the required values have been specified in the last option
      if( optionPtr != nullptr and not optionPtr->isFullyFilled() ){
        throw std::logic_error(optionPtr->getName() + ": missing values before starting " + nextOpt->getName()
                                + " (" + std::to_string(optionPtr->getNbExpectedVars()) + " values were expected)");
      }

      optionPtr = nextOpt;
      optionPtr->setIsTriggered(true);

      continue; // next entry of the loop
    }

    if( optionPtr != nullptr ){
      // if optionPtr has been previously set, then we are expected to do something
      if( optionPtr->getNbExpectedVars() == 0 ){
        // This is an option trigger: no extra arg is
        optionPtr->setIsTriggered(true);
      }
      else{
        optionPtr->setNextVariableValue(argument);
      }

      if(not CmdLineParserGlobals::_fascistMode_ and optionPtr->getNbExpectedVars() == optionPtr->getNbValues() ){
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

  if( optionPtr != nullptr and not optionPtr->isFullyFilled() ){
    throw std::logic_error(optionPtr->getName() + ": missing values (" + std::to_string(optionPtr->getNbExpectedVars()) + " values were expected)");
  }

  _isInitialized_ = true;

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  // will fill the options specified in the yaml file that were not provided in cmdline: i.e. cmdline options overrides yaml config
  parseYamlConfigFiles();
#endif

}

bool CmdLineParser::isOptionDefined(const std::string& name_){
  return ( this->getOptionIndex(name_) != -1 );
}
const CmdLineParserUtils::OptionHolder &CmdLineParser::getOption(const std::string &optionName_) {
  int optionIndex = this->getOptionIndex(optionName_);
  if( optionIndex == -1 ){
    throw std::logic_error("Option " + optionName_ + " is not defined");
  }
  return _optionsList_.at(optionIndex);
}
std::string CmdLineParser::getConfigSummary(){
  std::stringstream ss;

  for( const auto& option : _optionsList_ ){if( not ss.str().empty() ) ss << std::endl;
    ss << option.getSummary();
#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
    for( const auto& yamlOption : _yamlConfigs_ ){
      if( option.getName() == yamlOption ){
        ss << " ** YAML config option";
      }
    }
#endif
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
std::string CmdLineParser::getValueSummary(bool showNonCalledVars_) {
  std::stringstream ss;
  if( _isInitialized_ ){
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
  return ss.str();
}

bool CmdLineParser::isOptionTriggered(const std::string &optionName_) {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  return this->getOption(optionName_).isTriggered();
}
bool CmdLineParser::isOptionSet(const std::string &optionName_, size_t index_){
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  if( optionPtr->getNbExpectedVars() == 0 ) return isOptionTriggered(optionName_);
  if(optionPtr->getNbValues() > index_ ) return true;
  return false;
}
size_t CmdLineParser::getNbValueSet(const std::string &optionName_){
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  return this->getOption(optionName_).getNbValues();
}

template<class T> auto CmdLineParser::getOptionVal(const std::string &optionName_, int index_) -> T {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  if( index_ == -1 ){
    if(optionPtr->getNbValues() == 1 ){
      // there is no ambiguity, index is 0
      index_ = 0;
    }
    else{
      throw std::logic_error(optionName_ + ": " + std::to_string(optionPtr->getNbValues())
                              + " values where set. You need to provide the index of the one you want.");
    }
  } // index_ == -1
  return optionPtr->template getValue<T>(index_);
}
template<class T> auto CmdLineParser::getOptionVal(const std::string& optionName_, const T& defaultValue_, int index_) -> T{
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  try{
    return this->template getOptionVal<T>(optionName_,index_);
  }
  catch ( std::logic_error& e ){
    // Catch only logic errors. runtime error will still show
    return defaultValue_;
  }
}
template<class T> auto CmdLineParser::getOptionValList(const std::string &optionName_) -> std::vector<T> {
  if( not _isInitialized_ ){ throw std::logic_error("Can't call isOptionTriggered while parseCmdLine has not already been called"); }
  std::vector<T> outputList;
  const CmdLineParserUtils::OptionHolder* optionPtr = &this->getOption(optionName_);
  for(size_t iIndex = 0 ; iIndex < optionPtr->getNbValues() ; iIndex++ ){
    outputList.template emplace_back(optionPtr->template getValue<T>(iIndex));
  }
  return outputList;
}

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
void CmdLineParser::addYamlOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_) {
  this->addOption(optionName_, commandLineCallStrList_, description_, 1);
  _yamlConfigs_.emplace_back(optionName_);
}
void CmdLineParser::parseYamlFile(const std::string &yamlFilePath_){
  this->reset(); // rebuild from scratch
  this->setEnableYamlOptionAdding(true);
  this->addOption("CmdLineParser::parseYamlFile", {}, "Provided yaml file path", 1);
  _yamlConfigs_.emplace_back("CmdLineParser::parseYamlFile");
  this->fetchOptionByNamePtr("CmdLineParser::parseYamlFile")->setNextVariableValue(yamlFilePath_);
  this->fetchOptionByNamePtr("CmdLineParser::parseYamlFile")->setIsTriggered(true);
  this->parseCmdLine(0, nullptr);
}
std::string CmdLineParser::dumpConfigAsYamlStr() {
  if( not _isInitialized_ ){
    throw std::logic_error("Can't call dumpConfigAsYamlStr while parseCmdLine has not already been called");
  }

  YAML::Node yamlNode;
  for( const auto& option : _optionsList_ ){
    if( option.isTriggered() ){ // only taking into account explicit options

      if( option.getNbValues() == 0 ){
        yamlNode[option.getName()] = "true";
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

  return YAML::Dump(yamlNode);
}
void CmdLineParser::setEnableYamlOptionAdding(bool enableYamlOptionAdding_){
  _enableYamlOptionAdding_ = enableYamlOptionAdding_;
}
#endif


// Protected:
int CmdLineParser::getOptionIndex(const std::string& name_){
  for( size_t iOption = 0 ; iOption < _optionsList_.size() ; iOption++ ){
    if( _optionsList_.at(iOption).getName() == name_ ){
      return int(iOption);
    }
  }
  return -1;
}
CmdLineParserUtils::OptionHolder* CmdLineParser::fetchOptionPtr(const std::string& optionCallStr_){
  CmdLineParserUtils::OptionHolder* optionPtr = nullptr;
  for( auto& option : _optionsList_ ){
    bool thisArgIsOption = false;
    for( const auto& cmdLineCall : option.getCmdLineCallStrList() ){
      if( cmdLineCall == optionCallStr_ ){
        thisArgIsOption = true;
        break;
      }
    }
    if( thisArgIsOption ){
      optionPtr = &option;
    }
  }
  return optionPtr;
}
CmdLineParserUtils::OptionHolder* CmdLineParser::fetchOptionByNamePtr(const std::string& optionName_){
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
void CmdLineParser::parseYamlConfigFiles(){

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
