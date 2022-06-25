# json-validator
JSON Validator &amp; Formatter

## Installation

```
#For Linux
sudo apt-get install qtbase5-dev
sudo apt-get install qtdeclarative5-dev

#For Mac
brew install qt@5
```

## Build

```
#For Linux
cmake ..
make 

#For Mac ( Build Error )
ccmake -DQt5_DIR=$(brew --prefix qt5)/lib/cmake/Qt5 .. 
cmake ..
make
```