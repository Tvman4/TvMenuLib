#include "Input.h"
#include "Unity.h"
#include "Log.h"
#include <cmath>

namespace XR {
namespace {
float LTrig = 0, RTrig = 0, LGrip = 0, RGrip = 0;
bool LPrim = false, RPrim = false, LSec = false, RSec = false;
bool prevLTrigB = false, prevRTrigB = false, prevLGripB = false, prevRGripB = false;
bool prevLPrim = false, prevRPrim = false, prevLSec = false, prevRSec = false;
bool dLTrigB = false, dRTrigB = false, dLGripB = false, dRGripB = false;
bool dLPrim = false, dRPrim = false, dLSec = false, dRSec = false;

bool TryControllerInputPoller() {
    Class poll("", "ControllerInputPoller", Image("Assembly-CSharp.dll"));
    if (!poll.IsValid()) poll = U::FindClass("", "ControllerInputPoller");
    if (!poll.IsValid()) return false;
    Method<IL2CPP::Il2CppObject *> instM = poll.GetMethod("get_instance", 0);
    IL2CPP::Il2CppObject *inst = nullptr;
    if (instM.IsValid()) inst = instM.Call();
    if (!inst) {
        Field<IL2CPP::Il2CppObject *> f = poll.GetField("instance");
        if (f.IsValid()) inst = f.Get();
    }
    if (!inst) return false;

    auto readF = [&](const char *n) -> float {
        Field<float> f = poll.GetField(n);
        if (!f.IsValid()) return 0.f;
        f.SetInstance(inst);
        return f.Get();
    };
    auto readB = [&](const char *n) -> bool {
        Field<bool> f = poll.GetField(n);
        if (!f.IsValid()) return false;
        f.SetInstance(inst);
        return f.Get();
    };

    LTrig = readF("leftControllerIndexFloat");
    RTrig = readF("rightControllerIndexFloat");
    LGrip = readF("leftControllerGripFloat");
    RGrip = readF("rightControllerGripFloat");
    LPrim = readB("leftControllerPrimaryButton");
    RPrim = readB("rightControllerPrimaryButton");
    LSec  = readB("leftControllerSecondaryButton");
    RSec  = readB("rightControllerSecondaryButton");
    if (LTrig == 0.f && RTrig == 0.f) {
        LTrig = readF("leftIndex");
        RTrig = readF("rightIndex");
    }
    return true;
}

bool TryOVR() {
    Class ovr("", "OVRInput", Image("Assembly-CSharp.dll"));
    if (!ovr.IsValid()) ovr = Class("Oculus.VR", "OVRInput");
    if (!ovr.IsValid()) ovr = U::FindClass("", "OVRInput");
    if (!ovr.IsValid()) return false;
    // Get(Axis1D, Controller) — enum values vary; skip if signatures don't match
    return false;
}

void Edge(bool now, bool &prev, bool &down) {
    down = now && !prev;
    prev = now;
}
}

void Tick() {
    TryControllerInputPoller();
    Edge(RTrig > 0.55f, prevRTrigB, dRTrigB);
    Edge(LTrig > 0.55f, prevLTrigB, dLTrigB);
    Edge(RGrip > 0.55f, prevRGripB, dRGripB);
    Edge(LGrip > 0.55f, prevLGripB, dLGripB);
    Edge(RPrim, prevRPrim, dRPrim);
    Edge(LPrim, prevLPrim, dLPrim);
    Edge(RSec,  prevRSec,  dRSec);
    Edge(LSec,  prevLSec,  dLSec);
}

float GetFloat(FloatFeature f, Controller c) {
    if (f == FloatFeature::Trigger) return c == Controller::Right ? RTrig : LTrig;
    return c == Controller::Right ? RGrip : LGrip;
}
bool GetBool(BoolFeature f, Controller c) {
    switch (f) {
        case BoolFeature::TriggerButton: return (c == Controller::Right ? RTrig : LTrig) > 0.55f;
        case BoolFeature::GripButton:    return (c == Controller::Right ? RGrip : LGrip) > 0.55f;
        case BoolFeature::PrimaryButton: return c == Controller::Right ? RPrim : LPrim;
        case BoolFeature::SecondaryButton:
        default:                         return c == Controller::Right ? RSec : LSec;
    }
}
bool GetBoolDown(BoolFeature f, Controller c) {
    switch (f) {
        case BoolFeature::TriggerButton: return c == Controller::Right ? dRTrigB : dLTrigB;
        case BoolFeature::GripButton:    return c == Controller::Right ? dRGripB : dLGripB;
        case BoolFeature::PrimaryButton: return c == Controller::Right ? dRPrim : dLPrim;
        case BoolFeature::SecondaryButton:
        default:                         return c == Controller::Right ? dRSec : dLSec;
    }
}
}
