# Color Catch Memory Game (Arduino CPX)

A Simon-style memory game built for the Adafruit Circuit Playground Express (CPX) board.  
Test your memory and reflexes by repeating increasingly difficult sequences of light and sound using button and switch inputs!

---

## Game Concept

**Color Catch** is a pattern memory game where the CPX lights up a sequence of colored LEDs and plays tones. Your task is to **repeat the sequence using the correct input controls**. The game becomes more challenging with each level.

---

## Objective

- Memorize and repeat the correct sequence of colors.
- Each round adds a new step to the sequence.
- The game ends if a mistake is made or you reach the final level.

---

## Controls (Inputs)

| Color   | Action                       | Input                          |
|---------|------------------------------|--------------------------------|
| 🔴 Red   | Left button only             | `CircuitPlayground.leftButton()`  |
| 🔵 Blue  | Right button only            | `CircuitPlayground.rightButton()` |
| 🟡 Yellow| Slide switch toggle (state change) | `CircuitPlayground.slideSwitch()`  |

---

## Outputs

- **LEDs** display custom animations for each color (red pulse, blue sparkle, yellow ripple).
- **Speaker** plays distinct tones for each color and melodies for success/failure.
- **Serial Monitor** provides game status updates and final score.

---

## Game Rules

1. Press **A (left button)** to start the game.
2. A sequence of colors will play with lights and sounds.
3. Repeat the sequence using:
   - A for Red
   - B for Blue
   - Toggle the switch for Yellow
4. If your input is correct:
   - You advance to the next level.
   - Score increases by 1.
5. If incorrect:
   - A red flash and low tone will play.
   - The game ends and your score is shown.
6. Beat all 5 levels to win!

---

## Requirements

- Adafruit Circuit Playground Express (CPX)
- Arduino IDE with `Adafruit_CircuitPlayground` library
- USB cable for uploading

---




