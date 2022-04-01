/*
 * File:  main.c
 * Author:  João Paulo Borges Pereira
 * Description: A program that manipulates airports and flights in C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*Airport related constants*/
#define AIRPORT_ID_SIZE 4
#define MAX_AIRPORT_COUNTRY_SIZE 31
#define MAX_AIRPORT_CITY_SIZE 51
#define MAX_AMOUNT_AIRPORTS 41

/*Flight related constants*/
#define MAX_FLIGHT_ID_SIZE 7
#define MAX_AMOUNT_FLIGHTS 30001
#define MIN_CAPACITY 10
#define MAX_CAPACITY 100

/*Time related constants*/
#define TIME_SIZE 6
#define DATE_SIZE 11

/*------------------Class definitions & Global variables------------------*/

/*Definition of Date struct, representing days, months and years as integers.*/
typedef struct date {
    int day;
    int month;
    int year;
} Date;

/*Conversion of months to days, accumulative and non-accumulative.*/
int monthsToDays[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
int daysInAMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Date currentDate = {1, 1, 2022};

/*Definition of Time struct, representing minutes and hours as integers.*/
typedef struct time {
    int hour;
    int minute;
} Time;


/*Definition of Flight struct, containing all flight-relevant information.*/
typedef struct flight {
    char id[MAX_FLIGHT_ID_SIZE];
    char departureAirport[AIRPORT_ID_SIZE];
    char arrivalAirport[AIRPORT_ID_SIZE];
    Date departureDate;
    Time departureTime;
    Date arrivalDate;
    Time arrivalTime;
    Time duration;
    int maxPassengers;
} Flight;

/*Global array of flights, containing all flights in the system.*/
Flight flights[MAX_AMOUNT_FLIGHTS];
int flightIterator = 0;

/*Definition of Airport struct, containing all airport-relevant information.*/
typedef struct airport {
    char id[AIRPORT_ID_SIZE];
    char country[MAX_AIRPORT_COUNTRY_SIZE];
    char city[MAX_AIRPORT_CITY_SIZE];
    int nVoos;
    int arrivedFlights[MAX_AMOUNT_FLIGHTS];
    int arrivedIterator;
    int departedFlights[MAX_AMOUNT_FLIGHTS];
    int departedIterator;
} Airport;

/*Global array of airports, containing all airports in the system.*/
Airport airports[MAX_AMOUNT_AIRPORTS];
int airportIterator = 0;

/*--------------------------Auxiliary functions--------------------------*/

/*Converts both dates and times into minutes, and returns minute difference.*/
int compareDateAndTime(Date date1, Time time1, Date date2, Time time2) {
    int timestamp1 = date1.year*525600 + monthsToDays[date1.month-1]*1440
    + date1.day*1440 + time1.hour*60 + time1.minute;
    int timestamp2 = date2.year*525600 + monthsToDays[date2.month-1]*1440
    + date2.day*1440 + time2.hour*60 + time2.minute;

    return timestamp1-timestamp2;
}

/*Converts both dates to days, returning 1 if date is within bounds, 0 if not.*/
int invalidDateCheck(Date currentDate, Date date) {
    int currentDays = (currentDate.year-1)*365
    + monthsToDays[currentDate.month-1] + currentDate.day;
    int days = (date.year-1)*365 + monthsToDays[date.month-1] + date.day;

    int dayDiff = days- currentDays;
    return (dayDiff < 0) || (dayDiff > 365);
}

/*Converts Date structure into a formatted string and returns it.*/
void dateToString(Date date, char toStringDate[]) {
    char sday[3], smonth[3];

    if(date.day < 10) {
        sprintf(sday, "0%d", date.day);
    } else {
        sprintf(sday, "%d", date.day);
    }

    if(date.month < 10) {
        sprintf(smonth, "0%d", date.month);
    } else {
        sprintf(smonth, "%d", date.month);
    }

    sprintf(toStringDate, "%s-%s-%d", sday, smonth, date.year);
}

/*Converts Time structure into a formatted string and returns it.*/
void timeToString(Time time, char toStringTime[]) {
    char shour[3], sminute[3];

    if(time.hour < 10) sprintf(shour, "0%d", time.hour);
    else sprintf(shour, "%d", time.hour);
    
    if(time.minute < 10) sprintf(sminute, "0%d", time.minute);
    else sprintf(sminute, "%d", time.minute);

    sprintf(toStringTime, "%s:%s", shour, sminute);
}

/*Receives Flight with date, tIme and duration, and returns new date and time.*/
void addTime(Flight flight, Date *newDate, Time *newTime) {
    Date initDate = flight.departureDate;
    Time initTime = flight.departureTime, duration = flight.duration;
    int carryInH = 0, carryInD = 0, carryInM = 0, carryInY = 0;

    newTime->minute = initTime.minute + duration.minute;
    while(newTime->minute >= 60) {
        newTime->minute -= 60;
        carryInH++;
    }
    newTime->hour = initTime.hour + duration.hour + carryInH;
    while(newTime->hour >= 24) {
        newTime->hour -= 24;
        carryInD++;
    }
    newDate->day = initDate.day + carryInD;
    while(newDate->day > daysInAMonth[initDate.month+carryInM-1]) {
        newDate->day -= daysInAMonth[initDate.month+carryInM-1];
        carryInM++;
        if(initDate.month+carryInM > 12) {
            initDate.month = 0;
            carryInY++;
        }
    }
    newDate->month = initDate.month + carryInM;
    newDate->year = initDate.year + carryInY;
}

/*When called, sorts and prints the global airports array.*/
void sortAndPrintAirports() {
    int k, ptr, i;
    Airport key;

    for(k = 1; k < airportIterator; k++) {
        key = airports[k];
        ptr = k-1;
        while(ptr >=0 && strcmp(airports[ptr].id, key.id)>0) {
            airports[ptr+1] = airports[ptr];
            ptr--;
        }
        airports[ptr+1] = key;
    }

    for(i = 0; i < airportIterator; i++){
        printf("%s %s %s %d\n", airports[i].id, airports[i].city,
        airports[i].country, airports[i].nVoos);
    }
}

/*Receives a flight array, its size and a flag, and sorts the flight array 
according to the flag given.*/
void sortFlights(Flight flightList[], int size, int isArrival) {
    int k, ptr;
    Flight key;
    
    for(k = 1; k < size; k++) {
        key = flightList[k];
        ptr = k-1;
        if(isArrival) {
            while(ptr >=0 && compareDateAndTime(flightList[ptr].arrivalDate,
            flightList[ptr].arrivalTime, key.arrivalDate, key.arrivalTime)>0) {
                flightList[ptr+1] = flightList[ptr];
                ptr--;
            }
        } else {
            while(ptr >=0 && compareDateAndTime(flightList[ptr].departureDate,
            flightList[ptr].departureTime, key.departureDate,
            key.departureTime)>0) {
                flightList[ptr+1] = flightList[ptr];
                ptr--;
            }
        }
        flightList[ptr+1] = key;
    }
}

/*Receives a flight array, its size and a flag, sorting and printing the array
according to the flag given.*/
void sortAndPrintFlights(Flight flightList[], int size, int isArrival) {
    char sDate[DATE_SIZE], sTime[TIME_SIZE], tempID[AIRPORT_ID_SIZE];
    int i;
    Time newTime;
    Date newDate;

    sortFlights(flightList, size, isArrival);

    for(i = 0; i < size; i++){
        if(isArrival) {
            strcpy(tempID, flightList[i].departureAirport);
            newDate = flightList[i].arrivalDate;
            newTime = flightList[i].arrivalTime;
        } else {
            strcpy(tempID, flightList[i].arrivalAirport);
            newDate = flightList[i].departureDate;
            newTime = flightList[i].departureTime;
        }

        dateToString(newDate, sDate);
        timeToString(newTime, sTime);
        
        printf("%s %s %s %s\n", flightList[i].id, tempID, sDate, sTime);
    }
}

/*When called, prints all existing flights in creation order.*/
void printFlights() {
    char sDate[DATE_SIZE], sTime[TIME_SIZE];
    int i;

    for(i = 0; i < flightIterator; i++){
        dateToString(flights[i].departureDate, sDate);
        timeToString(flights[i].departureTime, sTime);
        printf("%s %s %s %s %s\n", flights[i].id, flights[i].departureAirport,
        flights[i].arrivalAirport, sDate, sTime);
    }
}

/*Receives all types necessary to initialize a flight, initializing it.*/
void initializeFlight(Flight *newFlight, char flightID[], char deAirportID[],
char arAirportID[], Date departureDate, Time departureTime, Time duration,
int capacity) {
    Date *arrivalDate = malloc(sizeof(Date));
    Time *arrivalTime = malloc(sizeof(Time));

    strcpy(newFlight->id, flightID);
    strcpy(newFlight->departureAirport, deAirportID);
    strcpy(newFlight->arrivalAirport, arAirportID);
    newFlight->departureDate = departureDate;
    newFlight->departureTime = departureTime;
    newFlight->duration = duration;
    newFlight->maxPassengers = capacity;
    addTime(*newFlight, arrivalDate, arrivalTime);
    newFlight->arrivalDate = *arrivalDate;
    newFlight->arrivalTime = *arrivalTime;
}

/*Receives a flight, and adds its index to their respective arrival and 
departure airports.*/
void addFlightsToAirports(Flight newFlight) {
    int i;

    for(i = 0; i < airportIterator; i++) {
        if(strcmp(newFlight.departureAirport, airports[i].id) == 0) {
            airports[i].departedFlights[airports[i].departedIterator++]
            = flightIterator;
        } 
        if (strcmp(newFlight.arrivalAirport, airports[i].id) == 0) {
            airports[i].arrivedFlights[airports[i].arrivedIterator++]
            = flightIterator;
        }
    }
}

/*-------------------------------Error checks-------------------------------*/

/*Receives an ID and returns a 1 if ID is invalid, 0 if not.*/
int invalidAirportID(char id[]) {
    char ch = id[0];
    int i = 0;
    while (ch != '\0') {
        if(!isupper(ch)) {
            printf("invalid airport ID\n");
            return 1;
        }
        ch = id[i++];
    }
    return 0;
}

/*Checks if there are too many airports, returns 1 if so, 0 if not.*/
int tooManyAirports() {
    if(airportIterator >= MAX_AMOUNT_AIRPORTS-1) {
        printf("too many airports\n");
        return 1;
    }
    return 0;
}

/*Receives an airport ID, and checks if an airport with it already exists,
returns 1 if so, 0 if not.*/
int duplicateAirport(char airportID[]) {
    int i;
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(airportID, airports[i].id) == 0) {
            printf("duplicate airport\n");
            return 1;
        }
    }
    return 0;
}

