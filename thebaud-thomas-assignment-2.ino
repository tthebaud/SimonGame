#include <LiquidCrystal.h>

const int buzzerPin = 2;
const int ledPins[] = { 3, 4, 5, 6 };
const int d7 = 7;
const int d6 = 8;
const int d5 = 9;
const int d4 = 10;
const int en = 11;
const int rs = 12;
const int pinCount = 4;
const int buzzerFrequency[] = { 261, 293, 329, 392 };

LiquidCrystal lcd( rs, en, d4, d5, d6, d7 );

int currentSequenceLED[ 100 ];
int score = 0;
int delayDifficulty = 500;
int difficulty = 1;

bool win = true;

void setup() {
  Serial.begin( 9600 );
  randomSeed( analogRead( 0 ) );

  lcd.begin( 16, 2 );
  pinMode( buzzerPin, OUTPUT );

  start_game();
}

void print_On_Lcd( String firstLine, String secondLine, bool clearAll ) {
  if ( clearAll ) {
    lcd.clear();
  }

  lcd.setCursor( 0, 0 );
  lcd.print( firstLine );
  lcd.setCursor( 0, 1 );
  lcd.print( secondLine );
}

void pin_Mode_To_Output() {
  for ( int i = 0; i < pinCount; i++ ) {
    pinMode( ledPins[i], OUTPUT );
  }
}

void pin_Mode_To_Input() {
  for ( int i = 0; i < pinCount; i++ ) {
    pinMode( ledPins[i], INPUT );
  }
}

void blink_LED_ALL( int count, int time_ms ) {
  for ( int i = 0; i < count; i++ ) {
    pin_Mode_To_Output();
    delay( time_ms );
    pin_Mode_To_Input();
    delay( time_ms );
  }
}

void blink_LED( int count, int pinNumber, int time_ms ) {
  for ( int i = 0; i < count; i++ ) {
    pinMode( pinNumber, OUTPUT );
    digitalWrite( pinNumber, LOW );
    delay( time_ms );
    digitalWrite( pinNumber, HIGH );
    delay( time_ms );
  }
}

void tone_Buzzer_Blink_LED( int count, int frequency, int pinNumber, int time_ms ) {
  for ( int i = 0; i < count; i++ ) {
    pinMode( pinNumber, OUTPUT );
    digitalWrite( pinNumber, LOW );
    tone( buzzerPin, frequency );

    delay( time_ms );

    digitalWrite( pinNumber, HIGH );
    noTone( buzzerPin );
    delay( time_ms );
  }
}

int get_Pin_Input() {
  pin_Mode_To_Input();
  Serial.print( F( "Enter your guess: " ) );

  int result = 0;
  //Wait for the player to push a button
  while ( !(digitalRead( ledPins[0] ) == HIGH &&
            digitalRead( ledPins[1] ) == HIGH &&
            digitalRead( ledPins[2] ) == HIGH &&
            digitalRead( ledPins[3] ) == HIGH) ||
          result == 0 ) {
    for ( int i = 0; i < pinCount; i++ ) {
      //delay to wait for the push button to stabilise
      delay(10);
      if ( digitalRead( ledPins[i] ) != HIGH ) {
        tone( buzzerPin, buzzerFrequency[i] );
        result = i + 1;
        delay(5);
      }
    }
  }

  noTone( buzzerPin );
  Serial.println( result );
  return result;
}

void check_Input() {
  int i = 0;
  while ( win && i <= score ) {
    if ( currentSequenceLED[i] != get_Pin_Input() ) {
      win = false;
    }
    i++;
  }
}

void leds_Sequence() {
  for ( int i = 0; i <= score; i++ ) {
    for ( int j = 1; j <= pinCount; j++ ) {
      if ( currentSequenceLED[i] == j ) {
        Serial.print( F( "LED number : " ) );
        Serial.println( j );
        tone_Buzzer_Blink_LED( 1, buzzerFrequency[j - 1], ledPins[j - 1], delayDifficulty );
      }
    }
  }
}

void set_Difficulty( int newDifficulty ) {
  difficulty = newDifficulty;
}

int adjust_Delay_Difficulty() {
  delayDifficulty <= 100 ? delayDifficulty = 100 : delayDifficulty -= difficulty * 15;
  return delayDifficulty;
}

void display_Difficulty() {
  switch ( difficulty ) {
    case 1:
      Serial.println( F( "Difficulty : very easy" ) );
      print_On_Lcd( "Difficulty :", "Very easy", true );
      break;
    case 2:
      Serial.println( F( "Difficulty : easy" ) );
      print_On_Lcd( "Difficulty :", "Easy", true );
      break;
    case 3:
      Serial.println( F( "Difficulty : normal" ) );
      print_On_Lcd( "Difficulty :", "Normal", true );
      break;
    case 4:
      Serial.println( F( "Difficulty : hard" ) );
      print_On_Lcd( "Difficulty :", "Hard", true );
      break;
  }
}

void start_game() {

  Serial.println( F( "Push a button to start a game and set the difficulty :" ) );
  print_On_Lcd( "Push a button to ", "set difficulty", true );

  int result = 0;
  //Wait for the player to push a button
  while ( !(digitalRead( ledPins[0] ) == HIGH &&
            digitalRead( ledPins[1] ) == HIGH &&
            digitalRead( ledPins[2] ) == HIGH &&
            digitalRead( ledPins[3] ) == HIGH) ||
          result == 0 ) {
    for ( int i = 0; i < pinCount; i++ ) {
      if ( digitalRead( ledPins[i] ) != HIGH ) {
        set_Difficulty( i + 1 );
        result = i + 1;
        
        //Reset variables for the new game
        score = 0;
        delayDifficulty = 500;
        win = true;
      }
    }
  }

  delay( 500 );
  display_Difficulty();
  blink_LED_ALL( 1, 1000 );
}

void print_Score_On_Lcd() {
    lcd.setCursor( 0, 0 );
    char scoreMessage[ 17 ]; 
    sprintf( scoreMessage, "Score: %-10d", score );
    lcd.print( scoreMessage );
}

void loop() {
  delay( 1000 );
  Serial.println( F( "------------" ) );

  if ( win ) {
    Serial.print( F( "Blink time : " ) );
    Serial.println( delayDifficulty );

    currentSequenceLED[score] = random( 1, 5 ); //add a random number each round

    adjust_Delay_Difficulty();
    leds_Sequence();
    check_Input();

    score++;
    print_Score_On_Lcd();

  } else {
    blink_LED_ALL( 5, 100 );
    Serial.println( F( "Sorry you lost, try again!" ) );
    Serial.print( F( "Your score was: " ) );
    Serial.println( score );
    Serial.println( F( "------------" ) );
    print_On_Lcd( "", "Game over         ", false );
    delay( 5000 );
    start_game();
  }
}
