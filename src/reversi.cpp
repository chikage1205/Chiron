//
// Created by denchu on 2018/04/14.
//

#include "reversi.hpp"
#include <stdint.h>
#include <algorithm>
#include <android/log.h>

void Reversi::init(uint64_t b, uint64_t w) {
    //ハンデとかで盤面いじる用, 別画面から帰ってきたとき用
    bBoard = b;
    wBoard = w;
    turn = 0;
    isBlack = true;
    node = 0;
    for (Board &i: record) {
        i.bBoard = 0;
        i.wBoard = 0;
        i.coordinate = 0;
        i.isBlack = true;
    }
    record[0].bBoard = bBoard;
    record[0].wBoard = wBoard;
    record[0].coordinate = 0;
    record[0].isBlack = true;
}

void Reversi::undo() {
    if (turn == 0) {
        return;
    }
    Board nowRecord = record[turn - 1];
    if (nowRecord.bBoard == 0 && nowRecord.wBoard == 0) {
        return;
    } else {
        bBoard = nowRecord.bBoard;
        wBoard = nowRecord.wBoard;
        isBlack = nowRecord.isBlack;
        turn--;
    }
}

void Reversi::redo() {
    if (turn == 60) {
        return;
    }
    Board nowRecord = record[turn + 1];
    if (nowRecord.bBoard == 0 && nowRecord.wBoard == 0) {
        return;
    } else {
        bBoard = nowRecord.bBoard;
        wBoard = nowRecord.wBoard;
        isBlack = nowRecord.isBlack;
        turn++;
    }
}

int_fast8_t Reversi::getStones(uint64_t board) const {
    //分割統治法を使用
    uint64_t result;
    result = (board & 0x5555555555555555) + ((board >> 1 & 0x5555555555555555));
    result = (result & 0x3333333333333333) + ((result >> 2 & 0x3333333333333333));
    result = (result & 0x0f0f0f0f0f0f0f0f) + ((result >> 4 & 0x0f0f0f0f0f0f0f0f));
    result = (result & 0x00ff00ff00ff00ff) + ((result >> 8 & 0x00ff00ff00ff00ff));
    result = (result & 0x0000ffff0000ffff) + ((result >> 16 & 0x0000ffff0000ffff));
    result = (result & 0x00000000ffffffff) + ((result >> 32 & 0x00000000ffffffff));
    return static_cast<int_fast8_t>(result);
}

