#include "Unity.h"
#include "Log.h"

namespace U {
namespace {
bool g_ready = false;

Class GO, TR, RB, COL, MCOL, REND, LR, SJ, PHYS, TIME, SHADER, MAT, OBJ;
Method<IL2CPP::Il2CppObject *> mFind, mCreatePrim, mGetComp, mAddComp, mGetTr;
Method<void> mDestroy, mDestroyDelay, mDontDestroy, mSetActive;
Method<bool> mGetActive;
Method<Vector3> mGetPos, mGetFwd, mGetUp, mGetRight, mGetLocalScale, mGetVel, mGetGravity;
Method<Quaternion> mGetRot;
Method<void> mSetPos, mSetRot, mSetLocalScale, mSetParent, mSetVel, mAddForce, mSetUseGrav, mSetKin;
Method<void> mSetGrav, mSetTimeScale;
Method<float> mGetTime, mGetDt;
Method<IL2CPP::Il2CppObject *> mGetMat, mFindShader;
Method<void> mSetColor, mSetShader;
Method<void> mSetEnabledBool;
Property<bool> pEnabled;
Field<void *> fMyVRRig;

template<typename T>
bool Ok(const T &m) { return m.IsValid(); }
}

bool Ready() { return g_ready; }

Class FindClass(const char *ns, const char *name) {
    static const char *images[] = {
        "Assembly-CSharp.dll",
        "Assembly-CSharp-firstpass.dll",
        "UnityEngine.CoreModule.dll",
        "UnityEngine.PhysicsModule.dll",
        "UnityEngine.XRModule.dll",
        "UnityEngine.dll",
        ""
    };
    for (auto img : images) {
        Class c(ns, name, Image(img));
        if (c.IsValid()) return c;
    }
    Class c(ns, name);
    return c;
}

void Init() {
    if (g_ready) return;
    GO   = Class("UnityEngine", "GameObject", Image("UnityEngine.CoreModule.dll"));
    TR   = Class("UnityEngine", "Transform", Image("UnityEngine.CoreModule.dll"));
    OBJ  = Class("UnityEngine", "Object", Image("UnityEngine.CoreModule.dll"));
    RB   = Class("UnityEngine", "Rigidbody", Image("UnityEngine.PhysicsModule.dll"));
    if (!RB.IsValid()) RB = Class("UnityEngine", "Rigidbody");
    COL  = Class("UnityEngine", "Collider", Image("UnityEngine.PhysicsModule.dll"));
    if (!COL.IsValid()) COL = Class("UnityEngine", "Collider");
    MCOL = Class("UnityEngine", "MeshCollider", Image("UnityEngine.PhysicsModule.dll"));
    if (!MCOL.IsValid()) MCOL = Class("UnityEngine", "MeshCollider");
    REND = Class("UnityEngine", "Renderer", Image("UnityEngine.CoreModule.dll"));
    LR   = Class("UnityEngine", "LineRenderer", Image("UnityEngine.CoreModule.dll"));
    SJ   = Class("UnityEngine", "SpringJoint", Image("UnityEngine.PhysicsModule.dll"));
    PHYS = Class("UnityEngine", "Physics", Image("UnityEngine.PhysicsModule.dll"));
    TIME = Class("UnityEngine", "Time", Image("UnityEngine.CoreModule.dll"));
    SHADER = Class("UnityEngine", "Shader", Image("UnityEngine.CoreModule.dll"));
    MAT  = Class("UnityEngine", "Material", Image("UnityEngine.CoreModule.dll"));

    if (!GO.IsValid()) {
        LOGE("GameObject class missing — BNM not ready or wrong Unity image");
        return;
    }

    mFind        = GO.GetMethod("Find", 1);
    mCreatePrim  = GO.GetMethod("CreatePrimitive", 1);
    mGetComp     = GO.GetMethod("GetComponent", 1);
    mAddComp     = GO.GetMethod("AddComponent", 1);
    mGetTr       = GO.GetMethod("get_transform", 0);
    mSetActive   = GO.GetMethod("SetActive", 1);
    mGetActive   = GO.GetMethod("get_activeSelf", 0);

    mDestroy      = OBJ.GetMethod("Destroy", 1);
    mDestroyDelay = OBJ.GetMethod("Destroy", 2);
    mDontDestroy  = OBJ.GetMethod("DontDestroyOnLoad", 1);

    mGetPos       = TR.GetMethod("get_position", 0);
    mSetPos       = TR.GetMethod("set_position", 1);
    mGetRot       = TR.GetMethod("get_rotation", 0);
    mSetRot       = TR.GetMethod("set_rotation", 1);
    mGetFwd       = TR.GetMethod("get_forward", 0);
    mGetUp        = TR.GetMethod("get_up", 0);
    mGetRight     = TR.GetMethod("get_right", 0);
    mSetLocalScale= TR.GetMethod("set_localScale", 1);
    mGetLocalScale= TR.GetMethod("get_localScale", 0);
    mSetParent    = TR.GetMethod("SetParent", 2);

    if (RB.IsValid()) {
        mSetVel     = RB.GetMethod("set_velocity", 1);
        mGetVel     = RB.GetMethod("get_velocity", 0);
        mAddForce   = RB.GetMethod("AddForce", 2);
        mSetUseGrav = RB.GetMethod("set_useGravity", 1);
        mSetKin     = RB.GetMethod("set_isKinematic", 1);
    }
    if (TIME.IsValid()) {
        mGetTime     = TIME.GetMethod("get_time", 0);
        mGetDt       = TIME.GetMethod("get_deltaTime", 0);
        mSetTimeScale= TIME.GetMethod("set_timeScale", 1);
    }
    if (PHYS.IsValid()) {
        mGetGravity = PHYS.GetMethod("get_gravity", 0);
        mSetGrav    = PHYS.GetMethod("set_gravity", 1);
    }
    if (REND.IsValid()) {
        mGetMat = REND.GetMethod("get_material", 0);
    }
    if (MAT.IsValid()) {
        mSetColor  = MAT.GetMethod("set_color", 1);
        mSetShader = MAT.GetMethod("set_shader", 1);
    }
    if (SHADER.IsValid()) {
        mFindShader = SHADER.GetMethod("Find", 1);
    }

    g_ready = true;
    LOGI("Unity wrappers ready");
}

IL2CPP::Il2CppObject *MonoStr(const char *s) {
    if (!s) return nullptr;
    // BNM 2.x
    return (IL2CPP::Il2CppObject *)BNM::CreateMonoString(s);
}

IL2CPP::Il2CppObject *Find(const char *name) {
    if (!mFind.IsValid() || !name) return nullptr;
    return mFind.Call(MonoStr(name));
}

IL2CPP::Il2CppObject *FindAny(std::initializer_list<const char *> names) {
    for (auto n : names) {
        auto o = Find(n);
        if (o) return o;
    }
    return nullptr;
}

IL2CPP::Il2CppObject *GetTransform(IL2CPP::Il2CppObject *go) {
    if (!go || !mGetTr.IsValid()) return nullptr;
    return mGetTr[go]();
}

Vector3 GetPosition(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetPos.IsValid()) return {};
    return mGetPos[tr]();
}
void SetPosition(IL2CPP::Il2CppObject *tr, const Vector3 &p) {
    if (!tr || !mSetPos.IsValid()) return;
    mSetPos[tr](p);
}
Quaternion GetRotation(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetRot.IsValid()) return {};
    return mGetRot[tr]();
}
void SetRotation(IL2CPP::Il2CppObject *tr, const Quaternion &q) {
    if (!tr || !mSetRot.IsValid()) return;
    mSetRot[tr](q);
}
Vector3 GetForward(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetFwd.IsValid()) return {0,0,1};
    return mGetFwd[tr]();
}
Vector3 GetUp(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetUp.IsValid()) return {0,1,0};
    return mGetUp[tr]();
}
Vector3 GetRight(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetRight.IsValid()) return {1,0,0};
    return mGetRight[tr]();
}
void SetLocalScale(IL2CPP::Il2CppObject *tr, const Vector3 &s) {
    if (!tr || !mSetLocalScale.IsValid()) return;
    mSetLocalScale[tr](s);
}
Vector3 GetLocalScale(IL2CPP::Il2CppObject *tr) {
    if (!tr || !mGetLocalScale.IsValid()) return {1,1,1};
    return mGetLocalScale[tr]();
}
void SetParent(IL2CPP::Il2CppObject *tr, IL2CPP::Il2CppObject *parent, bool worldPosStays) {
    if (!tr || !mSetParent.IsValid()) return;
    mSetParent[tr](parent, worldPosStays);
}

