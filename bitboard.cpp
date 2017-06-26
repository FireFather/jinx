#include "bitboard.h"

Bitboards gl_bitboards;

uint64 magic_bishop_db[64][1 << 9];

uint64 magicmoves_b_magics[64] =
    {
    (0x0002020202020200ULL), (0x0002020202020000ULL), (0x0004010202000000ULL), (0x0004040080000000ULL),
    (0x0001104000000000ULL), (0x0000821040000000ULL), (0x0000410410400000ULL), (0x0000104104104000ULL),
    (0x0000040404040400ULL), (0x0000020202020200ULL), (0x0000040102020000ULL), (0x0000040400800000ULL),
    (0x0000011040000000ULL), (0x0000008210400000ULL), (0x0000004104104000ULL), (0x0000002082082000ULL),
    (0x0004000808080800ULL), (0x0002000404040400ULL), (0x0001000202020200ULL), (0x0000800802004000ULL),
    (0x0000800400A00000ULL), (0x0000200100884000ULL), (0x0000400082082000ULL), (0x0000200041041000ULL),
    (0x0002080010101000ULL), (0x0001040008080800ULL), (0x0000208004010400ULL), (0x0000404004010200ULL),
    (0x0000840000802000ULL), (0x0000404002011000ULL), (0x0000808001041000ULL), (0x0000404000820800ULL),
    (0x0001041000202000ULL), (0x0000820800101000ULL), (0x0000104400080800ULL), (0x0000020080080080ULL),
    (0x0000404040040100ULL), (0x0000808100020100ULL), (0x0001010100020800ULL), (0x0000808080010400ULL),
    (0x0000820820004000ULL), (0x0000410410002000ULL), (0x0000082088001000ULL), (0x0000002011000800ULL),
    (0x0000080100400400ULL), (0x0001010101000200ULL), (0x0002020202000400ULL), (0x0001010101000200ULL),
    (0x0000410410400000ULL), (0x0000208208200000ULL), (0x0000002084100000ULL), (0x0000000020880000ULL),
    (0x0000001002020000ULL), (0x0000040408020000ULL), (0x0004040404040000ULL), (0x0002020202020000ULL),
    (0x0000104104104000ULL), (0x0000002082082000ULL), (0x0000000020841000ULL), (0x0000000000208800ULL),
    (0x0000000010020200ULL), (0x0000000404080200ULL), (0x0000040404040400ULL), (0x0002020202020200ULL)
    };

uint64 magicmoves_b_mask[64] =
    {
    (0x0040201008040200ULL), (0x0000402010080400ULL), (0x0000004020100A00ULL), (0x0000000040221400ULL),
    (0x0000000002442800ULL), (0x0000000204085000ULL), (0x0000020408102000ULL), (0x0002040810204000ULL),
    (0x0020100804020000ULL), (0x0040201008040000ULL), (0x00004020100A0000ULL), (0x0000004022140000ULL),
    (0x0000000244280000ULL), (0x0000020408500000ULL), (0x0002040810200000ULL), (0x0004081020400000ULL),
    (0x0010080402000200ULL), (0x0020100804000400ULL), (0x004020100A000A00ULL), (0x0000402214001400ULL),
    (0x0000024428002800ULL), (0x0002040850005000ULL), (0x0004081020002000ULL), (0x0008102040004000ULL),
    (0x0008040200020400ULL), (0x0010080400040800ULL), (0x0020100A000A1000ULL), (0x0040221400142200ULL),
    (0x0002442800284400ULL), (0x0004085000500800ULL), (0x0008102000201000ULL), (0x0010204000402000ULL),
    (0x0004020002040800ULL), (0x0008040004081000ULL), (0x00100A000A102000ULL), (0x0022140014224000ULL),
    (0x0044280028440200ULL), (0x0008500050080400ULL), (0x0010200020100800ULL), (0x0020400040201000ULL),
    (0x0002000204081000ULL), (0x0004000408102000ULL), (0x000A000A10204000ULL), (0x0014001422400000ULL),
    (0x0028002844020000ULL), (0x0050005008040200ULL), (0x0020002010080400ULL), (0x0040004020100800ULL),
    (0x0000020408102000ULL), (0x0000040810204000ULL), (0x00000A1020400000ULL), (0x0000142240000000ULL),
    (0x0000284402000000ULL), (0x0000500804020000ULL), (0x0000201008040200ULL), (0x0000402010080400ULL),
    (0x0002040810204000ULL), (0x0004081020400000ULL), (0x000A102040000000ULL), (0x0014224000000000ULL),
    (0x0028440200000000ULL), (0x0050080402000000ULL), (0x0020100804020000ULL), (0x0040201008040200ULL)
    };

