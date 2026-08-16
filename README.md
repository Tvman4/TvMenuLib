# TvMenu - Gorilla Tag Fan Game Mod Menu

Welcome to TvMenu, a feature-rich, open-source native mod menu written in C++ designed specifically for Android-based Gorilla Tag fan games and clones. 

---

## ⚠️ Anti-Skid & Usage Policy

> NOTICE: This repository is strictly for educational purposes and personal customization. Do not fork or skid this repository to claim it as your own. 
> 
> * If you are caught skidding, rebranding, or claiming this menu as your original work, you will be permanently blacklisted from our repositories and associated communities.
> * This project is not intended for unauthorized forking. All code provided here is property of the original developers. 
> * Looking for the compiled binary? Do not try to compile it yourself if you just want to use it—head straight to the Releases page to download the ready-to-use .so file!

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
1. Do not fork or clone this repository.
2. Go directly to the Releases page of this repository.
3. Download the compiled libTvMenu.so file ready for injection into your target APK.

---

## 📖 Complete Tutorial: Using Apktool GUI & No-Terminal Methods

This complete guide details how to use the standalone Apktool application interface to decompile your target APK, insert your native library, inject the const-string load hook, rebuild the application, and sign it for deployment.

### 📋 Prerequisites & Tools Needed
* Windows PC
* Apktool downloaded and opened via its application interface.
* Uber APK Signer (uber-apk-signer.jar)
* Target APK & your downloaded libTvMenu.so file (arm64-v8a).

---

### 🛠️ Step 1: Decompile the APK using Apktool
1. Open the Apktool application interface on your desktop.
2. Select your target Gorilla Tag fan game APK file inside the Apktool program window.
3. Choose the Decompile option to unpack all resources, AndroidManifest, and Smali source files into a working folder on your PC.

---

### 🛠️ Step 2: Insert the .so File
1. Open your decompiled game folder using your Windows File Explorer.
2. Navigate down the directory path: 
   lib/arm64-v8a/
   (Note: If the arm64-v8a folder does not exist inside lib/, create it manually).
3. Drag and drop your downloaded libTvMenu.so file directly into this arm64-v8a folder.

---

### 🛠️ Step 3: Add the Library Load Hook with const-string
To ensure the game loads your menu library automatically upon launch, inject the initialization block into your main activity smali file:

1. Navigate using File Explorer to:
   smali/com/unity3d/player/UnityPlayerActivity.smali
2. Open the file in any text editor (like Notepad or Notepad++) and search for the onCreate method declaration:
   ```smali
   .method protected onCreate(Landroid/os/Bundle;)V
   ```
3. Locate the super.onCreate instruction. Right underneath it, copy and paste this exact const-string Smali block:
   ```smali
   const-string v0, "TvMenu"
   
   invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
   ```
4. Save and close the file.

### 🛠️ Step 4: Rebuilding the APK using Apktool
1. Return to your open Apktool application window.
2. Select your modified folder and click the Build / Compile button to pack everything back into an unsigned APK file (e.g., modded_unsigned.apk).

### 🛠️ Step 5: Signing the APK
Android requires all installed APKs to be cryptographically signed.
1. Place your uber-apk-signer.jar file in your workspace directory.
2. Open your terminal/command prompt strictly for signing, and run the signing command:
   ```bash
   java -jar uber-apk-signer.jar --apks modded_unsigned.apk
   ```
3. The tool will sign the build automatically and output your final installable file:
   modded_unsigned-aligned-debugSigned.apk.

### 🎮 In-Game Controls
1. Uninstall any existing version of the game from your device to prevent signature conflicts.
2. Install your newly signed modded APK (modded_unsigned-aligned-debugSigned.apk).
3. Open the game, press the Y button on your controller, and your menu will appear right on your hand ready for action!
