#ifndef HASH_H
#define HASH_H

#include "piece.h"

enum HashType
    {
    Exact = 0,
    Alpha = 1,
    Beta = 2
    };

struct HashData
    {
    uint8 age;
    uint16 depth;
    int16 eval;
    uint16 move;
    uint8 type;
    };

class HashItem
    {
    public:
    HashItem(void);

    uint64 key;

    union
        {
        HashData data;
        uint64 k_data;
        };
    };

struct PawnHashItem
    {
    PawnHashItem(void);
    void clear(void);

    uint64 key;
    int32 eval_mg[2]; // middle game
    int32 eval_eg[2]; // end game
    uint64 passers[2];
    int8 king_safety[2][8];
    };

class Hashtable
    {
    public:
    Hashtable(void);
    ~Hashtable(void);

    void clear(void);
    void resize(uint32 size = 0);

    // Hashs for search
    HashItem * find(const uint64 & key);
    void record(const uint64 & key, int32 depth, int32 eval, HashType type, uint32 move);
    void reset(void);
    void aging(void);
    uint32 usage(void);

    // Hashs for evaluation
    PawnHashItem * findPawn(const uint64 & key);
    private:
    HashItem * m_table;
    uint32 m_tableSize;
    uint32 m_tableFree;
    uint32 m_age;
    private:
    PawnHashItem * m_tablePawn;
    uint32 m_tableSizePawn;
    uint32 m_tableFreePawn;
    private:
    uint32 m_size;
    };

class Hashkeys
    {
    public:
    Hashkeys(void);
    uint64 getKey(Square field, Piece piece);
    uint64 getKeyColor(void);
    private:
    uint64 m_rand;
    uint64 m_keys[64][12];
    uint64 m_keyColor;
    void initKeys(void);
    uint64 createKey(void);
    uint64 getRand(void);
    };

__forceinline uint64 Hashkeys::getKey (Square field, Piece piece)
    {
    return (m_keys[field][piece]);
    }

__forceinline uint64 Hashkeys::getKeyColor (void)
    {
    return (m_keyColor);
    }

extern Hashtable gl_hashtable;
extern Hashkeys gl_hashkeys;

#endif