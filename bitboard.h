#ifndef BITBOARD_H
#define BITBOARD_H

#include "piece.h"
#include "define.h"
#include "bitops.h"

// bitboards for ranks and files

const uint64 Rank_1 = (0x00000000000000ffULL);
const uint64 Rank_2 = (0x000000000000ff00ULL);
const uint64 Rank_3 = (0x0000000000ff0000ULL);
const uint64 Rank_4 = (0x00000000ff000000ULL);
const uint64 Rank_5 = (0x000000ff00000000ULL);
const uint64 Rank_6 = (0x0000ff0000000000ULL);
const uint64 Rank_7 = (0x00ff000000000000ULL);
const uint64 Rank_8 = (0xff00000000000000ULL);

const uint64 File_A = (0x0101010101010101ULL);
const uint64 File_B = (0x0202020202020202ULL);
const uint64 File_C = (0x0404040404040404ULL);
const uint64 File_D = (0x0808080808080808ULL);
const uint64 File_E = (0x1010101010101010ULL);
const uint64 File_F = (0x2020202020202020ULL);
const uint64 File_G = (0x4040404040404040ULL);
const uint64 File_H = (0x8080808080808080ULL);


// special bitboards

const uint64 FullBoard = (0xffffffffffffffffULL);
const uint64 EmptyBoard = (0x0000000000000000ULL);
const uint64 NotFile_A = (0xfefefefefefefefeULL);
const uint64 NotFile_H = (0x7f7f7f7f7f7f7f7fULL);
const uint64 NotFile_AB = (0xfcfcfcfcfcfcfcfcULL);
const uint64 NotFile_GH = (0x3f3f3f3f3f3f3f3fULL);
const uint64 WhiteSqs = (0x55AA55AA55AA55AAULL);
const uint64 BlackSqs = (0xAA55AA55AA55AA55ULL);


// move generation

__forceinline uint64 shift_N (const uint64 & b)
    {
    return (b << 8);
    }

__forceinline uint64 shift_NE (const uint64 & b)
    {
    return (b << 9) & NotFile_A;
    }

__forceinline uint64 shift_E (const uint64 & b)
    {
    return (b << 1) & NotFile_A;
    }

__forceinline uint64 shift_SE (const uint64 & b)
    {
    return (b >> 7) & NotFile_A;
    }

__forceinline uint64 shift_S (const uint64 & b)
    {
    return (b >> 8);
    }

__forceinline uint64 shift_SW (const uint64 & b)
    {
    return (b >> 9) & NotFile_H;
    }

__forceinline uint64 shift_W (const uint64 & b)
    {
    return (b >> 1) & NotFile_H;
    }

__forceinline uint64 shift_NW (const uint64 & b)
    {
    return (b << 7) & NotFile_H;
    }

__forceinline uint64 shift_NNE (const uint64 & b)
    {
    return (b << 17) & NotFile_A;
    }

__forceinline uint64 shift_NNW (const uint64 & b)
    {
    return (b << 15) & NotFile_H;
    }

__forceinline uint64 shift_SSE (const uint64 & b)
    {
    return (b >> 15) & NotFile_A;
    }

__forceinline uint64 shift_SSW (const uint64 & b)
    {
    return (b >> 17) & NotFile_H;
    }

__forceinline uint64 shift_ENE (const uint64 & b)
    {
    return (b << 10) & NotFile_AB;
    }

__forceinline uint64 shift_ESE (const uint64 & b)
    {
    return (b >> 6) & NotFile_AB;
    }

__forceinline uint64 shift_WNW (const uint64 & b)
    {
    return (b << 6) & NotFile_GH;
    }

__forceinline uint64 shift_WSW (const uint64 & b)
    {
    return (b >> 10) & NotFile_GH;
    }

// edit and request

static __forceinline bool test_bit (const uint64 & b, uint32 i)
    {
    return (0 != (b &((uint64)1 << i)));
    }

static __forceinline uint64 create_bit (uint32 i)
    {
    return ((uint64)1 << i);
    }

static __forceinline void set_bit (uint64 & b, uint32 i)
    {
    b |= ((uint64)1 << i);
    }

static __forceinline void del_bit (uint64 & b, uint32 i)
    {
    b &= ~((uint64)1 << i);
    }

static __forceinline void set_bits (uint64 & b, const uint64 & bb)
    {
    b |= bb;
    }

static __forceinline void del_bits (uint64 & b, const uint64 & bb)
    {
    b &= ~bb;
    }

static __forceinline Square getFirstBit (uint64 b)
    {
    return Square(LSB(b));
    }