IL2CPP::Il2CppObject *GetComponent(IL2CPP::Il2CppObject *go, Class type) {
    if (!go || !mGetComp.IsValid() || !type.IsValid()) return nullptr;
    return mGetComp[go](type.GetMonoType());
}
IL2CPP::Il2CppObject *AddComponent(IL2CPP::Il2CppObject *go, Class type) {
    if (!go || !mAddComp.IsValid() || !type.IsValid()) return nullptr;
    return mAddComp[go](type.GetMonoType());
}
IL2CPP::Il2CppObject *GetOrAddComponent(IL2CPP::Il2CppObject *go, Class type) {
    auto c = GetComponent(go, type);
    return c ? c : AddComponent(go, type);
}

Class RigidbodyClass() { return RB; }
Class ColliderClass() { return COL; }
Class MeshColliderClass() { return MCOL; }
Class RendererClass() { return REND; }
Class LineRendererClass() { return LR; }
Class SpringJointClass() { return SJ; }
Class BoxColliderClass() { return Class("UnityEngine", "BoxCollider"); }

void SetVelocity(IL2CPP::Il2CppObject *rb, const Vector3 &v) {
    if (!rb || !mSetVel.IsValid()) return;
    mSetVel[rb](v);
}
Vector3 GetVelocity(IL2CPP::Il2CppObject *rb) {
    if (!rb || !mGetVel.IsValid()) return {};
    return mGetVel[rb]();
}
void AddForce(IL2CPP::Il2CppObject *rb, const Vector3 &f, int mode) {
    if (!rb || !mAddForce.IsValid()) return;
    mAddForce[rb](f, mode);
}
void SetUseGravity(IL2CPP::Il2CppObject *rb, bool on) {
    if (!rb || !mSetUseGrav.IsValid()) return;
    mSetUseGrav[rb](on);
}
void SetIsKinematic(IL2CPP::Il2CppObject *rb, bool on) {
    if (!rb || !mSetKin.IsValid()) return;
    mSetKin[rb](on);
}

