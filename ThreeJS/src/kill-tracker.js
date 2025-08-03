import {entity} from './entity.js';

export const kill_tracker = (() => {

  class KillTracker extends entity.Component {
    constructor(params) {
      super();
      this._params = params || {};
      this._killCount = 0;
    }

    InitComponent() {
      // Listen for experience gain events which happen when enemies die
      this._RegisterHandler('health.add-experience', (m) => { this._OnKill(m); });
      
      // Initialize UI
      this._UpdateKillUI();
    }

    _OnKill(msg) {
      this._killCount++;
      console.log(`Kill count: ${this._killCount}`);
      this._UpdateKillUI();
    }

    _UpdateKillUI() {
      const killCountElement = document.getElementById('kill-count');
      if (killCountElement) {
        killCountElement.innerText = this._killCount;
      }
    }

    get KillCount() {
      return this._killCount;
    }
  }

  return {
    KillTracker: KillTracker,
  };
})();
