#pragma once
#include <Windows.h>
#include "Supervisor.h"
#include "Librarian.h"
#include "Faculty.h"
#include "Student.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
using namespace std;


chrono::system_clock::time_point today = chrono::system_clock::now();
time_t t = chrono::system_clock::to_time_t(today);
struct tm* now = localtime(&t);
int currentYear = now->tm_year + 1900; // years since 1900
int currentMonth = now->tm_mon + 1; // months since January [0-11]
int currentDay = now->tm_mday; // day of the month [1-31]


class Library{
    private:
        Person** personArr; //array to store Person object using polymorphism
        Book** bookArr; //array to store book 

        Person* userPtr; //pointer for log in and execute function based on type of object

        int numSupervisor;
        int numLibrarian;
        int numStudent;
        int numFaculty;

        int personArrSize;
        int bookArrSize;

        Person* isValidMem(string, string, string); //check log in info and return userPtr if correct

        void supervisorExecuteMenu(); //execute menu for supervisor 
        void librarianExecuteMenu(); //execute menu for librarian
        void userExecuteMenu(); //execute menu for user (student and faculty have the same menu)

        void addAdmin(); //add new supervisor/librarian to person array
        void deleteAdmin();  //delete new supervisor/librarian from Person array

        void addUser(); //add new faculty/student to person array
        void deleteUser(); //delete faculty/student from person array

        void addBook(); //add new book to books array
        void deleteBook(); //delete a book from books array

        void viewAllLibrarians();
        void viewAllUsers();
        void viewAllBooks();
        void viewAllAvailableBooks();

        void borrowBook(); //borrow books (only applied for faculty/student objects)
        void returnBook(); //return books (only applied for faculty/student objects)

        void toMenu(); //return to different menu specified by userPtr’s role, load, and save data
        void logOut();

        void setBookToNone(Book*); //set book to none
        void payLateFee();

        bool deletePerson(string );

        string logInMenu();

        void loadPerson();   //load members' info from person’s data file
        void loadBooks(); //books' info from books’ data  file

        void savePerson(); //save member s' info to person’s data file
        void saveBooks(); //save books’ info to books’ data file

    public:
        Library();
        void logIn();
        
};

Library::Library(){
    personArrSize = 0;
    bookArrSize = 0;
    numFaculty = 0;
    numLibrarian = 0;
    numSupervisor = 0;
    numStudent = 0;
    userPtr = nullptr;
    personArr = new Person* [215];
    bookArr = new Book* [100];
}

void Library::logOut(){
    cout << "Loging out..." << endl;
    Sleep(1500);
    system("cls");
    logIn();
}

void Library::toMenu(){
    cout << "Loading menu..." << endl;
    Sleep(1500);
    system("CLS");
    savePerson();
    saveBooks();
    if(userPtr->getRole() == "supervisor"){
        supervisorExecuteMenu();
    }
    else if(userPtr->getRole() == "librarian"){
        librarianExecuteMenu();
    }
    else if(userPtr->getRole() == "faculty" || userPtr->getRole() == "student" ){
        userExecuteMenu();
    }
}

void Library::supervisorExecuteMenu(){
    int choice =  userPtr->menu();
    switch (choice){
        case 1:
            addAdmin();
        case 2:
            deleteAdmin();
        case 3:
            addUser();
        case 4:
            deleteUser();
        case 5:
            addBook();
        case 6:
            deleteBook();
        case 7:
            viewAllLibrarians();
            toMenu();
        case 8:
            viewAllUsers();
            toMenu();
        case 9:
            viewAllBooks();
            toMenu();
        case 10:
            logOut();
        default:
            cout << "Invalid option. Please try again." << endl;
            toMenu();
    }
}

void Library::librarianExecuteMenu(){
    int choice =  userPtr->menu();
    switch(choice){
        case 1:
            addUser();
        case 2:
            deleteUser();
        case 3:
            addBook();
        case 4:
            deleteBook();
        case 5: 
            viewAllUsers();
            system("Pause");
            toMenu();
        case 6:
            viewAllBooks();
            system("Pause");
            toMenu();
        case 7:
            logOut();
        default:
            cout << "Invalid option. Please try again." << endl;
            librarianExecuteMenu();
    }
}

