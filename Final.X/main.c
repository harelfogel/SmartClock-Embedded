/*
 * File:   Final Project - SmartWatch
 * Author: Harel Fogel
 *
 * Created on July 1, 2022, 16:17 AM
 */
 

#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

/*
 * Prototypes-Functions 
 */
void periodHoursController();
void month_cycle();
int GetPotentiometerValues();
void trisConfiguration();
void convertUnsignedAtoi(unsigned char number, char* buffer);
char *getDigitalWatch();
char *getAlarm();
char *getDate();
void TIMER_Initialize(void);
void setInitialTime();
void setPreviousTime();
void stateInitialize();
void menuInitialize();
void deleteDate();
void displayDate();
void alarmVisibility();
void deletePeriod();
void displayPeriod();
void displayDigitalWatch();
void displayDigitalNumbers();
void deleteDigitalNumbers();
void displayAnalogWatch();
void displayAnalogWatch();
void deleteAnalogHands();
void displayIntervalMenu();
void monthController();
void dateController();
void hoursController();
void minutesController();
void modifyTime(void);
void watchInMenu();
void displayMenu();
void displayMenuOptions(void);
void deleteMenuSelector(void);
void displayAnalogDigitalMenu();
void displayDisplayMenuOptions();
void deleteDisplayDisplayMenu();
void displayAlarm();
void displayAlarmOptions();
void deleteAlarmOption();
void displayDigitalAlarm();
void __attribute__((__interrupt__)) _T1Interrupt(void);



/*
 * Data structures
 */
 

typedef struct Watch {
    unsigned char month, day, hour, minute, second, period;
} Watch;

typedef struct State {
    unsigned char menu, typeOfWatch, alarm, counter;
} State;

typedef struct Menu {
    unsigned char selected, option;
} Menu;


/*
 * global variables
 */
Watch watch, previousWatch, alarm;
Watch previousWatch;
State state;
Menu menu, previousMenu, optionMenu, previousOptionMenu;


unsigned char center = 47;
unsigned char analogCoordinates[60][2] = {
    {47, 7},
    {51, 7},
    {55, 8},
    {59, 9},
    {63, 10},
    {67, 12},
    {71, 15},
    {74, 17},
    {77, 20},
    {79, 23},
    {82, 27},
    {84, 31},
    {85, 35},
    {86, 39},
    {87, 43},
    {87, 47},
    {87, 51},
    {86, 55},
    {85, 59},
    {84, 63},
    {82, 67},
    {79, 71},
    {77, 74},
    {74, 77},
    {71, 79},
    {67, 82},
    {63, 84},
    {59, 85},
    {55, 86},
    {51, 87},
    {47, 87},
    {43, 87},
    {39, 86},
    {35, 85},
    {31, 84},
    {27, 82},
    {23, 79},
    {20, 77},
    {17, 74},
    {15, 71},
    {12, 67},
    {10, 63},
    {9, 59},
    {8, 55},
    {7, 51},
    {7, 47},
    {7, 43},
    {8, 39},
    {9, 35},
    {10, 31},
    {12, 27},
    {15, 23},
    {17, 20},
    {20, 17},
    {23, 15},
    {27, 12},
    {31, 10},
    {35, 9},
    {39, 8},
    {43, 7}};
unsigned char months[12] = {31, 28, 30, 30, 31, 30, 31, 30, 31, 30, 31};
char clock[9];
char date[6];
char alarmChar[9];
unsigned char tmpHour, tmpMinute, tmpDay, tmpMonth;
int alarmFlag = 0, alarmIndex = 0, alarmStopper = 0;


/*
 * name: periodHoursController
 * params:none
 * desc: Modifing the hours acording to pm mode.
 */
void periodHoursController(){
    if (watch.period == 1) 
    {
        watch.hour += 12;
        if(watch.hour == 0)
        {
            watch.hour=01;
        }
        if(watch.hour >= 24)
        {
          watch.hour = 0;
        }
    }
    
}

