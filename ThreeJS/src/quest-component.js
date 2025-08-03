import {entity} from "./entity.js";


export const quest_component = (() => {

  const _TITLE = 'Welcome Adventurer!';
  const _TEXT = `Welcome to Honeywood, brave adventurer! Ah, so thou art the Chosen One, the Dragonborn, and bearer of many other lofty titles destined to save this weary world. A grand fate lies before thee: to return the sacred rings to the fiery chasms of Mordor, to fell the dread dragon that plagues the land, and to undertake many other deeds of glory yet unsung.

But lo, before such greatness, a test of thy mettle is required. Prove thy worth with tasks most… essential. Venture forth and vanquish 30 wailing spirits, bringing back their cursed eyeballs as proof of thy valor. And whilst thou art at it, retrieve mine robes from the dry cleaner and fetch mine children from the village crèche. Go forth, hero, and make haste!`;

  class QuestComponent extends entity.Component {
    constructor() {
      super();

      const e = document.getElementById('quest-ui');
      e.style.visibility = 'hidden';
    }

    InitComponent() {
      this._RegisterHandler('input.picked', (m) => this._OnPicked(m));
    }

    _OnPicked(msg) {
      // HARDCODE A QUEST
      const quest = {
        id: 'foo',
        title: _TITLE,
        text: _TEXT,
      };
      this._AddQuestToJournal(quest);
    }

    _AddQuestToJournal(quest) {
      const ui = this.FindEntity('ui').GetComponent('UIController');
      ui.AddQuest(quest);
    }
  };

  return {
      QuestComponent: QuestComponent,
  };
})();
