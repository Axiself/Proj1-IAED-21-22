#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*Command related constants*/
#define MAX_COMMAND_LENGTH 200

/*Airport related constants*/
#define AIRPORT_ID_SIZE 4
#define MAX_AIRPORT_COUNTRY_SIZE 31
#define MAX_AIRPORT_CITY_SIZE 51
#define MAX_AMOUNT_AIRPORTS 41

/*Flight related constants*/
#define MAX_FLIGHT_ID_SIZE 7
#define MAX_AMOUNT_FLIGHTS 30001

#define TIME_SIZE 6
#define DATE_SIZE 11

/*------------------Class definitions & Global variables------------------*/

typedef struct date {
    int day;
    int month;
    int year;
} Date;

int monthsToDays[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
int daysInAMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Date currentDate = {1, 1, 2022};

typedef struct time {
    int hour;
    int minute;
} Time;

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

Flight flights[MAX_AMOUNT_FLIGHTS];
int flightIterator = 0;

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

Airport airports[MAX_AMOUNT_AIRPORTS];
int airportIterator = 0;

/*--------------------------Auxiliary functions--------------------------*/

int compareDateAndTime(Date date1, Time time1, Date date2, Time time2) {
    int timestamp1 = date1.year*525600 + monthsToDays[date1.month-1]*1440 + date1.day*1440 + time1.hour*60 + time1.minute;
    int timestamp2 = date2.year*525600 + monthsToDays[date2.month-1]*1440 + date2.day*1440 + time2.hour*60 + time2.minute;

    return timestamp1-timestamp2;
}

int invalidDateCheck(Date currentDate, Date date) {
    int currentDays = (currentDate.year-1)*365 + monthsToDays[currentDate.month-1] + currentDate.day;
    int days = (date.year-1)*365 + monthsToDays[date.month-1] + date.day;

    int dayDiff = days- currentDays;
    return (dayDiff < 0) || (dayDiff > 365);
}

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

void timeToString(Time time, char toStringTime[]) {
    char shour[3], sminute[3];

    if(time.hour < 10) {
        sprintf(shour, "0%d", time.hour);
    } else {
        sprintf(shour, "%d", time.hour);
    }

    if(time.minute < 10) {
        sprintf(sminute, "0%d", time.minute);
    } else {
        sprintf(sminute, "%d", time.minute);
    }

    sprintf(toStringTime, "%s:%s", shour, sminute);
}

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
        printf("%s %s %s %d\r\n", airports[i].id, airports[i].city, airports[i].country, airports[i].nVoos);
    }
}

void sortAndPrintFlights(Flight flightList[], int size, int isArrival) {
    char sDate[DATE_SIZE], sTime[TIME_SIZE], tempID[AIRPORT_ID_SIZE];
    int k, ptr, i;
    Flight key;
    Time newTime;
    Date newDate;

    for(k = 1; k < size; k++) {
        key = flightList[k];
        ptr = k-1;
        if(isArrival) {
            while(ptr >=0 && compareDateAndTime(flightList[ptr].arrivalDate, flightList[ptr].arrivalTime, key.arrivalDate, key.arrivalTime)>0) {
                flightList[ptr+1] = flightList[ptr];
                ptr--;
            }
        } else {
            while(ptr >=0 && compareDateAndTime(flightList[ptr].departureDate, flightList[ptr].departureTime, key.departureDate, key.departureTime)>0) {
                flightList[ptr+1] = flightList[ptr];
                ptr--;
            }
        }
        flightList[ptr+1] = key;
    }

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
        
        printf("%s %s %s %s\r\n", flightList[i].id, tempID, sDate, sTime);
    }
}

void printFlights() {
    char sDate[DATE_SIZE], sTime[TIME_SIZE];
    int i;

    for(i = 0; i < flightIterator; i++){
        dateToString(flights[i].departureDate, sDate);
        timeToString(flights[i].departureTime, sTime);
        printf("%s %s %s %s %s\r\n", flights[i].id, flights[i].departureAirport, flights[i].arrivalAirport, sDate, sTime);
    }
}


/*----------------------------Command functions----------------------------*/

void addAirport() {
    Airport newAirport;
    int i;
    char stringArray[MAX_AMOUNT_AIRPORTS][MAX_AIRPORT_CITY_SIZE], ch;

    scanf("%s", stringArray[0]);
    scanf("%s", stringArray[1]);
    getchar();
    scanf("%[^\r\n]%*c", stringArray[2]);

    /* invalid airport ID check */
    ch = stringArray[0][0];
    i = 0;
    while (ch != '\0') {
        if(!isupper(ch)) {
            printf("invalid airport ID\r\n");
            return;
        }
        ch = stringArray[0][i++];
    }


    /* too many airports check */
    if(airportIterator >= 40 ) {
        printf("too many airports\r\n");
        return;
    }

    /* duplicate airport check */
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(stringArray[0], airports[i].id) == 0) {
            printf("duplicate airport\r\n");
            return;
        }
    }


    strcpy(newAirport.id, stringArray[0]);
    strcpy(newAirport.country, stringArray[1]);
    strcpy(newAirport.city, stringArray[2]);
    
    newAirport.nVoos = 0;
    newAirport.arrivedIterator = 0;
    newAirport.departedIterator = 0;
    airports[airportIterator] = newAirport; 

    printf("airport %s\r\n", airports[airportIterator++].id);
}