uint64 magic_rook_db[64][1 << 12];

uint64 magicmoves_r_magics[64] =
    {
    (0x0080001020400080ULL), (0x0040001000200040ULL), (0x0080081000200080ULL), (0x0080040800100080ULL),
    (0x0080020400080080ULL), (0x0080010200040080ULL), (0x0080008001000200ULL), (0x0080002040800100ULL),
    (0x0000800020400080ULL), (0x0000400020005000ULL), (0x0000801000200080ULL), (0x0000800800100080ULL),
    (0x0000800400080080ULL), (0x0000800200040080ULL), (0x0000800100020080ULL), (0x0000800040800100ULL),
    (0x0000208000400080ULL), (0x0000404000201000ULL), (0x0000808010002000ULL), (0x0000808008001000ULL),
    (0x0000808004000800ULL), (0x0000808002000400ULL), (0x0000010100020004ULL), (0x0000020000408104ULL),
    (0x0000208080004000ULL), (0x0000200040005000ULL), (0x0000100080200080ULL), (0x0000080080100080ULL),
    (0x0000040080080080ULL), (0x0000020080040080ULL), (0x0000010080800200ULL), (0x0000800080004100ULL),
    (0x0000204000800080ULL), (0x0000200040401000ULL), (0x0000100080802000ULL), (0x0000080080801000ULL),
    (0x0000040080800800ULL), (0x0000020080800400ULL), (0x0000020001010004ULL), (0x0000800040800100ULL),
    (0x0000204000808000ULL), (0x0000200040008080ULL), (0x0000100020008080ULL), (0x0000080010008080ULL),
    (0x0000040008008080ULL), (0x0000020004008080ULL), (0x0000010002008080ULL), (0x0000004081020004ULL),
    (0x0000204000800080ULL), (0x0000200040008080ULL), (0x0000100020008080ULL), (0x0000080010008080ULL),
    (0x0000040008008080ULL), (0x0000020004008080ULL), (0x0000800100020080ULL), (0x0000800041000080ULL),
    (0x0000102040800101ULL), (0x0000102040008101ULL), (0x0000081020004101ULL), (0x0000040810002101ULL),
    (0x0001000204080011ULL), (0x0001000204000801ULL), (0x0001000082000401ULL), (0x0000002040810402ULL)
    };

