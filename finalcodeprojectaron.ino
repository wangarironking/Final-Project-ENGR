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


void setup() {
  Serial.begin(9600);  // Start serial communication for debugging output
  CircuitPlayground.begin();  // Initialize the Circuit Playground's hardware components

  randomSeed(analogRead(A0));  // Randomize the seed using analog noise to make sequences unpredictable
  lastSwitchState = CircuitPlayground.slideSwitch();  // Record initial state of the slide switch

  // Attach interrupt functions to respond to inputs immediately when triggered
  attachInterrupt(digitalPinToInterrupt(7), slideISR, CHANGE);       // Call slideISR when the slide switch changes state
  attachInterrupt(digitalPinToInterrupt(4), leftButtonISR, FALLING); // Call leftButtonISR when button A is pressed
  attachInterrupt(digitalPinToInterrupt(5), rightButtonISR, FALLING); // Call rightButtonISR when button B is pressed

  Serial.println("Color Catch Memory: Press A to start");  // Instruction message shown in Serial Monitor
}


void loop() {
  // Start the game when the user presses button A
  if (leftButtonPressed) {
    leftButtonPressed = false;  // Reset the flag
    resetGame();     // Clear previous game state and start fresh
    playGame();      // Enter the main game loop
    Serial.println("Press A to play again");  // After the game ends, prompt to restart
  }
}


// 
// INTERRUPT HANDLERS
// These functions are triggered immediately by input hardware (button/switch)
// 

void slideISR() {
  slideToggled = true;  // Set the flag when the slide switch changes
}

void leftButtonISR() {
  leftButtonPressed = true;  // Set the flag when button A is pressed
}

void rightButtonISR() {
  rightButtonPressed = true; // Set the flag when button B is pressed
}


// 
// GAME LOGIC
// 

void resetGame() {
  level = 1;                           // Reset to level 1
  score = 0;                           // Reset score
  gameOver = false;                   // Clear game over status
  lastSwitchState = CircuitPlayground.slideSwitch();  // Update the known switch state
  flashWhite();                       // Flash white LEDs to indicate reset
  delay(500);                         // Wait briefly
}

void playGame() {
  // Main game loop — runs until max level reached or player makes a mistake
  while (level <= MAX_SEQUENCE_LENGTH && !gameOver) {
    generateSequence();      // Create a new pattern based on the current level
    playLevelIntro(level);   // Play tones to indicate level number
    showSequence();          // Show the pattern to the user
    getUserInput();          // Collect the user's input
    checkUserInput();        // Compare input to the correct pattern
    delay(1000);             // Short pause before next level
  }

  // If all levels completed successfully
  if (!gameOver) {
    successMelody();         // Play celebratory melody
    Serial.print("You won! Final Score: ");  // Show final score
    Serial.println(score);
  }
}


// 
// GAME COMPONENTS
// 

void generateSequence() {
  for (int i = 0; i < level; i++) {
    sequence[i] = random(3);  // Generate a random number: 0 = red, 1 = blue, 2 = yellow
  }
}

void showSequence() {
  for (int i = 0; i < level; i++) {
    playPattern(sequence[i]);          // Display and play pattern element
    delay(500);                        // Wait before turning off LEDs
    CircuitPlayground.clearPixels();   // Turn off all LEDs
    delay(500);                        // Wait before showing next
  }
}


// 
// INPUT COLLECTION (with flags)
//

