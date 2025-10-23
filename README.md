# UEcs
A simple, intuitive ECS system for UnrealEngine

---

## 📚 Table of Contents

1. [Current State](#1-Current-State)
1. [Design Principles](#1-Design-Principles)

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

## TODO
1. Making entt private. Currently it needs to be public because I'm a lazy person.