uint64 magicmoves_r_mask[64] =
    {
    (0x000101010101017EULL), (0x000202020202027CULL), (0x000404040404047AULL), (0x0008080808080876ULL),
    (0x001010101010106EULL), (0x002020202020205EULL), (0x004040404040403EULL), (0x008080808080807EULL),
    (0x0001010101017E00ULL), (0x0002020202027C00ULL), (0x0004040404047A00ULL), (0x0008080808087600ULL),
    (0x0010101010106E00ULL), (0x0020202020205E00ULL), (0x0040404040403E00ULL), (0x0080808080807E00ULL),
    (0x00010101017E0100ULL), (0x00020202027C0200ULL), (0x00040404047A0400ULL), (0x0008080808760800ULL),
    (0x00101010106E1000ULL), (0x00202020205E2000ULL), (0x00404040403E4000ULL), (0x00808080807E8000ULL),
    (0x000101017E010100ULL), (0x000202027C020200ULL), (0x000404047A040400ULL), (0x0008080876080800ULL),
    (0x001010106E101000ULL), (0x002020205E202000ULL), (0x004040403E404000ULL), (0x008080807E808000ULL),
    (0x0001017E01010100ULL), (0x0002027C02020200ULL), (0x0004047A04040400ULL), (0x0008087608080800ULL),
    (0x0010106E10101000ULL), (0x0020205E20202000ULL), (0x0040403E40404000ULL), (0x0080807E80808000ULL),
    (0x00017E0101010100ULL), (0x00027C0202020200ULL), (0x00047A0404040400ULL), (0x0008760808080800ULL),
    (0x00106E1010101000ULL), (0x00205E2020202000ULL), (0x00403E4040404000ULL), (0x00807E8080808000ULL),
    (0x007E010101010100ULL), (0x007C020202020200ULL), (0x007A040404040400ULL), (0x0076080808080800ULL),
    (0x006E101010101000ULL), (0x005E202020202000ULL), (0x003E404040404000ULL), (0x007E808080808000ULL),
    (0x7E01010101010100ULL), (0x7C02020202020200ULL), (0x7A04040404040400ULL), (0x7608080808080800ULL),
    (0x6E10101010101000ULL), (0x5E20202020202000ULL), (0x3E40404040404000ULL), (0x7E80808080808000ULL)
    };

__forceinline uint64 Bmagic (const Square square, const uint64 occupied)
    {
    return magic_bishop_db[square][(((occupied) & magicmoves_b_mask[square])* magicmoves_b_magics[square]) >> 55];
    }

__forceinline uint64 Rmagic (const Square square, const uint64 occupied)
    {
    return magic_rook_db[square][(((occupied) & magicmoves_r_mask[square])* magicmoves_r_magics[square]) >> 52];
    }

uint64 initmagicmoves_occ (const int32 * squares, const int32 numSquares, const uint64 linocc)
    {
    int32 i;
    uint64 ret = 0;

    for ( i = 0; i < numSquares; i++ )
        if(linocc &(((uint64)(1)) << i))
            ret |= (((uint64)(1)) << squares[i]);
    return ret;
    }

uint64 initmagicmoves_Rmoves (const int32 square, const uint64 occ)
    {
    uint64 ret = 0;
    uint64 rowbits = (((uint64)0xFF) << (8 * (square / 8)));

    uint64 bit = (((uint64)(1)) << square);

    do
        {
        bit <<= 8;
        ret |= bit;
        } while (bit && !(bit & occ));

    bit = (((uint64)(1)) << square);

    do
        {
        bit >>= 8;
        ret |= bit;
        } while (bit && !(bit & occ));

    bit = (((uint64)(1)) << square);

    do
        {
        bit <<= 1;

        if(bit & rowbits)
            ret |= bit;
        else
            break;
        } while (!(bit & occ));

    bit = (((uint64)(1)) << square);

    do
        {
        bit >>= 1;

        if(bit & rowbits)
            ret |= bit;
        else
            break;
        } while (!(bit & occ));

    return ret;
    }

uint64 initmagicmoves_Bmoves (const int32 square, const uint64 occ)
    {
    uint64 ret = 0;
    uint64 rowbits = (((uint64)0xFF) << (8 * (square / 8)));

    uint64 bit = (((uint64)(1)) << square);
    uint64 bit2 = bit;

    do
        {
        bit <<= 8 - 1;
        bit2 >>= 1;

        if(bit2 & rowbits)
            ret |= bit;
        else
            break;
        } while (bit && !(bit & occ));

    bit = (((uint64)(1)) << square);
    bit2 = bit;

    do
        {
        bit <<= 8 + 1;
        bit2 <<= 1;

        if(bit2 & rowbits)
            ret |= bit;
        else
            break;
        } while (bit && !(bit & occ));

    bit = (((uint64)(1)) << square);
    bit2 = bit;

    do
        {
        bit >>= 8 - 1;
        bit2 <<= 1;

        if(bit2 & rowbits)
            ret |= bit;
        else
            break;
        } while (bit && !(bit & occ));

    bit = (((uint64)(1)) << square);
    bit2 = bit;

    do
        {
        bit >>= 8 + 1;
        bit2 >>= 1;

        if(bit2 & rowbits)
            ret |= bit;
        else
            break;
        } while (bit && !(bit & occ));

    return ret;
    }

