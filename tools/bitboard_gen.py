import enum


class Bitboard(enum.Enum):

    a1 = 0
    b1 = 1
    c1 = 2
    d1 = 3
    e1 = 4
    f1 = 5
    g1 = 6
    h1 = 7
    a2 = 8
    b2 = 9
    c2 = 10
    d2 = 11
    e2 = 12
    f2 = 13
    g2 = 14
    h2 = 15
    a3 = 16
    b3 = 17
    c3 = 18
    d3 = 19
    e3 = 20
    f3 = 21
    g3 = 22
    h3 = 23
    a4 = 24
    b4 = 25
    c4 = 26
    d4 = 27
    e4 = 28
    f4 = 29
    g4 = 30
    h4 = 31
    a5 = 32
    b5 = 33
    c5 = 34
    d5 = 35
    e5 = 36
    f5 = 37
    g5 = 38
    h5 = 39
    a6 = 40
    b6 = 41
    c6 = 42
    d6 = 43
    e6 = 44
    f6 = 45
    g6 = 46
    h6 = 47
    a7 = 48
    b7 = 49
    c7 = 50
    d7 = 51
    e7 = 52
    f7 = 53
    g7 = 54
    h7 = 55
    a8 = 56
    b8 = 57
    c8 = 58
    d8 = 59
    e8 = 60
    f8 = 61
    g8 = 62
    h8 = 63


def gen_bb(squares):
    print('Generating bitboard for squares : ' + str(squares))
    bb = 0x0
    for sq in squares:
        bb = bb | (1 << sq.value)

    print('The answer is {}'.format(hex(bb)))

rank7 = [Bitboard.a7, Bitboard.b7, Bitboard.c7, Bitboard.d7,
            Bitboard.e7, Bitboard.f7, Bitboard.g7, Bitboard.h7]
rank2 = [Bitboard.a2, Bitboard.b2, Bitboard.c2, Bitboard.d2,
            Bitboard.e2, Bitboard.f2, Bitboard.g2, Bitboard.h2]
rank1 = [Bitboard.a1, Bitboard.b1, Bitboard.c1, Bitboard.d1,
            Bitboard.e1, Bitboard.f1, Bitboard.g1, Bitboard.h1]



file1 = [Bitboard.a1, Bitboard.a2, Bitboard.a3, Bitboard.a4,
            Bitboard.a5, Bitboard.a6, Bitboard.a7, Bitboard.a8]
file8 = [Bitboard.h1, Bitboard.h2, Bitboard.h3, Bitboard.h4,
            Bitboard.h5, Bitboard.h6, Bitboard.h7, Bitboard.h8]


cast_wk = [Bitboard.f1, Bitboard.g1]
cast_wq = [Bitboard.b1, Bitboard.c1, Bitboard.d1]
cast_bk = [Bitboard.f8, Bitboard.g8]
cast_bq = [Bitboard.b8, Bitboard.c8, Bitboard.d8]



print('Rank 7\n')
gen_bb(rank7)
print('Rank 2\n')
gen_bb(rank2)
print('Rank 1\n')
gen_bb(rank1)


print('File 1\n')
gen_bb(file1)
print('File 8\n')
gen_bb(file8)

print('cast wk\n')
gen_bb(cast_wk)
print('cast wq\n')
gen_bb(cast_wq)
print('cast bk\n')
gen_bb(cast_bk)
print('cast bq\n')
gen_bb(cast_bq)
