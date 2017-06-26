#include "fen.h"
#include "notation.h"

Fen::Fen (Board & board) :
    m_board(& board)
    {
    }

Board & Fen::board (void)
    {
    return * m_board;
    }

void Fen::set (const string & fen)
    {
    board().clearAll();

    // separate groups of FEN-string
    strings FENgroups;
    split(FENgroups, fen.empty() ? STARTPOS : fen);

    uint32 iGroupPos(0);
    strings::iterator it = FENgroups.begin();

    while(it != FENgroups.end())
        {
        string & group = * it++;

        switch(++ iGroupPos)
            {
            case 1:
                setFENPositions(group);
                break;

            case 2:
                {
                if("b" == group)
                    {
                    board().m_side = Black;
                    board().m_hash ^= gl_hashkeys.getKeyColor();
                    }
                else
                    board().m_side = White;
                }
                break;

            case 3:
                {
                if("-" != group)
                    {
                    for ( string::size_type i = 0; i < group.size(); ++ i )
                        {
                        string::value_type & chr = group[i];

                        CastInfo info;

                        if(getRegularCastInfo(info, chr))
                            board().setCastling(info, string(1, chr));
                        else
                            board().setCastling(getExtCastField(chr), string(1, chr));
                        }
                    }
                }
                break;

            case 4:
                board().m_ep = strToField(group);
                break;

            case 5:
                board().m_fifty = strToInt32(group);
                break;

            case 6:
                {
                board().m_ply = (strToInt32(group, 1) - 1) * 2;

                if(1 > board().m_ply)
                    board().m_ply = 1;
                }
                break;
            }
        }
    board().initPieceDeps();
    }

string Fen::get (void)
    {
    string fen;

    // positions
    getFENPositions(fen);

    // side to move
    fen += " ";

    if(Black == board().m_side)
        fen += "b";
    else
        fen += "w";

    // castling rights
    fen += " ";
    string cast;

    if(board().canCastle(CastleWOO))
        cast += board().m_cast_info[CastleWOO].m_fenName;

    if(board().canCastle(CastleWOOO))
        cast += board().m_cast_info[CastleWOOO].m_fenName;

    if(board().canCastle(CastleBOO))
        cast += board().m_cast_info[CastleBOO].m_fenName;

    if(board().canCastle(CastleBOOO))
        cast += board().m_cast_info[CastleBOOO].m_fenName;

    if(cast.empty())
        cast = "-";
    fen += cast;

    // potential move for "en passant"
    fen += " ";
    fen += fieldToStr(board().m_ep);

    // count for fifty-moves-rule
    fen += " ";
    fen += int32ToStr(board().m_fifty);

    // number of next move (after every move of black + 1)
    fen += " ";
    fen += int32ToStr(board().m_ply / 2 + 1);

    return (fen);
    }

string Fen::flip (const string & fen)
    {
    string flipfen;

    strings FENgroups;
    split(FENgroups, fen);

    uint32 iGroupPos(0);
    strings::iterator it = FENgroups.begin();

    while(it != FENgroups.end())
        {
        string & group = * it++;

        switch(++ iGroupPos)
            {
            case 1:
                {
                strings piecelines;
                split(piecelines, group, "/");

                strings::reverse_iterator it = piecelines.rbegin();

                while(it != piecelines.rend())
                    {
                    string & line = * it++;

                    for ( string::size_type i = 0; i < line.size(); ++ i )
                        {
                        string::value_type & chr = line[i];
                        chr = toggleChar(chr);
                        }

                    if(!flipfen.empty())
                        flipfen += "/";
                    flipfen += line;
                    }
                }
                break;

            case 2:
                {
                if("b" == group)
                    flipfen += " w";
                else
                    flipfen += " b";
                }
                break;

            case 3:
                {
                for ( string::size_type i = 0; i < group.size(); ++ i )
                    {
                    string::value_type & chr = group[i];
                    chr = toggleChar(chr);
                    }
                flipfen += " " + group;
                }
                break;

            case 4:
                {
                for ( string::size_type i = 0; i < group.size(); ++ i )
                    {
                    string::value_type & chr = group[i];

                    switch(chr)
                        {
                        case '3':
                            chr = '6';
                            break;

                        case '6':
                            chr = '3';
                            break;
                        }
                    }
                flipfen += " " + group;
                }
                break;

            case 5:
                flipfen += " " + group;
                break;

            case 6:
                flipfen += " " + group;
                break;
            }
        }
    return (flipfen);
    }

