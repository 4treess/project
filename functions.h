#pragma once
#include "Main.h"




/*
@brief
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool search(Connection* connect){
    int optionCount = 3;
    std::string options[optionCount + 1] = {"orders", "services", "employees", "\0"};
    bool (*functions[optionCount])(Connection*) = {searchOrders(Connection*), searchServices(Connection*), searchEmployees(Connection*)};
    std::string option = "";
    std::cin >> option;

    return processCommand(option, options, functions, connect);
}

/*
@brief
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool cancel(Connection* connect){
    int optionCount = 2;
    std::string options[optionCount + 1] = {"order", "service", "\0"};
    std::string option = "";
    std::cin >> option;

    int i;
    for(i = 0; i < optionCount; ++i){
        if(option == optionCount[i]){
            break;
        }
    }
    if(i == 2){
        return 0;
    }

    std::string string;
    if(i){
        string = "Update Services set IsCompleted = 1, isCanceled = 1 where ServiceID = :1;"
    }else{
        string = "Update Services set isCompleted = 1, is Canceled = 1 where orderID = :1;"
    }

    return update(connect, string, 1);
}

/*
@brief Marks a given Service as completed
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool complete(Connection* connect){
    std::string string = "Update Services set IsCompleted = 1, isCanceled = 0 where ServiceID = :1;"
    return update(connect, string, 1);
}

/*
@brief marks a given service as allocated
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool assign(Connection* connect){
    std::string string = "Update Services set provider = :1, IsAllocated = 1 where serviceID = :2;"
    return update(connect, string, 2);
}

/*
@brief removes an employee from the given service
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool remove(Connection* connect){
    std::string string = "Update Services set provider = null, isAllocated = 0 where ServiceID = :1;"
    return update(connect, string, 1);
}


/*
@param Connection* connect: The connection object in which statements are created from
@return true
*/
bool returnTrue(Connection* connect){
    return true
}

/*
@brief Adds an employee into the database
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool addEmployee(Connection* connect){
    std::string string ="Insert into Employees (name, phoneNumber, email, address) values (:1, :2, :3, :4);";
    return update(connect, string, 4);
}

/*
@brief deletes an employee from the database
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool deleteEmployee(Connection* connect){
    std::string string ="delete from Employees where employeeID = :1;";
    return update(connect, string, 1);    
}

/*
@brief Searches the list of orders.
@param Connection* connect: The connection object in which statements are created from
@return: true if at least one result was outputted, false otherwise
*/
bool SearchOrders(Connection* connect){
    int numberOfParams = 8;
    std::string input;
    std::cin >> input;

    if(input == "by"){
        std::cin >> input;
    }

    std::string parameters[numberOfParams] = {"orderid", "name", "customerid", "email", "phonenumber", "completed", "Allocated", "new"};

    std::string sql = {
        "Select * from orders where orderID = :1;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.name like '%' || :1 || '%' group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.CustomerID = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.email = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.phoneNumber = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from (select * from Orders where completionTime is not null order by completionDate desc) where rownum >= 20;",
        "Select * from Orders where completionTime is null and isAllocated = 1 order by placeTime asc;",
        "Select * from Orders where completionTime is null and isAllocated = 0 order by placeTime asc;"
    };

    int i = 0;
    for(; i < 8; ++i){
        if(input == parameters[i]){
            break;
        }
    }
}

/*
@brief Searches the list of employees.
@param Connection* connect: The connection object in which statements are created from
@return: true if at least one result was outputted, false otherwise
*/
bool searchEmployees(Connection* connect){
    int numberOfParams = 5;
    std::string input;
    std::cin >> input;

    if(input == "by"){
        std::cin >> input;
    }

    std::string columns[numberOfParams] = {employeeID, name, phone#, email, address};
    int maxWidth[numberOfParams] = {10, 30, 14, 60, 120};
    if(input == "employeeid"){
        std::string cmd = "Select * from Employees where EmployeeID = :1; ";
        query(connect, cmd, 1, columns, maxWidth, numberOfParams);
    }else{
        if(input == "name"){
            std::string cmd = "Select * from Employees where name like '%' || :1 || '%' ";
            query(connect, cmd, 1, columns, maxWidth, numberOfParams);
        }
        return 0
    }
    return 0;
}