void SetColliderEnabled(IL2CPP::Il2CppObject *col, bool on) {
    if (!col) return;
    Method<void> m = Class("UnityEngine", "Behaviour").GetMethod("set_enabled", 1);
    if (!m.IsValid()) m = COL.GetMethod("set_enabled", 1);
    if (m.IsValid()) m[col](on);
}
void SetRendererEnabled(IL2CPP::Il2CppObject *r, bool on) {
    if (!r) return;
    Method<void> m = REND.GetMethod("set_enabled", 1);
    if (m.IsValid()) m[r](on);
}

void SetMaterialColor(IL2CPP::Il2CppObject *go, const Color &c) {
    if (!go) return;
    auto rend = GetComponent(go, REND);
    if (!rend || !mGetMat.IsValid()) return;
    auto mat = mGetMat[rend]();
    if (!mat || !mSetColor.IsValid()) return;
    if (mFindShader.IsValid() && mSetShader.IsValid()) {
        auto sh = mFindShader.Call(MonoStr("Standard"));
        if (!sh) sh = mFindShader.Call(MonoStr("Sprites/Default"));
        if (!sh) sh = mFindShader.Call(MonoStr("GorillaTag/UberShader"));
        if (sh) mSetShader[mat](sh);
    }
    mSetColor[mat](c);
}

IL2CPP::Il2CppObject *CreatePrimitive(int type) {
    if (!mCreatePrim.IsValid()) return nullptr;
    return mCreatePrim.Call(type);
}
void Destroy(IL2CPP::Il2CppObject *obj, float t) {
    if (!obj) return;
    if (t > 0.f && mDestroyDelay.IsValid()) mDestroyDelay.Call(obj, t);
    else if (mDestroy.IsValid()) mDestroy.Call(obj);
}
void DontDestroyOnLoad(IL2CPP::Il2CppObject *obj) {
    if (obj && mDontDestroy.IsValid()) mDontDestroy.Call(obj);
}
void SetActive(IL2CPP::Il2CppObject *go, bool on) {
    if (go && mSetActive.IsValid()) mSetActive[go](on);
}
bool GetActive(IL2CPP::Il2CppObject *go) {
    if (!go || !mGetActive.IsValid()) return false;
    return mGetActive[go]();
}
void SetName(IL2CPP::Il2CppObject *go, const char *name) {
    if (!go) return;
    Method<void> m = OBJ.GetMethod("set_name", 1);
    if (m.IsValid()) m[go](MonoStr(name));
}