void initmagicmoves ()
    {
    int32 i;

    int32 initmagicmoves_bitpos64_database[64] =
        {
        63, 0, 58, 1, 59, 47, 53, 2,
        60, 39, 48, 27, 54, 33, 42, 3,
        61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22, 4,
        62, 57, 46, 52, 38, 26, 32, 41,
        50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16, 9, 12,
        44, 24, 15, 8, 23, 7, 6, 5
        };

    for ( i = 0; i < 64; i++ )
        {
        int32 squares[64];
        int32 numsquares = 0;
        uint64 temp = magicmoves_b_mask[i];

        while(temp)
            {
            uint64 bit = temp & - (int64)temp;
            squares[numsquares++] = initmagicmoves_bitpos64_database[(bit * (0x07EDD5E59A4E28C2)) >> 58];
            temp ^= bit;
            }

        for ( temp = 0; temp < (((uint64)(1)) << numsquares); temp++ )
            {
            uint64 tempocc = initmagicmoves_occ(squares, numsquares, temp);
            magic_bishop_db[i][((tempocc) * magicmoves_b_magics[i]) >> 55] = initmagicmoves_Bmoves(i, tempocc);
            }
        }

    for ( i = 0; i < 64; i++ )
        {
        int32 squares[64];
        int32 numsquares = 0;
        uint64 temp = magicmoves_r_mask[i];

        while(temp)
            {
            uint64 bit = temp & - (int64)temp;
            squares[numsquares++] = initmagicmoves_bitpos64_database[(bit * (0x07EDD5E59A4E28C2)) >> 58];
            temp ^= bit;
            }

        for ( temp = 0; temp < (((uint64)(1)) << numsquares); temp++ )
            {
            uint64 tempocc = initmagicmoves_occ(squares, numsquares, temp);
            magic_rook_db[i][((tempocc) * magicmoves_r_magics[i]) >> 52] = initmagicmoves_Rmoves(i, tempocc);
            }
        }
    }

uint64 nortOccl (uint64 gen, uint64 pro)
    {
    gen |= pro &(gen << 8);
    pro &= (pro << 8);
    gen |= pro &(gen << 16);
    pro &= (pro << 16);
    gen |= pro &(gen << 32);
    return gen;
    }

uint64 soutOccl (uint64 gen, uint64 pro)
    {
    gen |= pro &(gen >> 8);
    pro &= (pro >> 8);
    gen |= pro &(gen >> 16);
    pro &= (pro >> 16);
    gen |= pro &(gen >> 32);
    return gen;
    }

uint64 eastOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_A;
    gen |= pro &(gen << 1);
    pro &= (pro << 1);
    gen |= pro &(gen << 2);
    pro &= (pro << 2);
    gen |= pro &(gen << 4);
    return gen;
    }

uint64 noEaOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_A;
    gen |= pro &(gen << 9);
    pro &= (pro << 9);
    gen |= pro &(gen << 18);
    pro &= (pro << 18);
    gen |= pro &(gen << 36);
    return gen;
    }

uint64 soEaOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_A;
    gen |= pro &(gen >> 7);
    pro &= (pro >> 7);
    gen |= pro &(gen >> 14);
    pro &= (pro >> 14);
    gen |= pro &(gen >> 28);
    return gen;
    }

uint64 westOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_H;
    gen |= pro &(gen >> 1);
    pro &= (pro >> 1);
    gen |= pro &(gen >> 2);
    pro &= (pro >> 2);
    gen |= pro &(gen >> 4);
    return gen;
    }

uint64 soWeOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_H;
    gen |= pro &(gen >> 9);
    pro &= (pro >> 9);
    gen |= pro &(gen >> 18);
    pro &= (pro >> 18);
    gen |= pro &(gen >> 36);
    return gen;
    }

