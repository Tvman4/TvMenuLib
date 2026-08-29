#include "Menu.h"
#include "Mods.h"
#include "Unity.h"
#include "Input.h"
#include "Log.h"
#include <cmath>
#include <algorithm>

namespace {
constexpr Color BLOOD {0.72f, 0.04f, 0.06f, 1.f};
constexpr Color BLOOD_DK {0.18f, 0.01f, 0.02f, 0.96f};
constexpr Color BLOOD_HI {1.f, 0.22f, 0.18f, 1.f};
constexpr Color ON_COL {1.f, 0.85f, 0.85f, 1.f};
constexpr Color OFF_COL {0.45f, 0.08f, 0.08f, 1.f};

std::vector<ModDef> gMods;
std::vector<char> gOn; // char to avoid vector<bool>
int page = 0;
int hover = -1;
bool open = true;
bool inited = false;

IL2CPP::Il2CppObject *panel = nullptr;
IL2CPP::Il2CppObject *pointer = nullptr;
std::vector<IL2CPP::Il2CppObject *> buttons;
std::vector<IL2CPP::Il2CppObject *> pageBtns;

constexpr int ROWS = 8;
constexpr int COLS = 2;
constexpr int PER_PAGE = ROWS * COLS;

int PageCount() {
    int n = (int)gMods.size();
    return std::max(1, (n + PER_PAGE - 1) / PER_PAGE);
}

IL2CPP::Il2CppObject *MakeCube(const Vector3 &scale, const Color &c, const char *name) {
    auto go = U::CreatePrimitive(3); // Cube
    if (!go) return nullptr;
    U::SetName(go, name);
    auto tr = U::GetTransform(go);
    U::SetLocalScale(tr, scale);
    U::SetMaterialColor(go, c);
    // strip default collider so we do our own overlap test
    auto col = U::GetComponent(go, U::ColliderClass());
    if (col) U::Destroy(col);
    return go;
}

void PlaceLocal(IL2CPP::Il2CppObject *go, IL2CPP::Il2CppObject *parentTr,
                const Vector3 &localPos) {
    if (!go || !parentTr) return;
    auto tr = U::GetTransform(go);
    U::SetParent(tr, parentTr, false);
    // after parent, set world pos from parent basis
    Vector3 p = U::GetPosition(parentTr);
    Vector3 r = U::GetRight(parentTr);
    Vector3 u = U::GetUp(parentTr);
    Vector3 f = U::GetForward(parentTr);
    Vector3 w{
        p.x + r.x * localPos.x + u.x * localPos.y + f.x * localPos.z,
        p.y + r.y * localPos.x + u.y * localPos.y + f.y * localPos.z,
        p.z + r.z * localPos.x + u.z * localPos.y + f.z * localPos.z
    };
    U::SetPosition(tr, w);
    U::SetRotation(tr, U::GetRotation(parentTr));
}

void Rebuild() {
    // recreate each attach in case scene reloaded
    if (panel) { U::Destroy(panel); panel = nullptr; }
    for (auto b : buttons) if (b) U::Destroy(b);
    buttons.clear();
    if (pointer) { U::Destroy(pointer); pointer = nullptr; }

    panel = MakeCube({0.18f, 0.22f, 0.008f}, BLOOD_DK, "TvMenuPanel");
    pointer = U::CreatePrimitive(0); // Sphere
    if (pointer) {
        U::SetName(pointer, "TvMenuDot");
        U::SetLocalScale(U::GetTransform(pointer), {0.018f, 0.018f, 0.018f});
        U::SetMaterialColor(pointer, BLOOD_HI);
        auto col = U::GetComponent(pointer, U::ColliderClass());
        if (col) U::Destroy(col);
    }
    buttons.resize(PER_PAGE, nullptr);
    for (int i = 0; i < PER_PAGE; i++) {
        buttons[i] = MakeCube({0.078f, 0.022f, 0.006f}, OFF_COL, "TvMenuBtn");
    }
}

float Dist2(const Vector3 &a, const Vector3 &b) {
    float dx = a.x-b.x, dy = a.y-b.y, dz = a.z-b.z;
    return dx*dx+dy*dy+dz*dz;
}
}

