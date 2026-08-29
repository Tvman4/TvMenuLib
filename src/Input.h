#pragma once

namespace XR {
enum class Controller { Left, Right };
enum class FloatFeature { Trigger, Grip, GripForce };
enum class BoolFeature { TriggerButton, GripButton, PrimaryButton, SecondaryButton, PrimaryTouch, SecondaryTouch };

void Tick();
float GetFloat(FloatFeature f, Controller c);
bool  GetBool(BoolFeature f, Controller c);
bool  GetBoolDown(BoolFeature f, Controller c);
}
