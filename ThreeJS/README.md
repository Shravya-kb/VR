## Slash 3D Survival Game





## Overview
This is a 3D survival game built using Three.js, featuring an immersive open-world environment with player character, NPCs, and dynamic gameplay mechanics.

## Features

### Game Mechanics
- Third-person character control
- Dynamic NPC spawning
- Health system
- Inventory management
- Weapon equipping
- Level-up system
- Quest interactions

### Environment
- Procedurally generated landscape
- Dynamic sky and lighting
- Foliage and cloud generation
- Spatial grid-based entity management

### Characters
- Player character with customizable attributes
- Multiple monster types (Ghost, Alien, Skull, Green Demon, Cyclops, Cactus)
- NPC interactions
- Health bars for entities

## Technical Components

### Core Technologies
- Three.js
- WebGL
- Custom entity management system
- Spatial hash grid for performance optimization

### Key Modules
- Player input handling
- Third-person camera
- Inventory controller
- Attack mechanics
- Level-up system
- Quest component

## Prerequisites
- Modern web browser with WebGL support
- JavaScript enabled
- Recommended browsers: Chrome, Firefox, Safari, Edge (with Hardware Acceleration enabled)

## Installation

1. Clone the repository
```bash
git clone https://github.com/notaprocoder69/Slash-3D-Survival-Game.git
```

2. Navigate to project directory
```bash
cd Slash-3D-Survival-Game
```

3. Serve the project using a local web server
   - Recommended: Use `live-server` or Python's `http.server`

## Running the Game
- Open `index.html` in a web browser
- Ensure all resource paths are correctly configured

## Resource Dependencies
- External Three.js library (CDN)
- Custom model resources in `./resources/` directory
  - Characters
  - Weapons
  - Environment models

## Gameplay Controls
- Movement: WASD keys
- Camera: Mouse movement
- Attack: Space Bar
- Inventory management: Drag n Drop

## Performance Optimization
- Spatial hash grid for efficient entity management
- Limited NPC and environment object rendering
- Efficient shadow and lighting techniques

## Future Roadmap
- Implement more complex quest system
- Add more weapon and character customization
- Expand NPC interactions
- Implement multiplayer functionality
- Add story mode
- improve Terrain, lighting & texture

## Known Issues
- Performance may vary based on hardware
- Potential memory leaks with large number of entities
- Browser compatibility variations