uint64 noWeOccl (uint64 gen, uint64 pro)
    {
    pro &= NotFile_H;
    gen |= pro &(gen << 7);
    pro &= (pro << 7);
    gen |= pro &(gen << 14);
    pro &= (pro << 14);
    gen |= pro &(gen << 28);
    return gen;
    }

Bitboards::Bitboards (void)
    {
    init();
    }

void Bitboards::init (void)
    {
    initmagicmoves();

    Dir dirs[8] =
        {
        N, NE, E, SE, S, SW, W, NW
        };

    atkfnc atks[8] =
        {
        nortOccl, noEaOccl, eastOccl, soEaOccl, soutOccl, soWeOccl, westOccl, noWeOccl
        };

    for ( int32 from = 0; from < 64; ++ from )
        {
        // init
        for ( int32 to = 0; to < 64; ++ to )
            {
            m_dir[from][to] = noDir;
            m_gap[from][to] = EmptyBoard;
            m_ray[from][to] = EmptyBoard;
            }

        // bitboards for directions, rays, gaps and attacks
        for ( int32 dirIdx = 0; dirIdx < 8; ++ dirIdx )
            {
            Dir dir = dirs[dirIdx];

            uint64 gap = EmptyBoard;
            uint64 x = create_bit(from);
            uint64 y = getNext(x, dir);
            int32 to = from + dir;

            while(x && y)
                {
                m_dir[from][to] = dir;
                m_gap[from][to] = gap;
                m_ray[from][to] = getLine(to, dir);
                m_atk[from][to] = atks[dirIdx];

                x = getNext(x, dir);
                gap |= x;

                y = getNext(y, dir);
                to += dir;
                }
            }

        // rook
        m_attacksR[from] = EmptyBoard;
        m_attacksR[from] |= getLine(from, N);
        m_attacksR[from] |= getLine(from, E);
        m_attacksR[from] |= getLine(from, S);
        m_attacksR[from] |= getLine(from, W);

        // bishop
        m_attacksB[from] = EmptyBoard;
        m_attacksB[from] |= getLine(from, NE);
        m_attacksB[from] |= getLine(from, SE);
        m_attacksB[from] |= getLine(from, SW);
        m_attacksB[from] |= getLine(from, NW);

        // queen
        m_attacksQ[from] = m_attacksR[from] | m_attacksB[from];

        // king
        m_attacksK[from] = EmptyBoard;
        m_attacksK[from] |= getNext(from, N);
        m_attacksK[from] |= getNext(from, NE);
        m_attacksK[from] |= getNext(from, E);
        m_attacksK[from] |= getNext(from, SE);
        m_attacksK[from] |= getNext(from, S);
        m_attacksK[from] |= getNext(from, SW);
        m_attacksK[from] |= getNext(from, W);
        m_attacksK[from] |= getNext(from, NW);

        // knight
        m_attacksN[from] = EmptyBoard;
        m_attacksN[from] |= getNext(from, NNE);
        m_attacksN[from] |= getNext(from, ENE);
        m_attacksN[from] |= getNext(from, ESE);
        m_attacksN[from] |= getNext(from, SSE);
        m_attacksN[from] |= getNext(from, SSW);
        m_attacksN[from] |= getNext(from, WSW);
        m_attacksN[from] |= getNext(from, WNW);
        m_attacksN[from] |= getNext(from, NNW);

        // pawn
        m_attacksP[White][from] = EmptyBoard;
        m_attacksP[White][from] |= getNext(from, NE);
        m_attacksP[White][from] |= getNext(from, NW);

        m_attacksP[Black][from] = EmptyBoard;
        m_attacksP[Black][from] |= getNext(from, SE);
        m_attacksP[Black][from] |= getNext(from, SW);

        // ranks & files
        m_ranks[Rank1] = Rank_1;
        m_ranks[Rank2] = Rank_2;
        m_ranks[Rank3] = Rank_3;
        m_ranks[Rank4] = Rank_4;
        m_ranks[Rank5] = Rank_5;
        m_ranks[Rank6] = Rank_6;
        m_ranks[Rank7] = Rank_7;
        m_ranks[Rank8] = Rank_8;

        m_files[FileA] = File_A;
        m_files[FileB] = File_B;
        m_files[FileC] = File_C;
        m_files[FileD] = File_D;
        m_files[FileE] = File_E;
        m_files[FileF] = File_F;
        m_files[FileG] = File_G;
        m_files[FileH] = File_H;

        m_nextFiles[FileA] = File_B;
        m_nextFiles[FileB] = File_A | File_C;
        m_nextFiles[FileC] = File_B | File_D;
        m_nextFiles[FileD] = File_C | File_E;
        m_nextFiles[FileE] = File_D | File_F;
        m_nextFiles[FileF] = File_E | File_G;
        m_nextFiles[FileG] = File_F | File_H;
        m_nextFiles[FileH] = File_G;

        // front spans (passed pawns)
        uint64 x = create_bit(from);
        uint64 y = EmptyBoard;
        uint64 z = EmptyBoard;

        m_frontSpans[White][from] = EmptyBoard;
        y = getNext(x, N);
        z = y | shift_E(y) | shift_W(y);

        while(z)
            {
            m_frontSpans[White][from] |= z;
            z = getNext(z, N);
            }

        m_frontSpans[Black][from] = EmptyBoard;
        y = getNext(x, S);
        z = y | shift_E(y) | shift_W(y);

        while(z)
            {
            m_frontSpans[Black][from] |= z;
            z = getNext(z, S);
            }

        // quadratics
        z = getNext(x, N);
        y = getLine(from, N) | x;

        while(z)
            {
            y |= shift_E(y);
            y |= shift_W(y);
            z = getNext(z, N);
            }
        m_quadratics[White][from] = y;

        z = getNext(x, S);
        y = getLine(from, S) | x;

        while(z)
            {
            y |= shift_E(y);
            y |= shift_W(y);
            z = getNext(z, S);
            }
        m_quadratics[Black][from] = y;
        }
    }

