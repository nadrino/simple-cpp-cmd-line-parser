//
// Created by Adrien BLANCHET on 03/05/2021.
//

#ifndef SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H
#define SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H


class OptionHolder {

public:
  OptionHolder() = default;
  template<typename ValueType> explicit OptionHolder(const ValueType & value) : _varPtr_(new VariableHolder<ValueType>(value)){}
  OptionHolder(const OptionHolder & other) : _varPtr_(other._varPtr_ ? other._varPtr_->clone() : nullptr) {}

  virtual ~OptionHolder() { delete _varPtr_; }


protected:

  struct PlaceHolder{
    virtual ~PlaceHolder() = default;
    virtual const std::type_info& getType() const = 0;
    virtual PlaceHolder * clone() const = 0;
  };

  template<typename VariableType> struct VariableHolder: public PlaceHolder{

    explicit VariableHolder(const VariableType& value_) : _variable_(value_){  }
    const std::type_info & getType() const override { return typeid(VariableType); }
    PlaceHolder* clone() const override { return new VariableHolder(_variable_); }

    VariableType _variable_;

  };

  PlaceHolder* _varPtr_{nullptr};


};

// Inspired from https://stackoverflow.com/questions/18566066/c-elegant-way-of-holding-different-types-of-data-in-a-member-variable

#endif //SIMPLE_CPP_OPTION_PARSER_OPTIONHOLDER_H
