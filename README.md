![](./resources/logo/logo.png)

## Synopsis

Are you getting sick of always writing the same piece of code to get various
command line arguments? Well, search no more since Simple C++ CmdLineParser
does this job for you! Indeed, as a header-only library you just have to
`#include "CmdLineParser.h"` at the beginning of you code, and that's all:
no extra library to include, you're in! :D


## Example

As an example is always better to show a summary of what it does, take a look
at the `example/main.cpp` file:

![](./resources/screenshot/example.png)


## Running the example

You want to run the example? There you go!

``` bash
mkdir build
cd build
cmake ..
make

# Command example
./OptionParserExample -o "test lol" -i 15 -d 1549846.654654355 -iii 13 15 16 -ss test.txt foo.bar

# With Yaml example:
./OptionParserExample -y ../resources/configExample.yaml --trigger

# Also works with a JSON file!
./OptionParserExample -y ../resources/configExample.json
```
