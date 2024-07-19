#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

// Define OLED parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

int hours = 0;
int minutes = 0;
int seconds = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0, 0, 0};
int alarm_minutes[] = {0, 0, 0};
bool alarm_triggered[] = {false, false, false};

// Define Buzzer, LED indicator and Push buttons Pins
#define BUZZER 15
#define LED 5
#define PB_CANCEL 34
#define PB_OK 33
#define PB_UP 35
#define PB_DOWN 32

#define DHTPIN 12

// Define the NTP server to use for time synchronization
#define NTP_SERVER     "pool.ntp.org"
int UTC_OFFSET; // Define the UTC offset (in seconds) from UTC time.
#define UTC_OFFSET_DST 0 // Define the UTC offset (in seconds) for daylight saving time (DST).


int n_notes = 8; // total number of notes

// Define the frequencies of each note
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;

int notes[] = {C, D, E, F, G, A, B, C_H};

int current_mode = 0;
int max_modes = 5;

// Define options for different modes as strings
String options[] = {"1 - Set Time", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Set Alarm 3", "5 - Remove Alarm"};


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DHTesp dhtSensor; // Initialize DHTesp sensor object

void setup() {

  // Set pin modes
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  // Start serial communication
  Serial.begin(9600);


  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for ( ;; );
  }

  display.display();
  delay(500);

  // Connect to WiFi
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi", 0, 0, 2);

  }
  display.clearDisplay();
  print_line("Connected to WiFi", 0, 0, 2);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);// Set up time synchronization

  display.clearDisplay();

  print_line("Welcome to Medibox", 10, 20, 2);
  delay(500);
  display.clearDisplay();

}

void loop() {
  // Update time and check alarm
  update_time_with_check_alarm();

  // Check if the OK button is pressed
  if (digitalRead(PB_OK) == LOW) {
    delay(2000);
    go_to_menu();
  }

  check_temp();


}



// Function for print a line of text on the display
void print_line(String text, int column, int row, int text_size ) {

  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);
  display.display();

}


// function for display the current time HH:MM:SS
void print_time_now(void) {

  display.clearDisplay();
  print_line("" + String(hours) + ":" + String(minutes) + ":" + String(seconds), 0, 0, 2);



}

// Function for update the time variables
void update_time() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  //Extract the time and date from the timeinfo

  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  hours = atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  minutes = atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond, 3, "%S", &timeinfo);
  seconds = atoi(timeSecond);


}



//Function for update time and check for triggered alarms
void update_time_with_check_alarm(void) {

  update_time();
  print_time_now();

  if (alarm_enabled) {

    for (int i = 0; i < n_alarms ; i++) {
      //check whether the alarm has not been triggered and if the current time matches the set alarm time
      if (alarm_triggered[i] == false && hours == alarm_hours[i] && minutes == alarm_minutes[i]) {
        //Activate the alarm and designate it as triggered
        ring_alarm();
        alarm_triggered[i] = true;


      }
    }
  }
}



//Funtion for ringing alarm
void ring_alarm() {

  display.clearDisplay();
  print_line("Medicine Time", 0, 0, 2); //print "Medicine Time" in display

  digitalWrite(LED, HIGH);

  bool break_happen = false;

  // Continue ringing the alarm until canceled or button is pressed
  while (break_happen == false && digitalRead(PB_CANCEL) == HIGH) {
    for (int i = 0; i < n_notes; i++) {
      if (digitalRead(PB_CANCEL) == LOW) {
        delay(200);
        break_happen = true;
        break;
      }


      tone(BUZZER, notes[i]);//Play the i th note
      delay(500);
      noTone(BUZZER);
      delay(2);

    }
  }

  digitalWrite(LED, LOW);
  display.clearDisplay();
}


void go_to_menu(void) {
  while (digitalRead(PB_CANCEL) == HIGH) {
    display.clearDisplay();
    print_line(options[current_mode], 2, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      current_mode += 1; // If the up button is pressed, increment the mode
      current_mode %= max_modes;
      delay(200);

    }

    //If the ok button is pressed,execute current mode
    else if (pressed == PB_OK) {
      delay(200);

      run_mode(current_mode);
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      current_mode -= 1; // If the up button is pressed, decrement the mod
      if (current_mode < 0) {
        current_mode = max_modes - 1;
      }
    }
    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }

  }
}

//Wait for a button press and return the pressed button
int wait_for_button_press() {
  while (true) {
    if (digitalRead(PB_UP) == LOW) {
      delay(200);
      return PB_UP;
    }

    else if (digitalRead(PB_DOWN) == LOW) {
      delay(200);
      return PB_DOWN;

    }

    else if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      return PB_CANCEL;
    }

    else if (digitalRead(PB_OK) == LOW) {
      delay(200);
      return PB_OK;
    }
    update_time(); // Ensure accurate time display while awaiting button press by updating the time

  }
}

//Funtion for execute the selected mode
void run_mode(int mode) {

  if (mode == 0) {
    set_time();
  }
  else if (mode == 1 || mode == 2 || mode == 3) {
    set_alarm(mode - 1);
  }
  else if (mode == 4) {
    alarm_enabled = false;

  }

}

// Function for set the device time  according to the offset
void set_time() {

  int temp_hour = hours;
  while (true) {
    display.clearDisplay();
    print_line("Enter offset hours: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_hour += 1; // If the up button is pressed, increment the hour offset
      temp_hour = temp_hour % 24;
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      temp_hour -= 1; // If the up button is pressed, decrement the hour offset
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }
    //If the ok button is pressed,exit the funtion
    else if (pressed == PB_OK) {
      delay(200);
      break;
    }

    else if (pressed == PB_CANCEL) {
      delay(200);
      break; // If the OK button is pressed, update the alarm minute and exit the loop
    }
  }
  int temp_minute = minutes;
  while (true) {
    display.clearDisplay();
    print_line("Enter offset minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP) {
      delay(200);
      temp_minute += 1; //If the up button is pressed, increment the minute offset
      temp_minute = temp_minute % 60;
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      temp_minute -= 1; // If the down button is pressed, decrement the hour offset
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }
    //If the ok button is pressed,exit the funtion
    else if (pressed == PB_OK) {
      delay(200);
      break;
    }

    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }
  //UTC offset in seconds
  UTC_OFFSET = temp_hour * 3600 + temp_minute * 60;

  // Again configure the time with the calculated UTC offset
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  display.clearDisplay();
  print_line("Time is set", 0, 0, 2);
  delay(1000);
}



// Function to set the specified alarm
void set_alarm(int alarm) {

  int temp_hour = alarm_hours[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    }

    else if (pressed == PB_OK) {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }

    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }
  int temp_minute = alarm_minutes[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == PB_DOWN) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    }

    else if (pressed == PB_OK) {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    }

    else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set", 0, 0, 2);
  delay(1000);
}

// Function to check temperature and humidity in healthy range
void check_temp(void) {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();


  if (data.temperature > 32) {
    display.clearDisplay();
    print_line("TEMP HIGH", 0, 40, 1);
  } else if (data.temperature < 26) {
    display.clearDisplay();
    print_line("TEMP LOW", 0, 40, 1);
  }

  if (data.humidity > 80) {
    display.clearDisplay();
    print_line("HUMD HIGH", 0, 50, 1);
  } else if (data.humidity < 60) {
    display.clearDisplay();
    print_line("HUMD LOW", 0, 50, 1);
  }


}