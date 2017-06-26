#include "hash.h"

Hashtable gl_hashtable;
Hashkeys gl_hashkeys;

HashItem::HashItem (void)
    {
    key = 0;
    k_data = 0;
    }

PawnHashItem::PawnHashItem (void)
    {
    clear();
    }

void PawnHashItem::clear (void)
    {
    key = 0;
    eval_mg[White] = 0;
    eval_eg[White] = 0;
    eval_mg[Black] = 0;
    eval_eg[Black] = 0;
    passers[White] = 0;
    passers[Black] = 0;

    for ( int32 file = 0; file < 8; ++ file )
        {
        king_safety[White][file] = 0;
        king_safety[Black][file] = 0;
        }
    }

Hashtable::Hashtable (void)
    {
    m_table = 0;
    m_tableSize = 0;
    m_tableFree = 0;
    m_age = 0;

    m_tablePawn = 0;
    m_tableSizePawn = 0;
    m_tableFreePawn = 0;

    m_size = 0;

    resize();
    }

Hashtable::~Hashtable (void)
    {
    clear();
    }

void Hashtable::clear (void)
    {
    delete [] m_table;
    m_table = 0;
    m_tableSize = 0;
    m_tableFree = 0;
    m_age = 0;

    delete [] m_tablePawn;
    m_tablePawn = 0;
    m_tableSizePawn = 0;
    m_tableFreePawn = 0;
    }

void Hashtable::resize (uint32 size)
    {
    if(m_size != size)
        {
        clear();
        m_size = size;

        uint32 itemSize = sizeof(HashItem);
        m_tableSize = uint32((0.90 * m_size * 1024 * 1024) / itemSize);
        m_tableFree = m_tableSize;

        if(m_tableSize)
            m_table = new HashItem[m_tableSize];

        uint32 itemSizePawn = sizeof(PawnHashItem);
        m_tableSizePawn = uint32((0.10 * m_size * 1024 * 1024) / itemSizePawn);
        m_tableFreePawn = m_tableSizePawn;

        if(m_tableSizePawn)
            m_tablePawn = new PawnHashItem[m_tableSizePawn];
        }
    }

HashItem * Hashtable::find (const uint64 & key)
    {
    if(m_table)
        {
        HashItem * item = & m_table[key % m_tableSize];

        if(item && item->key == (key ^ item->k_data))
            return (item);
        }

    return (0);
    }

PawnHashItem * Hashtable::findPawn (const uint64 & key)
    {
    if(m_tablePawn)
        {
        PawnHashItem * item = & m_tablePawn[key % m_tableSizePawn];
        return (item);
        }
    return (0);
    }

void Hashtable::record (const uint64 & key, int32 depth, int32 eval, HashType type, uint32 move)
    {
    if(m_table)
        {
        HashItem * item = & m_table[key % m_tableSize];

        if((item->data.age != m_age) || (item->data.depth <= depth))
            {
            if((0 == item->data.depth) || (item->data.age < m_age))
                -- m_tableFree;

            item->data.age = static_cast<uint8>(m_age);
            item->data.depth = static_cast<uint8>(depth);
            item->data.eval = static_cast<int16>(eval);
            item->data.move = static_cast<uint16>(move);
            item->data.type = static_cast<uint8>(type);
            item->key = (key ^ item->k_data);
            }
        }
    }

void Hashtable::reset (void)
    {
    for ( uint32 i = 0; i < m_tableSize; ++ i )
        {
        m_table[i].k_data = 0;
        m_table[i].key = 0;
        }
    m_tableFree = m_tableSize;
    m_age = 0;
    }

void Hashtable::aging (void)
    {
    m_tableFree = m_tableSize;
    ++ m_age;
    }

uint32 Hashtable::usage (void)
    {
    if(m_tableSize)
        return (uint32)(((m_tableSize - m_tableFree) / (float64)m_tableSize) * 1000);
    else
        return (0);
    }

Hashkeys::Hashkeys (void)
    {
    m_rand = 42;
    initKeys();
    }

void Hashkeys::initKeys (void)
    {
    for ( int32 field = 0; field < 64; ++ field )
        for ( int32 piece = 0; piece < 12; ++ piece )
            m_keys[field][piece] = createKey();

    m_keyColor = createKey();
    }

uint64 Hashkeys::createKey (void)
    {
    return (getRand() ^ (getRand() << 15) ^ (getRand() << 30) ^ (getRand() << 45) ^ (getRand() << 60));
    }

uint64 Hashkeys::getRand (void)
    {
    m_rand = 1664525 * m_rand + 1013904223;
    return (m_rand);
    }