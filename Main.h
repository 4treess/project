/*
Author: Trevor De Clark, Sno: 665182861, CSCI 370 VIU
*/
#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
using namespace oracle::occi;

#define NUM_OF_COMMANDS 9

// creates a connection to the oracle db by getting the users login info
Connection* login(Environment* ptr);

// termniates the provided connection and environment
void logoff(Environment* ptr, Connection* ptr2);

// Gets input from the user using std::cout, and turns all letters to lowercase
std::string getUserInput();

// Processes the command the user entered, if its valid, it processes it, and returns 1, if not, returns 0
bool processCommand(std::string &userInput, std::string* commands, bool* functions, connection* connection);

// Creates and executes a sql update statement with the given parameters The amount of parameters is indicated by count.
bool update(Connection* connect, std::string string, int count);

// Creates and executes a sql query statement with the given parameters The amount of parameters is indicated by count.
bool query(Connection* connect, std::string string, int count, std::string* columns, int* maxWidth, int size);