/*
 * name: month_cycle
 * params:none
 * desc: Months configuration for the potentiometer values 
 */
void month_cycle(){
    if(watch.month<=00){
        watch.month=01;
    }
    if(watch.month>=13){
        watch.month=12;
    }
}

/*
 * name: GetPotentiometerValues
 * params:none
 * desc: Gets A/D values from the potentiometer
 */
int GetPotentiometerValues()
{
    AD1CON1bits.SAMP = 1 ;  // A to D          
    int i = 0;
    for (i = 0 ; i < 1000 ; i++) ; 
    AD1CON1bits.SAMP = 0 ;   
    for (i = 0 ; i < 1000 ; i++) ; 
    while (!AD1CON1bits.DONE) ;  
    return ADC1BUF0 ;
}


/*
 * name: trisConfiguration
 * params:void
 * desc: Initialize the Tris ports and registers
 */
void trisConfiguration(){
    TRISBbits.TRISB12 = 1;
    ANSB |= (1 << 12);
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.MODE12 = 0;
    AD1CON1bits.ADON = 1;
    AD1CON1bits.FORM = 0;
    AD1CON2 = 0;
    AD1CON3bits.ADCS = 0xFF;
    AD1CON3bits.SAMC = 0x10;
    AD1CHS = 8;
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;
}



/*
 * name: convertUnsignedAtoi
 * params:unsigned char number, char* buffer
 * desc: Convert unsigned char to char*
 */
void convertUnsignedAtoi(unsigned char number, char* buffer) {

    buffer[0] = (number / 10) + 48;
    buffer[1] = (number % 10) + 48;
    buffer[2] = '\0';
}

/*
 * name: getDigitalWatch
 * params:none
 * desc: Prepare the digital watch to display *
 */
char *getDigitalWatch() {
    convertUnsignedAtoi(watch.hour, clock);
    clock[2] = ':';
    convertUnsignedAtoi(watch.minute, clock + 3);
    clock[5] = ':';
    convertUnsignedAtoi(watch.second, clock + 6);

    return clock;
}

/*
 * name: getAlarm
 * params:none
 * desc: Getting alarm hour and minute *
 */

char *getAlarm() {
    convertUnsignedAtoi(alarm.hour, alarmChar);
    alarmChar[2] = ':';
    convertUnsignedAtoi(alarm.minute, alarmChar + 3);
    alarmChar[5] = ':';
    convertUnsignedAtoi(alarm.second, alarmChar + 6);

    return alarmChar;
}


/*
 * name: getDate
 * params:none
 * desc: Getting the date configuration *
 */

char *getDate() {
    convertUnsignedAtoi(watch.day, date);
    date[2] = '/';
    convertUnsignedAtoi(watch.month, date + 3);

    return date;
}



/*
 * name: TIMER_Initialize
 * params:none
 * desc: Timer initialization for the alarm mode.
 */
void TIMER_Initialize(void) {
    T1CONbits.TON = 1; /* Start Timer */
    T1CONbits.TCKPS = 0b11; /* Select 1:256 Prescaler */
    PR1 = 16595; /* Count limit */
    IFS0bits.T1IF = 0; /* Flag interrupt */
    IEC0bits.T1IE = 1; /* Enable interrupt */
}

/*
 * name: setInitialTime
 * params:none
 * desc: Start point of the time.
 */
void setInitialTime() {
    watch.day = 8;
    watch.month = 8;
    watch.hour = 10;
    watch.minute = 59;
    watch.second = 56;
    watch.period = 1;
    alarm.hour = 11;
    alarm.minute = 0;
    alarm.period = 1;
}

/*
 * name: setPreviousTime
 * params:none
 * desc: Start point of the previous time.
 */
void setPreviousTime() {
    watch.day = 8;
    watch.month = 8;
    watch.hour = 10;
    watch.minute = 59;
    watch.second = 56;
    watch.period = 1;
}

