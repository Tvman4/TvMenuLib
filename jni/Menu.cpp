#include "Menu.h"
#include "Mods.h"
#include <android/log.h>

#define TAG "GtagCopyMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool Menu::isOpen = true;
bool Menu::notificationsEnabled = true;
int Menu::currentCategoryIndex = 0;
int Menu::selectedModIndex = 0;

const std::vector<Category> Menu::categories = {
    {"🏃 Movement Mods", {
        "Speed Boost", "Long Arms", "Fly", "Noclip", "Platform Gun", 
        "Air Control", "Gravity Modifier", "Spider-Man (Wall Climb Boost)", 
        "Iron Man (Hand Thrusters)", "Auto-Slide", "Blink / Teleport Dash", 
        "Super Jump", "Infinite Stamina", "Wall Run", "Slide Control", 
        "Swimming Boost", "Crawl Modifier", "Zero-G Float", "Momentum Keeper", 
        "Upward Booster", "Auto-Catch Ledges", "Ice Physics (Zero Friction)", 
        "Bounce Pad Spawner", "Hookshot", "Phase Dash", "PC Monke"
    }},
    {"👁️ Visual & Render", {
        "Fullbright", "ESP (Player Chams)", "Tracers", "Bone ESP", "NameTags", 
        "FOV Changer", "Third-Person Camera", "Custom Skybox", "X-Ray", 
        "FPS Counter", "Wireframe Mode", "Rainbow World", "Vertex Distortion", 
        "Hitbox Visualizer", "Player Direction Arrows", "Freecam", "Scope Overlay", 
        "Custom Crosshair", "Weather Overlay (Rain/Snow)", "Brightness Slider", 
        "Shadow Disabler", "Particle Effect Spawner", "Custom Fog Density", 
        "Headlamp / Flashlight", "Distance Culling Toggle"
    }},
    {"🛡️ Safety & Protection", {
        "Anti-Ban", "Anti-Report", "Report Block", "Name Spoofer", "ID Spoofer", 
        "RPC Spoofing", "Lobby Disconnect on Staff Join", "Stealth Hooks", 
        "Anti-Crash", "Anti-Freeze", "Anti-Audio Spam", "Anti-Teleport Grab", 
        "Auto-Reconnect", "Ghost Mode (Invisible to Staff)", "IP Masker", 
        "Ping Fixer / Latency Smoother", "Packet Loss Shield", 
        "Bad Word Filter Bypass / Protection", "Secure Handshake", "Log Cleaner", 
        "Anti-Kick", "Session Guard"
    }},
    {"🌐 Multiplayer & Trolling", {
        "Tag Gun", "Tag All", "Invis-All", "Sound Spam", "Rope Spaz / Fling", 
        "Move All Ropes", "Aura Tag", "Ghost Monkey", "Crasher (Lobby Lag)", 
        "Fake Tag", "Mute All", "Kick Gun", "Freeze Player", "Orbit Player", 
        "Copy Cat (Mirror Movement)", "Hand Lock", "Audio Pitch Shift", 
        "Voice Changer Filter", "Spam Chat / Notifications", "Randomize Player Colors", 
        "Teleport All to Me", "Swap Positions", "Invert Controls (Target)", 
        "Slow Motion (Target)", "Balloon Attach", "Fake Join/Leave Messages", 
        "Spectate Player First-Person", "Auto-Tag Nearest", "Orbiting Proximity Alarm", 
        "Decoy Clone Spawner"
    }},
    {"🎨 Cosmetic & Customization", {
        "Unlock All Cosmetics", "Custom Color Changer (RGB Cycle)", "Material Changer", 
        "Custom Holdables", "Badge Unlocker", "FakeFingerPainter", "Custom Hat Spawner", 
        "Custom Badge Creator", "Glow Skin Texture", "Particle Trail Hand FX", 
        "Rainbow Hands", "Ghostly Transparency Effect", "Custom Name Tag Color", 
        "Holdable Scale Changer", "Dynamic Aura Effect", "Custom Finger Gestures", 
        "Golden Skin Plating", "Matrix Code Texture", "Chrome Reflection Skin", 
        "Animated Texture Cycling"
    }},
    {"🌍 World & Fun", {
        "Teleport to Players", "Teleport to Map Locations", "Time of Day Changer", 
        "Water Walk", "No-Clip Trees", "Soundboard", "Path Recorder", 
        "Custom Object Spawner", "Low Gravity World", "Map Exploder", 
        "Infinite Sound Echo", "Weather Controller (Storm/Fog)", "Mirror Everywhere", 
        "Custom Gravity Zones", "Portal Gun", "Target Practice Target Spawner", 
        "Breakable Prop Spawner", "Laser Beam Tool", "Giant Scale Mode", 
        "Tiny Scale Mode", "World Texture Swap", "Day/Night Fast Cycle"
    }},
    {"⚙️ Settings & Menu Controls", {
        "Menu Customizer (Theme/Colors)", "Keybind Editor", "Toggle Notifications", 
        "Save Config", "Disable Menu Sounds", "FPS Limiter", "Menu Sound Effects Volume", 
        "Auto-Load Config on Startup", "Menu Transparency / Opacity", 
        "Controller Vibration Toggle", "Menu Layout Position Switcher", 
        "Panic Key (Instant Close All)", "Export/Import Config Profile", 
        "Diagnostic Overlay (CPU/GPU Usage)"
    }},
    {"⚡ OP Mods", {
        "Master God Mode", "Ultimate Admin God-Mode", "Infinite Health / Anti-Tag Out", 
        "Infinite Shield Projector", "Supreme Connection Shield", "Anti-Unban Bypass", 
        "Quantum Speed Boost", "Master Speed Multiplier", "God-Tier Reach", 
        "Master Hitbox Extender", "Infinite Master Teleport", "Infinite Teleport Trail", 
        "Master Phase Breaker", "Ultimate Boundary Eraser", "Infinite Object Grab", 
        "Supreme Invisible God", "Ultimate Aura", "Instant Server Crash Gun", 
        "Supreme Server Nuke", "Lobby Wipeout Tool", "Ultimate Room Hijacker", 
        "Supreme Room Lock", "Host Override", "Admin Powers Bypass", "Master Controller", 
        "Master Clone Hacker", "Universal Clone Army", "Supreme Ghost Army", 
        "Total Server Freeze", "Infinite Stun Gun", "Ultimate Hand Flinger", 
        "Global Mute Overpower", "Master Sound Exploder", "Infinite Audio Injection", 
        "Ultimate Server Echo", "Infinite Material Spawner", "Master Gravity Overload", 
        "Absolute Horizon Shift", "Reality Rewrite", "Supreme Reality Shatter", 
        "Master Visual Warp", "Ultimate Color Apocalypse", "Supreme Time Warp", 
        "Master Skin Injector", "Master Animation Overwriter", "Infinite Score Modifier", 
        "Master Tracker", "Ultimate Mirror Matrix", "Supreme Packet Flooder", 
        "Zero Latency Execution"
    }}
};