void getUserInput() {
  for (int i = 0; i < level; i++) {
    bool got = false;  // Keeps looping until a valid input is detected

    while (!got) {
      if (leftButtonPressed && !rightButtonPressed) {
        userInput[i] = 0;          // Register red (button A)
        playPattern(0);            // Give feedback
        leftButtonPressed = false;
        got = true;
      } else if (rightButtonPressed && !leftButtonPressed) {
        userInput[i] = 1;          // Register blue (button B)
        playPattern(1);
        rightButtonPressed = false;
        got = true;
      } else if (slideToggled) {
        delay(5);                  // Debounce the switch
        slideToggled = false;
        userInput[i] = 2;          // Register yellow (slide switch)
        playPattern(2);
        lastSwitchState = CircuitPlayground.slideSwitch();  // Update last switch state
        got = true;
      }

      delay(50);  // Debounce delay to prevent false triggers
    }

    CircuitPlayground.clearPixels();  // Turn off LEDs after each input
    delay(300);  // Pause between inputs
  }
}


// 
// RESPONSE CHECKING
// 

void checkUserInput() {
  for (int i = 0; i < level; i++) {
    if (userInput[i] != sequence[i]) {
      gameOver = true;  // End game if any input doesn't match
      CircuitPlayground.clearPixels();
      CircuitPlayground.playTone(200, 500);  // Error sound
      flashColor(255, 0, 0);                 // Red flash = error
      Serial.print("Game Over. Score: ");
      Serial.println(score);
      return;
    }
  }

  // If input is correct:
  score++;                 // Add to score
  level++;                 // Move to next level
  CircuitPlayground.playTone(800, 150);  // Success tone
  flashColor(0, 255, 0);   // Green flash = correct
}


// 
// VISUAL AND AUDIO FEEDBACK
// 

void flashColor(int r, int g, int b) {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, r, g, b);  // Set all 10 LEDs to the given color
  }
  delay(300);                // Keep the color on briefly
  CircuitPlayground.clearPixels();  // Turn LEDs off
}

void flashWhite() {
  flashColor(255, 255, 255);  // White flash = reset signal
}

void playPattern(int c) {
  // Selects the appropriate LED animation and tone based on the pattern value
  switch (c) {
    case 0: sweepRed(); break;      // Red = button A
    case 1: sparkleBlue(); break;   // Blue = button B
    case 2: rippleYellow(); break;  // Yellow = slide switch
  }
}


// Red animation: flashes LEDs 0, 4, and 8 in sequence
void sweepRed() {
  const int leds[3] = {0, 4, 8};
  for (int p = 0; p < 3; p++) {
    for (int j = 0; j < 3; j++) {
      CircuitPlayground.setPixelColor(leds[j], 255, 0, 0);  // Set to red
    }
    CircuitPlayground.playTone(440, 100);  // Play red tone
    delay(200);
    for (int j = 0; j < 3; j++) {
      CircuitPlayground.setPixelColor(leds[j], 0, 0, 0);    // Turn off
    }
    delay(200);
  }
}


// Blue animation: sparkles one random LED at a time
void sparkleBlue() {
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(random(10), 0, 0, 255);  // Set one LED to blue
    CircuitPlayground.playTone(660, 100);  // Play blue tone
    delay(200);
  }
  CircuitPlayground.clearPixels();  // Turn off all LEDs
}


// Yellow animation: lights up LEDs from edges inward
void rippleYellow() {
  for (int i = 0; i < 5; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);         // Yellow from left
    CircuitPlayground.setPixelColor(9 - i, 255, 255, 0);     // Yellow from right
    delay(200);
  }
  CircuitPlayground.playTone(740, 150);  // Play yellow tone
  CircuitPlayground.clearPixels();       // Clear LEDs after animation
}


// Plays a tone sequence based on the current level
void playLevelIntro(int lvl) {
  int notes[] = {262, 330, 392, 523, 659};  // Array of tones for levels 1–5
  for (int i = 0; i < lvl && i < 5; i++) {
    CircuitPlayground.playTone(notes[i], 120);  // Play one tone per level count
    delay(200);
  }
}


// Victory melody for successful completion of all levels
void successMelody() {
  int notes[] = {659, 783, 880, 1047};  // Final celebration melody
  for (int i = 0; i < 4; i++) {
    CircuitPlayground.playTone(notes[i], 120);
    delay(150);
  }
}


