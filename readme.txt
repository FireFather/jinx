Fridolin - Engine for chess
(c) 2010-2015 Christian Sommerfeld

mail: c_sommerfeld@gmx.de
web:  https://sites.google.com/site/fridolinchess/

Fridolin is a freeware chess-engine with no warranty of any kind. 
It supports standard protocols to communicate with external 
Graphical User Interfaces (GUI) like Shredder, Winboard, Arena and 
Chessbase. The engine is written in C++ and can be compiled under 
Windows and Linux.

Fridolin uses a lot of good ideas from other authors or programs. 
Thanks to all for sharing the work!

- Greko: protocol, search and eval
- Sungorus: sort and pickup moves
- Scorpio: Bitbases and probing code
- DiscoCheck: KPK Bitbase
- Crafty: Tablebases probing code, lockless hashing
- Eugene Nalimov: Tablebases probing code and permission
- Andrew Kadatch: Tablebases compression library and permission
- Pradyumna Kannan: Magic Bitboards
- Gerd Isenberg: chessprogramming wiki


--------------------------------------
Change Log
--------------------------------------

Version 2.00 (Leiden)
------------
26-Jun-2015

(new) Winboard protocol
(new) Chess960 (Fischer Random Chess)
(new) Multi-PV mode
(new) Nalimov Tablebases
(new) Magic Bitboards
(chg) all parts reworked


Version 1.23 (Leiden)
------------
26-Nov-2010

(new) support multiprocessor (shared hash-tables)
(chg) eval tuned
(chg) lockless hashing
(chg) pseudo-legal move-generation
(fix) 3-fold-repetition
(fix) no hash-information on analysing
(fix) no reset of move-count on new game
(fix) mate-dist-info (moves now instead of ply's)
   

Version 1.00 (Kanazawa)
------------
21-Sep-2010

(new) support of scorpio-bitbases
(fix) capture-move-generation (e.p.-treatment)

Version 0.95 (first release)
------------
2-Sep-2010

(new) UCI-protocol
(new) bitboards and attack-tables
(new) alpha-beta search with PVS
(new) quiescence search with SEE pruning
(new) null move, futility pruning, LMR
(new) evaluation function (PST, mobility, 
      pawns, king-safty, attacks, defends)