void Library::userExecuteMenu(){
    int choice =  userPtr->menu();
    switch(choice){
        case 1:
            borrowBook();
        case 2:
            returnBook();
        case 3:
            viewAllAvailableBooks();
            toMenu();
        case 4:
            userPtr->viewBorrowingBooks();
            system("Pause");
            toMenu();
        case 5:
            userPtr->viewLateFee();
            system("Pause");
            toMenu();
        case 6:
            payLateFee();
        case 7:
            logOut();
        default:
            cout << "Invalid option. Please try again." << endl;
            userExecuteMenu();
    }
    
}

void Library::borrowBook(){
    string index;
    if(userPtr->getAvailableSlot() == 0){
        cout << "Your slot is full. You can\'t borrow more books." <<
                "Return books if you want to borrow more." << endl;
        system("Pause");
        toMenu();
    }
    if(userPtr->getLateFee() != 0){
        cout << "You have late fee that must be paid. " << 
                "Please pay late fee before borrowing another book." << endl;
        system("Pause");
        toMenu();
    }
    viewAllAvailableBooks();
    cout << "----------------Borrowing Book----------------" << endl;
    cout << "Enter book's index you want to borrow (or \"quit\" to quit):" << endl;
    cin >> index;
    if(index == "quit"){
        toMenu();
    }
    for(int i = 0; i < bookArrSize; i++){
        if(bookArr[i]->available && bookArr[i]->index == index){
            bookArr[i]->available = false;
            bookArr[i]->borrowerName = userPtr->getFullName();
            bookArr[i]->startDay = currentDay;
            bookArr[i]->startMonth = currentMonth;
            bookArr[i]->startYear = currentYear;
            bookArr[i]->expirationDay = bookArr[i]->startDay;
            bookArr[i]->expirationMonth = bookArr[i]->startMonth + 1;
            bookArr[i]->expirationYear = bookArr[i]->startYear;
            if(bookArr[i]->expirationDay == 31){
                if(bookArr[i]->expirationMonth == 4 || bookArr[i]->expirationMonth == 6 ||
                    bookArr[i]->expirationMonth == 9 || bookArr[i]->expirationMonth == 11){
                        bookArr[i]->expirationDay = 30;
                }
            }
            if(bookArr[i]->expirationMonth == 2 && bookArr[i]->expirationDay > 28){
                if(bookArr[i]->expirationYear % 4 == 0){
                    bookArr[i]->expirationDay == 29;
                }
                else{
                    bookArr[i]->expirationDay == 28;
                }
            }
            bookArr[i]->overdueCharge = 0;
            userPtr->borrowBook(bookArr[i]);
            cout << "Proceed done!" << endl;
            toMenu();
        }
    }
    cout << "Invalid index. Please try again." << endl;
    borrowBook();
}

void Library::returnBook(){
    string index;
    bool found = false;
    cout << "-----------Returning Book-----------" << endl;
    userPtr->viewBorrowingBooks();
    cout << "Enter index of book you want to return ( or \"quit\" to quit)" << endl;
    cin >> index;
    if(index == "quit"){
        toMenu();
    }
    found = userPtr->returnBook(index);
    if(found){
        for(int i = 0; i < bookArrSize; i++){
            if(bookArr[i]->index == index){
                setBookToNone(bookArr[i]);
                toMenu();
            }
        }
    }
    else{
        cout << "Invalid index. Please try again." << endl;
        returnBook();
    }
}

void Library::addBook(){
    cout << "-----Add a book------" << endl;
    Book* b = new Book();
    cout << "Create book index:" << endl;
    cin >> b->index;
    cout << "Enter book's title:" << endl;
    cin.ignore();
    getline(cin, b->title);
    cout << "Enter author's name:" << endl;
    getline(cin, b->authorName);
    cout << "Enter book's publisher:" << endl;
    getline(cin, b->publisher);
    cout << "Enter book's pushlishing year: " << endl;
    cin >> b->publishingYear;
    bookArr[bookArrSize] = b;
    bookArrSize++;
    toMenu();
}

void Library::deleteBook(){
    bool found = false;
    string inputIndex;
    viewAllBooks();
    cout << "---------------Deleting a book--------------" << endl;
    cout << "Enter book's index you to want delete (or \"quit\" to quit):" << endl;
    cin >> inputIndex;
    if(inputIndex == "quit"){
        toMenu();
    }

    for(int i = 0; i < bookArrSize; i++){
        if(bookArr[i]->index == inputIndex){
            found = true;
            delete bookArr[i];
            bookArr[i] = nullptr;
            for(int j = i; j < bookArrSize - 1; j++){
                bookArr[j] = bookArr[j+1];
            }
            bookArrSize--;
            toMenu();
        }
    }
    
    if(!found){
        cout << "You entered invalid index. Please try again." << endl;
        deleteBook();
    }
}

