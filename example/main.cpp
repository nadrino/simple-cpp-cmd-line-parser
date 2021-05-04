//
// Created by Adrien BLANCHET on 03/05/2021.
//

#include "CmdLineParser.h"

#include "iostream"

int main(int argc, char** argv){

  CmdLineParser clParser;

  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("int-example", {"-i"}, "Specify int value");
  clParser.addOption("double-example", {"-d"}, "Specify double value");

  clParser.parseCmdLine(argc, argv);

  std::cout << clParser.getConfigSummary() << std::endl;

  if( clParser.isOptionSet("output-file") ) std::cout << "output-file = " << clParser.getOptionVal<std::string>("output-file") << std::endl;
  if( clParser.isOptionSet("int-example") ) std::cout << "int-example = " << clParser.getOptionVal<int>("int-example") << std::endl;
  if( clParser.isOptionSet("double-example") ) std::cout << "double-example = " << clParser.getOptionVal<double>("double-example") << std::endl;

}