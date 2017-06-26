#include "move.h"

MoveList::MoveList (void)
    {
    clear();
    }

Move MoveList::getNthMax (int32 n)
    {
    for ( int32 i = n + 1; i < m_size; ++ i )
        {
        if(m_data[i].getSort() > m_data[n].getSort())
            {
            Move tmp = m_data[n];
            m_data[n] = m_data[i];
            m_data[i] = tmp;
            }
        }
    return (m_data[n]);
    }

void MoveList::add (const MoveList & list)
    {
    for ( int32 i = 0; i < list.size(); ++ i )
        add(list[i]);
    }