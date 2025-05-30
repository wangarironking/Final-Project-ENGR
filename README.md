# Color Catch Memory (Adafruit Circuit Playground Express Game)

**Color Catch Memory** is a memory-based reaction game built for the Adafruit Circuit Playground Express (CPX). Players must watch a sequence of colors and sounds, then replicate them using button presses and the slide switch. The sequence grows longer with each level.

---

## 🕹️ Game Overview

The CPX displays a pattern of red, blue, and yellow using its onboard LEDs and sound. The player repeats the pattern using:
- **Left Button A** → Red
- **Right Button B** → Blue
- **Slide Switch Toggle** → Yellow

The game gets progressively harder with longer patterns up to 5 levels.

---

## 🎯 Objective

Correctly repeat all the randomly generated color patterns across 5 levels without making a mistake. If you enter the wrong pattern, the game ends with an error tone and red flash.

---

## 🔁 Game Flow

1. Press **Button A** to start.
2. The game generates a color sequence for the current level.
3. The player watches and listens to the pattern.
4. The player inputs the same pattern using:
   - Button A for red
   - Button B for blue
   - Slide switch for yellow
5. If correct:
   - Progress to the next level.
   - LEDs flash green and a tone plays.
6. If incorrect:
   - The game ends.
   - LEDs flash red and an error tone plays.
7. Win the game by completing all 5 levels.

---

## 🎮 Controls

| Input               | Action        | Color |
|--------------------|---------------|-------|
| Button A (left)    | Input "Red"   | 🔴     |
| Button B (right)   | Input "Blue"  | 🔵     |
| Slide Switch       | Input "Yellow"| 🟡     |

Interrupts are used to handle these inputs smoothly during gameplay.

---

## 🔊 Output & Feedback

- **Visual**: LEDs flash red, blue, or yellow based on the current pattern.
- **Audio**: Tones accompany each color (e.g., 440 Hz for red).
- **Success**: A short victory melody plays when all levels are completed.
- **Failure**: An error tone and red LED flash on mismatch.

---

## 🔧 Technical Features

- Uses **interrupts** for responsive input.
- Incorporates **LED animations** and tones for immersive feedback.
- Game logic managed with arrays and level progression.
- Debounce logic and cle