/*int_fast16_t Reversi::alphabeta(uint64_t b, uint64_t w, bool isB, int depth, int_fast16_t alpha, int_fast16_t beta, bool isRoot) {
    node++;

    uint64_t myMobility = getMobility(b, w, isB);
    uint64_t oppMobility = getMobility(b, w, isB);

    if (myMobility == 0 && oppMobility == 0) { //ゲーム終了なら
        return (getStones(b) - getStones(w)) * 100; //石差にバフかけて返す
    }

    if (depth == 0) { //リーフなら
        return evaluate(b, w); //評価関数返す
    }

    int_fast16_t value = isB ? -9999 : 9999;
    int_fast16_t min = alpha;
    int_fast16_t max = beta;
    uint64_t bTemp;
    uint64_t wTemp;

    int_fast8_t mobility = getStones(myMobility);
    Board nextBoard[mobility];

    for (Board &board: nextBoard) {
        uint64_t coord = myMobility & -myMobility;
        myMobility &= myMobility - 1;
        uint64_t rev = getFlip(coord, b, w, isB);
        if (isB) {
            bTemp = (b ^ rev) | coord;
            wTemp = w ^ rev;
        } else {
            bTemp = b ^ rev;
            wTemp = (w ^ rev) | coord;
        }
        board.bBoard = bTemp;
        board.wBoard = wTemp;
        board.coordinate = coord;
    }

    int_fast8_t bestCoord = 0;
    for (Board board: nextBoard) {
        bTemp = board.bBoard;
        wTemp = board.wBoard;
        int_fast16_t child = (getMobility(bTemp, wTemp, !isB) != 0) ?
                             alphabeta(bTemp, wTemp, !isB, depth - 1, min, max, false) :
                             alphabeta(bTemp, wTemp, isB, depth - 1, min, max, false);
        if (isB) {
            if (child > value) {
                value = child;
                min = child;
                bestCoord = getStones(board.coordinate - 1);
            }
            if ((value > max) || (!isRoot && value == max)) {
                return value;
            }
        } else {
            if (child < value) {
                value = child;
                max = child;
                bestCoord = getStones(board.coordinate - 1);
            }
            if ((value < min) || (!isRoot && value == min)) {
                return value;
            }
        }
    }
    if (isRoot) {
        return bestCoord;
    } else {
        return value;
    }
}*/
int_fast16_t Reversi::alphabeta(uint64_t b, uint64_t w, bool isB, int depth, int_fast16_t alpha, int_fast16_t beta, bool passed, bool isRoot) {
    node++;

    uint64_t mobility = getMobility(b, w, isB);

    if (mobility == 0) {
        if (passed){
            return getStones(b) - getStones(w);
        }else{
            return alphabeta(b, w, !isB, depth, alpha, beta, true, false);
        }
    }

    if (depth == 0) { //リーフなら
        return evaluate(b, w); //評価関数返す
    }

    int_fast16_t value = isB ? -9999 : 9999;
    int_fast16_t min = alpha;
    int_fast16_t max = beta;
    int_fast8_t bestCoord = 0;

    for (uint64_t coord  = mobility & -mobility; mobility != 0; mobility ^= coord, coord = mobility & -mobility) {
        uint64_t flip = getFlip(coord, b, w, isB);
        uint64_t bTemp;
        uint64_t wTemp;
        if (isB) {
            bTemp = (b ^ flip) | coord;
            wTemp = w ^ flip;
        } else {
            bTemp = b ^ flip;
            wTemp = (w ^ flip) | coord;
        }
        int_fast16_t child = alphabeta(bTemp, wTemp, !isB, depth - 1, min, max, false, false);
        if (isB) {
            if (child > value) {
                value = child;
                min = child;
                bestCoord = getStones(coord - 1);
            }
            if ((value > max) || (!isRoot && value == max)) {
                return value;
            }
        } else {
            if (child < value) {
                value = child;
                max = child;
                bestCoord = getStones(coord - 1);
            }
            if ((value < min) || (!isRoot && value == min)) {
                return value;
            }
        }
    }
    if (isRoot) {
        return bestCoord;
    } else {
        return value;
    }
}


