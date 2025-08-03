import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.118.1/build/three.module.js';
import {FBXLoader} from 'https://cdn.jsdelivr.net/npm/three@0.118.1/examples/jsm/loaders/FBXLoader.js';
import {entity} from './entity.js';

export const girl_controller = (() => {

  class GirlController extends entity.Component {
    constructor(params) {
      super();
      this._Init(params);
    }

    _Init(params) {
      this._params = params || {};
      this._detectionRange = params.detectionRange || 15; // Range to detect monsters
      this._fleeSpeed = params.fleeSpeed || 2.0; // Speed when fleeing
      this._walkSpeed = params.walkSpeed || 1.0; // Normal walking speed
      this._velocity = new THREE.Vector3(0, 0, 0);
      this._isRunning = false;
      this._fleeDirection = new THREE.Vector3();
      this._model = null;
      this._mixer = null;
      this._animations = {};
      this._currentAction = null;
      this._isLoaded = false;
      this._walkTimer = 0;
      this._walkDirection = new THREE.Vector3(
        Math.random() - 0.5, 
        0, 
        Math.random() - 0.5
      ).normalize();
    }

    InitComponent() {
      // Register to receive model updates
      this._RegisterHandler('load.character', (m) => { this._OnCharacterLoaded(m); });
      
      // Register to handle health updates when damaged
      this._RegisterHandler('health.damage', (m) => { this._OnHealthChanged(m); });
      
      // Register to handle death
      this._RegisterHandler('health.death', (m) => { this._OnDeath(m); });
      
      // Initialize health display
      this._UpdateHealthDisplay();
    }

    _OnDeath(msg) {
      console.log("Girl has died!");
      // Stop all movement
      this._velocity.set(0, 0, 0);
      this._isRunning = false;
      
      // Could add death animation here if available
      if (this._currentAction) {
        this._currentAction.stop();
      }
      
      // Kill the player when girl dies
      this._KillPlayer();
    }

    _KillPlayer() {
      // Get the entity manager (parent of the girl entity)
      const entityManager = this._parent._parent;
      if (entityManager) {
        const player = entityManager.Get('player');
        if (player) {
          console.log("Girl's death is causing player death!");
          
          // Get player's health component and kill them
          const playerHealth = player.GetComponent('HealthComponent');
          if (playerHealth) {
            playerHealth._health = 0;
            
            // Broadcast player death
            player.Broadcast({
              topic: 'health.death',
              health: 0,
              maxHealth: playerHealth._maxHealth,
            });
            
            console.log("Player has died due to girl's death!");
          }
        }
      }
    }

    _OnHealthChanged(msg) {
      // Update health display when health changes
      this._UpdateHealthDisplay();
      
      // Optional: Add visual feedback when taking damage
      console.log(`Girl took ${msg.value} damage!`);
    }

    _UpdateHealthDisplay() {
      // Get health component and broadcast health update for the health bar
      const healthComponent = this._parent.GetComponent('HealthComponent');
      if (healthComponent) {
        this._parent.Broadcast({
          topic: 'health.update',
          health: healthComponent._health,
          maxHealth: healthComponent._maxHealth,
        });
      }
    }

    _OnCharacterLoaded(msg) {
      this._model = msg.model;
      this._isLoaded = true;
      console.log("Girl model loaded successfully!");
      
      // Get the mixer from the AnimatedModelComponent
      const animComponent = this._parent.GetComponent('AnimatedModelComponent');
      if (animComponent && animComponent._mixer) {
        this._mixer = animComponent._mixer;
        this._LoadAdditionalAnimations();
      }
    }

    _LoadAdditionalAnimations() {
      if (!this._model || !this._mixer) {
        return;
      }

      const loader = new FBXLoader();
      loader.setPath('./resources/girl/');
      
      // Load the FBX file which contains both idle and run animations
      loader.load('Run_Look_Back_girl.fbx', (fbx) => {
        if (fbx.animations && fbx.animations.length >= 2) {
          // First animation is idle, second is run
          const idleClip = fbx.animations[0];
          const runClip = fbx.animations[1];
          
          this._animations.idle = {
            clip: idleClip,
            action: this._mixer.clipAction(idleClip),
          };
          
          this._animations.run = {
            clip: runClip,
            action: this._mixer.clipAction(runClip),
          };
          
          // Set initial animation to idle
          this._currentAction = this._animations.idle.action;
          this._currentAction.setLoop(THREE.LoopRepeat);
          this._currentAction.play();
          
          console.log("Both idle and run animations loaded successfully!");
        } else {
          console.log("Expected 2 animations in FBX, found:", fbx.animations?.length || 0);
        }
      });
    }

    _FindNearbyMonsters() {
      const gridController = this._parent.GetComponent('SpatialGridController');
      if (!gridController) {
        return [];
      }

      const nearby = gridController.FindNearbyEntities(this._detectionRange);
      
      // Filter to only include monsters (NPCs with NPCController component)
      const monsters = nearby.filter(entity => {
        const npcController = entity.entity.GetComponent('NPCController');
        const healthComponent = entity.entity.GetComponent('HealthComponent');
        
        // Check if it's an NPC and alive
        return npcController && healthComponent && healthComponent._health > 0;
      });

      return monsters;
    }

    _CalculateFleeDirection(monsters) {
      if (monsters.length === 0) {
        return new THREE.Vector3(0, 0, 0);
      }

      // Calculate average position of all nearby monsters
      const avgMonsterPos = new THREE.Vector3(0, 0, 0);
      monsters.forEach(monster => {
        avgMonsterPos.add(monster.entity._position);
      });
      avgMonsterPos.divideScalar(monsters.length);

      // Direction away from monsters
      const fleeDirection = new THREE.Vector3()
        .subVectors(this._parent._position, avgMonsterPos)
        .normalize();

      return fleeDirection;
    }

    _StartRunning() {
      if (!this._isRunning && this._animations.run) {
        this._isRunning = true;
        console.log("Girl started running!");
        
        // Switch to running animation
        if (this._currentAction) {
          this._currentAction.stop();
        }
        
        this._currentAction = this._animations.run.action;
        this._currentAction.reset();
        this._currentAction.setLoop(THREE.LoopRepeat);
        this._currentAction.play();
        
      } else if (!this._isRunning) {
        // Run animation not loaded yet, just mark as running
        this._isRunning = true;
        console.log("Girl started running (animation not loaded yet)");
      }
    }

    _StopRunning() {
      if (this._isRunning) {
        this._isRunning = false;
        this._velocity.set(0, 0, 0);
        console.log("Girl stopped running, back to idle");
        
        // Switch back to idle animation if available
        if (this._animations.idle && this._currentAction) {
          this._currentAction.stop();
          this._currentAction = this._animations.idle.action;
          this._currentAction.reset();
          this._currentAction.setLoop(THREE.LoopRepeat);
          this._currentAction.play();
        }
      }
    }

    Update(timeElapsed) {
      if (!this._parent || !this._parent._position || !this._isLoaded) {
        return;
      }

      // Check if girl is still alive
      const healthComp = this._parent.GetComponent('HealthComponent');
      if (healthComp && !healthComp.IsAlive()) {
        return; // Don't update if dead
      }

      // Update health display periodically
      this._UpdateHealthDisplay();

      // Update walk timer
      this._walkTimer += timeElapsed;

      // Find nearby monsters
      const nearbyMonsters = this._FindNearbyMonsters();

      // Check health status for more aggressive fleeing
      const healthComp2 = this._parent.GetComponent('HealthComponent');
      const healthPercent = healthComp2 ? (healthComp2._health / healthComp2._maxHealth) : 1.0;
      const isLowHealth = healthPercent < 0.5; // Flee more aggressively when below 50% health

      if (nearbyMonsters.length > 0) {
        console.log(`Girl detected ${nearbyMonsters.length} monsters nearby!`);
        
        // Calculate flee direction
        this._fleeDirection = this._CalculateFleeDirection(nearbyMonsters);
        
        // Start running animation
        this._StartRunning();

        // Update velocity for fleeing (faster if low health)
        this._velocity.copy(this._fleeDirection);
        const effectiveFleeSpeed = isLowHealth ? this._fleeSpeed * 1.5 : this._fleeSpeed;
        this._velocity.multiplyScalar(effectiveFleeSpeed);

        // Update position
        const newPosition = this._parent._position.clone();
        newPosition.add(this._velocity.clone().multiplyScalar(timeElapsed));
        
        // Keep the girl on the ground (y = 0)
        newPosition.y = 0;

        // Update the parent entity position
        this._parent.SetPosition(newPosition);

        // Rotate to face the flee direction
        if (this._fleeDirection.length() > 0) {
          const angle = Math.atan2(this._fleeDirection.x, this._fleeDirection.z);
          this._parent._rotation.setFromAxisAngle(new THREE.Vector3(0, 1, 0), angle);
          
          // Update model rotation if available
          if (this._model) {
            this._model.rotation.y = angle;
          }
        }

      } else {
        // No monsters nearby, just walk around casually
        this._StopRunning();
        
        // Change direction every 3-5 seconds
        if (this._walkTimer > 3 + Math.random() * 2) {
          this._walkDirection = new THREE.Vector3(
            Math.random() - 0.5, 
            0, 
            Math.random() - 0.5
          ).normalize();
          this._walkTimer = 0;
        }

        // Move in walk direction
        this._velocity.copy(this._walkDirection);
        this._velocity.multiplyScalar(this._walkSpeed);

        // Update position
        const newPosition = this._parent._position.clone();
        newPosition.add(this._velocity.clone().multiplyScalar(timeElapsed));
        
        // Keep the girl on the ground
        newPosition.y = 0;

        // Boundary check - keep girl within reasonable bounds
        const maxDistance = 200;
        if (Math.abs(newPosition.x) > maxDistance || Math.abs(newPosition.z) > maxDistance) {
          // Turn towards center
          this._walkDirection = new THREE.Vector3(-newPosition.x, 0, -newPosition.z).normalize();
        }

        this._parent.SetPosition(newPosition);

        // Rotate to face walk direction
        if (this._walkDirection.length() > 0) {
          const angle = Math.atan2(this._walkDirection.x, this._walkDirection.z);
          this._parent._rotation.setFromAxisAngle(new THREE.Vector3(0, 1, 0), angle);
          
          if (this._model) {
            this._model.rotation.y = angle;
          }
        }
      }
    }
  }

  return {
    GirlController: GirlController,
  };
})();
