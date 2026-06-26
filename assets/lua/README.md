# Lua Scripts

Drop these scripts onto your turtles. You can run them by name, but renaming to `startup.lua` is recommended since they're designed to run on startup.

## Scripts

- **simple_controllable_turtle.lua**  
  Basic controllable turtle. Equip it with a wireless modem to see coordinates. Accepts commands from the terminal and will eventually scout and save terrain data.

- **auto_gps_turtle.lua**  
  Automatically sets up a GPS constellation, provided you supply the right items and enough clear space.  
  **Requires a pickaxe equipped, otherwise it won't work.**  
  Placing the turtle on a chunk border facing inward is recommended since GPS works better when contained within a single chunk.

- **simple_gps_setup.lua**  
  Standalone GPS host setup. Use this if you prefer to build the constellation manually.

## GPS Setup

Every GPS host computer needs to run `simple_gps_setup` (unless you're using `auto_gps_turtle`).

Official guide: https://tweaked.cc/guide/gps_setup.html

If you're building the constellation yourself, the layout should look like this:

![GPS constellation layout](../gps_setup.png)