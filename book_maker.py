import sys
import chess.pgn
from collections import defaultdict, Counter

if len(sys.argv) < 3:
    print("Usage: python pgn2bookfile.py input.pgn output.txt")
    exit(1)

fen_moves = defaultdict(list)
with open(sys.argv[1]) as pgn:
    while True:
        game = chess.pgn.read_game(pgn)
        if game is None:
            break
        board = game.board()
        for move in game.mainline_moves():
            fen = board.fen()
            fen_moves[fen].append(move.uci())
            board.push(move)

with open(sys.argv[2], "w") as out:
    for fen, moves in fen_moves.items():
        move_counts = Counter(moves)
        moves_str = " ".join([f"{m}:{c}" for m, c in move_counts.items()])
        out.write(f"{fen} {moves_str}\n")