/*Receives a flight ID, and checks if it's invalid, returns 1 if so, 0 if not.*/
int invalidFlightCode(char flightID[]) {
    int idLen = strlen(flightID), i;
    if(idLen > 6 || idLen < 3) {
        printf("invalid flight code\n");
        return 1;
    } else {
        for(i = 0; i < idLen; i++) {
            if(i < 2 && !isupper(flightID[i])) {
                printf("invalid flight code\n");
                return 1;
            } else if(i >= 2 && (!isdigit(flightID[i]) || flightID[2] == '0')) {
                printf("invalid flight code\n");
                return 1;
            }
        }
        return 0;
    }
}

/*Receives a flight ID and a departure date, returning 1 (and prints the error)
if there already is a flight with the same ID and departure dare, returns 0 if not.*/
int flightAlreadyExists(char flightID[], Date departureDate) {
    Time temp = {0,0};
    int i;
    for(i = 0; i < flightIterator; i++) {
        if(strcmp(flightID, flights[i].id) == 0 &&
        compareDateAndTime(departureDate, temp, flights[i].departureDate,
        temp) == 0) {
            printf("flight already exists\n");
            return 1;
        }
    }
    return 0;
}

/*Receives two airport IDs involved in a flight, and returns 1 if one of the two
airport IDs is not found, also printing which one isn't, in the process, and 
return 0 if both are found.*/
int noSuchAirportID(char deAirportID[], char arAirportID[]) {
    int flag1 = 1, flag2 = 1, i;
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(deAirportID, airports[i].id) == 0) {
            flag1 = 0;
            airports[i].nVoos++;
        }
        if(strcmp(arAirportID, airports[i].id) == 0) flag2 = 0;
    }
    if(flag1) printf("%s: no such airport ID\n", deAirportID);
    if(flag2) printf("%s: no such airport ID\n", arAirportID);
    if(flag1 || flag2) return 1;
    return 0;
}