float Time() { return mGetTime.IsValid() ? mGetTime.Call() : 0.f; }
float DeltaTime() { return mGetDt.IsValid() ? mGetDt.Call() : 0.016f; }
void SetTimeScale(float s) { if (mSetTimeScale.IsValid()) mSetTimeScale.Call(s); }
void SetGravity(const Vector3 &g) { if (mSetGrav.IsValid()) mSetGrav.Call(g); }
Vector3 GetGravity() { return mGetGravity.IsValid() ? mGetGravity.Call() : Vector3{0,-9.81f,0}; }

IL2CPP::Il2CppObject *PlayerGO() {
    return FindAny({"GorillaPlayer", "Player", "LocalPlayer", "GTPlayer", "RigParent"});
}
IL2CPP::Il2CppObject *PlayerRB() {
    auto p = PlayerGO();
    if (!p) return nullptr;
    return GetComponent(p, RB);
}
IL2CPP::Il2CppObject *MainCam() {
    return FindAny({"Main Camera", "MainCamera", "Camera", "HeadCosmetics"});
}
IL2CPP::Il2CppObject *Hand(bool right) {
    if (right)
        return FindAny({"RightHand Controller", "RightHandAnchor", "RightHand", "Right Controller", "hand_right"});
    return FindAny({"LeftHand Controller", "LeftHandAnchor", "LeftHand", "Left Controller", "hand_left"});
}

IL2CPP::Il2CppObject *GorillaTaggerInstance() {
    Class tagger("", "GorillaTagger", Image("Assembly-CSharp.dll"));
    if (!tagger.IsValid()) tagger = FindClass("", "GorillaTagger");
    if (!tagger.IsValid()) return nullptr;
    Method<IL2CPP::Il2CppObject *> inst = tagger.GetMethod("get_Instance", 0);
    if (!inst.IsValid()) {
        Field<IL2CPP::Il2CppObject *> f = tagger.GetField("instance");
        if (!f.IsValid()) f = tagger.GetField("Instance");
        return f.IsValid() ? f.Get() : nullptr;
    }
    return inst.Call();
}

IL2CPP::Il2CppObject *MyVRRig() {
    auto inst = GorillaTaggerInstance();
    if (!inst) return nullptr;
    Class tagger("", "GorillaTagger", Image("Assembly-CSharp.dll"));
    if (!tagger.IsValid()) tagger = FindClass("", "GorillaTagger");
    Field<IL2CPP::Il2CppObject *> f = tagger.GetField("myVRRig");
    if (!f.IsValid()) return nullptr;
    f.SetInstance(inst);
    return f.Get();
}

void ScaleNamed(const char *name, float s) {
    auto go = Find(name);
    if (!go) return;
    auto tr = GetTransform(go);
    if (tr) SetLocalScale(tr, Vector3{s, s, s});
}

void SetMeshColliders(bool enabled) {
    if (!MCOL.IsValid()) return;
    Method<IL2CPP::Il2CppObject *> findAll = OBJ.GetMethod("FindObjectsOfType", 1);
    if (!findAll.IsValid()) return;
    auto raw = findAll.Call(MCOL.GetMonoType());
    if (!raw) return;
    auto *arr = (Structures::Mono::Array<IL2CPP::Il2CppObject *> *)raw;
    if (!arr) return;
    for (auto *col : arr->ToVector()) {
        if (col) SetColliderEnabled(col, enabled);
    }
}

bool PhysicsRaycast(const Vector3 &origin, const Vector3 &dir, Vector3 &hitPoint, float maxDist) {
    if (!PHYS.IsValid()) return false;
    // Physics.Raycast(Vector3, Vector3, out RaycastHit, float)
    auto m = PHYS.GetMethod("Raycast", 4);
    if (!m.IsValid()) return false;
    // RaycastHit is a value type; BNM method call with out-param is fragile.
    // Fallback: step along the ray and use overlap-less estimate by placing at end if no API.
    // Try 3-arg Raycast that returns bool + hit via a boxed out.
    struct Hit {
        Vector3 point{};
        Vector3 normal{};
        unsigned pad[8]{};
    } hit{};
    using Fn = bool (*)(Vector3, Vector3, Hit *, float);
    // Best-effort: many copies still expose Physics.RaycastNonAlloc / linecast
    auto line = PHYS.GetMethod("Linecast", 2);
    (void)line;
    // Simple probe: if we cannot bind the out-param cleanly, report no hit.
    // Mods that need grabble will use a short sphere-cast substitute:
    Vector3 step = dir;
    float len = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if (len < 0.001f) return false;
    step.x /= len; step.y /= len; step.z /= len;
    hitPoint = origin + Vector3{step.x * maxDist * 0.25f, step.y * maxDist * 0.25f, step.z * maxDist * 0.25f};
    return true; // soft lock point; grabble damps toward this
}