void Menu::InitMenu() {
    LOGI("Gtag Copy Menu Initialized successfully.");
}

void Menu::ShowNotification(std::string message) {
    if (notificationsEnabled) {
        LOGI("[NOTIFICATION] %s", message.c_str());
    }
}

void Menu::DisconnectLobbyGlobal() {
    LOGI("[QUICK ACTION] Emergency Disconnect Triggered.");
    Mods::ExecuteUniversalMod("Lobby Disconnect on Staff Join", true);
}

void Menu::ScrollLeft() {
    if (currentCategoryIndex > 0) {
        currentCategoryIndex--;
    } else {
        currentCategoryIndex = categories.size() - 1;
    }
    selectedModIndex = 0;
    ShowNotification("Tab Left: " + categories[currentCategoryIndex].name);
}

void Menu::ScrollRight() {
    if (currentCategoryIndex < categories.size() - 1) {
        currentCategoryIndex++;
    } else {
        currentCategoryIndex = 0;
    }
    selectedModIndex = 0;
    ShowNotification("Tab Right: " + categories[currentCategoryIndex].name);
}

void Menu::SelectCurrentMod() {
    std::string modName = categories[currentCategoryIndex].mods[selectedModIndex];
    Mods::ExecuteUniversalMod(modName, true);
}

void Menu::RenderUI() {
    const auto& cat = categories[currentCategoryIndex];
    LOGI("Active Category: [%s] | Selected Mod: [%s]", cat.name.c_str(), cat.mods[selectedModIndex].c_str());
}
