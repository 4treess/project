/*
Author: Trevor De Clark, Sno: 665182861, CSCI 370 VIU
Notes: Channel names cannot have spaces. All years must have 3 parameters

Unrelated: The password for logging into the database via C++ is the Yearline interface password
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
using namespace oracle::occi;


// creates a connection to the oracle db by getting the users login info
Connection* login(Environment* ptr);

// termniates the provided connection and environment
void logoff(Environment* ptr, Connection* ptr2);

//Gets input from the user (all Years excluding quit have 2 parameters)
int getUserInput();

//Takes the values out of the Resultset and puts them into the array
void prepareResults(ResultSet* resSet, int* array);
void prepareResults(ResultSet* resSet, std::string* names, int* counts);

//Outputs the data from other functions in a file in the format of a report.
void generateReport(int year, int* newUsers, int* postedMessages, std::ofstream* file);
void generateReport(std::string* names, int* counts, std::ofstream* file);


int main(){
    Environment* enviro = Environment::createEnvironment();
    Connection* connect = login(enviro);
    if(connect == NULL){
        Environment::terminateEnvironment(enviro);
        return 0;
    }

    std::ofstream outputFile;
    outputFile.open("Report.txt", std::ofstream::trunc);

    std::string str1 = "select count(*), to_number(to_char(signUpDate, 'mm')) from Users where to_number(to_char(signUpDate, 'yyyy')) = :1 group by to_number(to_char(signUpDate, 'mm')) order by to_number(to_char(signUpDate, 'mm')) asc";
    std::string str2 = "select count(*), to_number(to_char(postTime, 'mm')) from messages where to_number(to_char(postTime, 'yyyy')) = :1 group by to_number(to_char(postTime, 'mm')) order by to_number(to_char(postTime, 'mm')) asc";
    std::string str3 = "select username, nickname, count(messageID) as messageCount from Users left join Messages on Users.username = messages.poster where to_number(to_char(messages.postTime, 'yyyy')) = :1 group by username, nickname order by messageCount desc";
    std::string str4 = "select channelName, creator, count(messageID) as messageCount from Users left join Channels on Users.username = Channels.creator join messages on channels.channelName = messages.channel where to_number(to_char(messages.postTime, 'yyyy')) = :1 group by channelName, creator order by messageCount desc";

    int year;
    int newUserCount[12] = {};
    int messageCount[12] = {};

    std::cout << "Please enter the year you want reports generated for: ";
    year = getUserInput();
    if(year == -1){
        logoff(enviro, connect);
        return 0;
    }
        try{
            Statement* stmt = connect->createStatement(str1);
            stmt->setInt(1, year);
            ResultSet* resSet1 = stmt->executeQuery();
            prepareResults(resSet1, newUserCount);
            stmt->closeResultSet(resSet1);

            stmt = connect->createStatement(str2);
            stmt->setInt(1, year);
            ResultSet* resSet2 = stmt->executeQuery();
            prepareResults(resSet2, messageCount);
            stmt->closeResultSet(resSet2);

            generateReport(year, newUserCount, messageCount, &outputFile);

            std::string names[6] = {"null", "null", "null", "null", "null", "null"};
            int counts[3] = {};

            outputFile << "\nSummary report for the year of " << year << "\n\n"
                       << "Top Three Most Active Users:\n"
                       << "username         nickname        # of posts\n"
                       << "--------         -----------     ----------\n";

            stmt = connect->createStatement(str3);
            stmt->setInt(1, year);
            ResultSet* resSet3 = stmt->executeQuery();
            prepareResults(resSet3, names, counts);
            stmt->closeResultSet(resSet3);
            generateReport(names, counts, &outputFile);

            outputFile << "\nTop Three Most Active Channels:\n"
                       << "channelName      creator         # of posts\n"
                       << "-----------      --------        ----------\n";

            for(int i = 0; i < 6; i++){
                names[i] = "null";
            }
            for(int i = 0; i < 3; i++){
                counts[i] = 0;
            }

            stmt = connect->createStatement(str4);
            stmt->setInt(1, year);
            ResultSet* resSet4 = stmt->executeQuery();
            prepareResults(resSet4, names, counts);
            stmt->closeResultSet(resSet4);
            generateReport(names, counts, &outputFile);
            connect->terminateStatement(stmt);

            outputFile << "\n" << std::right << std::setfill('=') << std::setw(32) << "END OF REPORT" << std::setw(18) << " ";

        }catch(SQLException* excep){
                std::cerr << "Error! " << excep << "\n";
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
@brief: returns the user input and confirms its an integer
@param: N/A
@return: int. The year that the user inputted is returned
*/
int getUserInput(){
    int output;
    std::string year = "";
    bool validInput = false;
    while(!validInput){
        output = 0;
        std::cin >> year;

        if(year == "quit"){
            return -1;
        }


        int count = 1;
        for(std::string::reverse_iterator it = year.rbegin(); it != year.rend(); ++it){
            if(*it < 48 || *it > 57){
                count = -1;
                break;
            }
            output += (*it - 48)*count;
            count *= 10;
        }

        if(count == -1){
            std::cout << "Invalid Input, Please enter a decimal number only using digits from 0-9: ";
            continue;
        }


        if(output < 2015 || output > 2023){
            std::cout << "Invalid Input, Please enter a year between and including 2015 and 2023: ";
            continue;
        }
        validInput = true;
        
        return output;
    }
    return -1;
}