/*
 * name: stateInitialize
 * params:none
 * desc: Starting state of the watch.
 */
void stateInitialize() {
    state.typeOfWatch = 1;
    state.menu = 0;
    state.alarm = 0;
}

/*
 * name: menuInitialize
 * params:none
 * desc: Start state of the menu.
 */
void menuInitialize() {
    menu.selected = 0;
    menu.option = 0;
    previousMenu.selected = 0;
    optionMenu.selected = 0;
    previousOptionMenu.selected = 0;
}

/*
 * name: deleteDate
 * params:none
 * desc: Overiding date in black and delete it
 */
void deleteDate() {
    oledC_DrawRectangle(66, 88, 83, 95, OLEDC_COLOR_BLACK);
}
/*
 * name: displayDate
 * params:none
 * desc: Drawing date in white.
 */
void displayDate() {
    deleteDate();
    oledC_DrawString(66, 88, 1, 1, getDate(), OLEDC_COLOR_WHITE);
}

/*
 * name: alarmVisibility
 * params:none
 * desc: Showing the alarm mode.
 */
void alarmVisibility() {
    if (state.alarm) {
        oledC_DrawRing(6, 6, 3, 2, OLEDC_COLOR_RED);
    } else {
        oledC_DrawRing(6, 6, 2, 1, OLEDC_COLOR_BLACK);
    }
}

/*
 * name: deletePeriod
 * params:none
 * desc: Removing the am/pm mode
 */
void deletePeriod() {
    uint8_t Am[3] = {'A', 'M', '\0'};
    uint8_t Pm[3] = {'P', 'M', '\0'};

    if (watch.period == 1) {         // if its pm mode
        oledC_DrawString(1, 88, 1, 1, Pm, OLEDC_COLOR_BLACK);
    } else if (watch.period == 0) {  // if its am mode
        oledC_DrawString(1, 88, 1, 1, Am, OLEDC_COLOR_BLACK);
    }
}

/*
 * name: displayPeriod
 * params:none
 * desc: Drawing  the am/pm mode
 */
void displayPeriod() {
    deletePeriod();
    uint8_t Am[3] = {'A', 'M', '\0'};
    uint8_t Pm[3] = {'P', 'M', '\0'};

    if (watch.period == 1) {
        oledC_DrawString(1, 88, 1, 1, Pm, OLEDC_COLOR_WHITE);
    } else if (watch.period == 0) {
        oledC_DrawString(1, 88, 1, 1, Am, OLEDC_COLOR_WHITE);
    }
}

/*
 * name: displayDigitalWatch
 * params:none
 * desc: Drawing the digital watch mode.
 */
void displayDigitalWatch() {
    oledC_clearScreen();
    alarmVisibility();
    displayPeriod();
    displayDate();
    oledC_DrawString(5, 40, 2, 2, getDigitalWatch(), OLEDC_COLOR_WHITE);
}


/*
 * name: displayDigitalNumbers
 * params:none
 * desc: Drawing digital numbers.
 */
void displayDigitalNumbers() {
    oledC_DrawString(5, 40, 2, 2, getDigitalWatch(), OLEDC_COLOR_WHITE);
}


/*
 * name: deleteDigitalNumbers
 * params:none
 * desc: Removing digital numbers by black screen mode.
 */