std::vector<ModDef> &WristMenu::Mods() { return gMods; }
int WristMenu::Count() { return (int)gMods.size(); }
bool WristMenu::IsOn(int idx) { return idx >= 0 && idx < (int)gOn.size() && gOn[idx]; }
void WristMenu::SetOn(int idx, bool v) {
    if (idx < 0 || idx >= (int)gOn.size()) return;
    gOn[idx] = (char)v;
    if (gMods[idx].onSet) gMods[idx].onSet(v);
}

void WristMenu::Init() {
    if (inited) return;
    Mods::Register();
    gOn.assign(gMods.size(), 0);
    inited = true;
    LOGI("WristMenu init — %d mods", (int)gMods.size());
}

void WristMenu::RegisterAll() { Mods::Register(); }

void WristMenu::Tick() {
    if (!U::Ready()) return;
    if (!inited) Init();

    // Y / left secondary toggles menu
    if (XR::GetBoolDown(XR::BoolFeature::SecondaryButton, XR::Controller::Left) ||
        XR::GetBoolDown(XR::BoolFeature::PrimaryButton, XR::Controller::Left)) {
        open = !open;
        LOGI("menu %s", open ? "open" : "closed");
    }

    auto left = U::Hand(false);
    auto right = U::Hand(true);
    if (!left || !right) return;
    auto ltr = U::GetTransform(left);
    auto rtr = U::GetTransform(right);
    if (!ltr || !rtr) return;

    if (!panel || !pointer) Rebuild();
    if (!panel || !pointer) return;

    U::SetActive(panel, open);
    for (auto b : buttons) if (b) U::SetActive(b, open);

    // wrist pose: inner forearm / watch face
    Vector3 wristLocal{0.00f, 0.04f, 0.12f};
    PlaceLocal(panel, ltr, wristLocal);

    // red dot slightly ABOVE the right finger / hand
    Vector3 rp = U::GetPosition(rtr);
    Vector3 ru = U::GetUp(rtr);
    Vector3 rf = U::GetForward(rtr);
    Vector3 dotPos{
        rp.x + ru.x * 0.045f + rf.x * 0.03f,
        rp.y + ru.y * 0.045f + rf.y * 0.03f,
        rp.z + ru.z * 0.045f + rf.z * 0.03f
    };
    U::SetPosition(U::GetTransform(pointer), dotPos);
    U::SetMaterialColor(pointer, BLOOD);

    if (!open) {
        U::SetActive(pointer, true); // still visible so you can find it
        return;
    }

    int pages = PageCount();
    if (page >= pages) page = 0;

    // layout buttons on panel face
    hover = -1;
    float best = 0.035f * 0.035f;
    for (int i = 0; i < PER_PAGE; i++) {
        int idx = page * PER_PAGE + i;
        bool exists = idx < (int)gMods.size();
        if (buttons[i]) U::SetActive(buttons[i], exists);
        if (!exists || !buttons[i]) continue;

        int col = i / ROWS;
        int row = i % ROWS;
        float x = (col == 0) ? -0.042f : 0.042f;
        float y = 0.09f - row * 0.026f;
        float z = -0.008f;
        PlaceLocal(buttons[i], U::GetTransform(panel), {x, y, z});

        bool on = gOn[idx];
        Vector3 bp = U::GetPosition(U::GetTransform(buttons[i]));
        float d2 = Dist2(dotPos, bp);
        bool hv = d2 < best;
        if (hv) { best = d2; hover = idx; }
        Color c = hv ? BLOOD_HI : (on ? ON_COL : BLOOD);
        U::SetMaterialColor(buttons[i], c);
    }

    // page flip with right primary / grip+stick substitute: right secondary next page, right primary prev
    if (XR::GetBoolDown(XR::BoolFeature::SecondaryButton, XR::Controller::Right)) {
        page = (page + 1) % pages;
    }
    if (XR::GetBoolDown(XR::BoolFeature::PrimaryButton, XR::Controller::Right) && hover < 0) {
        page = (page + pages - 1) % pages;
    }

    // select with right trigger while dot hovers a button
    if (hover >= 0 && XR::GetBoolDown(XR::BoolFeature::TriggerButton, XR::Controller::Right)) {
        auto &m = gMods[hover];
        if (!m.toggle) {
            if (m.onSet) m.onSet(true);
        } else {
            SetOn(hover, !gOn[hover]);
            LOGI("%s = %d", m.name, (int)gOn[hover]);
        }
    }

    // run active ticks
    for (int i = 0; i < (int)gMods.size(); i++) {
        if (gOn[i] && gMods[i].onTick) gMods[i].onTick();
    }
}
