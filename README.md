# Jeu_Video_Flappy_JUMP

# 🏀 Basket Arcade 2D Game (Qt & C++)

A dynamic 2D arcade game built from scratch using **C++** and the **Qt Framework**. The game implements classic physics-based side-scrolling mechanics (similar to *Flappy Bird*), featuring local database user persistence, a real-time leaderboard, and responsive keyboard controls.

---

## 🛠️ Core Technologies & Architecture

* **Language:** C++ (OOP Principles)
* **Framework:** Qt Creator (Qt Widgets & Qt GUI)
* **Graphics Engine:** `QGraphicsScene` & `QGraphicsView` for managing dynamic entities, parallax background scrolling, and visual layer management ($Z$-value depth).
* **Database Engine:** `QSqlDatabase` (SQLite embedded local database) for reliable user authentication and score tracking.
* **Timing & Game Loop:** `QTimer` handles the asynchronous gameplay physics loop, real-time collision detection, and automated obstacle spawning.

---

## 💡 Key Features

### 1. User Authentication (SQLite)
* **Secure Local Database:** All player data is stored locally via an embedded SQLite database.
* **Registration & Validation:** New users can sign up by providing a unique email address, age, and password.
* **Profile Login:** Fetches previous stats and displays the user's specific high score (`Best: X`) upon successful login.

### 2. Gameplay Mechanics & Control System
* **Gravity Physics:** Continuous gravity scale affects the ball's velocity over time. 
* **Dynamic Scaling:** Obstacle speed and gravity scale slightly increase as your score goes up, creating a progressive challenge.
* **Fluid Controls:** 
  * `Spacebar`: Propels the ball upward by injecting an impulse vector against gravity.
  * `Enter` / `Return`: Instantly triggers a **Pause / Play** state, freezing the timers without losing application window focus.

### 3. Fail State Logic & Competitive Leaderboard
* **Collision Detection:** The game calculates bounding rect intersections between the ball and spawning ring obstacles. Failing to pass through the ring or hitting the floor triggers a Game Over sequence.
* **Auto-Save:** High scores are automatically synchronized with the SQLite database upon failure.
* **Top 3 Leaderboard:** A dynamic database query pulls the top 3 scores across all registered users and renders a mini-podium layout on the Game Over screen.

---

## 🎮 How to Control

| Key | Action |
|---|---|
| **`Spacebar`** | Jump / Fly Upwards |
| **`Enter` / `Return`** | Toggle Pause / Play |

---