/*int_fast16_t Reversi::solve(uint64_t b, uint64_t w, bool isB, int_fast16_t alpha, int_fast16_t beta, bool isRoot) {
    node++;

    uint64_t myMobility = getMobility(b, w, isB);
    uint64_t oppMobility = getMobility(b, w, isB);

    if (myMobility == 0 && oppMobility == 0) { //ゲーム終了なら
        return (getStones(b) - getStones(w)) * 100; //石差にバフかけて返す
    }

    int_fast16_t value = isB ? -9999 : 9999;
    int_fast16_t min = alpha;
    int_fast16_t max = beta;
    uint64_t bTemp;
    uint64_t wTemp;

    int_fast8_t mobility = getStones(myMobility);
    Board nextBoard[mobility];

    for (Board &board: nextBoard) {
        uint64_t coord = myMobility & -myMobility;
        myMobility &= myMobility - 1;
        uint64_t rev = getFlip(coord, b, w, isB);
        if (isB) {
            bTemp = (b ^ rev) | coord;
            wTemp = w ^ rev;
        } else {
            bTemp = b ^ rev;
            wTemp = (w ^ rev) | coord;
        }
        board.bBoard = bTemp;
        board.wBoard = wTemp;
        board.coordinate = coord;
    }

    int_fast8_t bestCoord = 0;
    for (Board board: nextBoard) {
        bTemp = board.bBoard;
        wTemp = board.wBoard;
        int_fast16_t child = (getMobility(bTemp, wTemp, !isB) != 0) ?
                             solve(bTemp, wTemp, !isB, min, max, false) :
                             solve(bTemp, wTemp, isB, min, max, false);
        if (isB) {
            if (child > value) {
                value = child;
                min = child;
                bestCoord = getStones(board.coordinate - 1);
            }
            if ((value > max) || (!isRoot && value == max)) {
                return value;
            }
        } else {
            if (child < value) {
                value = child;
                max = child;
                bestCoord = getStones(board.coordinate - 1);
            }
            if ((value < min) || (!isRoot && value == min)) {
                return value;
            }
        }
    }
    if (isRoot) {
        return bestCoord;
    } else {
        return value;
    }
}*/
int_fast16_t Reversi::solve(uint64_t b, uint64_t w, bool isB, int_fast16_t alpha, int_fast16_t beta, bool passed, bool isRoot) {
    node++;

    uint64_t mobility = getMobility(b, w, isB);

    if (mobility == 0) {
        if (passed){
            return getStones(b) - getStones(w);
        }else{
            return solve(b, w, !isB, alpha, beta, true, false);
        }
    }

    int_fast16_t value = isB ? -9999 : 9999;
    int_fast16_t min = alpha;
    int_fast16_t max = beta;
    int_fast8_t bestCoord = 0;

    for (uint64_t coord  = mobility & -mobility; mobility; mobility ^= coord, coord = mobility & -mobility) {
        uint64_t flip = getFlip(coord, b, w, isB);
        uint64_t bTemp;
        uint64_t wTemp;
        if (isB) {
            bTemp = (b ^ flip) | coord;
            wTemp = w ^ flip;
        } else {
            bTemp = b ^ flip;
            wTemp = (w ^ flip) | coord;
        }
        int_fast16_t child = solve(bTemp, wTemp, !isB, min, max, false, false);
        if (isB) {
            if (child > value) {
                value = child;
                min = child;
                bestCoord = getStones(coord - 1);
            }
            if ((value > max) || (!isRoot && value == max)) {
                return value;
            }
        } else {
            if (child < value) {
                value = child;
                max = child;
                bestCoord = getStones(coord - 1);
            }
            if ((value < min) || (!isRoot && value == min)) {
                return value;
            }
        }
    }
    if (isRoot) {
        return bestCoord;
    } else {
        return value;
    }
}

int_fast16_t Reversi::evaluate(uint64_t b, uint64_t w) const {
    int_fast16_t bp = 0;
    int_fast16_t fs = getStones(getConfirmedStones(b, w)) - getStones(getConfirmedStones(w, b));
    int_fast16_t cn = getStones(getMobility(b, w, true)) - getStones(getMobility(b, w, false));
    for (int_fast8_t i = 0; i < 64; i++) {
        if (1 & (b >> i)) {
            bp += evalTable[i];
        } else if (1 & (w >> i)) {
            bp -= evalTable[i];
        }
    }
    //return bp + fs * 18 + cn * 3;
    return bp + fs * 11 + cn * 2;
}

uint64_t Reversi::getMobility(uint64_t b, uint64_t w, bool isB) const {
    uint64_t me;
    uint64_t opp;
    uint64_t blank = ~(b | w);
    uint64_t mask;
    uint64_t result;
    uint64_t t;

    if (isB) {
        me = b;
        opp = w;
    } else {
        me = w;
        opp = b;
    }

    //left
    mask = opp & 0x7e7e7e7e7e7e7e7e;
    t = mask & (me << 1);
    t |= mask & (t << 1);
    t |= mask & (t << 1);
    t |= mask & (t << 1);
    t |= mask & (t << 1);
    t |= mask & (t << 1);
    t |= mask & (t << 1);
    result = blank & (t << 1);

    //right
    t = mask & (me >> 1);
    t |= mask & (t >> 1);
    t |= mask & (t >> 1);
    t |= mask & (t >> 1);
    t |= mask & (t >> 1);
    t |= mask & (t >> 1);
    t |= mask & (t >> 1);
    result |= blank & (t >> 1);

    //up
    mask = opp & 0x00ffffffffffff00;
    t = mask & (me << 8);
    t |= mask & (t << 8);
    t |= mask & (t << 8);
    t |= mask & (t << 8);
    t |= mask & (t << 8);
    t |= mask & (t << 8);
    t |= mask & (t << 8);
    result |= blank & (t << 8);

    //down
    t = mask & (me >> 8);
    t |= mask & (t >> 8);
    t |= mask & (t >> 8);
    t |= mask & (t >> 8);
    t |= mask & (t >> 8);
    t |= mask & (t >> 8);
    t |= mask & (t >> 8);
    result |= blank & (t >> 8);

    //up_left
    mask = opp & 0x007e7e7e7e7e7e00;
    t = mask & (me << 7);
    t |= mask & (t << 7);
    t |= mask & (t << 7);
    t |= mask & (t << 7);
    t |= mask & (t << 7);
    t |= mask & (t << 7);
    t |= mask & (t << 7);
    result |= blank & (t << 7);

    //up_right
    t = mask & (me << 9);
    t |= mask & (t << 9);
    t |= mask & (t << 9);
    t |= mask & (t << 9);
    t |= mask & (t << 9);
    t |= mask & (t << 9);
    t |= mask & (t << 9);
    result |= blank & (t << 9);

    //down_left
    t = mask & (me >> 9);
    t |= mask & (t >> 9);
    t |= mask & (t >> 9);
    t |= mask & (t >> 9);
    t |= mask & (t >> 9);
    t |= mask & (t >> 9);
    t |= mask & (t >> 9);
    result |= blank & (t >> 9);

    //down_right
    t = mask & (me >> 7);
    t |= mask & (t >> 7);
    t |= mask & (t >> 7);
    t |= mask & (t >> 7);
    t |= mask & (t >> 7);
    t |= mask & (t >> 7);
    t |= mask & (t >> 7);
    result |= blank & (t >> 7);

    return result;
}

