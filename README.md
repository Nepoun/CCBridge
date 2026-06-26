<img src="assets/icon.png" alt="CCBridge logo" width="128" align="left" style="margin-right: 16px;"/>

# CCBridge

A desktop control center for ComputerCraft turtles, written in C++.

The goal is to bridge CC:Tweaked turtles into a proper desktop interface, making it easier to monitor, debug, and control them without ever opening the in-game terminal.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![Status](https://img.shields.io/badge/status-alpha-orange)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Overview

CCBridge connects to turtles over WebSocket and exposes their state in a dockable UI built with Dear ImGui. The longer-term goal is a full 3D viewport that loads terrain data directly from the turtles and renders it using textures pulled from a local `.minecraft` installation.

This is a personal side project, so expect rough edges and breaking changes. The repository is currently a code showcase rather than a ready-to-use tool.

**Minecraft:** CC:Tweaked 1.119.0 for Minecraft 1.21.1 (NeoForge). Other versions may work but are untested.

## Status

Early alpha. The communication layer and base UI are working; most of the higher-level features are still in progress.

Current focus:
- Stabilizing the WebSocket communication layer
- Building out core UI panels
- Laying groundwork for the 3D world visualization

## Features

**Working now**
- [x] WebSocket communication backbone
- [x] Turtle registration and status tracking
- [x] Dockable UI framework (panels, tabs)
- [x] Debug and profiler panel

**In progress / planned**
- [ ] 3D rendering viewport
- [ ] Turtle details and inventory panels
- [ ] Block map built from exploration data
- [ ] Minecraft texture and model loading
- [ ] Mod support
- [ ] Lua script sync
- [ ] GPS auto-setup integration

## Building

> Rough around the edges for now. Proper build docs are coming once things stabilize.

**Requirements:**
- CMake 3.20+
- C++20 compiler
- Dependencies listed in [Tech Stack](#tech-stack) (Raylib needs to be installed manually for now)

```bash
git clone https://github.com/Nepoun/CCBridge.git
cd CCBridge
./BuildAndRun.bat
```

## Lua Scripts

ComputerCraft scripts live under `assets/lua/`. See [assets/lua/README.md](assets/lua/README.md) for setup instructions.

## Tech Stack

`C++20` · `Raylib` · `Dear ImGui` · `rlImGui` · `Crow` · `Asio` · `nlohmann/json`

## Goals

- Remove the need to open the turtle terminal for monitoring, debugging, or scripting
- Load textures and models from a local `.minecraft` installation
- Support CC:Tweaked addons
- Be usable by both programmers and non-programmers

---

*Screenshot coming once the UI stops changing every other day.*