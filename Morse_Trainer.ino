/*
    Name:       Morse Trainer.ino
    Created:	12/20/2020 5:48:15 PM
    Author:     Rich McDonald, WB0NRE

    Delay settings derived from information at http://www.kent-engineers.com/codespeed.htm
    Morse prosigns documented at https://en.wikipedia.org/wiki/Prosigns_for_Morse_code#
    pitches.h is available at https://gist.github.com/mikeputnam/2820675 add to local libraries dir

    Rev: 0.2  : added some documentation

*/

#include <string.h>
#include <Arduino.h>
#include <pitches.h>

boolean DEBUG_PRINT = true;
boolean done = false;
boolean finit = false;

int wpm = 20;
float element_duration_ms = 92.31;
int element_duration = 0;
int repeat_limit = 2;
int cnt = 0;

const char AR = { 3 };      // ASCII ETX - end of text
const char EOT = { 4 };     // ASCII ETX - end of transmission, here we just use it to insert extra quiet interval
const char BS = { 8 };      // ASCII BS - backspace key i.e. an error
const char AA = { 10 };     // ASCII NL - line feed, new line i.e. blank line     
const char SK = { 25 };     // ASCII EM - end of medium i.e. closing station

char alphabet[] =
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
  's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
  ' ', ',', '.', '/', ':', ';', '+', '-', '=', AR, EOT, BS, SK, '\0'  
};

String morse_alphabet[]=
{"13", "3111", "3131", "311", "1", "1131", "311", "1111", "11", "1333", "313", "1311","33","31", "333","1331", "3313", "131",
 "111", "3", "113", "1113", "133", "3113", "3133", "3311", "33333", "13333", "11333", "11133", "11113", "11111", "31111", "33111", "33311", "33331",
 "2", "331133", "131313", "31131", "333111", "313131", "13131", "311113", "31113", "13131", "4", "11111111", "111313", ""
};  // 1 = dot; 3 = dash

String lookup(char c) {
    return lookup(c, DEBUG_PRINT);
}

String lookup(char c, boolean debugPrint) {
        if (debugPrint) {
            String out;
            switch (c) {
            case BS:
                out = "ERROR";
                break;
            case SK:
                out = "   SK : ";
                break;
            case EOT:
                out = "  EOT : ";
                break;
            default:
                out = (String)c + " : ";
            }
            Serial.print(out);
        };
        for (int i = 0; i < (sizeof(alphabet)/sizeof(alphabet[0])); i++) {
            //for (int i = 0; alphabet[i] != '\0'; i++) {
            if (alphabet[i] == c) {
            if (debugPrint) { Serial.print(i);  Serial.print(" : "); };
            return morse_alphabet[i];
        }
    };
    return "";
}

void sendMorse(String arg) {
    sendMorse(arg, true, DEBUG_PRINT);
}

void sendMorse(String arg, boolean sound) {
    sendMorse(arg, sound, DEBUG_PRINT);
}

void sendMorse(String arg, boolean sound, boolean debugPrint) {
    int duration = 100;
    for (int i = 0; i < arg.length(); i++)
        switch (arg.charAt(i) - '0') {
            case 2 :
                delay(element_duration * 6);    // interword spacing is 7 dot intervals (last off + 6)
                break;
            case 4 :
                delay(element_duration * 7);    // keep the LED off for 3 dash intervals
                break;
            default : 
                digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
                duration = element_duration * (arg.charAt(i) - '0');
                if (sound) {
                    // pin8 outputs the tone
                    tone(8, NOTE_DS5, duration);    // NOTE_DS5 (D# 5th octave) is the closest to 600 Hz
                }
                if (debugPrint) { Serial.print(arg.charAt(i)); };
                delay(duration);
                digitalWrite(13, LOW);   // turn the LED off for one intra-character period
                delay(element_duration * 1);
    }
    delay(element_duration * 2);    // intercharacter spacing is 3 dot intervals (last element + 2 more)
    if (debugPrint) Serial.print('\n');
}

void setup()
{
    element_duration_ms = (60.0 * 1000) / (wpm * 50);
    element_duration = round(element_duration_ms);
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);
    Serial.begin(9600);
    while (!Serial);  // Wait until Serial is ready - Leonardo
    if (DEBUG_PRINT) { Serial.println("Initializing"); }
    if (DEBUG_PRINT) { Serial.print(wpm); Serial.println(" wpm\n"); }
    if (DEBUG_PRINT) { Serial.print(element_duration_ms); Serial.println(" ms\n"); }
    if (DEBUG_PRINT) { Serial.print(element_duration); Serial.println(" ms\n"); }
    if (DEBUG_PRINT) { Serial.println("Starting output"); }
}

void loop()
{
    String send_phrase = "now is the time for all good men to come to the aid of their country";
    //String send_phrase = "now is";

    if (cnt < repeat_limit) {
        for (char c : send_phrase) {
            if (DEBUG_PRINT) { Serial.print(cnt); Serial.print(" : "); };
            sendMorse(lookup(c));
            delay(element_duration * 2);    // keep the LED off for a total of 3 dot intervals
        };
        sendMorse(lookup(' '));
        sendMorse(lookup(EOT));
        cnt++;
    };
    if (done) {
        if(!finit) sendMorse(lookup(SK));
        finit = true;
    }
    else {
        done = true;
    };
}
