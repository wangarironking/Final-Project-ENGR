#include <Adafruit_CircuitPlayground.h>

#define MAX_SEQUENCE_LENGTH 5

// ——— Game State & Variables ———
int sequence[MAX_SEQUENCE_LENGTH];      // Stores the correct pattern for the level
int userInput[MAX_SEQUENCE_LENGTH];     // Stores the user's input

int level = 1;       // Tracks current level
int score = 0;       // Tracks current score
bool gameOver = false;   // Indicates if player has lost

bool lastSwitchState;    // Stores the last slide switch state

// Volatile flags for interrupt-driven inputs
volatile bool slideToggled = false;
volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  randomSeed(analogRead(A0));
  lastSwitchState = CircuitPlayground.slideSwitch();

  // Attach interrupts for inputs
  attachInterrupt(digitalPinToInterrupt(7), slideISR, CHANGE);      // Slide switch
  attachInterrupt(digitalPinToInterrupt(4), leftButtonISR, FALLING); // Button A
  attachInterrupt(digitalPinToInterrupt(5), rightButtonISR, FALLING); // Button B

  Serial.println("Color Catch Memory: Press A to start");
}

void loop() {
  // Start game when left button (A) is pressed
  if (leftButtonPressed) {
    leftButtonPressed = false;
    resetGame();     // Reset game state
    playGame();      // Start game loop
    Serial.println("Press A to play again");
  }
}

// —————————————————————
// INTERRUPT HANDLERS
// —————————————————————
void slideISR() {
  slideToggled = true;
}

void leftButtonISR() {
  leftButtonPressed = true;
}

void rightButtonISR() {
  rightButtonPressed = true;
}

// —————————————————————
// GAME LOGIC
// —————————————————————

// Resets the game state and flashes white to indicate a reset
void resetGame() {
  level = 1;
  score = 0;
  gameOver = false;
  lastSwitchState = CircuitPlayground.slideSwitch();
  flashWhite();     // Flash to signal reset
  delay(500);
}

// Main game loop: show, input, check; repeat until max level or failure
void playGame() {
  while (level <= MAX_SEQUENCE_LENGTH && !gameOver) {
    generateSequence();      // Create new random pattern
    playLevelIntro(level);   // Play level intro tones
    showSequence();          // Show color/sound sequence to user
    getUserInput();          // Get input from user
    checkUserInput();        // Check if user input matches
    delay(1000);
  }

  // If player wins all levels without error
  if (!gameOver) {
    successMelody();         // Victory sound
    Serial.print("You won! Final Score: ");
    Serial.println(score);
  }
}

// —————————————————————
// GAME COMPONENTS
// —————————————————————

// Fill the sequence[] array with random values (0 = red, 1 = blue, 2 = yellow)
void generateSequence() {
  for (int i = 0; i < level; i++) {
    sequence[i] = random(3);
  }
}

// Show the generated color/sound pattern to the user
void showSequence() {
  for (int i = 0; i < level; i++) {
    playPattern(sequence[i]);
    delay(500);
    CircuitPlayground.clearPixels();
    delay(500);
  }
}

// —————————————————————
// INPUT COLLECTION (with flags)
// —————————————————————

// Collects user input using interrupts for buttons and switch
void getUserInput() {
  for (int i = 0; i < level; i++) {
    bool got = false;

    while (!got) {
      if (leftButtonPressed && !rightButtonPressed) {
        userInput[i] = 0;          // Red input
        playPattern(0);
        leftButtonPressed = false;
        got = true;
      } else if (rightButtonPressed && !leftButtonPressed) {
        userInput[i] = 1;          // Blue input
        playPattern(1);
        rightButtonPressed = false;
        got = true;
      } else if (slideToggled) {
        delay(5);                  // Debounce
        slideToggled = false;
        userInput[i] = 2;          // Yellow input
        playPattern(2);
        lastSwitchState = CircuitPlayground.slideSwitch();
        got = true;
      }

      delay(50);  // Debounce and wait
    }

    CircuitPlayground.clearPixels();
    delay(300);
  }
}

// —————————————————————
// RESPONSE CHECKING
// —————————————————————

// Compares user input to correct sequence; ends game if wrong
void checkUserInput() {
  for (int i = 0; i < level; i++) {
    if (userInput[i] != sequence[i]) {
      gameOver = true;
      CircuitPlayground.clearPixels();
      CircuitPlayground.playTone(200, 500);  // Error tone
      flashColor(255, 0, 0);                 // Flash red
      Serial.print("Game Over. Score: ");
      Serial.println(score);
      return;
    }
  }

  // If correct, move to next level and reward player
  score++;
  level++;
  CircuitPlayground.playTone(800, 150);
  flashColor(0, 255, 0);  // Flash green
}

// —————————————————————
// VISUAL AND AUDIO FEEDBACK
// —————————————————————

void flashColor(int r, int g, int b) {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, r, g, b);
  }
  delay(300);
  CircuitPlayground.clearPixels();
}

void flashWhite() {
  flashColor(255, 255, 255);
}

// Choose animation + tone based on pattern value
void playPattern(int c) {
  switch (c) {
    case 0: sweepRed(); break;
    case 1: sparkleBlue(); break;
    case 2: rippleYellow(); break;
  }
}

// Red: pulses LEDs 0, 4, 8
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

// Blue: sparkles random LEDs with tone
void sparkleBlue() {
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(random(10), 0, 0, 255);
    CircuitPlayground.playTone(660, 100);
    delay(200);
  }
  CircuitPlayground.clearPixels();
}

// Yellow: ripple pattern from outside to center
void rippleYellow() {
  for (int i = 0; i < 5; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    CircuitPlayground.setPixelColor(9 - i, 255, 255, 0);
    delay(200);
  }
  CircuitPlayground.playTone(740, 150);
  CircuitPlayground.clearPixels();
}

// Play tones indicating the current level number
void playLevelIntro(int lvl) {
  int notes[] = {262, 330, 392, 523, 659};
  for (int i = 0; i < lvl && i < 5; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(200);
  }
}

// Celebrate the player winning with a melody
void successMelody() {
  int notes[] = {659, 783, 880, 1047};
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(150);
  }
}


