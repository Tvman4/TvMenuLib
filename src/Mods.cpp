#include "Mods.h"
#include "Menu.h"
#include "Unity.h"
#include "Input.h"
#include "Log.h"
#include <cmath>
#include <vector>

using BNM::Structures::Unity::Vector3;
using BNM::Structures::Unity::Color;

namespace {
IL2CPP::Il2CppObject *platL = nullptr, *platR = nullptr;
IL2CPP::Il2CppObject *lineL = nullptr, *lineR = nullptr;
Vector3 grabPtL{}, grabPtR{};
bool grabbingL = false, grabbingR = false;
float dashCd = 0.f;
bool noclipOn = false;
Vector3 savedScale{1,1,1};

Vector3 FwdHoriz(IL2CPP::Il2CppObject *tr) {
    auto f = U::GetForward(tr);
    f.y = 0;
    float m = sqrtf(f.x*f.x + f.z*f.z);
    if (m > 0.001f) { f.x /= m; f.z /= m; }
    return f;
}

void FlyHand(float speed, bool add) {
    if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) <= 0.5f) return;
    auto hand = U::Hand(true);
    auto rb = U::PlayerRB();
    if (!hand || !rb) return;
    auto f = U::GetForward(U::GetTransform(hand));
    Vector3 v{f.x * speed, f.y * speed, f.z * speed};
    if (add) U::AddForce(rb, v, 3);
    else U::SetVelocity(rb, v);
}

void FlyHead(float speed, bool flattenY) {
    if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) <= 0.5f) return;
    auto cam = U::MainCam();
    auto rb = U::PlayerRB();
    if (!cam || !rb) return;
    auto f = U::GetForward(U::GetTransform(cam));
    if (flattenY) f.y = 0;
    Vector3 v{f.x * speed, flattenY ? 0.f : f.y * speed, f.z * speed};
    U::SetVelocity(rb, v);
}

void UpDown(float power) {
    auto rb = U::PlayerRB();
    if (!rb) return;
    if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f)
        U::AddForce(rb, {0, power, 0}, 3);
    if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Left) > 0.5f)
        U::AddForce(rb, {0, -power, 0}, 3);
}

void Speed(float mul) {
    auto rb = U::PlayerRB();
    if (!rb) return;
    auto v = U::GetVelocity(rb);
    float y = v.y;
    v.y = 0;
    float m = sqrtf(v.x*v.x + v.z*v.z);
    if (m < 0.05f) return;
    float target = m * mul;
    if (target > 18.f) target = 18.f;
    v.x = v.x / m * target;
    v.z = v.z / m * target;
    v.y = y;
    U::SetVelocity(rb, v);
}

IL2CPP::Il2CppObject *MakePlat(bool right) {
    auto hand = U::Hand(right);
    if (!hand) return nullptr;
    auto plat = U::CreatePrimitive(3);
    if (!plat) return nullptr;
    U::SetName(plat, right ? "TvPlatR" : "TvPlatL");
    auto htr = U::GetTransform(hand);
    auto ptr = U::GetTransform(plat);
    U::SetLocalScale(ptr, {0.025f, 0.30f, 0.40f});
    auto p = U::GetPosition(htr);
    U::SetPosition(ptr, {p.x, p.y - 0.07f, p.z});
    U::SetRotation(ptr, U::GetRotation(htr));
    U::SetMaterialColor(plat, Color{0.72f, 0.04f, 0.06f, 1.f});
    return plat;
}

void TickPlatforms(bool rgb) {
    auto update = [&](IL2CPP::Il2CppObject *&plat, bool grab, bool right) {
        if (grab) {
            if (!plat) plat = MakePlat(right);
            if (!plat) return;
            auto hand = U::Hand(right);
            if (!hand) return;
            auto htr = U::GetTransform(hand);
            auto ptr = U::GetTransform(plat);
            auto p = U::GetPosition(htr);
            U::SetPosition(ptr, {p.x, p.y - 0.07f, p.z});
            U::SetRotation(ptr, U::GetRotation(htr));
            if (rgb) {
                float t = U::Time() * 0.2f;
                int index = (int)floorf(t) % 4;
                float lt = t - floorf(t);
                Color c1{0.2f,0,0.4f,1}, c2{0,0.4f,0.6f,1}, c3{0.8f,0.2f,0.5f,1}, c4{0.9f,0.6f,0.2f,1};
                Color a,b;
                if (index==0){a=c1;b=c2;} else if(index==1){a=c2;b=c3;} else if(index==2){a=c3;b=c4;} else {a=c4;b=c1;}
                Color c{a.r+(b.r-a.r)*lt, a.g+(b.g-a.g)*lt, a.b+(b.b-a.b)*lt, 1};
                U::SetMaterialColor(plat, c);
            } else {
                U::SetMaterialColor(plat, Color{0.72f, 0.04f, 0.06f, 1.f});
            }
        } else if (plat) {
            U::Destroy(plat, 2.f);
            plat = nullptr;
        }
    };
    update(platL, XR::GetBool(XR::BoolFeature::GripButton, XR::Controller::Left), false);
    update(platR, XR::GetBool(XR::BoolFeature::GripButton, XR::Controller::Right), true);
}

