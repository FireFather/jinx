#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

union Move
    {
    public:

    Move (void) :
        m_move(0)
        {
        }

    Move (uint32 move) :
        m_move(move)
        {
        }

    Move(Square from, Square to);
    Move(Square from, Square to, MoveType type);
    Move(Square from, Square to, MoveType type, uint8 info);

    operator uint32() const;
    uint32 getMoveData(void)const;

    Square getFrom(void)const;
    Square getTo(void)const;
    MoveType getType(void)const;
    uint8 getInfo(void)const;
    int32 getSort(void)const;
    void setSort(int32 sort);
    private:
    uint32 m_move;

    struct
        {
        uint32 from : 6;  // 00000000000000000000000000111111
        uint32 to : 6;    // 00000000000000000000111111000000
        uint32 type : 2;  // 00000000000000000011000000000000
        uint32 info : 2;  // 00000000000000001100000000000000
        uint32 sort : 16; // 11111111111111110000000000000000
        } m_data;
    };

class MoveList
    {
    public:

    MoveList(void);

    Move & operator [] (int32 n);
    const Move & operator [] (int32 n) const;

    void clear(void);
    int32 size(void)const;
    bool empty(void)const;
    void add(const Move & move);
    void addDir(Square from, Dir dir);
    void add(Square from, Square to);
    void add(Square from, Square to, MoveType type);
    void add(Square from, Square to, MoveType type, uint8 info);
    void add(const MoveList & list);
    void del(void);
    void setSort(int32 n, int32 sort);
    Move getNthMax(int32 n);
    private:
    Move m_data[220];
    int32 m_size;
    };

__forceinline Move::Move (Square from, Square to)
    {
    m_move = 0;
    m_data.from = from;
    m_data.to = to;
    }

__forceinline Move::Move (Square from, Square to, MoveType type)
    {
    m_move = 0;
    m_data.from = from;
    m_data.to = to;
    m_data.type = type;
    }

__forceinline Move::Move (Square from, Square to, MoveType type, uint8 info)
    {
    m_move = 0;
    m_data.from = from;
    m_data.to = to;
    m_data.type = type;
    m_data.info = info;
    }

__forceinline Move::operator uint32() const
    {
    return (m_move);
    }

__forceinline uint32 Move::getMoveData (void) const
    {
    return (m_move & 0x0000ffff);
    }

__forceinline Square Move::getFrom (void) const
    {
    return (Square(m_data.from));
    }

__forceinline Square Move::getTo (void) const
    {
    return (Square(m_data.to));
    }

__forceinline MoveType Move::getType (void) const
    {
    return (MoveType(m_data.type));
    }

__forceinline uint8 Move::getInfo (void) const
    {
    return (int8(m_data.info));
    }

__forceinline int32 Move::getSort (void) const
    {
    return (int16(m_data.sort));
    }

__forceinline void Move::setSort (int32 sort)
    {
    m_data.sort = int16(sort);
    }

__forceinline Move & MoveList::operator [] (int32 n)
    {
    return (m_data[n]);
    }

__forceinline const Move & MoveList::operator [] (int32 n) const
    {
    return (m_data[n]);
    }

__forceinline void MoveList::clear (void)
    {
    m_size = 0;
    }

__forceinline int32 MoveList::size (void) const
    {
    return (m_size);
    }

__forceinline bool MoveList::empty (void) const
    {
    return (0 == m_size);
    }

__forceinline void MoveList::add (const Move & move)
    {
    m_data[m_size++] = move;
    }

__forceinline void MoveList::addDir (Square from, Dir dir)
    {
    m_data[m_size++] = Move(from, Square(from + dir));
    }

__forceinline void MoveList::add (Square from, Square to)
    {
    m_data[m_size++] = Move(from, to);
    }

__forceinline void MoveList::add (Square from, Square to, MoveType type)
    {
    m_data[m_size++] = Move(from, to, type);
    }

__forceinline void MoveList::add (Square from, Square to, MoveType type, uint8 info)
    {
    m_data[m_size++] = Move(from, to, type, info);
    }

__forceinline void MoveList::del (void)
    {
    -- m_size;
    }

__forceinline void MoveList::setSort (int32 n, int32 sort)
    {
    m_data[n].setSort(sort);
    }

#endif