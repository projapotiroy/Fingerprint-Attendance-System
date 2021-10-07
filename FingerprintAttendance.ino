#include "Adafruit_Fingerprint.h"
#include<LiquidCrystal.h>
#include<DS3231.h>
#include<SoftwareSerial.h>
#include<Keypad.h>
#include<SPI.h>
#include<SD.h>

LiquidCrystal lcd(13,12,9,8,7,6);
SoftwareSerial fingerprint(10, 11);         //for tx & rx
DS3231 rtc(SDA, SCL);
File myFile,headerFile;
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprint);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {38, 39, 40, 41};
byte colPins[COLS] = {A12,A13,A14,A15};
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int i1, i2, i3, i4;
char c1, c2, c3, c4,course[100];
int count = 0,flag = 0,checkInHour,checkInMinute,studentCheckedHour,studentCheckedMinute;

void setup()
{
    lcd.begin(16, 2);
    lcd.print("Biometric");
    lcd.setCursor(0, 1);
    lcd.print("AttendanceSystem");
    delay(2000);
    finger.begin(57600);
    Serial.begin(9600);
    Serial.println("Biometric Attendance System");
    lcd.clear();
    lcd.print("Finding Module..");
    lcd.setCursor(0,1);
    delay(1000);

    if(finger.verifyPassword())
    {
        Serial.println("Found fingerprint Sensor!");
        lcd.clear();
        lcd.print("Module found");
        delay(1000);
    }
    else
    {
        Serial.println("Did not find fingerprint Sensor!");
        lcd.clear();
        lcd.print("Module not found ");
        lcd.setCursor(0, 1);
        lcd.print("Check Connections");
        while (1);
    }

    //SD Card Initialization
    Serial.println("Initializing SD card");
    if (!SD.begin(53))
    {
       Serial.println("Initialization failed!");
       return;
    }
    Serial.println("Initialization done!");
    myFile = SD.open("Data.txt", FILE_WRITE);          //Create or open file
    //if the file opened,write to it
    if(myFile)
    {
       Serial.println("Writing to file..");
       boolean flag_header= true;
       headerFile = SD.open("Data.txt");
       while(headerFile.available())
       {
           char finalString[200] = "";
           while(headerFile.available())
           {
               char str1[20] = {(char) headerFile.read() , '\0'};
               if(strcmp(str1,"\n")==0)
                  break;
               strcat(finalString,str1);
           }
           char delim[] = ",";
           char *ptr = strtok(finalString, delim);
           if(strcmp(ptr,"Course_No")==0)
           {
               Serial.println("Heading Exists!!");
               flag_header = false;
               break;
           }
        }
        if(flag_header)
        {
               myFile.println("Course_No,Date,Time,ID,Status");                //print the headers
               myFile.close();                                           //close the file
               Serial.println("Done!");
        }
    }
    else
    {
       Serial.println("File opened failed!");
    }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter Course No");
    char kk1 = myKeypad.waitForKey();
    if (kk1 != NO_KEY && kk1 != '*' && kk1 != '#' && kk1 != 'A' && kk1 != 'B' && kk1 != 'C' && kk1 != 'D')
    {
        c1 = kk1;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char kk2 = myKeypad.waitForKey();
    if (kk2 != NO_KEY && kk2 != '*' && kk2 != '#' && kk2 != 'A' && kk2 != 'B' && kk2 != 'C' && kk2 != 'D')
    {
        c2 = kk2;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    char kk3 = myKeypad.waitForKey();
    if (kk3 != NO_KEY && kk3 != '*' && kk3 != '#' && kk3 != 'A' && kk3 != 'B' && kk3 != 'C' && kk3 != 'D')
    {
        c3 = kk3;
        lcd.setCursor(2, 1);
        lcd.print(c3);
    }
    char kk4 = myKeypad.waitForKey();
    if (kk4 != NO_KEY && kk4 != '*' && kk4 != '#' && kk4 != 'A' && kk4 != 'B' && kk4 != 'C' && kk4 != 'D')
    {
        c4 = kk4;
        lcd.setCursor(3, 1);
        lcd.print(c4);
    }
    i1 = (c1 - 48) * 1000;
    i2 = (c2 - 48) * 100;
    i3 = (c3 - 48) * 10;
    i4 = c4 - 48;
    int course_no = i1 + i2 + i3 + i4;
    itoa(course_no, course, 10);                  //course_no covert int to string
    Serial.print("Enter Course No: ");
    Serial.println(course);
    delay(1000);
    
    Serial.println("Setup Class Time & Date");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Setup Class");
    lcd.setCursor(0,1);
    lcd.print("Date & Time ");
    delay(1000);
    Serial.println("Setup year");
    lcd.clear();
    lcd.print("Setup year");
    char k1 = myKeypad.waitForKey();
    if (k1 != NO_KEY && k1 != '*' && k1 != '#' && k1 != 'A' && k1 != 'B' && k1 != 'C' && k1 != 'D')
    {
        c1 = k1;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k2 = myKeypad.waitForKey();
    if (k2 != NO_KEY && k2 != '*' && k2 != '#' && k2 != 'A' && k2 != 'B' && k2 != 'C' && k2 != 'D')
    {
        c2 = k2;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    char k3 = myKeypad.waitForKey();
    if (k3 != NO_KEY && k3 != '*' && k3 != '#' && k3 != 'A' && k3 != 'B' && k3 != 'C' && k3 != 'D')
    {
        c3 = k3;
        lcd.setCursor(2, 1);
        lcd.print(c3);
    }
    char k4 = myKeypad.waitForKey();
    if (k4 != NO_KEY && k4 != '*' && k4 != '#' && k4 != 'A' && k4 != 'B' && k4 != 'C' && k4 != 'D')
    {
        c4 = k4;
        lcd.setCursor(3, 1);
        lcd.print(c4);
    }
    i1 = (c1 - 48) * 1000;
    i2 = (c2 - 48) * 100;
    i3 = (c3 - 48) * 10;
    i4 = c4 - 48;
    int N_year = i1 + i2 + i3 + i4;
    delay(1000);

    Serial.println("Setup Month");
    lcd.clear();
    lcd.print("Setup month");
    char k5 = myKeypad.waitForKey();
    if (k5 != NO_KEY && k5 != '*' && k5 != '#' && k5 != 'A' && k5 != 'B' && k5 != 'C' && k5 != 'D')
    {
        c1 = k5;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k6 = myKeypad.waitForKey();
    if (k6 != NO_KEY && k6 != '*' && k6 != '#' && k6 != 'A' && k6 != 'B' && k6 != 'C' && k6 != 'D')
    {
        c2 = k6;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    i1 = (c1 - 48) * 10;
    i2 = c2 - 48;
    int N_month = i1 + i2;
    delay(1000);

    Serial.println("Setup Day");
    lcd.clear();
    lcd.print("Setup Day");
    char k7 = myKeypad.waitForKey();
    if (k7 != NO_KEY && k7 != '*' && k7 != '#' && k7 != 'A' && k7 != 'B' && k7 != 'C' && k7 != 'D')
    {
        c1 = k7;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k8 = myKeypad.waitForKey();
    if (k8 != NO_KEY && k8 != '*' && k8 != '#' && k8 != 'A' && k8 != 'B' && k8 != 'C' && k8 != 'D')
    {
        c2 = k8;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    i1 = (c1 - 48) * 10;
    i2 = c2 - 48;
    int N_day = i1 + i2;
    delay(1000);

    Serial.println("Setup Hours");
    lcd.clear();
    lcd.print("Setup Hours");
    char k9 = myKeypad.waitForKey();
    if (k9 != NO_KEY && k9 != '*' && k9 != '#' && k9 != 'A' && k9 != 'B' && k9 != 'C' && k9 != 'D')
    {
        c1 = k9;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k10 = myKeypad.waitForKey();
    if (k10 != NO_KEY && k10 != '*' && k10 != '#' && k10 != 'A' && k10 != 'B' && k10 != 'C' && k10 != 'D')
    {
        c2 = k10;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    i1 = (c1 - 48) * 10;
    i2 = c2 - 48;
    int N_hrs = i1 + i2;
    delay(1000);

    Serial.println("Setup Minutes");
    lcd.clear();
    lcd.print("Setup Minutes");
    char k11 = myKeypad.waitForKey();
    if (k11 != NO_KEY && k11 != '*' && k11 != '#' && k11 != 'A' && k11 != 'B' && k11 != 'C' && k11 != 'D')
    {
        c1 = k11;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k12 = myKeypad.waitForKey();
    if (k12 != NO_KEY && k12 != '*' && k12 != '#' && k12 != 'A' && k12 != 'B' && k12 != 'C' && k12 != 'D')
    {
        c2 = k12;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    i1 = (c1 - 48) * 10;
    i2 = c2 - 48;
    int N_min = i1 + i2;
    delay(1000);

    Serial.println("Setup Seconds");
    lcd.clear();
    lcd.print("Setup Seconds");
    char k13 = myKeypad.waitForKey();
    if (k13 != NO_KEY && k13 != '*' && k13 != '#' && k13 != 'A' && k13 != 'B' && k13 != 'C' && k13 != 'D')
    {
        c1 = k13;
        lcd.setCursor(0, 1);
        lcd.print(c1);
    }
    char k14 = myKeypad.waitForKey();
    if (k14 != NO_KEY && k14 != '*' && k14 != '#' && k14 != 'A' && k14 != 'B' && k14 != 'C' && k14 != 'D')
    {
        c2 = k14;
        lcd.setCursor(1, 1);
        lcd.print(c2);
    }
    i1 = (c1 - 48) * 10;
    i2 = c2 - 48;
    int N_sec = i1 + i2;
    delay(1000);

    rtc.begin();
    rtc.setTime(N_hrs, N_min, N_sec);
    rtc.setDate(N_day, N_month, N_year);
    delay(1000);

    Serial.print("Date : ");
    Serial.print(rtc.getDateStr());
    Serial.print("  Time : ");
    Serial.println(rtc.getTimeStr());

    checkInHour = N_hrs;
    checkInMinute = N_min + 15;
}

void loop()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Date: ");
    lcd.print(rtc.getDateStr());
    lcd.setCursor(0, 1);
    lcd.print("Time: ");

    int hr = rtc.getTime().hour;
    int minute = rtc.getTime().min;
    int seconds = rtc.getTime().sec;

    if(hr >= 12)
    {
        flag = 1;
        lcd.setCursor(14,1);
        lcd.print("PM");
        hr = hr - 12;
        if(hr == 0)
        {
            hr = 12; //PM
        }
        if(hr <= 9)
        {
            lcd.setCursor(6,1);
            lcd.print("0");
            lcd.setCursor(7,1);
            lcd.print(hr,DEC);
        }
        else
        {
            lcd.setCursor(6,1);
            lcd.print(hr,DEC);
        }
    }
    else
    {
        if(hr == 0)
        {
            hr = 12; //AM
        }
        lcd.setCursor(14,1);
        lcd.print("AM");
        if(hr <= 9)
        {
            lcd.setCursor(6,1);
            lcd.print("0");
            lcd.setCursor(7,1);
            lcd.print(hr,DEC);
        }
        else
        {
            lcd.setCursor(6,1);
            lcd.print(hr,DEC);
        }
    }

    lcd.setCursor(8,1);
    lcd.print(":");
    if(minute <= 9)
    {
        lcd.setCursor(9,1);
        lcd.print("0");
        lcd.setCursor(10,1);
        lcd.print(minute,DEC);
    }
    else
    {
        lcd.setCursor(9,1);
        lcd.print(minute,DEC);
    }

    lcd.setCursor(11,1);
    lcd.print(":");
    if(seconds <= 9)
    {
        lcd.setCursor(12,1);
        lcd.print("0");
        lcd.setCursor(13,1);
        lcd.print(seconds,DEC);

    }
    else
    {
        lcd.setCursor(12,1);
        lcd.print(seconds,DEC);
    }
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press A or B for");
    lcd.setCursor(0,1);
    lcd.print("AttedanceEnroll");
    Serial.println("Press A for attendance & B for Enrollment");

    while(myKeypad.getKey() == NO_KEY)
    {
        char key = myKeypad.getKey();
        if(key == 'A')
        {
            Serial.println("Place Finger");
            lcd.clear();
            lcd.print("Place Finger");
            delay(5000);
            int result = getFingerprintIDez();
            if(result > 0 )
            {
                studentCheckedHour = rtc.getTime().hour;
                studentCheckedMinute = rtc.getTime().min;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("ID ");
                lcd.print(result);

                boolean flag_attend = true;
                boolean checked_course = false;
                char var_date[100] = "";
                char course_id[100] = "";
                char s_id[30],var_id[30];

                myFile = SD.open("Data.txt");        //read from sd card
                while(myFile.available())
                {
                    char finalString[1000] = "";
                    while(myFile.available())
                    {
                        char str1[100] = {(char) myFile.read() , '\0'};
                        if(strcmp(str1,"\n")==0)
                        {
                            break;
                        }
                        strcat(finalString,str1);
                    }
                  char delim[] = ",";
                  char *ptr = strtok(finalString, delim);  //course_no split
                  strcpy(course_id,ptr);                   //course_no copy to course_id
                  ptr = strtok(NULL, delim);               // date split 
                  strcpy(var_date,ptr);                    //date copy to var_date
                  ptr = strtok(NULL, delim);               //time split
                  ptr = strtok(NULL, delim);               //id split
                  strcpy(var_id,ptr);                      //id copy to var_id
                  itoa(result, s_id, 10);                  //result covert int to string

                  if(strcmp(var_date,rtc.getDateStr())==0 && strcmp(var_id,s_id)==0 && strcmp(course_id,course)==0)
                  {
                      Serial.println("Your Attendance Already Recorded!!!");
                      lcd.setCursor(0,1);
                      lcd.print("Already Done!");
                      flag_attend = false;
                      break;
                  }
                }
                if(flag_attend)
                {
                    myFile = SD.open("Data.txt",FILE_WRITE);
                    if(myFile)
                    {
                        myFile.print(course);
                        myFile.print(",");
                        myFile.print(rtc.getDateStr());
                        myFile.print(",");
                        myFile.print(hr);
                        myFile.print(":");
                        myFile.print(minute);
                        myFile.print(":");
                        myFile.print(seconds);

                        Serial.println(course);
                        Serial.println(",");
                        Serial.print(rtc.getDateStr());
                        Serial.print(",");
                        Serial.print(hr);
                        Serial.print(":");
                        Serial.print(minute);
                        Serial.print(":");
                        Serial.print(seconds);

                        if(flag == 1)
                        {
                            myFile.print("PM");
                            Serial.print("PM");
                        }
                        else
                        {
                            myFile.print("AM");
                            Serial.print("AM");
                        }

                        if((studentCheckedHour < checkInHour) || ((studentCheckedHour == checkInHour) && (studentCheckedMinute <= checkInMinute)))
                        {
                            myFile.print(",");
                            myFile.print(result);
                            myFile.print(",");
                            myFile.println("Present");
                            myFile.close();
                            lcd.setCursor(7,0);
                            lcd.print("Present");
                            lcd.setCursor(0,1);
                            lcd.print("You are welcome");

                            Serial.print(",");
                            Serial.print(result);
                            Serial.print(",");
                            Serial.println("Present");
                        }
                        else
                        {
                            myFile.print(",");
                            myFile.print(result);
                            myFile.print(",");
                            myFile.println("Late Present");
                            myFile.close();
                            lcd.setCursor(7,0);
                            lcd.print("Present");
                            lcd.setCursor(0,1);
                            lcd.print("Late Considered");

                            Serial.print(",");
                            Serial.print(result);
                            Serial.print(",");
                            Serial.println("You are considered as Late Present");
                        }

                    }
                    else
                        Serial.println("Failed to upload data in file!");
                }
            }
        }
        else if(key == 'B')
        {
            lcd.clear();
            lcd.print("Please Wait");
            delay(1000);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Press C or D");
            lcd.setCursor(0,1);
            lcd.print("To adjust id");
            Enroll();
       } 
     }
    delay(2000);
}

void Enroll()
{
    while(1)
    {
        char key1 = myKeypad.getKey();
        if(key1 == 'C')
        {
            count++;
            if(count>30)
            {
                count = 1;
                delay(500);
            }
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter Finger ID: ");
            lcd.setCursor(0,1);
            lcd.print(count);
        }
        else if(key1 == 'D')
        {
            count--;
            if(count<1)
            {
                count = 30;
                delay(500);
            }
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter Finger ID: ");
            lcd.setCursor(0,1);
            lcd.print(count);
        }
        else if(key1 == '#')
        {
            id = count;
            lcd.setCursor(0,1);
            lcd.print(id);
            getFingerprintEnroll();
        }
        else if(key1 == 'B' || key1 == 'A')
        {
            return;
        }
    }
}

uint8_t getFingerprintEnroll()
{
    int p = -1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finger ID ");
    lcd.print(id);
    lcd.setCursor(0,1);
    lcd.print("Place Finger");

    Serial.print("Finger ID");
    Serial.println(id);
    Serial.println("Place Finger");
    delay(2000);

    while(p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch(p)
        {
            case FINGERPRINT_OK:
                Serial.println("Image Taken");
                lcd.clear();
                lcd.print("Image Taken");
                break;
            case FINGERPRINT_NOFINGER:
                Serial.println("No Finger");
                lcd.clear();
                lcd.print("No Finger");
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication Error");
                lcd.clear();
                lcd.print("Commmun Error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging Error");
                lcd.clear();
                lcd.print("Imaging Error");
                break;
            default:
                Serial.println("Unknown error");
                lcd.clear();
                lcd.print("Unknown error");
                break;
        }
    }
    //ok success
    p = finger.image2Tz(1);
    switch(p)
    {
        case FINGERPRINT_OK:
            Serial.println("Image Converted");
            lcd.clear();
            lcd.print("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            lcd.clear();
            lcd.print("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication Error");
            lcd.clear();
            lcd.print("Commmun Error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            lcd.clear();
            lcd.print("Feature not found");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            lcd.clear();
            lcd.print("Feature not found");
            return p;
        default:
            Serial.println("Unknown error");
            lcd.clear();
            lcd.print("Unknown error");
            return p;
    }
    Serial.println("Remove Finger");
    lcd.clear();
    lcd.print("Remove Finger");
    delay(2000);
    p = 0;
    while(p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    lcd.clear();
    lcd.print("Place Same");
    lcd.setCursor(0,1);
    lcd.print("Finger Again");

    while(p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch(p)
        {
            case FINGERPRINT_OK:
                Serial.println("Image Taken");
                break;
            case FINGERPRINT_NOFINGER:
                Serial.println("No Finger");
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication Error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging Error");
                break;
            default:
                Serial.println("Unknown error");
                break;
        }
    }

    //ok success
    p = finger.image2Tz(2);
    switch(p)
    {
        case FINGERPRINT_OK:
            Serial.println("Image Converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication Error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
     //ok convered
    Serial.print("Creating model for #");
    Serial.println(id);
    p = finger.createModel();
    if(p == FINGERPRINT_OK)
    {
        Serial.println("Fingerprint matched");
    }
    else if(p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication Error");
        return p;
    }
    else if(p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Fingerprint did not match");
        return p;
    }
    else
    {
        Serial.println("Unknown Error");
        return p;
    }
    Serial.print("ID ");
    Serial.print(id);
    p = finger.storeModel(id);
    if(p == FINGERPRINT_OK)
    {
        Serial.println("Stored");
        lcd.clear();
        lcd.print("Stored!");
        delay(2000);
    }
    else if(p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication Error");
        return p;
    }
    else if(p == FINGERPRINT_BADLOCATION)
    {
        Serial.println("Could not store in that location");
        return p;
    }
    else if(p == FINGERPRINT_FLASHERR)
    {
        Serial.println("Error writing to flash");
        return p;
    }
    else
    {
        Serial.println("Unknown Error");
        return p;
    }
}

int getFingerprintIDez()
{
    uint8_t p = finger.getImage();
    if(p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if(p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if(p != FINGERPRINT_OK)
    {
        Serial.println("Finger Not Found");
        lcd.clear();
        lcd.print("Finger Not Found");
        lcd.setCursor(0,1);
        lcd.print("Try Again!");
        delay(2000);
        return -1;
    }

    Serial.print("Found ID #");
    Serial.println(finger.fingerID);
    return finger.fingerID;
}