void Fen::flip (void)
    {
    set(flip(get()));
    }

bool Fen::isCastExt (const string & fen)
    {
    strings FENgroups;
    split(FENgroups, fen);

    uint32 iGroupPos(0);
    strings::iterator it = FENgroups.begin();

    while(it != FENgroups.end())
        {
        string & group = * it++;

        if(3 == ++ iGroupPos)
            {
            for ( string::size_type i = 0; i < group.size(); ++ i )
                if(noSquare != getExtCastField(group[i]))
                    return true;
            }
        }

    return (false);
    }

void Fen::setFENPositions (const string & fen)
    {
    strings lines;
    split(lines, fen, "/");

    int32 linepos(8);
    strings::iterator itLine = lines.begin();

    while(itLine != lines.end())
        {
        strings::value_type & line = * itLine++;

        int32 startpos(0);

        switch(linepos--)
            {
            case 8:
                startpos = A8;
                break;

            case 7:
                startpos = A7;
                break;

            case 6:
                startpos = A6;
                break;

            case 5:
                startpos = A5;
                break;

            case 4:
                startpos = A4;
                break;

            case 3:
                startpos = A3;
                break;

            case 2:
                startpos = A2;
                break;

            case 1:
                startpos = A1;
                break;
            }

        string::iterator it = line.begin();

        while(it != line.end())
            {
            string::value_type & chr = * it++;

            Piece pc = chrToPiece(chr);

            if(noPiece != pc)
                board().setPiece(Square(startpos++), pc);
            else
                {
                switch(chr)
                    {
                    case '1':
                        startpos += 1;
                        break;

                    case '2':
                        startpos += 2;
                        break;

                    case '3':
                        startpos += 3;
                        break;

                    case '4':
                        startpos += 4;
                        break;

                    case '5':
                        startpos += 5;
                        break;

                    case '6':
                        startpos += 6;
                        break;

                    case '7':
                        startpos += 7;
                        break;

                    case '8':
                        startpos += 8;
                        break;
                    }
                }
            }
        }
    }

void Fen::getFENPositions (string & fen)
    {
    fen.erase();
    fen += getFENLine(A8, H8) + "/";
    fen += getFENLine(A7, H7) + "/";
    fen += getFENLine(A6, H6) + "/";
    fen += getFENLine(A5, H5) + "/";
    fen += getFENLine(A4, H4) + "/";
    fen += getFENLine(A3, H3) + "/";
    fen += getFENLine(A2, H2) + "/";
    fen += getFENLine(A1, H1);
    }

string Fen::getFENLine (Square from, Square to)
    {
    string line;

    int32 emptyCount(0);

    for ( int32 pos = from; pos <= to; ++ pos )
        {
        string str = " ";

        Piece piece = board().getPiece(Square(pos));

        if(noPiece == piece)
            ++ emptyCount;
        else
            str = pieceToStr(piece);

        if(" " != str)
            {
            if(0 < emptyCount)
                {
                line += int32ToStr(emptyCount);
                emptyCount = 0;
                }
            line += str;
            }
        }

    if(0 < emptyCount)
        line += int32ToStr(emptyCount);

    return (line);
    }

bool Fen::getRegularCastInfo (CastInfo & info, const string::value_type & chr)
    {
    switch(chr)
        {
        case 'K':
            info = CastleWOO;
            return (true);

        case 'Q':
            info = CastleWOOO;
            return (true);

        case 'k':
            info = CastleBOO;
            return (true);

        case 'q':
            info = CastleBOOO;
            return (true);
        }
    return (false);
    }

Square Fen::getExtCastField (const string::value_type & chr)
    {
    switch(chr)
        {
        case 'A':
            return (A1);

        case 'B':
            return (B1);

        case 'C':
            return (C1);

        case 'D':
            return (D1);

        case 'E':
            return (E1);

        case 'F':
            return (F1);

        case 'G':
            return (G1);

        case 'H':
            return (H1);

        case 'a':
            return (A8);

        case 'b':
            return (B8);

        case 'c':
            return (C8);

        case 'd':
            return (D8);

        case 'e':
            return (E8);

        case 'f':
            return (F8);

        case 'g':
            return (G8);

        case 'h':
            return (H8);
        }
    return (noSquare);
    }