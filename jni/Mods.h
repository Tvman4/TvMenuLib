#pragma once
#include <jni.h>

namespace Mods {
    // Movement Mods
    void SpeedBoost(bool enabled);
    void LongArms(bool enabled);
    void Fly(bool enabled);
    void Noclip(bool enabled);
    void PlatformGun(bool enabled);
    void AirControl(bool enabled);
    void GravityModifier(bool enabled);
    void SpiderMan(bool enabled);
    void IronMan(bool enabled);
    void AutoSlide(bool enabled);

    // Visual & Render Mods
    void Fullbright(bool enabled);
    void ESP(bool enabled);
    void Tracers(bool enabled);
    void BoneESP(bool enabled);
    void NameTags(bool enabled);
    void FOVChanger(bool enabled, float fov);
    void ThirdPersonCamera(bool enabled);
    void CustomSkybox(bool enabled);
    void XRay(bool enabled);
    void FPSCounter(bool enabled);

    // Safety & Protection Mods
    void AntiBan(bool enabled);
    void AntiReport(bool enabled);
    void ReportBlock(bool enabled);
    void NameSpoofer(bool enabled);
    void IDSpoofer(bool enabled);
    void RPCSpoofing(bool enabled);
    void LobbyDisconnectOnStaffJoin(bool enabled);
    void StealthHooks(bool enabled);

    // Multiplayer & Trolling Mods
    void TagGun(bool enabled);
    void TagAll(bool enabled);
    void InvisAll(bool enabled);
    void SoundSpam(bool enabled);
    void RopeSpaz(bool enabled);
    void MoveAllRopes(bool enabled);
    void AuraTag(bool enabled);
    void GhostMonkey(bool enabled);
    void Crasher(bool enabled);

    // Cosmetic & Customization Mods
    void UnlockAllCosmetics(bool enabled);
    void CustomColorChanger(bool enabled);
    void MaterialChanger(bool enabled);
    void CustomHoldables(bool enabled);
    void BadgeUnlocker(bool enabled);
    void FakeHandPainter(bool enabled);

    // World & Fun Mods
    void TeleportToPlayers(bool enabled);
    void TeleportToMapLocations(bool enabled);
    void TimeOfDayChanger(bool enabled);
    void WaterWalk(bool enabled);
    void NoClipTrees(bool enabled);
    void Soundboard(bool enabled);
    void PathRecorder(bool enabled);

    // Settings & Menu Controls
    void MenuCustomizer(bool enabled);
    void KeybindEditor(bool enabled);
    void ToggleNotifications(bool enabled);
    void SaveConfig();
    void DisableMenuSounds(bool enabled);
    void FPSLimiter(bool enabled, int limit);
}
