#pragma once
#include <string>
#include <vector>
#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Image.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Defaults.hpp>
#include <BNM/Utils.hpp>

using namespace BNM;
using namespace BNM::Structures;
using namespace BNM::Structures::Unity;

namespace U {

void Init();
bool Ready();

IL2CPP::Il2CppObject *MonoStr(const char *s);

IL2CPP::Il2CppObject *Find(const char *name);
IL2CPP::Il2CppObject *FindAny(std::initializer_list<const char *> names);
IL2CPP::Il2CppObject *GetTransform(IL2CPP::Il2CppObject *go);
Vector3 GetPosition(IL2CPP::Il2CppObject *tr);
void SetPosition(IL2CPP::Il2CppObject *tr, const Vector3 &p);
Quaternion GetRotation(IL2CPP::Il2CppObject *tr);
void SetRotation(IL2CPP::Il2CppObject *tr, const Quaternion &q);
Vector3 GetForward(IL2CPP::Il2CppObject *tr);
Vector3 GetUp(IL2CPP::Il2CppObject *tr);
Vector3 GetRight(IL2CPP::Il2CppObject *tr);
void SetLocalScale(IL2CPP::Il2CppObject *tr, const Vector3 &s);
Vector3 GetLocalScale(IL2CPP::Il2CppObject *tr);
void SetParent(IL2CPP::Il2CppObject *tr, IL2CPP::Il2CppObject *parent, bool worldPosStays = false);

IL2CPP::Il2CppObject *GetComponent(IL2CPP::Il2CppObject *go, Class type);
IL2CPP::Il2CppObject *AddComponent(IL2CPP::Il2CppObject *go, Class type);

Class RigidbodyClass();
Class ColliderClass();
Class MeshColliderClass();
Class RendererClass();
Class LineRendererClass();
Class SpringJointClass();

void SetVelocity(IL2CPP::Il2CppObject *rb, const Vector3 &v);
Vector3 GetVelocity(IL2CPP::Il2CppObject *rb);
void AddForce(IL2CPP::Il2CppObject *rb, const Vector3 &f, int mode);
void SetUseGravity(IL2CPP::Il2CppObject *rb, bool on);
void SetIsKinematic(IL2CPP::Il2CppObject *rb, bool on);
void SetColliderEnabled(IL2CPP::Il2CppObject *col, bool on);
void SetMaterialColor(IL2CPP::Il2CppObject *go, const Color &c);

IL2CPP::Il2CppObject *CreatePrimitive(int type);
void Destroy(IL2CPP::Il2CppObject *obj, float t = 0.f);
void DontDestroyOnLoad(IL2CPP::Il2CppObject *obj);
void SetActive(IL2CPP::Il2CppObject *go, bool on);
void SetName(IL2CPP::Il2CppObject *go, const char *name);

float Time();
float DeltaTime();
void SetTimeScale(float s);
void SetGravity(const Vector3 &g);
Vector3 GetGravity();

IL2CPP::Il2CppObject *PlayerGO();
IL2CPP::Il2CppObject *PlayerRB();
IL2CPP::Il2CppObject *MainCam();
IL2CPP::Il2CppObject *Hand(bool right);
IL2CPP::Il2CppObject *GorillaTaggerInstance();
IL2CPP::Il2CppObject *MyVRRig();

void ScaleNamed(const char *name, float s);
void SetMeshColliders(bool enabled);
}
