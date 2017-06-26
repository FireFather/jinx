#include "notation.h"
#include "movegen.h"
#include "uci.h"

bool isMove (const string & str)
    {
    if(("O-O" == str) || ("O-O-O" == str))
        return true;

    if(4 == str.size() || 5 == str.size())
        {
        if(noSquare == strToField(str.substr(0, 2)))
            return false;

        if(noSquare == strToField(str.substr(2, 2)))
            return false;

        if(5 == str.size())
            {
            string prom = str.substr(4, 1);
            string proms = "qrbn";

            if(string::npos == proms.find(prom))
                return false;
            }
        return true;
        }
    return false;
    }

string moveToStr (const uint32 & move, const Board & board, bool castling_san)
    {
    Move mov(move);
    string str = fieldToStr(mov.getFrom()) + fieldToStr(mov.getTo());

    if(uci.config.chess960 && (Castle == mov.getType()) && board.canCastle(CastInfo(mov.getInfo())))
        {
        CastInfo castl_info = CastInfo(mov.getInfo());

        if(castling_san)
            {
            bool castl_short = (CastleWOO == castl_info) || (CastleBOO == castl_info);
            str = castl_short ? "O-O" : "O-O-O";
            }
        else
            str = fieldToStr(mov.getFrom()) + fieldToStr(board.getCastRookPos(castl_info));
        }

    if(Promotion == mov.getType())
        {
        switch(mov.getInfo())
            {
            case PromQueen:
                str += "q";
                break;

            case PromRook:
                str += "r";
                break;

            case PromBishop:
                str += "b";
                break;

            case PromKnight:
                str += "n";
                break;
            }
        }
    return (str);
    }

uint32 strToMove (const string & str, const Board & board)
    {
    if(isMove(str))
        {
        MoveGen movegen(board);
        MoveList moves;
        movegen.getAll(moves);

        for ( int32 i = 0; i < moves.size(); ++ i )
            {
            Move move = moves[i];

            if(str == moveToStr(move, board, false) || str == moveToStr(move, board, true))
                return (move);
            }
        }

    return (0);
    }

string fieldToStr (Square field)
    {
    string ret;

    if(noSquare == field)
        ret = "-";
    else
        {
        ret += 'a' + (string::value_type)getFile(field);
        ret += '1' + (string::value_type)getRank(field);
        }
    return (ret);
    }

Square strToField (const string & str)
    {
    Square ret = noSquare;

    if(2 == str.length())
        {
        int32 col = str[0] - 'a';
        int32 row = str[1] - '1';

        if(col >= 0 && col <= 7 && row >= 0 && row <= 7)
            ret = Square(8 * row + col);
        }
    return (ret);
    }

string pieceToStr (Piece piece)
    {
    switch(piece)
        {
        case WP:
            return ("P");

        case WN:
            return ("N");

        case WB:
            return ("B");

        case WR:
            return ("R");

        case WQ:
            return ("Q");

        case WK:
            return ("K");

        case BP:
            return ("p");

        case BN:
            return ("n");

        case BB:
            return ("b");

        case BR:
            return ("r");

        case BQ:
            return ("q");

        case BK:
            return ("k");

        default:
            return ("");
        }
    return ("");
    }

Piece chrToPiece (const string::value_type & chr)
    {
    switch(chr)
        {
        case 'P':
            return (WP);

        case 'N':
            return (WN);

        case 'B':
            return (WB);

        case 'R':
            return (WR);

        case 'Q':
            return (WQ);

        case 'K':
            return (WK);

        case 'p':
            return (BP);

        case 'n':
            return (BN);

        case 'b':
            return (BB);

        case 'r':
            return (BR);

        case 'q':
            return (BQ);

        case 'k':
            return (BK);
        }
    return (noPiece);
    }

uint64 strToBB (const string & str)
    {
    uint64 ret = 0;

    if(isNumber(str))
        {
        string::const_iterator it = str.begin();

        while(it != str.end())
            {
            const string::value_type & chr = * it++;
            ret = ret * 10 + chr - '0';
            }
        }
    return ret;
    }