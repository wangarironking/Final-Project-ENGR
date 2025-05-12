#include <Adafruit_CircuitPlayground.h>  // Library to control CPX board's buttons, LEDs, speaker, etc.

#define MAX_SEQUENCE_LENGTH 5  // Maximum number of levels/sequence steps in the game

// Arrays to hold the correct pattern and the player's input
int sequence[MAX_SEQUENCE_LENGTH];
int userInput[MAX_SEQUENCE_LENGTH];

// Track current level and score
int level = 1;
int score = 0;
bool gameOver = false;  // Flag to indicate if the player has failed the sequence

// Track the previous position of the slide switch (used for detecting yellow input)
bool lastSwitchState;

void setup() {
  Serial.begin(9600);  // Enable serial monitor for debugging
  CircuitPlayground.begin();  // Initialize Circuit Playground hardware
  randomSeed(analogRead(A0));  // Seed the random number generator for varied sequences
  lastSwitchState = CircuitPlayground.slideSwitch();  // Store the initial slide switch state
  Serial.println("Color Catch Memory: Press A to start");  // Prompt user to begin game
}

void loop() {
  // Wait for the player to press the A (left) button to start a new game
  if (CircuitPlayground.leftButton()) {
    resetGame();   // Reset game variables and flash ready signal
    playGame();    // Start the gameplay loop
  }
}

// Resets the game state and flashes white to show readiness
void resetGame() {
  level = 1;
  score = 0;
  gameOver = false;
  lastSwitchState = CircuitPlayground.slideSwitch();  // Reset slide switch tracker
  flashWhite();  // Signal game is ready
  delay(500);
}

// Main game loop that progresses through levels or ends on mistake
void playGame() {
  while (level <= MAX_SEQUENCE_LENGTH && !gameOver) {
    generateSequence();     // Create a random sequence for current level
    playLevelIntro(level);  // Play a melody indicating the level number
    showSequence();         // Display the sequence to the player
    getUserInput();         // Wait for and capture player input
    checkUserInput();       // Compare input to the sequence
    delay(1000);            // Brief pause before next level or ending
  }

  // If the player completes all levels without a mistake
  if (!gameOver) {
    successMelody();  // Play winning sound
    Serial.print("You won! Final Score: ");
    Serial.println(score);
  }
}

// Randomly fill the sequence array for the current level
// Each number represents a color: 0 = Red, 1 = Blue, 2 = Yellow
void generateSequence() {
  for (int i = 0; i < level; i++) {
    sequence[i] = random(3);
  }
}

// Displays the current sequence using light and sound patterns
void showSequence() {
  for (int i = 0; i < level; i++) {
    playPattern(sequence[i]);       // Show each color one by one
    delay(500);
    CircuitPlayground.clearPixels();  // Clear LEDs between steps
    delay(500);
  }
}

// Captures user input by waiting for button press or switch toggle
void getUserInput() {
  for (int i = 0; i < level; i++) {
    bool got = false;  // Wait until valid input is received
    while (!got) {
      // Red input: A button only
      if (CircuitPlayground.leftButton() && !CircuitPlayground.rightButton()) {
        userInput[i] = 0;
        playPattern(0);
        got = true;

      // Blue input: B button only
      } else if (CircuitPlayground.rightButton() && !CircuitPlayground.leftButton()) {
        userInput[i] = 1;
        playPattern(1);
        got = true;

      // Yellow input: Slide switch toggle (change in state)
      } else {
        bool curr = CircuitPlayground.slideSwitch();
        if (curr != lastSwitchState) {
          userInput[i] = 2;
          playPattern(2);
          lastSwitchState = curr;  // Update state to detect only one toggle
          got = true;
        }
      }
      delay(100);  // Small debounce delay
    }
    CircuitPlayground.clearPixels();  // Clear after input
    delay(300);  // Brief pause before next input
  }
}

// Compare the user's input with the original sequence
void checkUserInput() {
  for (int i = 0; i < level; i++) {
    if (userInput[i] != sequence[i]) {
      // If user gets a step wrong
      gameOver = true;
      CircuitPlayground.clearPixels();
      CircuitPlayground.playTone(200, 500);  // Error tone
      flashColor(255, 0, 0);  // Flash red = failure
      Serial.print("Game Over. Score: ");
      Serial.println(score);
      return;
    }
  }

  // If all steps were correct:
  score++;  // Increase score
  level++;  // Advance to next level
  CircuitPlayground.playTone(800, 150);  // Success tone
  flashColor(0, 255, 0);  // Flash green = success
}

// Light up all LEDs to a specific color briefly
void flashColor(int r, int g, int b) {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, r, g, b);
  }
  delay(300);
  CircuitPlayground.clearPixels();
}

// Flash white to indicate the game is ready to begin
void flashWhite() {
  flashColor(255, 255, 255);
}

// Helper function to call the appropriate animation for each color
void playPattern(int c) {
  switch (c) {
    case 0: sweepRed();    break;
    case 1: sparkleBlue(); break;
    case 2: rippleYellow();break;
  }
}

// === Color Patterns (LED + Sound) ===

// Red pattern: Pulse LEDs 0, 4, 8 three times with tone
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

// Blue pattern: Light up 4 random blue pixels in sequence
void sparkleBlue() {
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(random(10), 0, 0, 255);
    CircuitPlayground.playTone(660, 100);
    delay(200);
  }
  CircuitPlayground.clearPixels();
}

// Yellow pattern: Ripple effect from outside to center
void rippleYellow() {
  for (int i = 0; i < 5; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    CircuitPlayground.setPixelColor(9 - i, 255, 255, 0);
    delay(200);
  }
  CircuitPlayground.playTone(740, 150);
  CircuitPlayground.clearPixels();
}

// Intro melody: Plays ascending tones based on level number
void playLevelIntro(int lvl) {
  int notes[] = {262, 330, 392, 523, 659};
  for (int i = 0; i < lvl && i < 5; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(200);
  }
}

// Victory melody: Played when all levels are completed successfully
void successMelody() {
  int notes[] = {659, 783, 880, 1047};
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(150);
  }
}
