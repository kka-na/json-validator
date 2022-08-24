# json-validator

JSON Validator &amp; Formatter

## Installation

Qt5

```
#For Linux
sudo apt-get install qtbase5-dev
sudo apt-get install qtdeclarative5-dev

#For Mac
brew install qt@5

```

RapidJson

```
sudo apt install rapidjson-dev
```

Valijson
https://github.com/tristanpenman/valijson#install-valijson-and-import-it

```
# Install Valijson
git clone --depth=1 git@github.com:tristanpenman/valijson.git
cd valijson
mkdir build
cd build
cmake ..
cmake --install .
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

## UI

[![JSON Validator Test Video](http://img.youtube.com/vi/l2LaRuJgu68/0.jpg)](https://youtu.be/l2LaRuJgu68)
