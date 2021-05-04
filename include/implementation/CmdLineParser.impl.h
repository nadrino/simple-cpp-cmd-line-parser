//
// Created by Adrien BLANCHET on 03/05/2021.
//

#pragma once
#ifndef SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H
#define SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H

#include <stdexcept>
#include <functional>
#include "iostream"
#include "sstream"

//#include <CmdLineParser.h>

CmdLineParser::CmdLineParser() = default;
CmdLineParser::~CmdLineParser() = default;

void CmdLineParser::addTriggerOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_) {
  this->addOption(optionName_, commandLineCallStrList_, description_, 0);
}
void CmdLineParser::addOption(const std::string &optionName_, const std::vector<std::string> &commandLineCallStrList_, const std::string &description_, int nbExpectedVars_) {
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

void CmdLineParser::parseCmdLine(int argc, char** argv){

  _commandLineArgs_.clear();
  for( int iArg = 0 ; iArg < argc ; iArg++ ){
    _commandLineArgs_.emplace_back(argv[iArg]);
  }

  std::function<OptionHolder*(const std::string&)> findAssociatedOption = [this](const std::string& argStr_){
    OptionHolder* optionPtr = nullptr;
    for( auto& option : _optionsList_ ){
      bool thisArgIsOption = false;
      for( const auto& cmdLineCall : option.getCmdLineCallStrList() ){
        if( cmdLineCall == argStr_ ){
          thisArgIsOption = true;
          break;
        }
      }
      if( thisArgIsOption ){
        optionPtr = &option;
      }
    }
    return optionPtr;
  };

  OptionHolder* optionPtr = nullptr;
  for(auto & argument : _commandLineArgs_){

    OptionHolder* nextOpt = findAssociatedOption(argument);

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

    }
  }

  if( optionPtr != nullptr and not optionPtr->isFullyFilled() ){
    throw std::logic_error(optionPtr->getName() + ": missing values (" + std::to_string(optionPtr->getNbExpectedVars()) + " values were expected)");
  }

}

bool CmdLineParser::isOptionDefined(const std::string& name_){
  return ( this->getOptionIndex(name_) != -1 );
}
const OptionHolder &CmdLineParser::getOption(const std::string &optionName_) {
  int optionIndex = this->getOptionIndex(optionName_);
  if( optionIndex == -1 ){
    throw std::logic_error("Option " + optionName_ + " is not defined");
  }
  return _optionsList_.at(optionIndex);
}
bool CmdLineParser::isOptionTriggered(const std::string &optionName_) {
  if( this->getOption(optionName_).getNbExpectedVars() != 0 ){
    throw std::logic_error("Option \"" + optionName_ + "\" is not a trigger (nbExpectedVars != 0)");
  }
  return this->getOption(optionName_).isTriggered();
}
bool CmdLineParser::isOptionSet(const std::string &optionName_, size_t index_){
  if( this->getOption(optionName_).getNbExpectedVars() == 0 ) return isOptionTriggered(optionName_);
  if( this->getOption(optionName_).getVarListSize() > index_ ) return true;
  return false;
}
template<class T> auto CmdLineParser::getOptionVal(const std::string &optionName_, size_t index_) -> T {
  return this->getOption(optionName_).template getValue<T>(index_);
}
template<class T> auto CmdLineParser::getOptionValList(const std::string &optionName_) -> std::vector<T> {
  std::vector<T> outputList;
  for( size_t iIndex = 0 ; iIndex < this->getOption(optionName_).getVarListSize() ; iIndex++ ){
    outputList.template emplace_back(this->getOption(optionName_).template getValue<T>(iIndex));
  }
  return outputList;
}
std::string CmdLineParser::getConfigSummary(){
  std::stringstream ss;

  ss << "Options:";
  for( const auto& option : _optionsList_ ){
    ss << std::endl << "  " << option.getSummary();
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

int CmdLineParser::getOptionIndex(const std::string& name_){
  for( size_t iOption = 0 ; iOption < _optionsList_.size() ; iOption++ ){
    if( _optionsList_.at(iOption).getName() == name_ ){
      return int(iOption);
    }
  }
  return -1;
}




#endif //SIMPLE_CPP_CMD_LINE_PARSER_CMDLINEPARSER_IMPL_H