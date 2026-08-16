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

## 📖 Complete Decompile Tutorial: Injection, Rebuilding, and Signing

This complete guide details the **decompile method** using Apktool to unpack your target APK, insert your native library, inject the `const-string` load hook, rebuild the application, and sign it for deployment.

### 📋 Prerequisites & Tools Needed
* **Java Runtime Environment (JRE)**
* **Apktool Executable (`apktool.jar`)**
* **Uber APK Signer (`uber-apk-signer.jar`)**
* **Target APK** & your downloaded `libTvMenu.so` file (`arm64-v8a`).

---

### 🛠️ Step 1: Decompile the APK
1. Place `apktool.jar`, your target game APK, and your `libTvMenu.so` file in a dedicated workspace folder on your computer.
2. Open your command prompt or terminal in that workspace directory.
3. Execute the **decompile** command to unpack the APK:
   ```bash
   java -jar apktool.jar d target_game.apk -o decoded_game
