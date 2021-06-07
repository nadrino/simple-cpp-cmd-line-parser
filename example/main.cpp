//
// Created by Nadrino on 03/05/2021.
//

#include "iostream"

#include "CmdLineParser.h"

void doExample1(int argc, char** argv);
void doExample2(int argc, char** argv);
#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
void doExample3(int argc, char** argv);
#endif

int main(int argc, char** argv){

  // If you want to allow some (irresponsible!!!) user to provide extra unrecognised args, you can
  // disable the fascistMode! This value can be set by default with DEFAULT_FASCIST_MODE macro
  // -> But we are not fascist here in this example so:
  CmdLineParser::setIsFascist(false);
  // "QU'EST-CE QUE J'AI A VOIR AVEC GEORGES? RIEN EN FAIT! GEORGES EST UN FACHISTE DE MERDE, UN FA-CHI-STE DE MERDE!!"

  doExample1(argc, argv); // simple example
  doExample2(argc, argv); // more complete example
#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
  doExample3(argc, argv); // example with yaml extension
#endif

  std::cout << "**** Have fun! :)" << std::endl;
}


void doExample1(int argc, char** argv){

  std::cout << std::endl << "EXAMPLE 1" << std::endl;

  CmdLineParser clParser;

  clParser.addTriggerOption("dry-run", {"--dry-run"},"Enable dry run");
  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("count", {"-c"}, "Specify count");

  std::cout << clParser.getConfigSummary() << std::endl;

  clParser.parseCmdLine(argc, argv);

  std::string outputFile = clParser.getOptionVal<std::string>("output-file", "");
  int count              = clParser.getOptionVal<int>("count", 0, 0);
  bool isDryRun          = clParser.isOptionTriggered("dry-run");

  if( isDryRun ){
    std::cout << count << ": " << outputFile << std::endl;
  }

  // ^^^ The above example was for the screenshot :)

  std::cout << std::endl;

}

void doExample2(int argc, char** argv){

  std::cout << "EXAMPLE 2" << std::endl;

  CmdLineParser clParser;

  // Option configuration:
  clParser.addTriggerOption("trigger-example", {"--trigger"},"Set trigger");
  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("int-example", {"-i"}, "Specify int value");
  clParser.addOption("double-example", {"-d"}, "Specify double value");
  clParser.addOption("3-int", {"-iii"}, "Specify 3 int", 3); // exactly 3 values are expected
  clParser.addOption("multiple-strings", {"-ss"}, "Specify multiple args", -1); // infinite number of values
  // this option is called without a dash (careful with those: if you want to provide an argument which have the same name, the option will override the argument)
  clParser.addOption("get", {"get"}, "Specify get values");

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
  std::cout << "All options:" << std::endl;
  std::cout << clParser.getValueSummary(true) << std::endl;
  std::cout << std::endl;

  // Use values
  std::cout << "**** Get specific values:" << std::endl;
  if( clParser.isOptionSet("output-file") ) std::cout    << "output-file = "    << clParser.getOptionVal<std::string>("output-file") << std::endl;
  if( clParser.isOptionSet("int-example") ) std::cout    << "int-example = "    << clParser.getOptionVal<int>("int-example") << std::endl;
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

}

#ifdef CMDLINEPARSER_YAML_CPP_ENABLED
void doExample3(int argc, char** argv){

  std::cout << "EXAMPLE 3 (yaml extension)" << std::endl;

  CmdLineParser clParser;

  // Option configuration:
  clParser.addTriggerOption("trigger-example", {"--trigger"},"Set trigger");
  clParser.addOption("output-file", {"-o", "--output"},"Specify output file path");
  clParser.addOption("int-example", {"-i"}, "Specify int value");
  clParser.addOption("double-example", {"-d"}, "Specify double value");
  clParser.addOption("3-int", {"-iii"}, "Specify 3 int", 3); // exactly 3 values are expected
  clParser.addOption("multiple-strings", {"-ss"}, "Specify multiple args", -1); // infinite number of values

  clParser.addYamlOption("yaml-config", {"-y"},"Specify yaml config file path");
  clParser.addOption("yaml-test", {"-yt"},"An option we gonna provide through the yaml-config");

  clParser.parseCmdLine(argc, argv);

  if( clParser.isOptionTriggered("yaml-config") ){

    std::cout << "**** YAML dump:" << std::endl;
    std::cout << clParser.dumpConfigAsYamlStr() << std::endl;

    // example to configure everything with a single yaml-file
    std::cout << std::endl << "**** Example to configure everything with a single yaml-file:" << std::endl;
    if( clParser.isOptionTriggered("yaml-config") ){
      // inheriting the provided file path
      std::string yamlFilePath = clParser.getOptionVal<std::string>("yaml-config");

      CmdLineParser clParserYamlOnly;
      clParserYamlOnly.parseYamlFile(yamlFilePath);
      std::cout << "Loaded options: " << std::endl << clParserYamlOnly.getConfigSummary() << std::endl;
      std::cout << std::endl << "Loaded values: " << std::endl << clParserYamlOnly.getValueSummary() << std::endl;
    }

  }
  else{

    std::cout << "-y was not provided." << std::endl;

  }

  std::cout << std::endl;

}
#endif
