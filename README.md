# TvMenu - Gorilla Tag Fan Game Mod Menu

Welcome to **TvMenu**, a feature-rich, open-source native mod menu written in C++ designed specifically for Android-based Gorilla Tag fan games and clones. 

---

## ⚠️ Anti-Skid & Usage Policy

> **NOTICE:** This repository is strictly for educational purposes and personal customization. **Do not fork or skid this repository to claim it as your own.** 
> 
> * If you are caught skidding, rebranding, or claiming this menu as your original work, **you will be permanently blacklisted** from our repositories and associated communities.
> * This project is **not** intended for unauthorized forking. All code provided here is property of the original developers. 
> * **Looking for the compiled binary?** Do not try to compile it yourself if you just want to use it—head straight to the **Releases page** to download the ready-to-use `.so` file!

---

## 🚀 Features Included

### 🏃 Movement Mods
* Speed Boost
* Long Arms
* Fly
* Noclip
* Platform Gun
* Air Control
* Gravity Modifier
* Spider-Man (Wall Climb Boost)
* Iron Man (Hand Thrusters)
* Auto-Slide

### 👁️ Visual & Render Mods
* Fullbright
* ESP (Player Chams)
* Tracers
* Bone ESP
* NameTags
* FOV Changer
* Third-Person Camera
* Custom Skybox
* X-Ray
* FPS Counter

### 🛡️ Safety & Protection Mods
* Anti-Ban
* Anti-Report
* Report Block
* Name Spoofer
* ID Spoofer
* RPC Spoofing
* Lobby Disconnect on Staff Join
* Stealth Hooks

### 🌐 Multiplayer & Trolling Mods
* Tag Gun
* Tag All
* Invis-All
* Sound Spam
* Rope Spaz / Fling
* Move All Ropes
* Aura Tag
* Ghost Monkey
* Crasher (Lobby Lag)

### 🎨 Cosmetic & Customization Mods
* Unlock All Cosmetics
* Custom Color Changer (RGB Cycle)
* Material Changer
* Custom Holdables
* Badge Unlocker
* Fake Hand Painter

### 🌍 World & Fun Mods
* Teleport to Players
* Teleport to Map Locations
* Time of Day Changer
* Water Walk
* No-Clip Trees
* Soundboard
* Path Recorder

### ⚙️ Settings & Menu Controls
* Menu Customizer (Theme/Colors)
* Keybind Editor
* Toggle Notifications
* Save Config
* Disable Menu Sounds
* FPS Limiter

---

## 📥 How to Download

If you are an end-user looking to use this menu:
1. **Do not fork or clone this repository.**
2. Go directly to the **Releases page** of this repository.
3. Download the compiled `libTvMenu.so` file ready for injection into your target APK.

---

## 📖 How to Use

This guide explains how to inject and use the compiled `libTvMenu.so` file into a Gorilla Tag fan game or clone APK.

### 📋 Requirements
* A target Android APK file (Gorilla Tag fan game/clone).
* An APK editor or decompilation tool (e.g., **APK Easy Tool**, **MT Manager**, or **APKTool M**).
* A signing tool or keystore to sign the modified APK.
* An Android device or emulator with developer options and USB debugging enabled.

### 🛠️ Step-by-Step Injection Guide

#### Step 1: Download the Binary
1. Go directly to the **Releases page** of this repository.
2. Download the compiled `libTvMenu.so` file (make sure you grab the correct architecture, usually `arm64-v8a` for modern devices).

#### Step 2: Decompile the Target APK
1. Open your APK management tool (like MT Manager or APK Easy Tool).
2. Select your target Gorilla Tag fan game APK and choose **Decompile** / **Extract** (or view contents).

#### Step 3: Place the `.so` File
1. Navigate inside the decompiled folder to the **`lib`** directory:
   `lib/arm64-v8a/` (or `lib/armeabi-v7a/` depending on your target app's architecture).
2. Paste the downloaded `libTvMenu.so` file into this folder alongside any existing native libraries.

#### Step 4: Hook the Library (If Not Auto-Loaded)
* *Note: Many menu bases automatically load via `JNI_OnLoad`, but some APKs require a smali hook.*
* If needed, open the main entry point activity (usually found in `smali/com/.../unity/player/UnityPlayerActivity.smali` or similar) and add a static load call inside the onCreate or static constructor block:
  ```smali
  invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
  # Load TvMenu
  const-string v0, "TvMenu"
  invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
