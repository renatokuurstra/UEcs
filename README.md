# UEcs
A simple, intuitive ECS system for UnrealEngine

---

## 📚 Table of Contents

1. [Current State](#1-current-state)
1. [Design Principles](#2-design-principles)
1. [Usage](#3-usage)

---

## 1. Current State
Just started, everything is bad and little work. Refactoring as I use it myself and write more and more tests.
I'm using Entt(https://github.com/skypjack/entt), to test this you will need to go and grab it (check the third party folder there is a .md that explain everything). If you plan to use this, never reference somehting directly from Entt, as I'm not 100% I'll be keep using it in the future.
Ideally, in the future I could switch into leveraging MASS, as the archetype structure could make for a more robust and flexible thread safety model. 

---

## 2. Design Principles

I'm a big fan of Data Oriented Programming. Since using Unreal Engine I always struggle to follow this pattern as it feels you are working against the grain of the engine.
When MASS was announced I was happy, finally I could be using DOP very efficently, and easily. Turns out, no I can't. I think MASS is envisioned as a tool to expand the engine, and not to build game logic quickly and efficently. Each step is a struggle, and even understanding how to properly set order of execution of the various system is a chore.

I want this to be very simple, relateively strict and intuitive. After a first setup to understand how to better work with templates(entt is heavely relying on templates for everything) and UE blueprint reflection.

In my world even blueprints will be able to use this, as I'm a big fan of democratizing the development process. The goal of this plugin is not only forcing the user to follow more DOP principles, but also to simplify and publicize the ECS pattern.

---

## 3. Usage

### IEcsEventElement
The `IEcsEventElement` is a blueprintable interface that defines the lifecycle for any element in the ECS event chain. Both `UEcsSystem` and `AEcsContext` implement this interface, allowing for nested contexts and modular logic.

- **Initialize**: Called to set up the element with its owning context.
- **Update**: Called periodically or every frame to execute logic.
- **Deinitialize**: Called to clean up the element.

### AEcsContext
The `AEcsContext` actor is the heart of the ECS. It owns the `entt::registry` and manages the lifecycle of systems and nested contexts. It now also implements `IEcsEventElement`, allowing one context to be nested within another.

### UEcsSystem
Systems are `UObject`s that implement logic. To create a system, inherit from `UEcsSystem`. They implement `IEcsEventElement` to participate in the ECS lifecycle.

### UEcsChainEvents
A `UDataAsset` that maps `FName` event identifiers to a set of `IEcsEventElement`s. This allows for data-driven execution of logic at specific moments (like `BeginPlay` or `EndPlay`) or periodically.

#### Periodic Events
Each entry in `ChainEvents` can be configured as a periodic update:
- **bIsUpdateSystems**: If true, `AEcsContext` will start a looping timer for this event on `BeginPlay`.
- **UpdateFreqSec**: The frequency in seconds at which the elements in this event will be executed.
- **Elements**: The list of `IEcsEventElement`s (Systems or Contexts) stored as `TScriptInterface<IEcsEventElement>`.

### Utility: MakeEcsEventElement
A helper function is provided in `EcsEventElement.h` to easily wrap a `UObject*` into a `TScriptInterface<IEcsEventElement>`:

```cpp
TScriptInterface<IEcsEventElement> Interface = MakeEcsEventElement(MyObject);
```

### Overriding Lifecycle Methods
Since `IEcsEventElement` methods are `BlueprintNativeEvent`s, when overriding them in C++, use the `_Implementation` suffix:

```cpp
virtual void Initialize_Implementation(AEcsContext* InContext) override;
virtual void Update_Implementation(float DeltaTime) override;
virtual void Deinitialize_Implementation() override;
```

To invoke these methods from C++, always use the generated `Execute_` functions:

```cpp
IEcsEventElement::Execute_Initialize(Element.GetObject(), Context);
IEcsEventElement::Execute_Update(Element.GetObject(), DeltaTime);
IEcsEventElement::Execute_Deinitialize(Element.GetObject());
```

#### Accessing the Registry
Inside a C++ system, use `GetRegistry()` to access the `entt::registry`.

#### Accessing the Context
Inside a system, use `GetContext()` or `GetTypedContext<T>()` to access the owning `AEcsContext`. This is preferred over `GetOuter()` as systems may be owned by `UDataAsset`s (e.g., when using `UEcsChainEvents`).

## TODO
1. Making entt private. Currently it needs to be public because I'm a lazy person.