/*Receives a flight ID, departure and arrival airport IDs, and check for all the
previous erros and more, returning 1 if any of the errors are triggered, and 0
if otherwise.*/
int flightErrors(char flightID[], char deAirportID[], char arAirportID[],
Date departureDate, Time duration, int capacity) {
    if(invalidFlightCode(flightID)) return 1;
    if(flightAlreadyExists(flightID, departureDate)) return 1;
    if(noSuchAirportID(deAirportID, arAirportID)) return 1;
    if(flightIterator >= MAX_AMOUNT_FLIGHTS-1) {
        printf("too many flights\n");
        return 1;
    }
    if(invalidDateCheck(currentDate, departureDate)) {
        printf("invalid date\n");
        return 1;
    } 
    if(duration.hour*60 + duration.minute > 12*60) {
        printf("invalid duration\n");
        return 1;
    }
    if(MIN_CAPACITY > capacity || capacity > MAX_CAPACITY) {
        printf("invalid capacity\n");
        return 1;
    }
    return 0;
}

/*----------------------------Command functions----------------------------*/

void addAirport() {
    Airport newAirport;
    char airportID[AIRPORT_ID_SIZE], country[MAX_AIRPORT_COUNTRY_SIZE],
    city[MAX_AIRPORT_CITY_SIZE];
    /*Receives input from the user, namely the ID, country and city.*/
    scanf("%s", airportID);
    scanf("%s", country);
    getchar();
    scanf("%[^\n]%*c", city);
    /*Checks for the different errors, and stops if any trigger.*/
    if(invalidAirportID(airportID)) return;
    if(tooManyAirports()) return;
    if(duplicateAirport(airportID)) return;
    /*Initializes the new airport and adds it to the list of airports.*/
    strcpy(newAirport.id, airportID);
    strcpy(newAirport.country, country);
    strcpy(newAirport.city, city);
    newAirport.nVoos = 0;
    newAirport.arrivedIterator = 0;
    newAirport.departedIterator = 0;
    airports[airportIterator] = newAirport; 

    printf("airport %s\n", airports[airportIterator++].id);
}

