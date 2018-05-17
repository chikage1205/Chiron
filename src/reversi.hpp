//
// Created by denchu on 2018/04/14.
//
#include <stdint.h>
#include <string>

#ifndef CHIRON_REVERSI_HPP
#define CHIRON_REVERSI_HPP
struct Board {
    uint64_t bBoard;
    uint64_t wBoard;
    uint64_t coordinate;
    int_fast16_t value;
    bool isBlack;
};

class Reversi {
public:

    void init(uint64_t b = 0x810000000, uint64_t w = 0x1008000000);

    void undo();

    void redo();

    int_fast8_t getStones(uint64_t board) const;

    int_fast16_t alphabeta(uint64_t b, uint64_t w, bool isB, int depth, int_fast16_t alpha, int_fast16_t beta, bool passed, bool isRoot);

    int_fast16_t solve(uint64_t b, uint64_t w, bool isB, int_fast16_t alpha, int_fast16_t beta, bool passed, bool isRoot);

    int_fast16_t evaluate(uint64_t b, uint64_t w) const;

    uint64_t getMobility(uint64_t b, uint64_t w, bool isB) const;

    uint64_t getFlip(uint64_t coord, uint64_t b, uint64_t w, bool isB) const;

    void setStoneToBoard(uint64_t coord, uint64_t rev, bool isB);

    //void setRecord(std::string record);

    std::string getRecord();

    std::string getCoordString(uint64_t coord, bool isB);

    uint64_t getConfirmedStones(uint64_t me, uint64_t opp) const;

    uint64_t getBBoard() const;

    uint64_t getWBoard() const;

    uint64_t getLastHand() const;

    bool getIsBlack() const;

    int32_t getNode() const;

    int32_t getEmpty() const;

    int8_t getTurn() const;

private:
    uint64_t bBoard;
    uint64_t wBoard;
    int8_t turn;
    bool isBlack;
    int_fast32_t node;
    int_fast32_t evalTable[64] = {45, -11, 4, -1, -1, 4, -11, 45, -11, -16, -1, -3, -3, 2, -16, -11, 4, -1, 2, -1, -1, 2, -1, 4, -1, -3, -1, 0, 0, -1, -3, -1, -1, -3, -1, 0, 0, -1, -3, -1, 4, -1, 2, -1, -1, 2, -1, 4, -11, -16, -1, -3, -3, 2, -16, -11, 45, -11, 4, -1, -1, 4, -11, 45};
    Board record[61];
};

#endif //CHIRON_REVERSI_HPP