void Library::addUser(){
    int role, inputID;
    Person* p = nullptr;
    string input;
    cout << "--------------Adding an User-------------" << endl;
    cout << "Enter a role: " << endl;
    cout << "1 - Faculty " << endl;
    cout << "2 - Student " << endl;
    cin >> role;

    if(role == 1){
        if(numFaculty == 100){
            cout << "Faculty slot is full." << endl;
            toMenu();
        }
        p = new Faculty();
        numFaculty++;
    }
    else if(role == 2){
        if(numStudent == 100){
            cout << "Student slot is full." << endl;
            toMenu();
        }
        p = new Student();
        numStudent++;
    }
    cout << "-------------------------" << endl;
    cout << "Enter first Name:" << endl;
    cin >> input;
    p->setFirstName(input);
    cout << "-------------------------" << endl;
    cout << "Enter middle Name:" << endl;
    cin >> input;
    p->setMiddleName(input);
    cout << "-------------------------" << endl;
    cout << "Enter last Name:" << endl;
    cin >> input;
    p->setLastName(input);
    cout << "-------------------------" << endl;
    cout << "Enter date of birth (mm-dd-yyy):" << endl;
    cin >> input;
    p->setDateOfBirth(input);
    cout << "-------------------------" << endl;
    cout << "Enter an username:" << endl;
    cin >> input;
    p->setUsername(input);
    cout << "-------------------------" << endl;
    cout << "Enter a password:" << endl;
    cin >> input;
    p->setPassword(input);
    cout  << "Enter ID:" << endl;
    cin >> inputID;
    p->setID(inputID);
    personArr[personArrSize] = p;
    personArrSize++;
    toMenu();
}

void Library::addAdmin(){
    int role , inputID;
    Person* p = nullptr;
    string input;
    cout << "-----Add an Admin-----" << endl;
    cout << "Enter a role: " << endl;
    cout << "1 - Supervisor " << endl;
    cout << "2 - Librarian " << endl;
    cin >> role;

    if(role == 1){
        if(numSupervisor == 5){
            cout << "Supervisor slot is full." << endl;
            toMenu();
        }
        p = new Supervisor();
        numSupervisor++;
    }
    else if(role == 2){
        if(numLibrarian == 10){
            cout << "Librarian slot is full." << endl;
            toMenu();
        }
        p = new Librarian();
        numLibrarian++;
    }
    cout << "-------------------------" << endl;
    cout << "Enter first Name:" << endl;
    cin >> input;
    p->setFirstName(input);
    cout << "-------------------------" << endl;
    cout << "Enter middle Name:" << endl;
    cin >> input;
    p->setMiddleName(input);
    cout << "-------------------------" << endl;
    cout << "Enter last Name:" << endl;
    cin >> input;
    p->setLastName(input);
    cout << "-------------------------" << endl;
    cout << "Enter date of birth (mm-dd-yyy):" << endl;
    cin >> input;
    p->setDateOfBirth(input);
    cout << "-------------------------" << endl;
    cout << "Enter an username:" << endl;
    cin >> input;
    p->setUsername(input);
    cout << "-------------------------" << endl;
    cout << "Enter a password:" << endl;
    cin >> input;
    p->setPassword(input);
    cout  << "Enter ID:" << endl;
    cin >> inputID;
    p->setID(inputID);
    personArr[personArrSize] = p;
    personArrSize++;
    toMenu();
}

void Library::deleteUser(){
    viewAllUsers();
    cout << "-----Delete an User-----" << endl;
    string input;
    int inputID;
    bool deleted = false;
    cout << "-------------------------" << endl;
    cout << "Enter user's ID you want to remove (or \"quit\" to quit):" << endl;
    cin >> input;
    if(input == "quit"){
        cout << "Backing to menu..." << endl;
        toMenu();
    }
    else{
        deleted = deletePerson(input);
    }
    if(!deleted){
        cout << "You entered invalid ID. Please try again." << endl;
        deleteUser();
    }
    toMenu();
}

