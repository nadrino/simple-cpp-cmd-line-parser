//
// Created by Adrien BLANCHET on 03/05/2021.
//

#include "CmdLineParser.h"

#include "iostream"

int main(int argc, char** argv){

  CmdLineParser clParser;

  std::cout << std::endl;

  // Option configuration:
  clParser.addTriggerOption("trigger-example", {"--trigger"},"Set trigger");
  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("int-example", {"-i"}, "Specify int value");
  clParser.addOption("double-example", {"-d"}, "Specify double value");
  clParser.addOption("3-int", {"-iii"}, "Specify 3 int", 3); // exactly 3 values are expected
  clParser.addOption("multiple-strings", {"-ss"}, "Specify multiple args", -1); // infinite number of values
  clParser.addOption("get", {"get"}, "Specify get values"); // this option is called without a dash

  // Show configuration
  std::cout << "**** Show command line configuration:" << std::endl;
  std::cout << clParser.getConfigSummary() << std::endl;
  std::cout << std::endl;

  // Parsing Command Line
  clParser.parseCmdLine(argc, argv);

  // Show values
  std::cout << "**** Show specified options:" << std::endl;
  std::cout << "Only the ones you called:" << std::endl;
  std::cout << clParser.getValueSummary() << std::endl;
  std::cout << std::endl;
  std::cout << "All options:" << std::endl;
  std::cout << clParser.getValueSummary(true) << std::endl;
  std::cout << std::endl;

  // Use values
  std::cout << "**** Get specific values:" << std::endl;
  if( clParser.isOptionSet("output-file") ) std::cout << "output-file = " << clParser.getOptionVal<std::string>("output-file") << std::endl;
  if( clParser.isOptionSet("int-example") ) std::cout << "int-example = " << clParser.getOptionVal<int>("int-example") << std::endl;
  if( clParser.isOptionSet("double-example") ) std::cout << "double-example = " << clParser.getOptionVal<double>("double-example") << std::endl;
  std::cout << std::endl;

  // Is it trigger? Or does the option has been set?
  std::cout << "**** Find out if the option has been called:" << std::endl;
  if( clParser.isOptionTriggered("trigger-example") ) std::cout << "trigger-example is ON" << std::endl;
  if( clParser.isOptionTriggered("output-file") ) std::cout << "out-file option has been specified" << std::endl;
  std::cout << std::endl;

  // If you don't want to check if the option is set
  std::cout << "**** Values with default in case they were not set:" << std::endl;
  std::cout << "output-file = " << clParser.getOptionVal<std::string>("output-file", "") << std::endl;
  std::cout << "3-int(0) = " << clParser.getOptionVal<int>("3-int", -1, 0) << std::endl;
  std::cout << "3-int(1) = " << clParser.getOptionVal<int>("3-int", -1, 1) << std::endl;
  std::cout << "3-int(2) = " << clParser.getOptionVal<int>("3-int", -1, 2) << std::endl;
  std::cout << "multiple-strings(0) = " << clParser.getOptionVal<std::string>("multiple-strings", "", 0) << std::endl;
  std::cout << std::endl;

  // Show all specified values
  std::cout << "**** Grab the list of values:" << std::endl;
  size_t nbValues = clParser.getNbValueSet("multiple-strings");
  auto multiStrList = clParser.getOptionValList<std::string>("multiple-strings");
  if(not multiStrList.empty()){
    std::cout << "multiple-strings["<< nbValues <<"]: { ";
    for(const auto& str : multiStrList){
      std::cout << str << ", ";
    }
    std::cout << "}" << std::endl;
  }
  std::cout << std::endl;

  std::cout << "**** Have fun! :)" << std::endl;
}