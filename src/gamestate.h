#ifndef GAMESTATE_H
#define GAMESTATE_H

class Player;

// stuff that needs to persist over mulitple levels
struct GameState {
    GameState();
    ~GameState();

    void StartNewGame();
    void StartNewDemo();

    // current level, as displayed to user
    int PerceivedLevel() const;

    // increment level - returns true if wrapped past last level
    bool LevelCompleted();

    // set if all blastable dudes blasted at level end
    bool KeepYourSectorTidy;

    // double-size player
    bool BigHeadMode;

    // no more extra-life bonuses
    bool NoExtraLives;

    int m_Level;
    int m_WrapCnt;
    bool m_Demo;

    Player* m_Player;
};

#endif // GAMESTATE_H
