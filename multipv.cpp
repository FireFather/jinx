#include "search.h"
#include "multipv.h"

ItemPV::ItemPV (void)
    {
    clear();
    }

ItemPV::ItemPV (const MoveList & pv, int32 eval)
    {
    m_pv.clear();
    m_pv.add(pv);
    m_eval = eval;
    }

const ItemPV & ItemPV::operator = (const ItemPV & src)
    {
    if(this != & src)
        {
        m_pv.clear();
        m_pv.add(src.m_pv);
        m_eval = src.m_eval;
        }
    return ( * this);
    }

void ItemPV::clear (void)
    {
    m_pv.clear();
    m_eval = - mateValue;
    }

MultiPV::MultiPV (void)
    {
    clear();
    }

void MultiPV::clear (void)
    {
    m_size = 0;
    }

int32 MultiPV::size (void) const
    {
    return m_size;
    }

void MultiPV::add (const MoveList & pv, int32 eval)
    {
    if((m_size + 1) < maxMPV)
        m_items[m_size++] = ItemPV(pv, eval);
    }

bool MultiPV::getNthMax (ItemPV & pv, int32 n)
    {
    if(n < size())
        {
        for ( int32 i = n + 1; i < size(); ++ i )
            {
            if(m_items[i].m_eval > m_items[n].m_eval)
                {
                ItemPV tmp = m_items[n];
                m_items[n] = m_items[i];
                m_items[i] = tmp;
                }
            }
        pv = m_items[n];
        return (true);
        }
    return (false);
    }