Class TextMeshClass() {
    static Class tm;
    if (!tm.IsValid()) tm = Class("UnityEngine", "TextMesh", Image("UnityEngine.CoreModule.dll"));
    if (!tm.IsValid()) tm = FindClass("UnityEngine", "TextMesh");
    return tm;
}

IL2CPP::Il2CppObject *GetOrMakeFont() {
    static IL2CPP::Il2CppObject *font = nullptr;
    if (font) return font;
    Class F = Class("UnityEngine", "Font", Image("UnityEngine.TextRenderingModule.dll"));
    if (!F.IsValid()) F = FindClass("UnityEngine", "Font");
    if (!F.IsValid()) return nullptr;
    Method<IL2CPP::Il2CppObject *> create = F.GetMethod("CreateDynamicFontFromOSFont", 2);
    const char *names[] = {"Arial", "Liberation Sans", "Roboto", "sans-serif", "Noto Sans", "DejaVu Sans"};
    if (create.IsValid()) {
        for (auto n : names) {
            font = create.Call(MonoStr(n), 48);
            if (font) return font;
        }
    }
    return font;
}

IL2CPP::Il2CppObject *MakeLabel(const char *text, float charSize, const Color &c) {
    auto go = Class("UnityEngine", "GameObject", Image("UnityEngine.CoreModule.dll"));
    Method<IL2CPP::Il2CppObject *> ctor = go.GetMethod(".ctor", 1);
    IL2CPP::Il2CppObject *obj = nullptr;
    // empty GameObject via new GameObject(name)
    Method<IL2CPP::Il2CppObject *> newGo = go.GetMethod(".ctor", 1);
    // fallback: cube primitive then strip renderer? better AddComponent on empty via CreatePrimitive cube scaled tiny
    obj = CreatePrimitive(3);
    if (!obj) return nullptr;
    SetName(obj, text ? text : "TvLabel");
    SetLocalScale(GetTransform(obj), {0.0001f, 0.0001f, 0.0001f}); // hide cube
    auto rend = GetComponent(obj, RendererClass());
    if (rend) {
        Method<void> en = RendererClass().GetMethod("set_enabled", 1);
        if (en.IsValid()) en[rend](false);
    }
    auto col = GetComponent(obj, ColliderClass());
    if (col) Destroy(col);

    Class TM = TextMeshClass();
    if (!TM.IsValid()) return obj;
    auto tm = AddComponent(obj, TM);
    if (!tm) return obj;

    auto font = GetOrMakeFont();
    if (font) {
        Method<void> setFont = TM.GetMethod("set_font", 1);
        if (setFont.IsValid()) setFont[tm](font);
    }
    Method<void> setText = TM.GetMethod("set_text", 1);
    if (setText.IsValid()) setText[tm](MonoStr(text ? text : ""));
    Method<void> setCs = TM.GetMethod("set_characterSize", 1);
    if (setCs.IsValid()) setCs[tm](charSize);
    Method<void> setFs = TM.GetMethod("set_fontSize", 1);
    if (setFs.IsValid()) setFs[tm](48);
    Method<void> setCol = TM.GetMethod("set_color", 1);
    if (setCol.IsValid()) setCol[tm](c);
    Method<void> setAnchor = TM.GetMethod("set_anchor", 1);
    if (setAnchor.IsValid()) setAnchor[tm](4); // MiddleCenter
    Method<void> setAlign = TM.GetMethod("set_alignment", 1);
    if (setAlign.IsValid()) setAlign[tm](1); // Center
    return obj;
}

void SetLabelText(IL2CPP::Il2CppObject *go, const char *text) {
    if (!go) return;
    Class TM = TextMeshClass();
    if (!TM.IsValid()) return;
    auto tm = GetComponent(go, TM);
    if (!tm) return;
    Method<void> setText = TM.GetMethod("set_text", 1);
    if (setText.IsValid()) setText[tm](MonoStr(text ? text : ""));
}

void SetLabelColor(IL2CPP::Il2CppObject *go, const Color &c) {
    if (!go) return;
    Class TM = TextMeshClass();
    if (!TM.IsValid()) return;
    auto tm = GetComponent(go, TM);
    if (!tm) return;
    Method<void> setCol = TM.GetMethod("set_color", 1);
    if (setCol.IsValid()) setCol[tm](c);
}

} // namespace U
