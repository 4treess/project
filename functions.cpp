#pragma once
#include "functions.h"




/*
@brief
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool search(Connection* connect){
    int optionCount = 3;
    std::string options[optionCount + 1] = {"orders", "services", "employees", "\0"};
    bool (*functions[optionCount])(Connection*) = {searchOrders(Connection*), searchServices(Connection*), searchEmployees(Connection*)};
    std::string option = getUserInput();

    return processCommand(option, options, functions, connect);
}

/*
@brief
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool cancel(Connection* connect){
    int optionCount = 2;
    std::string options[optionCount] = {"order", "service"};
    std::string option = getUserInput();

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
    std::string string = "Update Services set employeeID = :1, IsAllocated = 1 where serviceID = :2;"
    return update(connect, string, 2);
}

/*
@brief removes an employee from the given service
@param Connection* connect: The connection object in which statements are created from
@return: true if the command the user inputted exists and it ran properly, false otherwise
*/
bool remove(Connection* connect){
    std::string string = "Update Services set employeeID = null, isAllocated = 0 where ServiceID = :1;"
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
@brief Figures out which command the user is looking for, and sends it out for processing
@param Connection* connect: The connection object in which statements are created from
@return: true if at least one result was outputted, false otherwise
*/
bool SearchOrders(Connection* connect){
    int numberOfCommands = 8;
    std::string input = getUserInput();
    std::string parameters[numberOfCommands] = {"orderid", "name", "customerid", "email", "phonenumber", "completed", "allocated", "new"};
    std::string sql[numberOfCommands] = {
        "Select * from orders where orderID = :1;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.name like '%' || :1 || '%' group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.CustomerID = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.email = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select * from Orders, Customers where Customers.CustomerID = Orders.CustomerID and Customers.phoneNumber = :1 group by Orders.orderID order by Orders.placeTime desc;",
        "Select orderID, customerID, totalPrice, completionTime from (select orderID, customerID, totalPrice, completionTime from Orders where completionTime is not null order by completionTime desc) where rownum >= 20;",
        "Select orderID, placeTime, customerID, totalPrice from Orders where completionTime is null and isAllocated = 1 order by placeTime asc;",
        "Select orderID, placeTime, customerID, totalPrice from Orders where completionTime is null and isAllocated = 0 order by placeTime asc;"
    };

    int i = 0;
    for(; i < numberOfCommands; ++i){
        if(input == parameters[i]){
            switch(i){
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                {
                    int numberOfOutputs = 7;
                    std::string columns[numberOfOutputs] = {"OrderID", "Place Time", "CustomerID", "Total Price", "Allocated", "Completed", "Completion Time"};
                    int maxWidth[numberOfOutputs] = {15, 15, 9, 12, 1, 1, 15};
                    return query(connect, sql[i], 1, columns, maxWidth, numberOfOutputs);
                    break;
                }
                case 5:
                {
                    int numberOfOutputs = 4;
                    std::string columns[numberOfOutputs] = {"OrderID", "CustomerID", "Total Price", "Completion Time"};
                    int maxWidth[numberOfOutputs] = {15, 9, 12, 15};
                    return query(connect, sql[i], 0, columns, maxWidth, numberOfOutputs);
                    break;
                }
                case 6:
                case 7:
                {
                    int numberOfOutputs = 4;
                    std::string columns[numberOfOutputs] = {"OrderID", "Place Time", "CustomerID", "Total Price"};
                    int maxWidth[numberOfOutputs] = {15, 15, 9, 12};
                    return query(connect, sql[i], 0, columns, maxWidth, numberOfOutputs);
                    break;
                }
                default:
                    break;
            }   
        }
    }
    return 0;
}

/*
@brief Searches the list of services, and sends it out for processing
@param Connection* connect: The connection object in which statements are created from
@return: true if at least one result was outputted, false otherwise
*/
bool SearchServices(Connection* connect){
    int numberOfParams = 10;
    std::string input getUserInput();

    std::string parameters[numberOfParams] = {"serviceid", "orderid", "name", "customerid", "email", "phonenumber", "completed", "allocated", "new", "employeeid"};

    std::string sql[numberOfParams] = {
        "Select orderID, employeeID, serviceType, isCanceled, isCompleted from Services where ServiceID = :1;",
        "Select serviceID, employeeID, serviceType, isCanceled, isCompleted from Services where orderID = :1; ",
        "Select Services.serviceID, Orders.orderID, employeeID, serviceType, Services.isCanceled, Services.isCompleted, Customers.Name from Orders join Customers on Customers.CustomerID = Orders.CustomerID join Services on Services.orderID = Orders.orderID and Customers.name like '%' || :1 || '%' group by Orders.orderID order by Orders.placeTime desc, Services. serviceID asc;",
        "Select Services.serviceID, Orders.orderID, Services.employeeID, Services.serviceType, Services.isCanceled, Services.isCompleted, Customers.name from Orders, Customers, Services where Customers.CustomerID = Orders.CustomerID and Services.OrderID = Orders.OrderID and Customers.CustomerID = :1 group by Orders.orderID order by Orders.placeTime desc, Services. serviceID asc;",
        "Select serviceID, Orders.orderID, employeeID, serviceType, Services.isCanceled, Services.isCompleted, Customers.name from Orders, Customers, Services where Customers.CustomerID = Orders.CustomerID and Services.orderID = Orders.orderID and Customers.email = :1 group by Orders.orderID order by Orders.placeTime desc, Services. serviceID asc; ",
        "Select serviceID, Orders.orderID, employeeID, serviceType, Services.isCanceled, Services.isCompleted, Customers.name from Orders, Customers, Services where Customers.CustomerID = Orders.CustomerID and Services.orderID = Orders.orderID and Customers.phoneNumber = :1 group by Orders.orderID order by Orders.placeTime desc, Services. serviceID asc; ",
        "Select serviceID, orderID, employeeID, serviceType, isCanceled from ( Select serviceID, orderID, employeeID, serviceType, isCanceled from Services where IsCompleted = 1; order by ServiceID desc) where rowNum < 30 ",
        "Select serviceID, orderID, employeeID, serviceType from Services where IsCompleted = 0; order by ServiceID asc;",
        "Select serviceID, orderID, employeeID, serviceType from Services and isCanceled = 1 order by ServiceID desc; ",
        "select serviceID, orderID, name, isCompleted from employees natural join services, where employeeID = :1 order by serviceID desc"
    };

    int i = 0;
    for(; i < numberOfParams; ++i){
        if(input == parameters[i]){
            switch(i){
                case 0:
                case 1:
                {
                    int numberOfOutputs = 5;
                    std::string columns[numberOfOutputs] = {"ID", "EmployeeID", "ServiceType", "Canceled", "Completed"};
                    std::string maxWidth[numberOfOutputs] = {15, 10, 30, 1, 1};
                    return query(connect, sql[i] , 1, columns, maxWidth, numberOfOutputs);
                    break;
                }
                case 2:
                case 3:
                case 4:
                case 5:
                {
                    int numberOfOutputs = 7;
                    std::string columns[numberOfOutputs] = {"ServiceID", "OrderID", "EmployeeID", "Service Type", "Canceled", "Completed", "Customers Name"};
                    std::string maxWidth[numberOfOutputs] = {12, 15, 10, 30, 1, 1, 40};
                    return query(connect, sql[i] , 1, columns, maxWidth, numberOfOutputs);
                    break;
                }
                case 6:
                {
                    int numberOfOutputs = 5;
                    std::string columns[numberOfOutputs] = {"ServiceID", "OrderID", "EmployeeID", "Service Type", "Canceled"};
                    std::string maxWidth[numberOfOutputs] = {12, 15, 10, 30, 1};
                    return query(connect, sql[i] , 0, columns, maxWidth, numberOfOutputs);
                    break;
                }
                }
                case 7:
                case 8:
                {
                    int numberOfOutputs = 4;
                    std::string columns[numberOfOutputs] = {"ServiceID", "OrderID", "EmployeeID", "Service Type"};
                    std::string maxWidth[numberOfOutputs] = {12, 15, 10, 30};
                    return query(connect, sql[i] , 0, columns, maxWidth, numberOfOutputs);
                    break;
                }
                case 9:
                {
                    int numberOfOutputs = 4;
                    std::string columns[numberOfOutputs] = {"ServiceID", "OrderID", "Employee Name", "Completed"};
                    std::string maxWidth[numberOfOutputs] = {12, 15, 40, 1};
                    return query(connect, sql[i] , 1, columns, maxWidth, numberOfOutputs);
                    break;
                }
                default:
                    break;
            }   
        }
    }
    return 0;
}