void Library::deleteAdmin(){
    viewAllLibrarians();
    cout << "-----Delete an Admin-----" << endl;
    string input;
    int inputID;
    bool deleted = false;
    cout << "-------------------------" << endl;
    cout << "Enter librarian's ID you want to remove (or \"quit\" to quit):" << endl;
    cin >> input;
    if(input == "quit"){
        toMenu();
    }
    else{
        deleted = deletePerson(input);
    }
    if(!deleted){
        cout << "You entered invalid ID. Please try again." << endl;
        deleteAdmin();
    }
    toMenu();
}

bool Library::deletePerson(string input){
    int inputID;
    istringstream iss(input);
    iss >> inputID;
    cout << "You entered: " << inputID << endl;
    for(int i = 0; i < personArrSize; i++){
        if(personArr[i]->getID() == inputID){
            if(personArr[i]->getRole() == "librarian"){
                numLibrarian--;
            }
            else if(personArr[i]->getRole() == "faculty"){
                numFaculty--;
            }
            else if(personArr[i]->getRole() == "student"){
                numStudent--;
            }
            delete personArr[i];
            personArr[i] = nullptr;
            for(int j = i; j < personArrSize - 1; j++){
                personArr[j] = personArr[j+1];
            }
            personArrSize--;
            
            return true;
        }
    }
    return false;
}

void Library::viewAllLibrarians(){
    cout << "-------------View all librarians-------------- " << endl;
    for(int i = 0; i < personArrSize; i++){
        if(personArr[i]->getRole() == "librarian"){
            personArr[i]->printInfo();
            cout << endl;
        }
    }
    system("pause");
}

void Library::viewAllUsers(){
    cout << "-------------View all users-------------- " << endl;
    for(int i = 0; i < personArrSize; i++){
        if(personArr[i]->getRole() == "student" || personArr[i]->getRole() == "faculty"){
            personArr[i]->printInfo();
            cout << endl;
        }
    }
    system("pause");
}

void Library::viewAllAvailableBooks(){
    cout << "Available Book" << endl;
    for(int i = 0; i < bookArrSize; i++){
        if(bookArr[i]->available == true){
            cout << "Index           : " << bookArr[i]->index << endl;
            cout << "Title           : " << bookArr[i]->title << endl;
            cout << "Author          : " << bookArr[i]->authorName << endl;
            cout << "Publisher       : " << bookArr[i]->publisher << endl;
            cout << "Publishing Year : " << bookArr[i]->publishingYear << endl;
            cout << endl;
        }
    }
    system("pause");
}

void Library::viewAllBooks(){
    cout << "All Books' Information" << endl;
    for(int i = 0; i < bookArrSize; i++){
        cout << "Index           : " << bookArr[i]->index << endl;
        cout << "Title           : " << bookArr[i]->title << endl;
        cout << "Author          : " << bookArr[i]->authorName << endl;
        cout << "Publisher       : " << bookArr[i]->publisher << endl;
        cout << "Publishing Year : " << bookArr[i]->publishingYear << endl;
        cout << "Borrower        : " << bookArr[i]->borrowerName << endl;
        cout << "Start Date      : " << to_string(bookArr[i]->startMonth) 
                                        + "-" + to_string(bookArr[i]->startDay)  
                                        + "-" + to_string(bookArr[i]->startYear) << endl;
        cout << "Expiration Date : " << to_string(bookArr[i]->expirationMonth) 
                                        + "-" + to_string(bookArr[i]->expirationDay) 
                                        + "-" + to_string(bookArr[i]->expirationYear) << endl;
        cout << "Overdue Charge  : " << bookArr[i]->overdueCharge << endl;        
        cout << endl;
    }
    system("pause");
}

void Library::logIn(){
    loadBooks();
    loadPerson();
    string role;
    string username, password;
    role = logInMenu();
    cout << "----------------------------------------------" << endl;
    cout << "Enter your username: " << endl;
    cin >> username;
    cout << "----------------------------------------------" << endl;
    cout << "Enter your password: " << endl;
    cin >> password;

    userPtr = isValidMem(role, username, password);
    cout << 1 << endl;
    if(userPtr){
        if(userPtr->getRole() == "supervisor"){
            cout << "Signing in as Supervisor..." << endl;

        }
        else if(userPtr->getRole() == "librarian"){
            cout << "Signing in as Librarian..." << endl;

        }
        else if(userPtr->getRole() == "student"){
            cout << "Signing in as Student..." << endl;

        }
        else if(userPtr->getRole() == "faculty"){
            cout << "Signing in as Faculty..." << endl;

        }
        toMenu();
    }
    else{
        cout << "Login failed. Please try again." << endl;
        Sleep(2000);
        system("CLS");
        logIn();
    }
}

