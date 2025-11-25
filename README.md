# 🌀 S.M. Sajjad Hossain JIM — Animated OpenGL Showcase

## 🎨 Overview
This project is a **C++ OpenGL** animated visualization featuring dynamic shapes, glowing effects, and text animation of the name:
> **S.M. Sajjad Hossain JIM**

It uses **GLUT** and **OpenGL** to render smooth transitions between **three different animations**, each with distinct color, rotation, and movement effects. The project is optimized for macOS but also runs on Linux and Windows.

---

## ✨ Features
✅ 3 unique animations played in sequence  
✅ Smooth color transitions and glowing effects  
✅ Dynamic name animation with visibility enhancements  
✅ Custom timing and transitions  
✅ Lightweight and runs directly from terminal  

---

## 🧠 Tech Stack
- **Language:** C++17  
- **Graphics Library:** OpenGL / GLUT  
- **Platform:** macOS (works on Linux/Windows too)  

---

## 🧩 Folder Structure
```
Jim-OpenGL-Animation/
│
├── main.cpp            # Main OpenGL animation code
├── dependencies/
│   ├── include/        # (Optional) Headers for GLUT if using local build
│   └── library/        # (Optional) Library files for linking
├── README.md           # This file
└── build/              # (Optional) Compiled output
```

---

## 💻 Installation Guide (For macOS)

Follow these steps carefully 👇

### 1. 🧱 Install Homebrew (if not already)
Open Terminal and run:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 2. 📦 Install OpenGL + GLUT
```bash
brew install freeglut
```
> macOS already includes OpenGL, but we install `freeglut` for additional functionality.

---

## ⚙️ Build & Run the Project

### 1. Navigate to the project folder:
```bash
cd path/to/Jim-OpenGL-Animation
```

### 2. Compile using `clang++`:
```bash
clang++ main.cpp -std=c++17 -framework OpenGL -framework GLUT -o JimAnimation
```

> 💡 If you are using a local GLUT installation, link it manually:
```bash
clang++ main.cpp -I/dependencies/include -L/dependencies/library -lglut -lGLU -lGL -o JimAnimation
```

### 3. Run the program:
```bash
./app
```

---

## 🎞️ Animation Description

### 🩵 Animation 1 – Glowing Triangles
Rotating glowing triangles create a base ambient background with soft transitions.

### 💜 Animation 2 – Rotating Text
Your name “S.M. Sajjad Hossain JIM” appears in vibrant colors, spinning and scaling smoothly.

### 🧡 Animation 3 – Wave Transition
A blend of colors ripple through the screen while your name gently pulses and glows.

Each animation starts at a **different time** to create a cinematic sequence.

---

## 🎛️ Controls
| Key | Action |
|-----|--------|
| `ESC` | Exit the program |
| `r` | Restart animations |
| `space` | Pause/Resume |

---

## 🪄 Tips for Better Performance
- Close background apps when running heavy OpenGL animations.  
- Use **full-screen mode** for the best experience (you can enable it in the code by setting window size to screen width/height).  
- Try increasing `glutTimerFunc` intervals for slower transitions.

---

## 🧑‍💻 Author
**👤 S.M. Sajjad Hossain JIM**  
🎓 Computer Science & Engineering Student  
💻 Full Stack Web Developer | OpenGL Enthusiast  

---

## 📜 License
This project is open-source under the **MIT License** — feel free to use and modify it for learning or creative projects.