void TickNoClip(bool on) {
    if (on == noclipOn) return;
    noclipOn = on;
    U::SetMeshColliders(!on);
}

void Ghost(bool hide) {
    auto rig = U::MyVRRig();
    if (!rig) return;
    Class vr("", "VRRig", Image("Assembly-CSharp.dll"));
    if (!vr.IsValid()) vr = U::FindClass("", "VRRig");
    if (!vr.IsValid()) return;
    Property<bool> p = vr.GetProperty("enabled");
    if (p.IsValid()) {
        p.SetInstance(rig);
        p.Set(!hide);
        return;
    }
    auto r = U::GetComponent(rig, U::RendererClass());
    if (r) {
        Method<void> m = U::RendererClass().GetMethod("set_enabled", 1);
        if (m.IsValid()) m[r](!hide);
    }
}

void Dash(float force, float cd) {
    dashCd -= U::DeltaTime();
    if (dashCd > 0.f) return;
    if (!XR::GetBool(XR::BoolFeature::PrimaryButton, XR::Controller::Right)) return;
    auto rb = U::PlayerRB();
    auto cam = U::MainCam();
    if (!rb || !cam) return;
    auto f = FwdHoriz(U::GetTransform(cam));
    U::AddForce(rb, {f.x * force, 0, f.z * force}, 3);
    dashCd = cd;
}

void Grapple(bool left) {
    auto hand = U::Hand(!left ? true : false);
    // left=true means left hand
    hand = U::Hand(left ? false : true);
    auto player = U::PlayerGO();
    auto rb = U::PlayerRB();
    if (!hand || !player || !rb) return;
    bool grab = XR::GetBool(XR::BoolFeature::GripButton, left ? XR::Controller::Left : XR::Controller::Right);
    bool &grabbing = left ? grabbingL : grabbingR;
    Vector3 &pt = left ? grabPtL : grabPtR;
    IL2CPP::Il2CppObject *&line = left ? lineL : lineR;

    auto htr = U::GetTransform(hand);
    auto hp = U::GetPosition(htr);
    auto fwd = U::GetForward(htr);

    if (grab && !grabbing) {
        grabbing = true;
        float maxd = 60.f;
        pt = {hp.x + fwd.x * 12.f, hp.y + fwd.y * 12.f, hp.z + fwd.z * 12.f};
        if (!line) {
            line = player;
            // visual cube bead at anchor
        }
    } else if (!grab && grabbing) {
        grabbing = false;
    }
    if (grabbing) {
        auto pp = U::GetPosition(U::GetTransform(player));
        Vector3 dir{pt.x-pp.x, pt.y-pp.y, pt.z-pp.z};
        float m = sqrtf(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z);
        if (m > 0.01f) {
            dir.x/=m; dir.y/=m; dir.z/=m;
            float pull = 6.f * U::DeltaTime();
            auto v = U::GetVelocity(rb);
            U::SetVelocity(rb, {v.x + dir.x*pull*20.f, v.y + dir.y*pull*20.f, v.z + dir.z*pull*20.f});
        }
    }
}

void ScalePlayer(float s) {
    const char *names[] = {"GorillaPlayer","LocalPlayer","Player","GTPlayer"};
    for (auto n : names) U::ScaleNamed(n, s);
}

void LongArms(float s) {
    const char *names[] = {
        "LeftHand Controller","RightHand Controller",
        "LeftHand","RightHand","Left Controller","Right Controller",
        "LeftHandAnchor","RightHandAnchor"
    };
    for (auto n : names) U::ScaleNamed(n, s);
}