void listAirports() {
    char airportID[AIRPORT_ID_SIZE], x, ch = ' ';
    int i, flag;
    /*Checks whether the user gave any more input after the command ID*/
    if((x = getchar()) == ' ') {
        while(ch == ' ' && ch != EOF) {
            scanf("%s", airportID);
            /*No such airport ID flag, used to detect if airport exists.*/
            flag = 1;
            for(i = 0; i < airportIterator; i++) {
                /*If it exists, prints the airport, breaks and changes flag.*/
                if(strcmp(airportID, airports[i].id) == 0) {
                    printf("%s %s %s %d\n", airports[i].id, airports[i].city,
                    airports[i].country, airports[i].nVoos);
                    flag = 0;
                    break;
                }
            }
            if(flag) {
                printf("%s: no such airport ID\n", airportID);
            }
            ch = getchar();
        }
    } else { 
        sortAndPrintAirports();
    }
}

void addFlight() {
    Flight newFlight;
    char flightID[MAX_FLIGHT_ID_SIZE], arAirportID[AIRPORT_ID_SIZE],
    deAirportID[AIRPORT_ID_SIZE], sDate[DATE_SIZE], sTime[TIME_SIZE], x;
    Date departureDate;
    Time departureTime, duration;
    int capacity;
    /*Checks whether the user gave any more input after the command ID*/
    if((x = getchar()) == ' ') {
        scanf("%s %s %s %d-%d-%d %d:%d %d:%d %d", flightID, deAirportID,
        arAirportID, &departureDate.day, &departureDate.month,
        &departureDate.year, &departureTime.hour, &departureTime.minute,
        &duration.hour, &duration.minute, &capacity);
        dateToString(departureDate, sDate);
        timeToString(departureTime, sTime);
        /*Checks for all possible errors, and stops if any trigger.*/
        if(flightErrors(flightID, deAirportID, arAirportID,
        departureDate, duration, capacity)) return;
        /*Then initializes the actual Flight, then adding it to the list*/
        initializeFlight(&newFlight, flightID, deAirportID, arAirportID,
        departureDate, departureTime, duration, capacity);
        addFlightsToAirports(newFlight);
        flights[flightIterator++] = newFlight;
    } else {
        printFlights();
    }
}