void listAirports() {
    char airportID[AIRPORT_ID_SIZE], x, ch = ' ';
    int i, flag;
    if((x = getchar()) == ' ') {
        while(ch == ' ' && ch != EOF) {
            scanf("%s", airportID);
            /* no such airport ID flag */
            flag = 1;
            for(i = 0; i < airportIterator; i++) {
                if(strcmp(airportID, airports[i].id) == 0) {
                    printf("%s %s %s %d\r\n", airports[i].id, airports[i].city, airports[i].country, airports[i].nVoos);
                    flag = 0;
                    break;
                }
            }
            if(flag) {
                printf("%s: no such airport ID\r\n", airportID);
            }
            ch = getchar();
        }
    } else { 
        sortAndPrintAirports();
    }
}

void addFlight() {
    Flight newFlight;
    char flightID[MAX_FLIGHT_ID_SIZE], arAirportID[AIRPORT_ID_SIZE], deAirportID[AIRPORT_ID_SIZE];
    char sDate[DATE_SIZE], sTime[TIME_SIZE], x;
    Date departureDate, *arrivalDate = malloc(sizeof(Date));
    Time departureTime, duration, *arrivalTime = malloc(sizeof(Time)), temp = {0,0};
    int capacity, i, idLen, flag1, flag2;

    if((x = getchar()) == ' ') {
        scanf("%s %s %s %d-%d-%d %d:%d %d:%d %d", flightID, deAirportID, arAirportID, 
            &departureDate.day, &departureDate.month, &departureDate.year, &departureTime.hour, 
            &departureTime.minute, &duration.hour, &duration.minute, &capacity);

        dateToString(departureDate, sDate);
        timeToString(departureTime, sTime);

        /* invalid flight code check */
        idLen = strlen(flightID);
        if(idLen > 6 || idLen < 3) {
            printf("invalid flight code\r\n");
            return;
        } else {
            for(i = 0; i < idLen; i++) {
                if(i < 2 && !isupper((int)flightID[i])) {
                    printf("invalid flight code\r\n");
                    return;
                } else if(i >= 2 && (!isdigit((int)flightID[i]) || (flightID[i] == '0' && i == 2)) ) {
                    printf("invalid flight code\r\n");
                    return;
                }
            }
        }

        /* flight already exists check */
        for(i = 0; i < flightIterator; i++) {
            if(strcmp(flightID, flights[i].id) == 0 && compareDateAndTime(departureDate, temp, 
            flights[i].departureDate, temp) == 0) {
                printf("flight already exists\r\n");
                return;
            }
        }

        /* no such airport ID check */
        flag1 = 1; 
        flag2 = 1;
        for(i = 0; i < airportIterator; i++) {
            if(strcmp(deAirportID, airports[i].id) == 0) {
                flag1 = 0;
                airports[i].nVoos++;
            }
            if(strcmp(arAirportID, airports[i].id) == 0) flag2 = 0;
        }
        if(flag1) printf("%s: no such airport ID\r\n", deAirportID);
        if(flag2) printf("%s: no such airport ID\r\n", arAirportID);
        if(flag1 || flag2) return;

        /* too many flights check */
        if(flightIterator > 30000) {
            printf("too many flights\r\n");
            return;
        }

        /* invalid date check */
        if(invalidDateCheck(currentDate, departureDate)) {
            printf("invalid date\r\n");
            return;
        } 

        /* invalid duration check */
        if(duration.hour*60 + duration.minute > 12*60) {
            printf("invalid duration\r\n");
            return;
        }

        /* invalid capacity check */
        if(10 > capacity || capacity > 100) {
            printf("invalid capacity\r\n");
            return;
        }

        strcpy(newFlight.id, flightID);
        strcpy(newFlight.departureAirport, deAirportID);
        strcpy(newFlight.arrivalAirport, arAirportID);
        newFlight.departureDate = departureDate;
        newFlight.departureTime = departureTime;
        newFlight.duration = duration;
        newFlight.maxPassengers = capacity;

        addTime(newFlight, arrivalDate, arrivalTime);
        newFlight.arrivalDate = *arrivalDate;
        newFlight.arrivalTime = *arrivalTime;

        for(i = 0; i < airportIterator; i++) {
            if(strcmp(newFlight.departureAirport, airports[i].id) == 0) {
                airports[i].departedFlights[airports[i].departedIterator++] = flightIterator;
            } 
            if (strcmp(newFlight.arrivalAirport, airports[i].id) == 0) {
                airports[i].arrivedFlights[airports[i].arrivedIterator++] = flightIterator;
            }
        }

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

    /* no such airport ID check */
    flag = 1; 
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(airportID, airports[i].id) == 0) {
            flag = 0;
            index = i;
        }
    }
    if(flag) {
        printf("%s: no such airport ID\r\n", airportID);
        return;
    }

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

    /* no such airport ID check */
    flag = 1; 
    for(i = 0; i < airportIterator; i++) {
        if(strcmp(airportID, airports[i].id) == 0) {
            flag = 0;
            index = i;
        }
    }
    if(flag) {
        printf("%s: no such airport ID\r\n", airportID);
        return;
    }

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
    
    /* invalid date check */
    if(invalidDateCheck(currentDate, newDate)) {
        printf("invalid date\r\n");
        return;
    } 

    currentDate = newDate;
    printf("%s\r\n", sDate);
}

int main() {
    char command;
    while (1) {
        scanf("%c", &command);
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