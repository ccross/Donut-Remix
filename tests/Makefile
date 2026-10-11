CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC_GUARDRAILS = ../src/Guardrails.cpp
SRC_SECURITY = ../src/Security.cpp
SRC_CONFIG = ../src/Config.cpp

SRC_DEVICES = ../src/Devices.cpp
SRC_GAMEID = ../src/GameIDEngine.cpp

TESTS = test_guardrails test_security test_config test_gameid

all: $(TESTS)

test_guardrails: $(SRC_GUARDRAILS) test_guardrails.cpp
	$(CXX) $(CXXFLAGS) -I../src $^ -o $@

test_security: $(SRC_SECURITY) test_security.cpp
	$(CXX) $(CXXFLAGS) -I../src $^ -o $@

test_config: $(SRC_GUARDRAILS) $(SRC_CONFIG) test_config.cpp
	$(CXX) $(CXXFLAGS) -I../src $^ -o $@

test_gameid: $(SRC_GUARDRAILS) $(SRC_CONFIG) $(SRC_DEVICES) $(SRC_GAMEID) test_gameid.cpp
	$(CXX) $(CXXFLAGS) -I../src $^ -o $@

test: $(TESTS)
	./test_guardrails
	./test_security
	./test_config
	./test_gameid

clean:
	rm -f $(TESTS)

.PHONY: all test clean
