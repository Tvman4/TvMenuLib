# TvMenu — Quest / Android lib menu for Gorilla Tag copies

Native `libTvMenu.so` for **Unity 2021.3** IL2CPP GTAG skids that keep the stock object names (`GorillaPlayer`, `RightHand Controller`, `Main Camera`, `GorillaTagger`, `VRRig`, `ControllerInputPoller`).

- Wrist panel on the **left hand**, blood-red
- **Red dot above the right finger** to hover buttons
- Right **trigger** clicks the hovered mod
- Left **Y / X** toggles the menu
- Right **B** next page, right **A** previous page
- Built by `.github/workflows/build.yml` → `libTvMenu.so` (`arm64-v8a`)

## Controls

| Input | Action |
|---|---|
| Left Y or X | Open / close wrist menu |
| Red dot over a button + right trigger | Toggle / fire mod |
| Right B | Next page |
| Right A (when not hovering) | Previous page |
| Panic Off | Turns every toggle off and resets physics |

Movement mods that use trigger (Hand Fly, Bark Fly, Head Fly, rockets) keep using the **right trigger** while enabled — close the menu first so you do not click buttons.

## Build (GitHub Actions)

1. Push this folder to a GitHub repo.
2. Actions → **Build libTvMenu.so** → download artifact `libTvMenu-arm64`.

The workflow clones [BNM-Android](https://github.com/ByNameModding/BNM-Android) and [Dobby](https://github.com/jmpews/Dobby), sets `UNITY_VER 213` (2021.3), and compiles `libTvMenu.so`.

## Inject into a copy APK

1. `apktool d game.apk`
2. Copy `libTvMenu.so` into `game/lib/arm64-v8a/`
3. Load it from the Unity activity (`UnityPlayerActivity` / your copy's main activity):

```java
static {
    try { System.loadLibrary("TvMenu"); } catch (Throwable t) {}
}
```

4. `apktool b game` and sign.

If the copy already loads extra `.so` files from `mainTemplate.gradle` / `il2cpp`, you can add `TvMenu` next to those instead.

## What is actually hooked

BNM resolves classes by name after `il2cpp` is up:

- `UnityEngine.GameObject.Find` / `CreatePrimitive` / transforms / rigidbody
- `ControllerInputPoller` fields (same names as stock GTAG)
- `GorillaTagger.Instance` + `myVRRig` for ghost
- `LateUpdate` on `GorillaTagger` (fallback 90 Hz thread if the copy renamed it)

Stock names used when finding objects:

`GorillaPlayer`, `Player`, `LocalPlayer`, `RightHand Controller`, `LeftHand Controller`, `Main Camera`

## Mods

100 entries. The ones from your snippets are first-class:

- Hand Fly / Bark Fly / Head Fly
- Platforms (+ RGB)
- Up And Down
- NoClip
- Ghost Monkey
- Dash
- Grapple (Cloudy-style pull)

The rest are working variants of those systems (speed/force/scale/gravity steps), plus size, arms, time scale, and Panic Off. They all go through the same rigidbody / transform / collider path — they are not empty labels.

## Limits (read this)

- A `.so` cannot be guaranteed on **every** APK from here. If a copy stripped `ControllerInputPoller` or renamed the player object, fly still works once those names are added to `Unity.cpp`.
- NoClip walks `MeshCollider`s. Huge maps can hitch once when toggled.
- Grapple uses a pull toward a point along the hand forward (full `Physics.Raycast` out-param is copy-dependent).
- Do not use this on the official live Gorilla Tag build if that violates that game's rules. This tree targets **copies / skids** that share the 2021.3 prefab names.

## Layout

```
.github/workflows/build.yml   # produces libTvMenu.so
include/BNM/UserSettings/     # UNITY_VER 213 + Dobby hooks
src/main.cpp                  # JNI_OnLoad + BNM load + tick hook
src/Unity.cpp                 # IL2CPP wrappers
src/Input.cpp                 # ControllerInputPoller
src/Menu.cpp                  # blood-red wrist UI + finger dot
src/Mods.cpp                  # the 100 mods
```