/*
@brief Searches the list of employees.
@param Connection* connect: The connection object in which statements are created from
@return: true if at least one result was outputted, false otherwise
*/
bool searchEmployees(Connection* connect){
    int numberOfParams = 5;
    std::string input = getUserInput();

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

bool helpScreen(Connection* dud){
    std::cout << "\n\n\nItems in () you choose one of the options, items in <> represents where you put the data you are looking for\n"
              << "List of Commands:\n\n"
              << "Search:\n\n"
              << "Displays all elements that have the specfied data in the specified slot. IE orderID = 123\n"
              << "Search Orders (orderid/name/customerID/email/phonenumber) <data>\n\n"
              << "Displays all orders that fit these properties. [There is a limit to the amount shown for completed orders]\n"
              << "Search Orders (completed/allocated/new)\n\n"
              << "Displays all elements that have the specfied data in the specified slot. IE serviceID = 456\n"
              << "Search Services (serviceID/orderid/name/customerID/email/phonenumber/employeeID) <data>\n\n"
              << "Displays all services that fit these properties. [There is a limit to the amount shown for completed services]\n"
              << "Search Services (completed/allocated/new)\n\n"
              << "Searches the list of employees and displays their information\n"
              << "Search employees (employeeid/name) <data>\n\n"
              << "Add\n\n"
              << "Adds an employee into the system A employeeID is automatically generated for them No spaces are allowed in the data\n"
              << "Add <Employee_Name> <Phone_Number> <email> <address>\n\n"
              << "Delete\n\n"
              << "Deletes an employees data from the system\n"
              << "delete <employee_id>\n\n"
              << "Cancel\n\n"
              << "Cancels an order or service. ID is either the order ID or service ID depending on what youre cancelling\n"
              << "Cancel (Order/Service) <id>\n\n"
              << "Complete\n\n"
              << "Completes a service. If all services are completed, the order is automatically marked as complete\n"
              << "complete <serviceID>\n\n"
              << "Assign\n\n"
              << "Assigns the specified employee to the specified service\n"
              << "Assign <employeeID> <serviceID>\n\n"
              << "Remove\n\n"
              << "Removes the employee from the specified service\n"
              << "Remove <serviceID>\n\n"
              << "Help\n\n"
              << "Displays this screen\n"
              << "Help\n\n"
              << "Quit\n\n"
              << "Exits the program\n"
              << "quit\n\n"
              << "Note: All commands are case insensitive\n" << std::endl;

              return 1;

}