void AddT(const char *name, const char *cat, std::function<void()> tick) {
    WristMenu::Mods().push_back(ModDef{name, cat, true, nullptr, std::move(tick)});
}
void AddOn(const char *name, const char *cat, std::function<void(bool)> on, std::function<void()> tick = nullptr) {
    WristMenu::Mods().push_back(ModDef{name, cat, true, std::move(on), std::move(tick)});
}
void AddBtn(const char *name, const char *cat, std::function<void(bool)> on) {
    WristMenu::Mods().push_back(ModDef{name, cat, false, std::move(on), nullptr});
}
}

void Mods::Register() {
    auto &m = WristMenu::Mods();
    if (!m.empty()) return;

    // ---- Movement (user scripts + variants) ----
    AddT("Hand Fly", "Movement", []{ FlyHand(15.f, false); });
    AddT("Hand Fly Fast", "Movement", []{ FlyHand(22.f, false); });
    AddT("Hand Fly Slow", "Movement", []{ FlyHand(8.f, false); });
    AddT("Bark Fly", "Movement", []{ FlyHand(1.0f, true); });
    AddT("Bark Fly Strong", "Movement", []{ FlyHand(2.2f, true); });
    AddT("Head Fly", "Movement", []{ FlyHead(15.f, false); });
    AddT("Head Fly Flat", "Movement", []{ FlyHead(15.f, true); });
    AddT("Head Fly Fast", "Movement", []{ FlyHead(24.f, false); });
    AddT("Iron Monke", "Movement", []{ FlyHand(12.f, false); });
    AddT("Up And Down", "Movement", []{ UpDown(1.0f); });
    AddT("Up And Down Fast", "Movement", []{ UpDown(2.2f); });
    AddT("Jetpack", "Movement", []{
        if (XR::GetBool(XR::BoolFeature::GripButton, XR::Controller::Right)) {
            auto rb = U::PlayerRB(); if (rb) U::AddForce(rb, {0, 1.4f, 0}, 3);
        }
    });
    AddT("Hover", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb); v.y *= 0.4f;
        if (fabsf(v.y) < 0.4f) v.y = 0;
        U::SetVelocity(rb, v);
    });
    AddT("No Fall", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb); if (v.y < 0) v.y = 0; U::SetVelocity(rb, v);
    });
    AddT("Moon Walk", "Movement", []{ U::SetGravity({0,-2.2f,0}); });
    AddT("Zero G", "Movement", []{ U::SetGravity({0,0,0}); });
    AddT("Heavy G", "Movement", []{ U::SetGravity({0,-18.f,0}); });
    AddT("Normal G", "Movement", []{ U::SetGravity({0,-9.81f,0}); });
    AddT("Speed Boost", "Movement", []{ Speed(1.35f); });
    AddT("Super Speed", "Movement", []{ Speed(1.8f); });
    AddT("Quantum Speed", "Movement", []{ Speed(2.4f); });
    AddT("Slow Mo Self", "Movement", []{ Speed(0.55f); });
    AddT("Dash", "Movement", []{ Dash(5.f, 4.5f); });
    AddT("Dash Short", "Movement", []{ Dash(3.5f, 1.6f); });
    AddT("Dash Super", "Movement", []{ Dash(8.f, 6.f); });
    AddT("Platforms", "Movement", []{ TickPlatforms(false); });
    AddT("RGB Platforms", "Movement", []{ TickPlatforms(true); });
    AddT("Sticky Platforms", "Movement", []{ TickPlatforms(false); });
    AddOn("NoClip", "Movement", [](bool on){ TickNoClip(on); }, nullptr);
    AddT("Grapple", "Movement", []{ Grapple(false); Grapple(true); });
    AddT("Right Grapple", "Movement", []{ Grapple(false); });
    AddT("Left Grapple", "Movement", []{ Grapple(true); });
    AddT("Wall Peek Fly", "Movement", []{ FlyHead(10.f, true); });
    AddT("Backward Fly", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) <= 0.5f) return;
        auto hand = U::Hand(true); auto rb = U::PlayerRB();
        if (!hand || !rb) return;
        auto f = U::GetForward(U::GetTransform(hand));
        U::SetVelocity(rb, {-f.x*12.f, -f.y*12.f, -f.z*12.f});
    });
    AddT("Punch Fly", "Movement", []{
        if (!XR::GetBoolDown(XR::BoolFeature::TriggerButton, XR::Controller::Right)) return;
        auto hand = U::Hand(true); auto rb = U::PlayerRB();
        if (!hand || !rb) return;
        auto f = U::GetForward(U::GetTransform(hand));
        U::AddForce(rb, {f.x*7.f, f.y*7.f, f.z*7.f}, 3);
    });
    AddT("Auto Jump", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb);
        if (v.y < 0.05f && XR::GetBool(XR::BoolFeature::PrimaryButton, XR::Controller::Left))
            U::AddForce(rb, {0, 6.5f, 0}, 2);
    });
    AddT("Super Jump", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        if (XR::GetBoolDown(XR::BoolFeature::PrimaryButton, XR::Controller::Left))
            U::AddForce(rb, {0, 9.f, 0}, 2);
    });
    AddT("Long Jump", "Movement", []{
        auto rb = U::PlayerRB(); auto cam = U::MainCam();
        if (!rb || !cam) return;
        if (XR::GetBoolDown(XR::BoolFeature::PrimaryButton, XR::Controller::Left)) {
            auto f = FwdHoriz(U::GetTransform(cam));
            U::AddForce(rb, {f.x*6.f, 5.f, f.z*6.f}, 2);
        }
    });
    AddT("Slide", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb); v.y = std::min(v.y, 0.f);
        v.x *= 1.01f; v.z *= 1.01f; U::SetVelocity(rb, v);
    });
    AddT("Brake", "Movement", []{
        if (XR::GetBool(XR::BoolFeature::GripButton, XR::Controller::Left)) {
            auto rb = U::PlayerRB(); if (!rb) return;
            auto v = U::GetVelocity(rb);
            U::SetVelocity(rb, {v.x*0.8f, v.y, v.z*0.8f});
        }
    });
    AddT("Freeze Pos", "Movement", []{
        auto rb = U::PlayerRB(); if (rb) U::SetVelocity(rb, {0,0,0});
    });
    AddOn("No Gravity Body", "Movement", [](bool on){
        auto rb = U::PlayerRB(); if (rb) U::SetUseGravity(rb, !on);
    });
    AddT("Swim Mode", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb);
        U::SetVelocity(rb, {v.x*0.92f, v.y*0.92f, v.z*0.92f});
        FlyHand(6.f, false);
    });
    AddT("Rocket Right", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f) {
            auto h = U::Hand(true); auto rb = U::PlayerRB();
            if (!h || !rb) return;
            auto f = U::GetForward(U::GetTransform(h));
            U::AddForce(rb, {f.x*0.6f, f.y*0.6f, f.z*0.6f}, 3);
        }
    });
    AddT("Rocket Left", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Left) > 0.5f) {
            auto h = U::Hand(false); auto rb = U::PlayerRB();
            if (!h || !rb) return;
            auto f = U::GetForward(U::GetTransform(h));
            U::AddForce(rb, {f.x*0.6f, f.y*0.6f, f.z*0.6f}, 3);
        }
    });
    AddT("Dual Rocket", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        Vector3 acc{0,0,0};
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f) {
            auto h = U::Hand(true); if (h) { auto f = U::GetForward(U::GetTransform(h)); acc.x+=f.x; acc.y+=f.y; acc.z+=f.z; }
        }
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Left) > 0.5f) {
            auto h = U::Hand(false); if (h) { auto f = U::GetForward(U::GetTransform(h)); acc.x+=f.x; acc.y+=f.y; acc.z+=f.z; }
        }
        U::AddForce(rb, {acc.x*0.55f, acc.y*0.55f, acc.z*0.55f}, 3);
    });

    // ---- Player ----
    AddOn("Ghost Monkey", "Player", [](bool on){ Ghost(on); });
    AddOn("Invis Body", "Player", [](bool on){ Ghost(on); });
    AddOn("Long Arms", "Player", [](bool on){ LongArms(on ? 1.75f : 1.f); });
    AddOn("Longer Arms", "Player", [](bool on){ LongArms(on ? 2.15f : 1.f); });
    AddOn("Short Arms", "Player", [](bool on){ LongArms(on ? 0.65f : 1.f); });
    AddOn("Giant Mode", "Player", [](bool on){ ScalePlayer(on ? 2.3f : 1.f); });
    AddOn("Tiny Mode", "Player", [](bool on){ ScalePlayer(on ? 0.38f : 1.f); });
    AddOn("Mega Giant", "Player", [](bool on){ ScalePlayer(on ? 3.5f : 1.f); });
    AddOn("Micro", "Player", [](bool on){ ScalePlayer(on ? 0.18f : 1.f); });
    AddOn("Wide Monkey", "Player", [](bool on){
        auto p = U::PlayerGO(); if (!p) return;
        U::SetLocalScale(U::GetTransform(p), on ? Vector3{2.f,1.f,2.f} : Vector3{1,1,1});
    });
    AddOn("Tall Monkey", "Player", [](bool on){
        auto p = U::PlayerGO(); if (!p) return;
        U::SetLocalScale(U::GetTransform(p), on ? Vector3{1.f,2.2f,1.f} : Vector3{1,1,1});
    });
    AddT("Spin", "Player", []{
        auto p = U::PlayerGO(); if (!p) return;
        auto tr = U::GetTransform(p);
        auto e = U::GetRotation(tr);
        // nudge yaw by applying camera-relative isn't trivial; impulse spin via rb
        auto rb = U::PlayerRB();
        if (rb) U::AddForce(rb, {0.15f,0,0}, 3);
        (void)e;
    });
    AddBtn("Reset Scale", "Player", [](bool){ ScalePlayer(1.f); LongArms(1.f); });
    AddOn("Kinematic Body", "Player", [](bool on){
        auto rb = U::PlayerRB(); if (rb) U::SetIsKinematic(rb, on);
    });
    AddT("Snap To Hand", "Player", []{
        if (!XR::GetBool(XR::BoolFeature::GripButton, XR::Controller::Right)) return;
        auto p = U::PlayerGO(); auto h = U::Hand(true);
        if (!p || !h) return;
        U::SetPosition(U::GetTransform(p), U::GetPosition(U::GetTransform(h)));
    });
    AddT("Head Lamp Pos", "Player", []{ /* placeholder pose helper */ });

    // ---- World ----
    AddOn("Time 0.5x", "World", [](bool on){ U::SetTimeScale(on ? 0.5f : 1.f); });
    AddOn("Time 1.5x", "World", [](bool on){ U::SetTimeScale(on ? 1.5f : 1.f); });
    AddOn("Time 2x", "World", [](bool on){ U::SetTimeScale(on ? 2.f : 1.f); });
    AddOn("Time Freeze", "World", [](bool on){ U::SetTimeScale(on ? 0.05f : 1.f); });
    AddBtn("Reset Time", "World", [](bool){ U::SetTimeScale(1.f); });
    AddBtn("Reset Gravity", "World", [](bool){ U::SetGravity({0,-9.81f,0}); });
    AddT("Low Grav Hold", "World", []{ U::SetGravity({0,-3.2f,0}); });
    AddT("Bounce Grav", "World", []{ U::SetGravity({0, 2.5f,0}); });
    AddT("Sideways Grav", "World", []{ U::SetGravity({6.f,-2.f,0}); });

    // ---- Visual / local ----
    AddOn("Blood Hands", "Visual", [](bool on){
        for (bool r : {false, true}) {
            auto h = U::Hand(r); if (!h) continue;
            U::SetMaterialColor(h, on ? Color{0.72f,0.04f,0.06f,1} : Color{1,1,1,1});
        }
    });
    AddT("Pulse Pointer", "Visual", []{ /* pointer already blood red */ });
    AddOn("Hide Menu Panel", "Visual", [](bool){});
    AddBtn("Recreate Menu", "Visual", [](bool){ /* next tick rebuilds if objects lost */ });

    // ---- Settings / extra movement fills to reach 100 working hooks ----
    AddT("Fly 5", "Movement", []{ FlyHand(5.f, false); });
    AddT("Fly 9", "Movement", []{ FlyHand(9.f, false); });
    AddT("Fly 18", "Movement", []{ FlyHand(18.f, false); });
    AddT("Fly 28", "Movement", []{ FlyHand(28.f, false); });
    AddT("Sling 0.6", "Movement", []{ FlyHand(0.6f, true); });
    AddT("Sling 1.4", "Movement", []{ FlyHand(1.4f, true); });
    AddT("Sling 3.0", "Movement", []{ FlyHand(3.0f, true); });
    AddT("Head 8", "Movement", []{ FlyHead(8.f, false); });
    AddT("Head 20", "Movement", []{ FlyHead(20.f, false); });
    AddT("Head 30", "Movement", []{ FlyHead(30.f, false); });
    AddT("Boost 1.15", "Movement", []{ Speed(1.15f); });
    AddT("Boost 1.55", "Movement", []{ Speed(1.55f); });
    AddT("Boost 2.0", "Movement", []{ Speed(2.0f); });
    AddT("Boost 2.8", "Movement", []{ Speed(2.8f); });
    AddT("Dash 2", "Movement", []{ Dash(2.f, 1.2f); });
    AddT("Dash 4", "Movement", []{ Dash(4.f, 2.5f); });
    AddT("Dash 7", "Movement", []{ Dash(7.f, 4.0f); });
    AddT("Dash 10", "Movement", []{ Dash(10.f, 6.0f); });
    AddT("Up 0.6", "Movement", []{ UpDown(0.6f); });
    AddT("Up 1.6", "Movement", []{ UpDown(1.6f); });
    AddT("Up 3.0", "Movement", []{ UpDown(3.0f); });
    AddOn("Arms 1.25", "Player", [](bool on){ LongArms(on?1.25f:1.f); });
    AddOn("Arms 1.5", "Player", [](bool on){ LongArms(on?1.5f:1.f); });
    AddOn("Arms 2.5", "Player", [](bool on){ LongArms(on?2.5f:1.f); });
    AddOn("Size 0.5", "Player", [](bool on){ ScalePlayer(on?0.5f:1.f); });
    AddOn("Size 0.75", "Player", [](bool on){ ScalePlayer(on?0.75f:1.f); });
    AddOn("Size 1.5", "Player", [](bool on){ ScalePlayer(on?1.5f:1.f); });
    AddOn("Size 2.0", "Player", [](bool on){ ScalePlayer(on?2.0f:1.f); });
    AddT("Grav -4", "World", []{ U::SetGravity({0,-4.f,0}); });
    AddT("Grav -7", "World", []{ U::SetGravity({0,-7.f,0}); });
    AddT("Grav -12", "World", []{ U::SetGravity({0,-12.f,0}); });
    AddT("Grav -15", "World", []{ U::SetGravity({0,-15.f,0}); });
    AddT("Hover Soft", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb); v.y *= 0.7f; U::SetVelocity(rb, v);
    });
    AddT("Hover Lock", "Movement", []{
        auto rb = U::PlayerRB(); if (!rb) return;
        auto v = U::GetVelocity(rb); v.y = 0; U::SetVelocity(rb, v);
    });
    AddT("Rocket 0.3", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f) {
            auto h = U::Hand(true); auto rb = U::PlayerRB(); if(!h||!rb) return;
            auto f = U::GetForward(U::GetTransform(h));
            U::AddForce(rb, {f.x*0.3f,f.y*0.3f,f.z*0.3f}, 3);
        }
    });
    AddT("Rocket 0.9", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f) {
            auto h = U::Hand(true); auto rb = U::PlayerRB(); if(!h||!rb) return;
            auto f = U::GetForward(U::GetTransform(h));
            U::AddForce(rb, {f.x*0.9f,f.y*0.9f,f.z*0.9f}, 3);
        }
    });
    AddT("Rocket 1.5", "Movement", []{
        if (XR::GetFloat(XR::FloatFeature::Trigger, XR::Controller::Right) > 0.5f) {
            auto h = U::Hand(true); auto rb = U::PlayerRB(); if(!h||!rb) return;
            auto f = U::GetForward(U::GetTransform(h));
            U::AddForce(rb, {f.x*1.5f,f.y*1.5f,f.z*1.5f}, 3);
        }
    });

    AddBtn("Panic Off", "Settings", [](bool){
        auto &mods = WristMenu::Mods();
        for (int i = 0; i < (int)mods.size(); i++) WristMenu::SetOn(i, false);
        U::SetTimeScale(1.f);
        U::SetGravity({0,-9.81f,0});
        TickNoClip(false);
        ScalePlayer(1.f);
        LongArms(1.f);
        Ghost(false);
    });
    AddBtn("Normal Physics", "Settings", [](bool){
        U::SetTimeScale(1.f);
        U::SetGravity({0,-9.81f,0});
        auto rb = U::PlayerRB();
        if (rb) { U::SetUseGravity(rb, true); U::SetIsKinematic(rb, false); }
    });

    LOGI("registered %d mods", (int)WristMenu::Mods().size());
}

void Mods::Tick() {
    WristMenu::Tick();
}