Person* Library::isValidMem(string role, string username, string password){
    for(int i = 0; i < personArrSize; i++){
        if(personArr[i]->getUsername() == username && personArr[i]->getPassword() == password
            && personArr[i]->getRole() == role){
            cout << "Login Successfully." << endl;
            return personArr[i];
        }
    }
    return nullptr;
}

string Library::logInMenu(){
    int inputNum;
    string role;
    cout << "**********************************************" << endl;
    cout << "||   Enter the role you want to sign in:    ||" << endl;
    cout << "||                                          ||" << endl;
    cout << "||   1 - Supervisor                         ||" << endl;
    cout << "||   2 - Librarian                          ||" << endl;
    cout << "||   3 - Faculty                            ||" << endl;
    cout << "||   4 - Student                            ||" << endl;
    cout << "||   5 - Quit                               ||" << endl;
    cout << "||                                          ||" << endl;
    cout << "**********************************************" << endl;
    cout << endl;
    cin >> inputNum;
    cout << endl;
    if(inputNum == 1){
        role = "supervisor";
        cout << "--------------Loging in as Supervisor--------------" << endl;
    }
    else if(inputNum == 2){
        role = "librarian";
        cout << "--------------Loging in as Librarian---------------" << endl;
    }
    else if(inputNum == 3){
        role = "faculty";
        cout << "--------------Loging in as Faculty---------------" << endl;
    }
    else if(inputNum == 4){
        role = "student";
        cout << "--------------Loging in as Student---------------" << endl;
    }
    else if(inputNum == 5){
        cout << "Exiting program..." << endl;
        Sleep(2000);
        exit(0);
    }
    else{
        cout << "Invalid role. Please enter 1 - 5 only." << endl;
        logInMenu();
    }
    return role;
}

void Library::setBookToNone(Book* book){
    book->available = true;
    book->borrowerName = "none";
    book->startDay = 0;
    book->startMonth = 0;
    book->startYear = 0;
    book->expirationDay = 0;
    book->expirationMonth = 0;
    book->expirationYear = 0;
    book->overdueCharge = 0;
}

void Library::payLateFee(){
    string index;
    int recharge = 0;
    userPtr->viewLateFee();
    if(userPtr->getLateFee() == 0){
        toMenu();
    }
    cout << "Enter the index of book you want to pay." << endl;
    cin >> index;
    userPtr->payLateFee(index);
    for(int i = 0; i < bookArrSize; i++){
        if(bookArr[i]->borrowerName == userPtr->getFullName() && bookArr[i]->index == index){
            bookArr[i]->startDay = currentDay;
            bookArr[i]->startMonth = currentMonth;
            bookArr[i]->startYear = currentYear;

            bookArr[i]->expirationDay = bookArr[i]->startDay;
            bookArr[i]->expirationMonth = bookArr[i]->startMonth + 1;
            bookArr[i]->expirationYear = bookArr[i]->startYear;
            bookArr[i]->overdueCharge = 0;
        }
    }
    toMenu();
}

void Library::loadPerson(){
    Person* p = nullptr;
    string line, bookIndex;
    int id, numBook;
    ifstream ifs ("PersonData.txt");
    if(!ifs.is_open()){
        cout << "Person Data File Not Found!" << endl;
        return;
    }
    personArrSize = 0;
    while(ifs >>line){
        if(line == "supervisor"){
            p = new Supervisor();
            numSupervisor++;
        }
        else if(line == "librarian"){
            p = new Librarian();
            numLibrarian++;
        }
        else if(line == "faculty"){
            p = new Faculty();
            numFaculty++;
        }
        else if(line == "student"){
            p = new Student();
            numStudent++;
        }
        ifs >> line;
        p->setUsername(line);
        ifs >> line;
        p->setPassword(line);
        ifs >> id;
        p->setID(id);
        ifs >> line;
        p->setFirstName(line);
        ifs >> line;
        p->setMiddleName(line);
        ifs >> line;
        p->setLastName(line);
        ifs >> line;
        p->setDateOfBirth(line);
        if(p->getRole() == "faculty" || p->getRole() == "student"){
            int lateFee = 0;
            ifs >> numBook;
            p->setNumBooksBorrowed(numBook);
            for(int i = 0; i < numBook; i++){
                ifs >> bookIndex;
                for(int j = 0; j < bookArrSize; j++){
                    if(bookArr[j]->index == bookIndex){
                        p->getBookBorrowed()[i] = bookArr[j];
                        if(bookArr[j]->overdueCharge > 0){
                            lateFee += bookArr[j]->overdueCharge;
                        }
                    }
                }
            }
            p->setLateFee(lateFee);
        }
        personArr[personArrSize] = p;
        personArrSize++;
        ifs.ignore();
    }
}
        

