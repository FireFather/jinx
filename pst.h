#ifndef PST_H
#define PST_H

#include "define.h"

static int32 pstPawnEg[64] =
    {
	0, 0, 0, 0, 0, 0, 0, 0,
	3, 35, 49, 31, 24, 60, 50, 27,
	37, 39, 32, 18, 15, 18, 27, 17,
	26, 25, 15, 4, 6, 6, 19, 16,
	19, 16, 7, 6, 3, 6, 10, 9,
	9, 8, 11, 10, 13, 7, -2, -2,
	15, 9, 17, 20, 21, 16, -2, -5,
	0, 0, 0, 0, 0, 0, 0, 0
    };

static int32 pstPawnMg[64] =
    {
 	0, 0, 0, 0, 0, 0, 0, 0,
	174, 110, 85, 101, 95, 37, 23, 6,
	16, 22, 26, 32, 56, 68, 56, 31,
	8, 16, 15, 29, 28, 15, 3, -13,
	-4, 4, 15, 19, 22, 10, 2, -18,
	2, -1, -1, 5, 12, 12, 24, 2,
	-10, -6, -17, -20, -19, 14, 26, -16,
	0, 0, 0, 0, 0, 0, 0, 0
    };

static int32 pstKnightEg[64] =
    {
	-17, -14, -11, -18, -31, -22, -12, -32,
	-26, -18, -13, -7, -17, -30, -22, -17,
	-27, -17, 3, 10, -1, 4, -8, -26,
	-19, -5, 8, 16, 31, 7, 4, -34, -36,
	-13, 6, 20, 13, 10, -13, -37, -58,
	-33, -18, 3, -11, -21, -36, -47, -65,
	-36, -40, -35, -34, -33, -27, -47, -93,
	-65, -50, -28, -45, -51, -65, -87
    };

static int32 pstKnightMg[64] =
    {
	-252, -100, -68, -18, 36, -37, -107, -246,
	-65, -51, -8, 30, 44, 36, -26, -52,
	-32, 15, 36, 47, 81, 82, 33, -29,
	-21, 0, 41, 40, 14, 60, 11, 11,
	-24, -4, 14, 9, 22, 9, 15, -15,
	-43, -12, -10, 12, 22, 5, 9, -34,
	-53, -45, -26, -5, -5, -11, -36, -24,
	-62, -31, -53, -51, -15, -27, -33, -75
    };

static int32 pstBishopEg[64] =
    {
	5, 8, -4, 4, 5, 0, -25, 7,
	1, 5, 4, 7, -1, -9, -3, -12,
	-12, 8, 0, -13, -4, -4, -5, -13,
	-6, 1, 1, 11, 10, 4, -1, -8,
	-17, -6, 5, 10, 2, -2, -7, -20,
	-25, -17, -7, -2, 4, -11, -18, -25,
	-41, -32, -25, -13, -18, -28, -26, -63,
	-46, -37, -25, -23, -18, -13, -42, -34
    };

static int32 pstBishopMg[64] =
    {
	-58, -66, -20, -36, -42, -74, 15, -64,
	-50, -40, -28, -19, -21, -5, -47, -62,
	-12, -8, 14, 27, 33, 65, 35, 7,
	-33, -1, 1, 35, 18, 9, 2, -22,
	-14, -6, -5, 17, 20, -8, -2, -4,
	-11, 10, 2, 0, 0, 7, 8, -5,
	8, 6, 9, -10, -4, 14, 26, 3,
	-7, 7, -17, -25, -23, -21, -6, -21
    };

static int32 pstRookEg[64] =
    {
	38, 44, 34, 33, 36, 38, 38, 31,
	40, 39, 32, 29, 28, 22, 26, 25,
	41, 28, 30, 17, 9, 21, 19, 26,
	32, 29, 22, 15, 11, 21, 16, 19,
	25, 30, 23, 11, 13, 18, 16, 17,
	8, 7, 4, 0, -9, -1, -4, -3,
	-15, -14, -13, -19, -18, -16, -23, -8,
	-15, -15, -13, -21, -26, -14, -15, -24
    };

static int32 pstRookMg[64] =
    {
	12, 7, 32, 34, 30, 44, 37, 51,
	1, -6, 29, 42, 44, 50, 29, 37,
	-17, 13, 15, 36, 63, 72, 48, 8,
	-22, -14, 4, 15, 21, 21, 11, -5,
	-47, -43, -29, -20, -17, -17, -13, -35,
	-50, -36, -31, -25, -7, -19, -11, -36,
	-48, -32, -14, -9, -8, -5, -4, -71,
	-15, -10, -2, 4, 11, 3, -11, -4
    };

static int32 pstQueenEg[64] =
    {
	0, 14, 9, 25, 29, 3, 4, 9,
	14, 37, 40, 37, 51, 35, 36, 13,
	-23, 6, 37, 36, 36, 29, 10, 11,
	-9, 13, 25, 45, 51, 35, 36, 10,
	-37, -8, -1, 27, 13, 4, -4, 4,
	-50, -47, -14, -37, -15, -26, -16, -43,
	-74, -59, -65, -58, -54, -95, -111, -65,
	-89, -85, -80, -62, -81, -94, -116, -105
    };

static int32 pstQueenMg[64] =
    {
	-20, -2, 26, 26, 31, 81, 51, 37,
	-46, -60, -20, 0, -7, 35, -1, 28,
	-21, -21, -16, -4, 34, 65, 59, 18,
	-24, -23, -16, -23, -11, 4, 5, -9,
	-14, -25, -15, -14, -10, -5, 2, -17,
	-22, -5, -12, -6, -5, 1, 3, -13,
	-15, -11, 6, 7, 6, 23, 5, -33,
	5, -8, -5, 2, 4, -23, -30, -8
    };

static int32 pstKingEg[64] =
    {
	-116, -33, -29, -32, -27, -18, -17, -119,
	-46, 20, 25, 13, 19, 32, 42, -55,
	3, 31, 37, 30, 28, 40, 43, 4,
	-16, 13, 30, 35, 33, 34, 23, -1,
	-29, 3, 21, 24, 30, 25, 9, -17,
	-29, -5, 10, 17, 20, 20, 4, -21,
	-24, -6, -1, 8, 10, 11, -3, -25,
	-45, -38, -15, -4, -32, -16, -33, -65
    };

static int32 pstKingMg[64] =
    {
	-70, 93, 108, 133, 125, 65, 36, -22,
	2, 50, 33, 43, 40, 21, -23, -57,
	-54, 50, 9, 9, 7, 16, 18, -45,
	-11, 47, 21, -13, 2, 3, 28, -66,
	-30, 24, 2, 12, -18, -14, -7, -62,
	-40, 15, -19, -18, -29, -35, -10, -36,
	-10, -3, -11, -42, -41, -32, 7, -2,
	-31, 27, 9, -69, -14, -31, 30, 8
    };

#endif