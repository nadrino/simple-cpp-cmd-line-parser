//
// Created by Adrien BLANCHET on 03/05/2021.
//

#include "CmdLineParser.h"

#include "iostream"

int main(int argc, char** argv){

  CmdLineParser clParser;

  // Option configuration:
  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("int-example", {"-i"}, "Specify int value");
  clParser.addOption("double-example", {"-d"}, "Specify double value");
  clParser.addOption("3-int", {"-iii"}, "Specify 3 int", 3);
  clParser.addOption("multiple-strings", {"-ss"}, "Specify multiple args", -1);

  // Show configuration
  std::cout << clParser.getConfigSummary() << std::endl;

  // Parsing Command Line
  clParser.parseCmdLine(argc, argv);

  // Show specified values
  std::cout << clParser.getValueSummary() << std::endl;

  // Get specific values
  if( clParser.isOptionSet("output-file") ) std::cout << "output-file = " << clParser.getOptionVal<std::string>("output-file") << std::endl;
  if( clParser.isOptionSet("int-example") ) std::cout << "int-example = " << clParser.getOptionVal<int>("int-example") << std::endl;
  if( clParser.isOptionSet("double-example") ) std::cout << "double-example = " << clParser.getOptionVal<double>("double-example") << std::endl;

  // If you don't want to check if the option is set
  std::cout << "output-file = " << clParser.getOptionVal<std::string>("output-file", "") << std::endl;
  std::cout << "3-int(0) = " << clParser.getOptionVal<int>("3-int", -1, 0) << std::endl;
  std::cout << "3-int(1) = " << clParser.getOptionVal<int>("3-int", -1, 1) << std::endl;
  std::cout << "3-int(2) = " << clParser.getOptionVal<int>("3-int", -1, 2) << std::endl;
  std::cout << "multiple-strings(0) = " << clParser.getOptionVal<std::string>("multiple-strings", "", 0) << std::endl;

  // Show all specified values
  size_t nbValues = clParser.getNbValueSet("multiple-strings");
  auto multiStrList = clParser.getOptionValList<std::string>("multiple-strings");
  if(not multiStrList.empty()){
    std::cout << "multiple-strings["<< nbValues <<"]: { ";
    for(const auto& str : multiStrList){
      std::cout << str << ", ";
    }
    std::cout << "}" << std::endl;
  }

}