uint64 Bitboards::getNext (uint64 pos, Dir dir)
    {
    switch(dir)
        {
        case N:
            return (shift_N(pos));

        case NE:
            return (shift_NE(pos));

        case E:
            return (shift_E(pos));

        case SE:
            return (shift_SE(pos));

        case S:
            return (shift_S(pos));

        case SW:
            return (shift_SW(pos));

        case W:
            return (shift_W(pos));

        case NW:
            return (shift_NW(pos));

        case NNE:
            return (shift_NNE(pos));

        case ENE:
            return (shift_ENE(pos));

        case ESE:
            return (shift_ESE(pos));

        case SSE:
            return (shift_SSE(pos));

        case SSW:
            return (shift_SSW(pos));

        case WSW:
            return (shift_WSW(pos));

        case WNW:
            return (shift_WNW(pos));

        case NNW:
            return (shift_NNW(pos));

        case noDir:
            return (pos);
        }
    return (pos);
    }

uint64 Bitboards::getNext (int32 field, Dir dir)
    {
    if(0 <= field && field < 64)
        return (getNext(create_bit(field), dir));
    return (EmptyBoard);
    }

uint64 Bitboards::getLine (int32 field, Dir dir)
    {
    uint64 line = EmptyBoard;
    uint64 pos = getNext(field, dir);

    while(pos)
        {
        line |= pos;
        pos = getNext(pos, dir);
        }
    return (line);
    }

uint64 Bitboards::getAttacksR (Square field, const uint64 & occ)
    {
    return (Rmagic(field, occ));
    }

uint64 Bitboards::getAttacksB (Square field, const uint64 & occ)
    {
    return (Bmagic(field, occ));
    }

uint64 Bitboards::getAttacksQ (Square field, const uint64 & occ)
    {
    return (getAttacksR(field, occ) | getAttacksB(field, occ));
    }

uint64 Bitboards::getAttacks (Square from, Square to, const uint64 & occ)
    {
    atkfnc fnc = m_atk[from][to];

    if(fnc)
        return fnc(create_bit(from), ~occ);

    return EmptyBoard;
    }