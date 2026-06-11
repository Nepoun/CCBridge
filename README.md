# CCBridge

CCBridge is a desktop control center for ComputerCraft turtles, written in C++.

The project started as a personal tool to avoid dealing with ComputerCraft's built-in terminal interface, which gets unwieldy fast when managing multiple turtles at once. The idea grew into something more complete: a local application where multiple turtles can connect simultaneously and be monitored, controlled and scripted from a single interface.

Built for fun and as a portfolio project.

## Current Status

Early stage. Most systems are still experimental and subject to major changes.

Current focus is on solidifying the communication layer between C++ and turtles, building out the core UI panels, and establishing the 3D world visualization system.

## Using This Repository

Currently meant as a code showcase rather than a ready-to-use tool.
To get things running you will need Raylib, Dear ImGui, rlImGui, Crow and Asio set up locally, as well as a ComputerCraft environment to connect turtles from. Setup instructions will improve as the project matures.

## Features

**Communication:** WebSocket server, turtle registration and unregistration, real-time state updates, command queue.

**Interface:** Dockable window layout, turtle list with online/offline indicators, debug and profiler panel.

**Planned:** Turtle details and inventory panels, 3D world viewport, block map reconstructed from exploration data, Minecraft texture and model loading, mod support, Lua script sync.

## Tech Stack

C++20, Raylib, Dear ImGui, rlImGui, Crow, Asio, nlohmann/json

## Goals

Remove the need to ever open the turtle terminal for monitoring, debugging or scripting. Eventually support mods and load textures and models directly from the local .minecraft installation.