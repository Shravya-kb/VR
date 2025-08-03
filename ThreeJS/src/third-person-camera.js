import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.118/build/three.module.js';
import {entity} from './entity.js';

export const third_person_camera = (() => {
  
  class ThirdPersonCamera extends entity.Component {
    constructor(params) {
      super();

      this._params = params;
      this._camera = params.camera;

      // Current camera state
      this._currentPosition = new THREE.Vector3();
      this._currentLookat = new THREE.Vector3();

      // Orbital camera parameters
      this._orbitEnabled = true;
      this._orbitSpeed = 0.5;
      this._minPolarAngle = 0; // Minimum vertical rotation
      this._maxPolarAngle = Math.PI; // Maximum vertical rotation
      this._sphericalCoords = {
        radius: 25,
        theta: 0, // Horizontal rotation
        phi: Math.PI / 6 // Vertical rotation
      };

      // Manual position offset
      this._manualOffset = new THREE.Vector3(0, 0, 0);

      // Mouse/touch tracking for orbital control
      this._isDragging = false;
      this._previousMousePosition = {
        x: 0,
        y: 0
      };

      // Bind event handlers
      this._onMouseMove = this._onMouseMove.bind(this);
      this._onMouseDown = this._onMouseDown.bind(this);
      this._onMouseUp = this._onMouseUp.bind(this);
      this._onWheel = this._onWheel.bind(this);

      // Window event listeners
      window.addEventListener('mousedown', this._onMouseDown);
      window.addEventListener('mousemove', this._onMouseMove);
      window.addEventListener('mouseup', this._onMouseUp);
      window.addEventListener('wheel', this._onWheel);
    }

    // New method to manually set camera position offset
    SetCameraOffset(x = 0, y = 0, z = 0) {
      this._manualOffset.set(x, y, z);
    }

    // New method to incrementally adjust camera position
    AdjustCameraOffset(x = 0, y = 0, z = 0) {
      this._manualOffset.x += x;
      this._manualOffset.y += y;
      this._manualOffset.z += z;
    }

    _onMouseDown(event) {
      this._isDragging = true;
      this._previousMousePosition = {
        x: event.clientX,
        y: event.clientY
      };
    }

    _onMouseMove(event) {
      if (!this._isDragging || !this._orbitEnabled) return;

      const deltaMove = {
        x: event.clientX - this._previousMousePosition.x,
        y: event.clientY - this._previousMousePosition.y
      };

      // Update spherical coordinates
      this._sphericalCoords.theta -= deltaMove.x * this._orbitSpeed * 0.01;
      this._sphericalCoords.phi -= deltaMove.y * this._orbitSpeed * 0.01;

      // Clamp vertical rotation
      this._sphericalCoords.phi = Math.max(
        this._minPolarAngle, 
        Math.min(this._maxPolarAngle, this._sphericalCoords.phi)
      );

      this._previousMousePosition = {
        x: event.clientX,
        y: event.clientY
      };
    }

    _onMouseUp() {
      this._isDragging = false;
    }

    _onWheel(event) {
      // Zoom in/out
      this._sphericalCoords.radius += event.deltaY * 0.1;
      
      // Optional: Add min/max zoom constraints
      this._sphericalCoords.radius = Math.max(5, Math.min(50, this._sphericalCoords.radius));
    }

    _CalculateIdealOffset() {
      // Convert spherical coordinates to Cartesian
      const target = this._params.target;
      const sinPhiRadius = Math.sin(this._sphericalCoords.phi) * this._sphericalCoords.radius;
      
      const offset = new THREE.Vector3(
        sinPhiRadius * Math.sin(this._sphericalCoords.theta),
        Math.cos(this._sphericalCoords.phi) * this._sphericalCoords.radius,
        sinPhiRadius * Math.cos(this._sphericalCoords.theta)
      );

      // Offset relative to target and add manual offset
      offset.add(target._position);
      offset.add(this._manualOffset);
      return offset;
    }

    _CalculateIdealLookat() {
      // Always look at the target
      return this._params.target._position.clone();
    }

    Update(timeElapsed) {
      const idealOffset = this._CalculateIdealOffset();
      const idealLookat = this._CalculateIdealLookat();

      // Smooth camera movement
      const t = 1.0 - Math.pow(0.01, timeElapsed);

      this._currentPosition.lerp(idealOffset, t);
      this._currentLookat.lerp(idealLookat, t);

      this._camera.position.copy(this._currentPosition);
      this._camera.lookAt(this._currentLookat);
    }

    // Cleanup method to remove event listeners
    Dispose() {
      window.removeEventListener('mousedown', this._onMouseDown);
      window.removeEventListener('mousemove', this._onMouseMove);
      window.removeEventListener('mouseup', this._onMouseUp);
      window.removeEventListener('wheel', this._onWheel);
    }
  }

  return {
    ThirdPersonCamera: ThirdPersonCamera
  };
})();