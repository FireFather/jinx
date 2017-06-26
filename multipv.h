#ifndef MULTIPV_H
#define MULTIPV_H

#include "search.h"

const int32 maxMPV = 16;

class ItemPV
    {
    public:

    ItemPV(void);
    ItemPV(const MoveList & pv, int32 eval);
    const ItemPV & operator = (const ItemPV & src);
    void clear(void);
    public:
    MoveList m_pv;
    int32 m_eval;
    };

class MultiPV
    {
    public:
    MultiPV(void);

    void clear(void);
    int32 size(void)const;
    void add(const MoveList & pv, int32 eval);
    bool getNthMax(ItemPV & pv, int32 n);
    private:
    ItemPV m_items[maxMPV];
    int32 m_size;
    };

#endif