static __forceinline Square popFirstBit (uint64 & b)
    {
    Square fld = Square(LSB(b));
    b &= b - 1;
    return (fld);
    }

static __forceinline uint32 countBits (const uint64 b)
    {
    return popcount(b);
    }

static __forceinline bool isOneBit (const uint64 & b)
    {
    return (b && !(b &(b - 1)));
    }

static __forceinline bool isMultiBit (const uint64 & b)
    {
    return (b && (b &(b - 1)));
    }

typedef uint64 (*atkfnc)(uint64, uint64);


// bitboard lookup's

class Bitboards
    {
    public:
    Bitboards(void);

    Dir getDir(Square from, Square to);
    uint64 getGap(Square from, Square to);
    uint64 getRay(Square from, Square to);
    uint64 getAttacksR(Square field);               // rook attacks
    uint64 getAttacksB(Square field);               // bishop attacks
    uint64 getAttacksQ(Square field);               // queen attacks
    uint64 getAttacksN(Square field);               // knight attacks
    uint64 getAttacksK(Square field);               // king attacks
    uint64 getAttacksP(Square field, Color color);  // pawn attacks
    uint64 getAttacksP(uint64 fields, Color color); // pawn attacks

    uint64 getRank(Rank rank);
    uint64 getFile(File file);
    uint64 getNextFiles(Square field);
    uint64 getFrontSpans(Square field, Color color); // e.g. for passed pawns
    uint64 getQuadratics(Square field, Color color); // e.g. for pawn endgame

    uint64 getAttacksR(Square field, const uint64 & occ);
    uint64 getAttacksB(Square field, const uint64 & occ);
    uint64 getAttacksQ(Square field, const uint64 & occ);
    uint64 getAttacks(Square from, Square to, const uint64 & occ);
    private:
    void init(void);
    uint64 getNext(uint64 pos, Dir dir);
    uint64 getNext(int32 field, Dir dir);
    uint64 getLine(int32 field, Dir dir);

    Dir m_dir[64][64];
    uint64 m_gap[64][64];
    uint64 m_ray[64][64];
    atkfnc m_atk[64][64];
    uint64 m_attacksR[64];
    uint64 m_attacksB[64];
    uint64 m_attacksQ[64];
    uint64 m_attacksN[64];
    uint64 m_attacksK[64];
    uint64 m_attacksP[2][64];

    uint64 m_ranks[8];
    uint64 m_files[8];
    uint64 m_nextFiles[8];
    uint64 m_frontSpans[2][64];
    uint64 m_quadratics[2][64];
    };

__forceinline Dir Bitboards::getDir (Square from, Square to)
    {
    return (m_dir[from][to]);
    }

__forceinline uint64 Bitboards::getGap (Square from, Square to)
    {
    return (m_gap[from][to]);
    }

__forceinline uint64 Bitboards::getRay (Square from, Square to)
    {
    return (m_ray[from][to]);
    }

__forceinline uint64 Bitboards::getAttacksR (Square field)
    {
    return (m_attacksR[field]);
    }

__forceinline uint64 Bitboards::getAttacksB (Square field)
    {
    return (m_attacksB[field]);
    }

__forceinline uint64 Bitboards::getAttacksQ (Square field)
    {
    return (m_attacksQ[field]);
    }

__forceinline uint64 Bitboards::getAttacksN (Square field)
    {
    return (m_attacksN[field]);
    }

__forceinline uint64 Bitboards::getAttacksK (Square field)
    {
    return (m_attacksK[field]);
    }

__forceinline uint64 Bitboards::getAttacksP (Square field, Color color)
    {
    return (m_attacksP[color][field]);
    }

__forceinline uint64 Bitboards::getAttacksP (uint64 fields, Color color)
    {
    if(White == color)
        return (shift_NW(fields) | shift_NE(fields));
    else
        return (shift_SW(fields) | shift_SE(fields));
    }

__forceinline uint64 Bitboards::getFrontSpans (Square field, Color color)
    {
    return (m_frontSpans[color][field]);
    }

__forceinline uint64 Bitboards::getQuadratics (Square field, Color color)
    {
    return (m_quadratics[color][field]);
    }

__forceinline uint64 Bitboards::getRank (Rank rank)
    {
    return (m_ranks[rank]);
    }

__forceinline uint64 Bitboards::getFile (File file)
    {
    return (m_files[file]);
    }

__forceinline uint64 Bitboards::getNextFiles (Square field)
    {
    return (m_nextFiles[::getFile(field)]);
    }

extern Bitboards gl_bitboards;

#endif