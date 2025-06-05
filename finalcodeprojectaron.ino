#include <Adafruit_CircuitPlayground.h>  // Include the library to access all built-in hardware features of the Circuit Playground Express board

#define MAX_SEQUENCE_LENGTH 5  // Define the maximum number of steps the memory sequence can have

// ——— Game State & Variables ———
int sequence[MAX_SEQUENCE_LENGTH];      // Stores the randomly generated pattern for the current level (0 = red, 1 = blue, 2 = yellow)
int userInput[MAX_SEQUENCE_LENGTH];     // Stores the sequence entered by the user for comparison

int level = 1;       // Keeps track of the current game level (starts at 1)
int score = 0;       // Player's current score, increases with each correct level
bool gameOver = false;   // Flag indicating if the game has ended due to a wrong input

bool lastSwitchState;    // Records the most recent state of the slide switch (used to detect changes)

// Volatile flags for interrupt-driven inputs — these get triggered immediately by hardware events
volatile bool slideToggled = false;     // Becomes true when the slide switch is toggled
volatile bool leftButtonPressed = false; // Becomes true when button A is pressed
volatile bool rightButtonPressed = false; // Becomes true when button B is pressed

// --------------------------------------------------
// setup(): Initializes hardware and interrupt setup
// --------------------------------------------------
void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  randomSeed(analogRead(A0));
  lastSwitchState = CircuitPlayground.slideSwitch();

  attachInterrupt(digitalPinToInterrupt(7), slideISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(4), leftButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(5), rightButtonISR, FALLING);

  Serial.println("Color Catch Memory: Press A to start");
}

// --------------------------------------------------
// loop(): Waits for user to press A to start/restart
// --------------------------------------------------
void loop() {
  if (leftButtonPressed) {
    leftButtonPressed = false;
    resetGame();
    playGame();
    Serial.println("Press A to play again");
  }
}

// --------------------------------------------------
// slideISR(): Sets flag when slide switch changes
// --------------------------------------------------
void slideISR() {
  slideToggled = true;
}

// --------------------------------------------------
// leftButtonISR(): Sets flag when button A is pressed
// --------------------------------------------------
void leftButtonISR() {
  leftButtonPressed = true;
}

// --------------------------------------------------
// rightButtonISR(): Sets flag when button B is pressed
// --------------------------------------------------
void rightButtonISR() {
  rightButtonPressed = true;
}

// --------------------------------------------------
// resetGame(): Resets game state and shows white flash
// --------------------------------------------------
void resetGame() {
  level = 1;
  score = 0;
  gameOver = false;
  lastSwitchState = CircuitPlayground.slideSwitch();
  flashWhite();
  delay(500);
}

// --------------------------------------------------
// playGame(): Main loop for generating and checking sequences
// --------------------------------------------------
void playGame() {
  while (level <= MAX_SEQUENCE_LENGTH && !gameOver) {
    generateSequence();
    playLevelIntro(level);
    showSequence();
    getUserInput();
    checkUserInput();
    delay(1000);
  }

  if (!gameOver) {
    successMelody();
    Serial.print("You won! Final Score: ");
    Serial.println(score);
  }
}

// --------------------------------------------------
// generateSequence(): Creates random color pattern for the level
// --------------------------------------------------
void generateSequence() {
  for (int i = 0; i < level; i++) {
    sequence[i] = random(3);
  }
}

// --------------------------------------------------
// showSequence(): Displays color/sound pattern to the player
// --------------------------------------------------
void showSequence() {
  for (int i = 0; i < level; i++) {
    playPattern(sequence[i]);
    delay(500);
    CircuitPlayground.clearPixels();
    delay(500);
  }
}

// --------------------------------------------------
// getUserInput(): Captures user inputs and maps them to colors
// --------------------------------------------------
void getUserInput() {
  for (int i = 0; i < level; i++) {
    bool got = false;

    while (!got) {
      if (leftButtonPressed && !rightButtonPressed) {
        userInput[i] = 0;
        playPattern(0);
        leftButtonPressed = false;
        got = true;
      } else if (rightButtonPressed && !leftButtonPressed) {
        userInput[i] = 1;
        playPattern(1);
        rightButtonPressed = false;
        got = true;
      } else if (slideToggled) {
        delay(5);
        slideToggled = false;
        userInput[i] = 2;
        playPattern(2);
        lastSwitchState = CircuitPlayground.slideSwitch();
        got = true;
      }

      delay(50);
    }

    CircuitPlayground.clearPixels();
    delay(300);
  }
}

// --------------------------------------------------
// checkUserInput(): Compares user input to correct sequence
// --------------------------------------------------
void checkUserInput() {
  for (int i = 0; i < level; i++) {
    if (userInput[i] != sequence[i]) {
      gameOver = true;
      CircuitPlayground.clearPixels();
      CircuitPlayground.playTone(200, 500);
      flashColor(255, 0, 0);
      Serial.print("Game Over. Score: ");
      Serial.println(score);
      return;
    }
  }

  score++;
  level++;
  CircuitPlayground.playTone(800, 150);
  flashColor(0, 255, 0);
}

// --------------------------------------------------
// flashColor(): Lights all LEDs to specified color briefly
// --------------------------------------------------
void flashColor(int r, int g, int b) {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, r, g, b);
  }
  delay(300);
  CircuitPlayground.clearPixels();
}

// --------------------------------------------------
// flashWhite(): Convenience function for white flash
// --------------------------------------------------
void flashWhite() {
  flashColor(255, 255, 255);
}

// --------------------------------------------------
// playPattern(): Plays specific animation/sound for input color
// --------------------------------------------------
void playPattern(int c) {
  switch (c) {
    case 0: sweepRed(); break;
    case 1: sparkleBlue(); break;
    case 2: rippleYellow(); break;
  }
}

// --------------------------------------------------
// sweepRed(): Red animation and sound (LEDs 0, 4, 8)
// --------------------------------------------------
void sweepRed() {
  const int leds[3] = {0, 4, 8};
  for (int p = 0; p < 3; p++) {
    for (int j = 0; j < 3; j++) {
      CircuitPlayground.setPixelColor(leds[j], 255, 0, 0);
    }
    CircuitPlayground.playTone(440, 100);
    delay(200);
    for (int j = 0; j < 3; j++) {
      CircuitPlayground.setPixelColor(leds[j], 0, 0, 0);
    }
    delay(200);
  }
}

// --------------------------------------------------
// sparkleBlue(): Blue sparkle animation with sound
// --------------------------------------------------
void sparkleBlue() {
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(random(10), 0, 0, 255);
    CircuitPlayground.playTone(660, 100);
    delay(200);
  }
  CircuitPlayground.clearPixels();
}

// --------------------------------------------------
// rippleYellow(): Yellow ripple animation and sound
// --------------------------------------------------
void rippleYellow() {
  for (int i = 0; i < 5; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    CircuitPlayground.setPixelColor(9 - i, 255, 255, 0);
    delay(200);
  }
  CircuitPlayground.playTone(740, 150);
  CircuitPlayground.clearPixels();
}

// --------------------------------------------------
// playLevelIntro(): Plays tones to indicate level number
// --------------------------------------------------
void playLevelIntro(int lvl) {
  int notes[] = {262, 330, 392, 523, 659};
  for (int i = 0; i < lvl && i < 5; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(200);
  }
}

// --------------------------------------------------
// successMelody(): Plays final win melody
// --------------------------------------------------
void successMelody() {
  int notes[] = {659, 783, 880, 1047};
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(150);
  }
}