uint64_t Reversi::getFlip(uint64_t coord, uint64_t b, uint64_t w, bool isB) const {
    uint64_t me;
    uint64_t opp;
    uint64_t rev = 0;
    uint64_t temp;
    uint64_t mask;

    if (isB) {
        me = b;
        opp = w;
    } else {
        me = w;
        opp = b;
    }
    uint64_t edgeMask = 0x7e7e7e7e7e7e7e7e & opp;

    //right
    mask = 0xfe * coord;
    temp = (edgeMask | ~mask) + 1 & mask & me;
    if (temp != 0) {
        rev |= temp - 1 & mask;
    }

    //right_down
    mask = 0x8040201008040200 * coord;
    temp = (edgeMask | ~mask) + 1 & mask & me;
    if (temp != 0) {
        rev |= temp - 1 & mask;
    }

    //down
    mask = 0x0101010101010100 * coord;
    temp = (opp | ~mask) + 1 & mask & me;
    if (temp != 0) {
        rev |= temp - 1 & mask;
    }

    //left_down
    mask = 0x102040810204080 * coord;
    temp = (edgeMask | ~mask) + 1 & mask & me;
    if (temp != 0) {
        rev |= temp - 1 & mask;
    }

    //left
    temp = coord >> 1 & edgeMask;
    temp |= temp >> 1 & edgeMask;
    temp |= temp >> 1 & edgeMask;
    temp |= temp >> 1 & edgeMask;
    temp |= temp >> 1 & edgeMask;
    temp |= temp >> 1 & edgeMask;
    rev |= temp & -(temp >> 1 & me);

    //up
    temp = coord >> 8 & opp;
    temp |= temp >> 8 & opp;
    temp |= temp >> 8 & opp;
    temp |= temp >> 8 & opp;
    temp |= temp >> 8 & opp;
    temp |= temp >> 8 & opp;
    rev |= temp & -(temp >> 8 & me);

    //left_up
    temp = coord >> 9 & edgeMask;
    temp |= temp >> 9 & edgeMask;
    temp |= temp >> 9 & edgeMask;
    temp |= temp >> 9 & edgeMask;
    temp |= temp >> 9 & edgeMask;
    temp |= temp >> 9 & edgeMask;
    rev |= temp & -(temp >> 9 & me);

    //right_up
    temp = coord >> 7 & edgeMask;
    temp |= temp >> 7 & edgeMask;
    temp |= temp >> 7 & edgeMask;
    temp |= temp >> 7 & edgeMask;
    temp |= temp >> 7 & edgeMask;
    temp |= temp >> 7 & edgeMask;
    rev |= temp & -(temp >> 7 & me);

    return rev;
}