void deleteDigitalNumbers() {
    //seconds mode
    if (watch.second % 10 == 0)
        oledC_DrawRectangle(71, 40, 93, 56, OLEDC_COLOR_BLACK);
    else
        oledC_DrawRectangle(82, 40, 93, 56, OLEDC_COLOR_BLACK);

    previousWatch.second = watch.second;

    //minutes mode
    if (previousWatch.minute != watch.minute) {
        if (watch.minute % 10 == 0)
            oledC_DrawRectangle(38, 40, 60, 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(49, 40, 60, 56, OLEDC_COLOR_BLACK);

        previousWatch.minute = watch.minute;
    }

    //hours mode
    if (previousWatch.hour != watch.hour) {
        if (watch.hour % 10 == 0)
            oledC_DrawRectangle(5, 40, 27, 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(16, 40, 27, 56, OLEDC_COLOR_BLACK);

        previousWatch.hour = watch.hour;
    }
}

/*
 * name: displayAnalogWatch
 * params:none
 * desc: Drawing the analog watch
 */
void displayAnalogWatch() {
    uint8_t i;
    unsigned char xPoint, yPoint, xTemp, yTemp;

    oledC_clearScreen();
    alarmVisibility();
    displayPeriod();
    displayDate();

    for (i = 0; i < 60; i++) {
        if (i % 5 == 0) // for each 5 minutes in the watch
        {
            xTemp = analogCoordinates[i][0];
            yTemp = analogCoordinates[i][1];
            xPoint = (center + 9 * (xTemp - center) / 10);
            yPoint = (center + 9 * (yTemp - center) / 10);
            if (i % 15 != 0) // Normal minute
            {
                oledC_DrawLine(xPoint, yPoint, xTemp, yTemp, 1, OLEDC_COLOR_WHITE);
            } else     //Big minutes(3,6,9,12)
            {
                oledC_DrawLine(xPoint, yPoint, xTemp, yTemp, 2, OLEDC_COLOR_BLUE);
            }
        }
    }
}

/*
 * name: displayAnalogHands
 * params:none
 * desc: Drawing the analog hands
 */
void displayAnalogHands() {
    unsigned char coordX, coordY;

    // seconds
    coordX = center + 3 * (analogCoordinates[watch.second][0] - center) / 4;
    coordY = center + 3 * (analogCoordinates[watch.second][1] - center) / 4;
    oledC_DrawLine(center, center, coordX, coordY, 1, OLEDC_COLOR_RED);

    //minutes
    coordX = center + 3 * (analogCoordinates[watch.minute][0] - center) / 4;
    coordY = center + 3 * (analogCoordinates[watch.minute][1] - center) / 4;
    oledC_DrawLine(center, center, coordX, coordY, 2, OLEDC_COLOR_WHITE);

    //hours
    coordX = center + 2 * (analogCoordinates[(watch.hour * 5) % 60][0] - center) / 5;
    coordY = center + 2 * (analogCoordinates[(watch.hour * 5) % 60][1] - center) / 5;
    oledC_DrawLine(center, center, coordX, coordY, 3, OLEDC_COLOR_WHITE);
}

/*
 * name: deleteAnalogHands
 * params:none
 * desc: Removing the analog hands
 */
void deleteAnalogHands() {
    unsigned char coordX, coordY;

    // seconds
    coordX = (center + 3 * (analogCoordinates[previousWatch.second][0] - center) / 4);
    coordY = (center + 3 * (analogCoordinates[previousWatch.second][1] - center) / 4);
    oledC_DrawLine(center, center, coordX, coordY, 1, OLEDC_COLOR_BLACK);
    previousWatch.second = watch.second;

    // minutes
    if (previousWatch.minute != watch.minute) {
        coordX = (center + 3 * (analogCoordinates[previousWatch.minute][0] - center) / 4);
        coordY = (center + 3 * (analogCoordinates[previousWatch.minute][1] - center) / 4);
        oledC_DrawLine(center, center, coordX, coordY, 2, OLEDC_COLOR_BLACK);
        previousWatch.minute = watch.minute;
    }

    //hours
    if (previousWatch.hour != watch.hour) {
        coordX = (center + 2 * (analogCoordinates[(previousWatch.hour * 5) % 60][0] - center) / 5);
        coordY = (center + 2 * (analogCoordinates[(previousWatch.hour * 5) % 60][1] - center) / 5);
        oledC_DrawLine(center, center, coordX, coordY, 3, OLEDC_COLOR_BLACK);
        previousWatch.hour = watch.hour;
    }
}

/*
 * name: displayIntervalMenu
 * params:none
 * desc: Drawing the 12/24 interval mode.
 */
void displayIntervalMenu() {
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    oledC_DrawString(15, 20, 1, 1, "12H Mode", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "24H Mode", OLEDC_COLOR_WHITE);

    oledC_DrawString(3, (optionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}

/*
 * name: monthController
 * params:none
 * desc: Month configuration.
 */
void monthController() {
    if (watch.month == 13) {
        watch.month = 01;
    }
}
/*
 * name: dateController
 * params:none
 * desc: Date configuration.
 */
void dateController() {
    if (watch.day > months[watch.month - 1]) {
        watch.day = 01;
        watch.month++;
        monthController();
    }
    
    if(watch.day==00){
        watch.day=01;
        monthController();
    }
}

/*
 * name: hoursController
 * params:none
 * desc: Hours configuration.
 */
void hoursController() {
    if (watch.period == 2 || alarm.period == 2) {
        if (watch.hour == 24) {
            watch.hour = 0;
            watch.day++;
            dateController();
        }
        if (alarm.hour == 24) {
            alarm.hour = 0;
            alarm.day++;
        }

    } else {
        if (watch.hour == 12) {
            if (watch.period == 1) {
                watch.period = 0;
            } else if (watch.period == 0) {
                watch.period = 1;
            }

            if (previousWatch.period == 1) {
                watch.day++;
                dateController();
            }
        } else if (watch.hour >= 13) {
            watch.hour -= 12;
        } else if (alarm.hour == 12) {
            if (alarm.period == 1) {
                alarm.period = 0;
            } else if (alarm.period == 0) {
                alarm.period = 1;
            }
        } else if (alarm.hour == 13) {
            alarm.hour = 1;
        }
    }
}
/*
 * name: minutesController
 * params:none
 * desc: Minutes configuration.
 */
void minutesController() {
    if (watch.minute == 60) {
        watch.minute = 0;
        watch.hour++;
        hoursController();
    }
    if (alarm.minute == 60) {
        alarm.minute = 0;
        alarm.hour++;
        hoursController();
    }
}

/*
 * name: modifyTime
 * params:none
 * desc: Changing time acording to am/pm or 24 interval mode.
 */
void modifyTime(void) {
    watch.second++;

    if (watch.second == 60) {
        watch.second = 0;
        watch.minute++;

        minutesController();
    }
}

/*
 * name: watchInMenu
 * params:none
 * desc: Drawing the watch.
 */
void watchInMenu() {
    if (previousWatch.hour != watch.hour) {

        if (watch.hour % 10 == 0)
            oledC_DrawRectangle(48, 6, 60, 14, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(56, 6, 60, 14, OLEDC_COLOR_BLACK);

        previousWatch.hour = watch.hour;
    }

    if (previousWatch.minute != watch.minute) {
        if (watch.minute % 10 == 0)
            oledC_DrawRectangle(66, 6, 78, 14, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(72, 6, 78, 14, OLEDC_COLOR_BLACK);

        previousWatch.hour = watch.hour;

        previousWatch.minute = watch.minute;
    }

    if (watch.second % 10 == 0)
        oledC_DrawRectangle(84, 6, 96, 14, OLEDC_COLOR_BLACK);
    else
        oledC_DrawRectangle(90, 6, 96, 14, OLEDC_COLOR_BLACK);

    previousWatch.second = watch.second;
}

/*
 * name: displayMenu
 * params:none
 * desc: Drawing the menu.
 */
void displayMenu() {
    menu.option = 0;
    oledC_clearScreen();
    alarmVisibility();

    oledC_DrawString(48, 6, 1, 1, getDigitalWatch(), OLEDC_COLOR_WHITE);

    oledC_DrawString(15, 20, 1, 1, "Display Mode", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "12H/24H Mode", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 40, 1, 1, "Set Time", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 50, 1, 1, "Set Date", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 60, 1, 1, "Alarm", OLEDC_COLOR_WHITE);

    oledC_DrawString(3, (menu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}

/*
 * name: displayMenuOptions
 * params:none
 * desc: Drawing the option of the menu.
 */
void displayMenuOptions(void) {
    oledC_DrawString(48, 6, 1, 1, getDigitalWatch(), OLEDC_COLOR_WHITE);
    oledC_DrawString(3, (menu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}

/*
 * name: deleteMenuSelector
 * params:none
 * desc: Removing the option of the menu.
 */
void deleteMenuSelector(void) {
    watchInMenu();

    if (menu.selected != previousMenu.selected) {
        oledC_DrawString(3, (previousMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_BLACK);
        previousMenu.selected = menu.selected;
    }
}

/*
 * name: displayAnalogDigitalMenu
 * params:none
 * desc:  Change the mode between analog to digital
 */
void displayAnalogDigitalMenu() {
    oledC_clearScreen();
    alarmVisibility();
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    oledC_DrawString(15, 20, 1, 1, "Analog mode", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "Digital mode", OLEDC_COLOR_WHITE);

    oledC_DrawString(3, (optionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}

/*
 * name: displayDisplayMenuOptions
 * params:none
 * desc:  Change the mode cursor.
 */
void displayDisplayMenuOptions() {
    oledC_DrawString(48, 6, 1, 1, getDigitalWatch(), OLEDC_COLOR_WHITE);
    oledC_DrawString(3, (optionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}

/*
 * name: deleteDisplayDisplayMenu
 * params:none
 * desc:  Remove the mode cursor.
 */
void deleteDisplayDisplayMenu() {
    watchInMenu();
    if (optionMenu.selected != previousOptionMenu.selected) {
        oledC_DrawString(3, (previousOptionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_BLACK);
        previousOptionMenu.selected = optionMenu.selected;
    }
}

/*
 * name: displayAlarm
 * params:none
 * desc:  Draw the alarm options.
 */
void displayAlarm() {
    alarmVisibility();
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    oledC_DrawString(15, 20, 1, 1, "Alarm ON", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "Alarm OFF", OLEDC_COLOR_WHITE);

    oledC_DrawString(3, (optionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}
/*
 * name: displayAlarmOptions
 * params:none
 * desc:  Draw the alarm option's cursor.
 */
void displayAlarmOptions() {
    oledC_clearScreen();
    oledC_DrawString(48, 6, 1, 1, getAlarm(), OLEDC_COLOR_WHITE);
    oledC_DrawString(3, (optionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_GOLD);
}
/*
 * name: deleteAlarmOption
 * params:none
 * desc:  Remove the alarm option's cursor.
 */
void deleteAlarmOption() {
    if (optionMenu.selected != previousOptionMenu.selected) {
        oledC_DrawString(3, (previousOptionMenu.selected + 2) * 10, 1, 1, ">>", OLEDC_COLOR_BLACK);
        previousOptionMenu.selected = optionMenu.selected;
    }
}

/*
 * name: displayDigitalAlarm
 * params:none
 * desc:  Draw the alarm option's cursor.
 */
void displayDigitalAlarm() {
    oledC_clearScreen();
    oledC_DrawRectangle(5, 40, 95, 95, OLEDC_COLOR_BLACK);
    alarmVisibility();
    displayPeriod();
    displayDate();
    oledC_DrawString(5, 40, 2, 2, getAlarm(), OLEDC_COLOR_WHITE);
} 

/*
 * name: __attribute__((__interrupt__)) _T1Interrupt(void)
 * params: none
 * desc:  Draw the alarm option's cursor.
 */
void __attribute__((__interrupt__)) _T1Interrupt(void) {
    
    if ((watch.hour == alarm.hour && watch.minute == alarm.minute && state.alarm) || (watch.hour == alarm.hour - 12 && watch.period == 1)) {
        if(watch.second < 21 && alarmStopper == 0)
        {
            if (alarmIndex == 0) {
               oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0);
               alarmIndex = 1;
            } 
            else if (alarmIndex == 1) {
                oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_INVERSE, NULL, 0);
                alarmIndex = 0;
            }
        }
        else 
        {
            oledC_sendCommand(OLEDC_CMD_SET_DISPLAY_MODE_ON, NULL, 0);
        }
    }
    if (PORTAbits.RA11 == 0) {
        state.counter++;
    } else {
        state.counter = 0;
    }
    
    if (!state.menu && alarmFlag == 0) {
        // Analog watch
        if (!state.typeOfWatch) {
            // hand refresh
            deleteAnalogHands();
            displayAnalogHands();
        }
        // Digital watch
        if (state.typeOfWatch) {
            //digital watch refresh
            deleteDigitalNumbers();
            displayDigitalNumbers();
        }
        if (watch.period != previousWatch.period) {
            displayPeriod();
            previousWatch.period = watch.period;
        }
        if (watch.day != previousWatch.day) {
            displayDate();
            previousWatch.day = watch.day;
            previousWatch.month = watch.month;
        }
    } else if (state.menu) {
        if (menu.option) {
            deleteDisplayDisplayMenu();
            displayDisplayMenuOptions();
        } else {
            //Clean the menu previous status
            deleteMenuSelector();

            //Draw the menu new status
            displayMenuOptions();
        }
    }

    modifyTime();
    IFS0bits.T1IF = 0;
}

int main() {

    //Initialize the system
    SYSTEM_Initialize();
    TIMER_Initialize();
    //Oled initialize
    oledC_setBackground(OLEDC_COLOR_BLACK);
    oledC_clearScreen();
    setInitialTime();
    setPreviousTime();
    stateInitialize();
    menuInitialize();
    trisConfiguration();
    if (!state.menu) {
        if (!state.typeOfWatch) {
            displayAnalogWatch();
        } else if (state.typeOfWatch) {
            displayDigitalWatch();
        }
    }

    int displayModeFlag = 0, periodFlag = 0, setTimeFlag = 0, timeEditFlag = 0, setDateFlag = 0, dateEditFlag = 0, alarmEditFlag = 0,prev_time_value=0,actual_time=0,prev_date=0;
    while (1) {
        int i;
        if (PORTAbits.RA12 == 0) {
            alarmStopper = 1;
            LATAbits.LATA9 = 1;
            for (i = 0; i < 100; i++);
            LATAbits.LATA9 = 0;

            if (displayModeFlag) {
                alarmStopper = 0;
                state.menu = 0;
                if (!optionMenu.selected) {
                    oledC_clearScreen();
                    state.typeOfWatch = 0;
                    displayAnalogWatch();
                } else {
                    oledC_clearScreen();
                    state.typeOfWatch = 1;
                    displayDigitalWatch();
                }
                displayModeFlag = 0;
            }

            if (periodFlag) {
                alarmStopper = 0;
                state.menu = 0;
                if (!optionMenu.selected) {
                    oledC_clearScreen();
                    if ((watch.hour > 11 && watch.hour <= 23 && watch.period == 2) || watch.period == 1) 
                    {
                        watch.period = 1;
                        watch.hour -= 12;
                    }
                    else if ((watch.hour >= 0 && watch.hour < 12 && watch.period == 2) || watch.period == 0)
                    {
                        watch.period = 0;
                    }
                        
                        
                } else {
                    oledC_clearScreen();
                    periodHoursController();
                    watch.period = 2;
                    alarm.period = 2;
                    
                }

                if (state.typeOfWatch == 0) {
                    displayAnalogWatch();
                } else if (state.typeOfWatch == 1) {
                    displayDigitalWatch();
                }

                periodFlag = 0;
            }
            prev_time_value=GetPotentiometerValues();
            actual_time= 0;
            if (setTimeFlag) {
                alarmStopper = 0;
                oledC_clearScreen();
                state.menu = 0;

                if (timeEditFlag) {
                     prev_time_value/=41;
                     if(prev_time_value==0){
                         prev_time_value=1;
                     }
                     periodHoursController();
                     watch.hour=prev_time_value;
                    hoursController();
                } else {
                    prev_time_value/=17;
                    watch.minute=prev_time_value;
                    minutesController();
                }

                displayDigitalWatch();
            }
            prev_date=GetPotentiometerValues();
            int actual_month=0;
            if (setDateFlag) {
                alarmStopper = 0;
                oledC_clearScreen();
                state.menu = 0;

                if (dateEditFlag) {
                    prev_date/=32;
                    watch.day=prev_date;
                    dateController();
                } else {
                    actual_month=GetPotentiometerValues();
                    actual_month/=67;
                    watch.month=actual_month;
                     month_cycle();
                }
                if (state.typeOfWatch == 0) {
                    displayAnalogWatch();
                } else if (state.typeOfWatch == 1) {
                    displayDigitalWatch();
                }
            }
            int prev_alarm_time=GetPotentiometerValues();
            int actual_time_alarm=0;
            if (alarmFlag) {
                alarmStopper = 0;
                state.menu = 0;
                if (!optionMenu.selected) {
                    state.alarm = 1;
                    if (alarmEditFlag) {
                        prev_alarm_time/=41;
                        alarm.hour=prev_alarm_time;
                        hoursController();
                    } else {
                        prev_alarm_time/=17;
                        alarm.minute=prev_alarm_time;
                        minutesController();
                    }
                    displayDigitalAlarm();
                } else {
                    state.alarm = 0;
                    displayDigitalWatch();
                }
            }


            if (state.menu == 1) {
                menu.option = 1;
                optionMenu.selected = 0;
                switch (menu.selected) {
                    case 0:
                        displayAnalogDigitalMenu();
                        displayModeFlag = 1;
                        break;
                    case 1:
                        displayIntervalMenu();
                        periodFlag = 1;
                        break;
                    case 2:
                        setTimeFlag = 1;
                        break;
                    case 3:
                        setDateFlag = 1;
                        break;
                    case 4:
                        displayAlarm();
                        alarmFlag = 1;
                        break;
                }
            }
        }

        if (PORTAbits.RA11 == 0) {
            alarmStopper = 1;
            LATAbits.LATA8 = 1;
            for (i = 0; i < 100; i++);
            LATAbits.LATA8 = 0;

            if (state.menu == 1 && menu.selected == previousMenu.selected && menu.option != 1) {
                menu.selected++;
                menu.selected = menu.selected % 5;
            }
            if (state.menu == 1 && optionMenu.selected == previousOptionMenu.selected && menu.option == 1) {
                if (optionMenu.selected) {
                    optionMenu.selected = 0;
                } else {
                    optionMenu.selected = 1;
                }
            }

            if (setTimeFlag) {
                if (timeEditFlag)
                    timeEditFlag = 0;
                else
                    timeEditFlag = 1;
            }

            if (setDateFlag) {
                if (dateEditFlag)
                    dateEditFlag = 0;
                else
                    dateEditFlag = 1;
            }
            if (alarmFlag) {
                if (alarmEditFlag)
                    alarmEditFlag = 0;
                else
                    alarmEditFlag = 1;
            }
        }

        if (state.counter == 2) {
            alarmStopper = 0;
            setTimeFlag = 0;
            setDateFlag = 0;
            state.counter = 0;
            state.menu++;
            state.menu = state.menu % 2;
            if (state.menu == 1) {
                oledC_clearScreen();
                displayMenu();
                alarmFlag = 0;
            } else if (state.menu == 0 && alarmFlag == 0) {
                if (state.typeOfWatch == 0) {
                    //Drawing analog clock
                    displayAnalogWatch();
                } else if (state.typeOfWatch == 1) {
                    displayDigitalWatch();
                }
            }
        }
    }

    return 1;
}