void departingFlightsFrom() {
    char airportID[AIRPORT_ID_SIZE];
    int flag, i, index;
    Flight deFlights[MAX_AMOUNT_FLIGHTS];
    scanf("%s", airportID);
    /*Checks for the no such airport ID error, while also obtaining the index
    of the relevant airport, if it exists.*/
    flag = 1; 
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(airportID, airports[i].id) == 0) {
            flag = 0;
            index = i;
        }
    }
    if(flag) {
        printf("%s: no such airport ID\n", airportID);
        return;
    }
    /*Iterates though the list of flights that depart from the airport, adding
    it to a new list, that will be sorted and printed.*/
    for(i = 0; i < airports[index].departedIterator; i++) {
        deFlights[i] = flights[airports[index].departedFlights[i]];
    }
    sortAndPrintFlights(deFlights, airports[index].departedIterator, 0);
}

void arrivingFlightsTo() {
    char airportID[AIRPORT_ID_SIZE];
    int flag, i, index;
    Flight arFlights[MAX_AMOUNT_FLIGHTS];
    scanf("%s", airportID);
    /*Checks for the no such airport ID error, while also obtaining the index
    of the relevant airport, if it exists.*/
    flag = 1; 
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(airportID, airports[i].id) == 0) {
            flag = 0;
            index = i;
        }
    }
    if(flag) {
        printf("%s: no such airport ID\n", airportID);
        return;
    }
    /*Iterates though the list of flights that arrive at the airport, adding
    it to a new list, that will be sorted and printed.*/
    for(i = 0; i < airports[index].arrivedIterator; i++) {
        arFlights[i] = flights[airports[index].arrivedFlights[i]];
    }
    sortAndPrintFlights(arFlights, airports[index].arrivedIterator, 1);
}

void advanceDate() {
    Date newDate;
    char sDate[DATE_SIZE];
    scanf("%d-%d-%d", &newDate.day, &newDate.month, &newDate.year);
    dateToString(newDate, sDate);
    
    /*Checks if the date is in the past or more than one year in the future.*/
    if(invalidDateCheck(currentDate, newDate)) {
        printf("invalid date\n");
        return;
    } 

    currentDate = newDate;
    printf("%s\n", sDate);
}

int main() {
    char command;
    /*Loops infinitely, asking for a new command once one has been finished.*/
    while (1) {
        command = getchar();
        switch (command) {
            case 'q': 
                exit(0);
                break;

            case 'a':
                addAirport();
                break;

            case 'l':
                listAirports();
                break;

            case 'v':
                addFlight();
                break;

            case 'p':
                departingFlightsFrom();
                break;

            case 'c':
                arrivingFlightsTo();
                break;

            case 't':
                advanceDate();
                break;
        }
    }
    return 0;
}