void Reversi::setStoneToBoard(uint64_t coord, uint64_t rev, bool isB) {

    if (isB) {
        bBoard |= coord;
    } else {
        wBoard |= coord;
    }
    bBoard ^= rev;
    wBoard ^= rev;

    if (getMobility(bBoard, wBoard, !isB) != 0) {
        isBlack = !isBlack;
    }

    turn++;
    record[turn].bBoard = bBoard;
    record[turn].wBoard = wBoard;
    record[turn].coordinate = coord;
    record[turn].isBlack = isBlack;

    if (turn < 59) {
        for (int i = turn + 1; record[i].bBoard != 0 || record[i].wBoard != 0; i++) {
            record[i].bBoard = 0;
            record[i].wBoard = 0;
            record[i].isBlack = true;
        }
    }
}

uint64_t Reversi::getConfirmedStones(uint64_t me, uint64_t opp) const {
    uint64_t cs = me & 0x8100000000000081;
    uint64_t top = 0xFF;
    uint64_t bottom = 0xFF00000000000000;
    uint64_t right = 0x8080808080808080;
    uint64_t left = 0x101010101010101;
    uint64_t empty = ~(me | opp);
    if ((empty & top) == 0) { cs |= me & top; }
    if ((empty & bottom) == 0) { cs |= me & bottom; }
    if ((empty & right) == 0) { cs |= me & right; }
    if ((empty & left) == 0) { cs |= me & left; }
    if (cs != 0) {
        uint64_t upmask = 0x8100000000000000 & cs;
        uint64_t downmask = 0x81 & cs;
        uint64_t rightmask = 0x100000000000001 & cs;
        uint64_t leftmask = 0x8000000000000080 & cs;

        upmask = upmask >> 8 & me;
        cs |= upmask;
        upmask = upmask >> 8 & me;
        cs |= upmask;
        upmask = upmask >> 8 & me;
        cs |= upmask;
        upmask = upmask >> 8 & me;
        cs |= upmask;
        upmask = upmask >> 8 & me;
        cs |= upmask;
        upmask = upmask >> 8 & me;
        cs |= upmask;

        downmask = downmask << 8 & me;
        cs |= downmask;
        downmask = downmask << 8 & me;
        cs |= downmask;
        downmask = downmask << 8 & me;
        cs |= downmask;
        downmask = downmask << 8 & me;
        cs |= downmask;
        downmask = downmask << 8 & me;
        cs |= downmask;
        downmask = downmask << 8 & me;
        cs |= downmask;

        rightmask = rightmask << 1 & me;
        cs |= rightmask;
        rightmask = rightmask << 1 & me;
        cs |= rightmask;
        rightmask = rightmask << 1 & me;
        cs |= rightmask;
        rightmask = rightmask << 1 & me;
        cs |= rightmask;
        rightmask = rightmask << 1 & me;
        cs |= rightmask;
        rightmask = rightmask << 1 & me;
        cs |= rightmask;

        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
        leftmask = leftmask >> 1 & me;
        cs |= leftmask;
    }
    return cs;
}

uint64_t Reversi::getBBoard() const {
    return bBoard;
}

uint64_t Reversi::getWBoard() const {
    return wBoard;
}

bool Reversi::getIsBlack() const {
    return isBlack;
}

int32_t Reversi::getNode() const {
    return static_cast<int32_t>(node);
}

int32_t Reversi::getEmpty() const {
    return getStones(~(bBoard | wBoard));
}

uint64_t Reversi::getLastHand() const {
    return record[turn].coordinate;
}

int8_t Reversi::getTurn() const {
    return turn;
}

/*void Reversi::setRecord(std::string record) {
 * //TODO 棋譜のロード 時間ができたらやる
}*/

std::string Reversi::getRecord() {
    std::string result;

    for (int_fast8_t i = 1; i <= turn; i++) {
        result += getCoordString(record[i].coordinate, record[i].isBlack);
    }
    return result;
}

std::string Reversi::getCoordString(uint64_t coord, bool isB) {
    std::string coordTable[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    std::string coordTable2[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};

    int_fast8_t ind = getStones(coord - 1);
    if (isB) {
        return coordTable[ind % 8] + std::to_string(ind / 8 + 1);
    } else {
        return coordTable2[ind % 8] + std::to_string(ind / 8 + 1);
    }
}