void Library::loadBooks(){
    string startDate;
    string expirationDate;
    ifstream ifs("BookData.txt");
    if(!ifs.is_open()){
        cout << "Book Data File Not Found!" << endl;
        return;
    }
    string line;
    bookArrSize = 0;
    string index;
    while(ifs >> index){
        int dateArr [3];
        ifs.ignore();
        Book* book1 = new Book;
        book1->index = index;
        getline(ifs, book1->title);  
        getline(ifs, book1->authorName);  
        getline(ifs, book1->publisher);  
        ifs >> book1->publishingYear;
        ifs.ignore();  
        getline(ifs, book1->borrowerName); 
        
        getline(ifs, startDate); 
        getline(ifs, expirationDate);  

        ifs >> book1->overdueCharge;
        if(book1->borrowerName != "none"){
            book1->available = false;
            int i = 0;
            int position = 0;
            while(startDate.find("-", 0) != string::npos){
                position = startDate.find("-", 0);
                dateArr[i] = stoi(startDate.substr(0, position));
                startDate = startDate.substr(position + 1);
                i++;
            }
            dateArr[2] = stoi(startDate);
            book1->startMonth = dateArr[0];
            book1->startDay = dateArr[1];
            book1->startYear = dateArr[2];

            i = 0;
            position = 0;
            while(expirationDate.find("-", 0) != string::npos){
                position = expirationDate.find("-", 0);
                dateArr[i] = stoi(expirationDate.substr(0, position));
                expirationDate = expirationDate.substr(position + 1);
                i++;
            }
            dateArr[2] = stoi(expirationDate);
            book1->expirationMonth = dateArr[0];
            book1->expirationDay = dateArr[1];
            book1->expirationYear = dateArr[2];

            int yearDiff = currentYear - book1->expirationYear;
            int monthDiff = currentMonth + (12 * yearDiff) - book1->expirationMonth;
            book1->overdueCharge = monthDiff * 50;
        }

        
        bookArr[bookArrSize] = book1;
        bookArrSize++;
    }
    ifs.close();
}

void Library::savePerson(){
    ofstream ofs ("PersonData.txt");
    for(int i = 0; i < personArrSize; i++){
        ofs << personArr[i]->getRole() << endl;
        ofs << personArr[i]->getUsername() << endl;
        ofs << personArr[i]->getPassword() << endl;
        ofs << personArr[i]->getID() << endl;
        ofs << personArr[i]->getFirstName() << endl;
        ofs << personArr[i]->getMiddleName() << endl;
        ofs << personArr[i]->getLastName() << endl;
        ofs << personArr[i]->getDateOfBirth() << endl;
        if(personArr[i]->getRole() == "student"|| personArr[i]->getRole() == "faculty"){
            ofs << personArr[i]->getNumBookBorrowed() << endl;
            for(int j = 0; j < personArr[i]->getNumBookBorrowed(); j++){
                ofs << personArr[i]->getBookBorrowed()[j]->index << endl;
            }
        }
        ofs << endl;
    }
    ofs.close();
}

void Library::saveBooks(){
    ofstream ofs("BookData.txt");
    for(int i = 0; i < bookArrSize; i++){
        ofs << bookArr[i]->index << endl;
        ofs << bookArr[i]->title << endl;
        ofs << bookArr[i]->authorName << endl;
        ofs << bookArr[i]->publisher << endl;
        ofs << bookArr[i]->publishingYear << endl;
        ofs << bookArr[i]->borrowerName << endl;
        ofs << to_string(bookArr[i]->startMonth) 
                + "-" + to_string(bookArr[i]->startDay)  
                 + "-" + to_string(bookArr[i]->startYear) << endl;
        ofs << to_string(bookArr[i]->expirationMonth) 
                + "-" + to_string(bookArr[i]->expirationDay) 
                + "-" + to_string(bookArr[i]->expirationYear) << endl;
        ofs << bookArr[i]->overdueCharge << endl;        
        ofs << endl;
    }
    ofs.close();
}
