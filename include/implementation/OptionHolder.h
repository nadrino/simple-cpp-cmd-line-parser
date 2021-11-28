//
// Created by Nadrino on 03/05/2021.
//

#ifndef SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H
#define SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H

#include "string"
#include "sstream"
#include <iomanip>
#include <iostream>

namespace CmdLineParserUtils{

  template<typename T>
  struct identity { typedef T type; };

  class OptionHolder {

  public:
    OptionHolder() = default;
    virtual ~OptionHolder() = default;

    // Setters
    void setName(const std::string &name) {
      _name_ = name;
    }
    void setDescription(const std::string &description) {
      _description_ = description;
    }
    void addCmdLineCallStr(const std::string &cmdLineCallStr){
      _cmdLineCallStrList_.emplace_back(cmdLineCallStr);
    }
    void setNbExpectedVars(int nbExpectedVars_){
      _nbExpectedVars_ = nbExpectedVars_;
    }
    void setIsTriggered(bool isTriggered_){
      _isTriggered_ = isTriggered_;
    }
    void setAllowEmptyValue(bool allowEmptyValue_){
      _allowEmptyValue_ = allowEmptyValue_;
    }
    void setNextVariableValue(const std::string& valueStr_){
      if( _nbExpectedVars_ == -1 or _strValuesList_.size() < _nbExpectedVars_ ){
        _strValuesList_.emplace_back(valueStr_);
      }
      else{
        throw std::logic_error( _name_ + ": Can't set more values since only " + std::to_string(_nbExpectedVars_) + " are expected." );
      }
    }

    // Getters
    bool isTriggered() const { return _isTriggered_; }
    bool isAllowEmptyValue() const { return _allowEmptyValue_; }
    const std::string &getName() const {
      return _name_;
    }
    const std::string &getDescription() const {
      return _description_;
    }
    const std::vector<std::string> &getCmdLineCallStrList() const {
      return _cmdLineCallStrList_;
    }
    int getNbExpectedVars() const { return _nbExpectedVars_; }
    const std::vector<std::string> &getValuesList() const {
      return _strValuesList_;
    }

    // Others
    bool isFullyFilled() const {
      return ( _nbExpectedVars_ == -1 ) or ( _nbExpectedVars_ == _strValuesList_.size() );
    }
    size_t getNbValues() const {
      return _strValuesList_.size();
    }

    // Eval
    template<typename T> T getValue(size_t index_) const {
      return getValue(index_, identity<T>());
    }

    // Misc
    std::string getSummary() const{
      std::stringstream ss;
      ss << _name_;
      if( not _cmdLineCallStrList_.empty() ){
        ss << " {";
        for(auto it = _cmdLineCallStrList_.begin(); it != _cmdLineCallStrList_.end(); ++it) {
          ss << *it;
          if(std::next(it) != _cmdLineCallStrList_.end()) {
            ss << ",";
          }
        }
        ss << "}";
      }
      ss << ": " << _description_ << " (";

      if( _nbExpectedVars_ != 0 ){
        if( _allowEmptyValue_ ) ss << "optional: ";
        else ss << "expected: ";

        if( _nbExpectedVars_ == 1 ) ss << "1 value";
        else if( _nbExpectedVars_ > 1 ) ss << _nbExpectedVars_ << " values";
        else ss << "N values";

      }
      else{
        ss << "trigger";
      }
      ss << ")";
      return ss.str();
    }

  protected:
    template<typename T> T getValue(size_t index_, identity<T>) const {
      if( index_ >= _strValuesList_.size() ){
        if( _nbExpectedVars_ >= 2 ){
          throw std::logic_error(_name_ + ": Value #" + std::to_string(index_) + " not set." );
        }
        else if( _nbExpectedVars_ == 1 ){
          throw std::logic_error(_name_ + ": Value not set." );
        }
        else if( _nbExpectedVars_ == 0){
          throw std::logic_error(_name_ + ": is a trigger option, not value can be fetch. Use isTriggered() instead." );
        }
        else{
          throw std::logic_error(_name_ + ": Value #" + std::to_string(index_) + " can't be fectch since var size is " + std::to_string(_strValuesList_.size()) );
        }
      }
      T outputVar;
      std::stringstream  sStream(_strValuesList_.at(index_));
      sStream >> outputVar;

      // Sanity check
      std::stringstream sStreamLoopBack;
      sStreamLoopBack.precision(int(_strValuesList_.at(index_).size()));
      sStreamLoopBack << outputVar;

      if( sStreamLoopBack.str() != sStream.str() ){
        throw std::runtime_error( _name_ + ": Could not convert \"" + sStream.str() + "\" to specified type. (Could be double precision involved)");
      }

      return outputVar;
    }
    double getValue(size_t index_, identity<double>) const {
      // Override to bypass
      if( index_ >= _strValuesList_.size() ){
        throw std::logic_error(_name_ + ": Could not fetch index value: " + std::to_string(index_) + " when var list size is " + std::to_string(_strValuesList_.size()) );
      }
      return std::stod(_strValuesList_.at(index_));
    }
    float getValue(size_t index_, identity<float>) const {
      if( index_ >= _strValuesList_.size() ){
        throw std::logic_error(_name_ + ": Could not fetch index value: " + std::to_string(index_) + " when var list size is " + std::to_string(_strValuesList_.size()) );
      }
      return std::stof(_strValuesList_.at(index_));
    }
    std::string getValue(size_t index_, identity<std::string>) const {
      if( index_ >= _strValuesList_.size() ){
        throw std::logic_error(_name_ + ": Could not fetch index value: " + std::to_string(index_) + " when var list size is " + std::to_string(_strValuesList_.size()) );
      }
      return _strValuesList_.at(index_);
    }

  private:

    bool _isTriggered_{false};
    bool _allowEmptyValue_{false};
    std::string _name_;
    std::string _description_;
    std::vector<std::string> _cmdLineCallStrList_;
    int _nbExpectedVars_{-1}; // default is infinite
    std::vector<std::string> _strValuesList_; // Don't need to store multiple variable types since every argument comes from a char*...

  };
}


// Deprecated tho (not necessary, but kept here in case I need it :) ):
// Inspired from https://stackoverflow.com/questions/18566066/c-elegant-way-of-holding-different-types-of-data-in-a-member-variable

#endif //SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H