/*
@brief: Takes the values and indexes out of the resultSet objects, and puts them into an array
@param: ResultSet* resSet: the result set to take the values from
@param: Int[] array: The array to put values into
@return: void
*/
void prepareResults(ResultSet* resSet, int* array){
    while(resSet->next()){
        //The Month number is in the 2nd column, so subtracting 1 gives the index.
        //The count from the query is in the first column
        array[resSet->getInt(2) - 1] = resSet->getInt(1);
    }
}

/*
@brief: Takes the values and indexes out of the resultSet objects, and puts them into their respective array
@param: ResultSet* resSet: the result set to take the values from
@param: std::string[] names: The array holding the names of the channel/User
@param: int[] counts: the array holding the message counts for the respective channel/user
@return: void
*/
void prepareResults(ResultSet* resSet, std::string* names, int* counts){
    int i = 0;
    int j = 0;
    while(resSet->next() && j < 3){
        names[i++] = resSet->getString(1);
        names[i++] = resSet->getString(2);
        counts[j++] = resSet->getInt(3);
    }
}

/*
@brief: Outputs the data from other functions in a file in the format of a report. (new users and number of messages posted)
@param: int year: the year the data is from.
@param: int[] newUsers: The counts of new users that signed up to the platform in the year in year, by month ordered by their index
@param: int[] postedMessages: The amount of messages posted in the given year, by month ordered by index
@param: std::ofstream file: the ofstream object coreesponding to the file you want the report in.
@return: void
*/
void generateReport(int year, int* newUsers, int* postedMessages, std::ofstream* file){
    std::string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    *file << "Activity Report By Month for the Year of " << year <<"\n\n"
         << "       # of New    # of Posted\n"
         << "Month  Users       Messages\n"
         << "-----  ---------  -------------\n";

    for(int i = 0; i < 12; i++){
        *file << "  " << months[i] << "  " << std::setw(9) << newUsers[i] << "  " << std::setw(13) << postedMessages[i] << "\n";
    }

    *file << std::setfill('=') << std::setw(50) << "\n";
}

/*
@brief: Outputs the data from other functions in a file in the format of a report.
@param: std::string names[]: An array with the usernames/channelNames (even indexes) and nicknames/poster (odd indexes) of users/channels
@param: int[] counts: The number of messages posted by/in the given user/channel
@param: std::ofstream file: the ofstream object coreesponding to the file you want the report outputted to.
@return: void
*/
void generateReport(std::string* names, int* counts, std::ofstream* file){
    for(int i = 0; i < 3; i++){
        *file << std::left << std::setfill(' ') << std::setw(15) << names[2*i] << "  " << std::setw(15) << names[2*i + 1] << "  " << std::setw(12) << counts[i] << "\n";
    }
}
