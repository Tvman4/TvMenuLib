#include "Menu.h"
#include "Mods.h"
#include "Unity.h"
#include "Input.h"
#include "Log.h"
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace {
// Dark shell, red accent — readable in a cave
constexpr Color PANEL   {0.07f, 0.07f, 0.08f, 1.f};
constexpr Color FRAME   {0.75f, 0.08f, 0.10f, 1.f};
constexpr Color HEADER  {0.12f, 0.02f, 0.03f, 1.f};
constexpr Color BTN     {0.16f, 0.16f, 0.18f, 1.f};
constexpr Color BTN_ON  {0.12f, 0.42f, 0.18f, 1.f};
constexpr Color BTN_HV  {0.85f, 0.16f, 0.14f, 1.f};
constexpr Color TXT     {1.00f, 0.95f, 0.95f, 1.f};
constexpr Color TXT_ON  {0.85f, 1.00f, 0.85f, 1.f};
constexpr Color DOT     {1.00f, 1.00f, 1.00f, 1.f};
constexpr Color DOT_ON  {1.00f, 0.20f, 0.15f, 1.f};

std::vector<ModDef> gMods;
std::vector<char> gOn;
int page = 0;
int hover = -1;
bool open = true;
bool inited = false;

IL2CPP::Il2CppObject *root = nullptr;     // empty parent on left wrist
IL2CPP::Il2CppObject *panel = nullptr;
IL2CPP::Il2CppObject *frame = nullptr;
IL2CPP::Il2CppObject *header = nullptr;
IL2CPP::Il2CppObject *title = nullptr;
IL2CPP::Il2CppObject *pageLbl = nullptr;
IL2CPP::Il2CppObject *pointer = nullptr;
std::vector<IL2CPP::Il2CppObject *> buttons;
std::vector<IL2CPP::Il2CppObject *> labels;
std::vector<IL2CPP::Il2CppObject *> pageDots;

constexpr int ROWS = 8;
constexpr int COLS = 1;
constexpr int PER_PAGE = ROWS * COLS;

int PageCount() {
    int n = (int)gMods.size();
    return std::max(1, (n + PER_PAGE - 1) / PER_PAGE);
}

void Kill(IL2CPP::Il2CppObject *&o) {
    if (o) { U::Destroy(o); o = nullptr; }
}

IL2CPP::Il2CppObject *Cube(const Vector3 &scale, const Color &c, const char *name) {
    auto go = U::CreatePrimitive(3);
    if (!go) return nullptr;
    U::SetName(go, name);
    U::SetLocalScale(U::GetTransform(go), scale);
    U::SetMaterialColor(go, c);
    auto col = U::GetComponent(go, U::ColliderClass());
    if (col) U::Destroy(col);
    return go;
}

IL2CPP::Il2CppObject *Sphere(const Vector3 &scale, const Color &c, const char *name) {
    auto go = U::CreatePrimitive(0);
    if (!go) return nullptr;
    U::SetName(go, name);
    U::SetLocalScale(U::GetTransform(go), scale);
    U::SetMaterialColor(go, c);
    auto col = U::GetComponent(go, U::ColliderClass());
    if (col) U::Destroy(col);
    return go;
}

// Place `go` in the local space of parentTr (parent's right/up/forward basis)
void Place(IL2CPP::Il2CppObject *go, IL2CPP::Il2CppObject *parentTr, const Vector3 &lp) {
    if (!go || !parentTr) return;
    auto tr = U::GetTransform(go);
    U::SetParent(tr, parentTr, false);
    Vector3 p = U::GetPosition(parentTr);
    Vector3 r = U::GetRight(parentTr);
    Vector3 u = U::GetUp(parentTr);
    Vector3 f = U::GetForward(parentTr);
    Vector3 w{
        p.x + r.x * lp.x + u.x * lp.y + f.x * lp.z,
        p.y + r.y * lp.x + u.y * lp.y + f.y * lp.z,
        p.z + r.z * lp.x + u.z * lp.y + f.z * lp.z
    };
    U::SetPosition(tr, w);
    U::SetRotation(tr, U::GetRotation(parentTr));
}

void Rebuild() {
    Kill(root);
    Kill(panel);
    Kill(frame);
    Kill(header);
    Kill(title);
    Kill(pageLbl);
    Kill(pointer);
    for (auto b : buttons) if (b) U::Destroy(b);
    for (auto l : labels) if (l) U::Destroy(l);
    for (auto d : pageDots) if (d) U::Destroy(d);
    buttons.clear();
    labels.clear();
    pageDots.clear();

    // root is a tiny hidden cube we park on the left wrist
    root = Cube({0.001f, 0.001f, 0.001f}, PANEL, "TvMenuRoot");
    if (!root) return;

    // back plate
    panel = Cube({0.16f, 0.24f, 0.006f}, PANEL, "TvMenuPanel");
    // red rim slightly larger, behind the plate
    frame = Cube({0.172f, 0.252f, 0.004f}, FRAME, "TvMenuFrame");
    // header strip
    header = Cube({0.16f, 0.032f, 0.007f}, HEADER, "TvMenuHeader");

    title = U::MakeLabel("TvMenu", 0.012f, TXT);
    pageLbl = U::MakeLabel("1/1", 0.008f, Color{0.85f, 0.7f, 0.7f, 1});

    buttons.resize(PER_PAGE, nullptr);
    labels.resize(PER_PAGE, nullptr);
    for (int i = 0; i < PER_PAGE; i++) {
        buttons[i] = Cube({0.14f, 0.022f, 0.007f}, BTN, "TvMenuBtn");
        labels[i] = U::MakeLabel("...", 0.0075f, TXT);
    }

    int pages = 16;
    pageDots.resize(pages, nullptr);
    for (int i = 0; i < pages; i++) {
        pageDots[i] = Sphere({0.008f, 0.008f, 0.008f}, FRAME, "TvPageDot");
    }

    pointer = Sphere({0.022f, 0.022f, 0.022f}, DOT, "TvMenuPointer");
    LOGI("menu rebuilt");
}

float Dist2(const Vector3 &a, const Vector3 &b) {
    float dx = a.x-b.x, dy = a.y-b.y, dz = a.z-b.z;
    return dx*dx + dy*dy + dz*dz;
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

    if (!root || !panel || !pointer) Rebuild();
    if (!root || !panel || !pointer) return;

    U::SetActive(root, true);
    U::SetActive(panel, open);
    if (frame) U::SetActive(frame, open);
    if (header) U::SetActive(header, open);
    if (title) U::SetActive(title, open);
    if (pageLbl) U::SetActive(pageLbl, open);
    for (auto b : buttons) if (b) U::SetActive(b, open);
    for (auto l : labels) if (l) U::SetActive(l, open);
    for (auto d : pageDots) if (d) U::SetActive(d, open);
    U::SetActive(pointer, true);

    // Park root on the INNER left wrist, facing out from the arm
    Place(root, ltr, {0.00f, 0.03f, 0.13f});
    auto rtrRoot = U::GetTransform(root);

    // layout relative to root
    Place(frame,  rtrRoot, {0, 0.00f, 0.000f});
    Place(panel,  rtrRoot, {0, 0.00f, -0.004f});
    Place(header, rtrRoot, {0, 0.108f, -0.008f});
    if (title)   Place(title,   rtrRoot, {0, 0.108f, -0.014f});
    if (pageLbl) Place(pageLbl, rtrRoot, {0.052f, 0.108f, -0.014f});

    // pointer sits ABOVE the right hand / finger
    Vector3 rp = U::GetPosition(rtr);
    Vector3 ru = U::GetUp(rtr);
    Vector3 rf = U::GetForward(rtr);
    Vector3 dotPos{
        rp.x + ru.x * 0.055f + rf.x * 0.04f,
        rp.y + ru.y * 0.055f + rf.y * 0.04f,
        rp.z + ru.z * 0.055f + rf.z * 0.04f
    };
    U::SetPosition(U::GetTransform(pointer), dotPos);
    U::SetMaterialColor(pointer, hover >= 0 ? DOT_ON : DOT);

    if (!open) return;

    int pages = PageCount();
    if (page >= pages) page = 0;

    char pbuf[16];
    std::snprintf(pbuf, sizeof(pbuf), "%d/%d", page + 1, pages);
    if (pageLbl) U::SetLabelText(pageLbl, pbuf);

    hover = -1;
    float best = 0.028f * 0.028f;

    for (int i = 0; i < PER_PAGE; i++) {
        int idx = page * PER_PAGE + i;
        bool exists = idx < (int)gMods.size();
        if (buttons[i]) U::SetActive(buttons[i], exists && open);
        if (labels[i])  U::SetActive(labels[i], exists && open);
        if (!exists || !buttons[i]) continue;

        float y = 0.078f - i * 0.0245f;
        Place(buttons[i], rtrRoot, {0.f, y, -0.010f});
        if (labels[i]) {
            Place(labels[i], rtrRoot, {0.f, y, -0.016f});
            U::SetLabelText(labels[i], gMods[idx].name);
        }

        bool on = gOn[idx];
        Vector3 bp = U::GetPosition(U::GetTransform(buttons[i]));
        float d2 = Dist2(dotPos, bp);
        bool hv = d2 < best;
        if (hv) { best = d2; hover = idx; }

        Color bc = hv ? BTN_HV : (on ? BTN_ON : BTN);
        U::SetMaterialColor(buttons[i], bc);
        if (labels[i]) U::SetLabelColor(labels[i], on ? TXT_ON : TXT);
    }

    // page dots along the bottom
    int showDots = std::min(pages, (int)pageDots.size());
    for (int i = 0; i < (int)pageDots.size(); i++) {
        if (!pageDots[i]) continue;
        bool vis = i < showDots && open;
        U::SetActive(pageDots[i], vis);
        if (!vis) continue;
        float x = -0.06f + i * 0.012f;
        Place(pageDots[i], rtrRoot, {x, -0.112f, -0.012f});
        U::SetMaterialColor(pageDots[i], i == page ? DOT_ON : Color{0.35f, 0.12f, 0.12f, 1});
    }

    if (XR::GetBoolDown(XR::BoolFeature::SecondaryButton, XR::Controller::Right)) {
        page = (page + 1) % pages;
    }
    if (XR::GetBoolDown(XR::BoolFeature::PrimaryButton, XR::Controller::Right) && hover < 0) {
        page = (page + pages - 1) % pages;
    }

    if (hover >= 0 && XR::GetBoolDown(XR::BoolFeature::TriggerButton, XR::Controller::Right)) {
        auto &m = gMods[hover];
        if (!m.toggle) {
            if (m.onSet) m.onSet(true);
        } else {
            SetOn(hover, !gOn[hover]);
            LOGI("%s = %d", m.name, (int)gOn[hover]);
        }
    }

    for (int i = 0; i < (int)gMods.size(); i++) {
        if (gOn[i] && gMods[i].onTick) gMods[i].onTick();
    }
}
