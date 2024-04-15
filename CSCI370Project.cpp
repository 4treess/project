/*
Author: Trevor De Clark, Sno: 665182861, CSCI 370 VIU
*/

#include "Main.h"
#include "functions.cpp"



//Takes the values out of the Resultset and puts them into the array
void prepareResults(ResultSet* resSet, int* array);
void prepareResults(ResultSet* resSet, std::string* names, int* counts);


int main(){
    Environment* enviro = Environment::createEnvironment();
    Connection* connect = login(enviro);
    if(connect == NULL){
        Environment::terminateEnvironment(enviro);
        return 0;
    }

    std::string commands[NUM_OF_COMMANDS + 1] = {"search", "add", "delete", "cancel", "complete", "assign", "remove", "help", "quit", "\0"};
    bool (*functions[NUM_OF_COMMANDS])(Connection*) = {search, addEmployee, deleteEmployee, cancel, complete, assign, remove, helpScreen returnTrue};


    std::string input = getUserInput();
    
    while(true){
        if(input == "quit"){
            logoff(enviro, connect);
            return 0;
        }

        if(processCommand(input, commands, functions, connect)){
            std::cout <<"\nCommand Processed Sucessfully!\n";
        }else{
            std::cout <<"\nError! Command not found! Type \"Help\" for a list of commands\n";
        }
        std::cout << "Please Enter Your Command: ";
        std::string input = getUserInput();
    }

    logoff(enviro, connect);
    return 0;
}

/*
@brief: Gets the database login credentials from the user, exits only if the login is successful or if the user gets frustrated and types quit
@param: Environment* ptr: a pointer to the environment class object
@return: Connection*: A pointer to the newly created connection
*/
Connection* login(Environment* ptr){
    while(true){
        std::string username, password;
        std::cout << "Please enter your Oracle Database login credentials\nUsername: ";
        std::getline(std::cin, username);
        if(username == "quit"){
            return NULL;
        }
        std::cout << "Password: ";
        std::getline(std::cin, password);
        if(password == "quit"){
            return NULL;
        }

        Connection* connect;
        //got the exception handling code from the occi manual
        try{
          connect = ptr->createConnection(username, password, "database.csci.viu.ca");
        } catch(SQLException &except){
            std::cout << "Error! " << except.getMessage() << std::endl;
            continue;
        }
        return connect;
    }
}

/*
@brief: termninates both the connection and the environment variables provied to it, so the program can shutdown
@param: Environment* enviro: a pointer to the Envrionment class object
@param: Connection* connect: a pointer to the Connection object
@return: void
*/
void logoff(Environment* enviro, Connection* connect){
    enviro->terminateConnection(connect);
    Environment::terminateEnvironment(enviro);
}

/*
@brief: Gets the input from the user, makes all characters lowercase. Uses std::cin so it is whitespace sensitive
@param: N/A
@return: std::string, 
*/
std::string getUserInput(){
    std::string input = "";
    std::cin >> input;

    for(std::string::iterator it = year.begin(); it != year.end(); ++it){
        if(*it > 64 || *it < 91){
            *it += 32;
        }
    }
        
    return input;
}

/*
@brief: Turns the users input into a command
@param: std::string userInput, A string containing only the name of the command to be processed
@param: std::string* commands, a stiring of command names to compare the user input to.
@param: bool* functions, an array of boolean function pointers to the functions that actually do the hard work
@param: Connection* connect, a pointer to the connection object
@return: bool: 1 if the command successfully was processed, 0 otherwise
*/
bool processCommand(std::string &userInput, std::string* commands, bool* functions, Connection* connect){
    int i;
    for(i = 0; commands[i] != "\0"; ++i){
        if(userInput == commands[i]){
            if(functions[i](connect)){
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

/*
@brief: Takes the string in "string" and executes it via execute update
@param: Connection* connect: A pointer to the connection object 
@param: std::string string: the statement to be executed
@param: the amount of additional user inputs left to get
@return: void
*/
bool update(Connection* connect, std::string string, int count){
    try{
        Statement* stmt = connect->createStatement(string);
        int i = 0;
        while(i < count){
            std::string param getUserInput();
            stmt->setString(i++, param);
        }
        int result = stmt->executeUpdate();
        std::cout << "Updated " << result << " Rows!";
        connect->termniateStatement(stmt);
    }catch(SQLException &except){
        std::cerr << "Error " << except.what() << "\n";
        return 0;
    }
    return 1;
}

/*
@brief: Takes the string in "string" and executes it via execute update
@param: Connection* connect: A pointer to the connection object 
@param: std::string string: the statement to be executed
@param: int count: The amount of user inputs still yet to be retrieved 
@param: std::string* columns: An array with the name of all the columns
@param: int* maxWidth: an array with the maximum amount of characters for each column
@param: int size: The size of the arrays.
@return: void
*/
bool query(Connection* connect, std::string string, int count, std::string* columns, int* maxWidth, int size){
    try{
        Statement* stmt = connect->createStatement(string);
        int i = 0;
        while(i < count){
            std::string param = getUserInput();
            stmt->setString(i++, param);
        }
        ResultSet* rs = stmt->executeQuery();

        int i = 0;
        for(; i < size; i++){
            std::cout << std::setw(maxWidth[i]) << columns[i];
        }

        i = 0;
        std::cout << "\n";
        while(rs->next()){
            std::cout << std::setw(maxWidth[i]) << rs->getString;

            if(++i >= size){
                i = 0;
                std::cout << "\n";
            }
        }
    }
}

