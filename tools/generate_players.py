#!/usr/bin/env python3
"""Generate a deterministic Hungarian lottery dataset.

The dataset directory receives three files:

* ``players.txt``  one ticket per line, five distinct numbers from 1 to 90.
* ``draws.txt``    one draw per line, in the same format.
* ``expected.txt`` for every draw, the winners with 2, 3, 4 and 5 matches,
  formatted exactly like the ``lottery`` output.

Half of the draws are random. The other half are copies of existing tickets,
some with one number swapped, so that every match category is exercised even
for small player counts. The expected output is computed independently of the
C++ code by a plain brute force count, which makes it a test oracle.

Usage:
    generate_players.py --players 100000 --draws 20 --seed 7 --out-dir tests/data/medium
"""

import argparse
import random
import sys
from pathlib import Path

NUMBERS_PER_TICKET = 5
HIGHEST_NUMBER = 90
ALL_NUMBERS = range(1, HIGHEST_NUMBER + 1)


def parse_arguments(argv):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--players", type=int, required=True, help="number of tickets to generate")
    parser.add_argument("--draws", type=int, default=10, help="number of draws to generate")
    parser.add_argument("--seed", type=int, default=42, help="random seed, same seed gives same files")
    parser.add_argument("--out-dir", type=Path, required=True, help="directory that receives the files")
    parser.add_argument("--no-expected", action="store_true",
                        help="skip the brute force oracle (useful for very large benchmark inputs)")
    return parser.parse_args(argv)


def random_ticket(rng):
    return rng.sample(ALL_NUMBERS, NUMBERS_PER_TICKET)


def generate_players(rng, count):
    return [random_ticket(rng) for _ in range(count)]


def swap_one_number(rng, ticket):
    replacement = rng.choice([n for n in ALL_NUMBERS if n not in ticket])
    changed = list(ticket)
    changed[rng.randrange(NUMBERS_PER_TICKET)] = replacement
    return changed


def draw_from_players(rng, players):
    ticket = list(rng.choice(players))
    return swap_one_number(rng, ticket) if rng.random() < 0.5 else ticket


def generate_draws(rng, players, count):
    draws = []

    for index in range(count):
        use_player = players and index % 2 == 1
        draws.append(draw_from_players(rng, players) if use_player else random_ticket(rng))

    return draws


def to_bitmask(ticket):
    mask = 0

    for number in ticket:
        mask |= 1 << number

    return mask


def count_winners(player_masks, draw):
    draw_mask = to_bitmask(draw)
    winners = {2: 0, 3: 0, 4: 0, 5: 0}

    for player_mask in player_masks:
        matches = bin(player_mask & draw_mask).count("1")

        if matches >= 2:
            winners[matches] += 1

    return winners


def format_ticket(ticket):
    return " ".join(str(number) for number in ticket)


def format_winners(winners):
    return f"{winners[2]} {winners[3]} {winners[4]} {winners[5]}"


def write_lines(path, lines):
    with path.open("w", encoding="ascii") as output:
        output.writelines(f"{line}\n" for line in lines)


def write_expected(out_dir, players, draws):
    player_masks = [to_bitmask(ticket) for ticket in players]
    results = [format_winners(count_winners(player_masks, draw)) for draw in draws]
    write_lines(out_dir / "expected.txt", results)


def main(argv):
    args = parse_arguments(argv)
    rng = random.Random(args.seed)

    players = generate_players(rng, args.players)
    draws = generate_draws(rng, players, args.draws)

    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_lines(args.out_dir / "players.txt", (format_ticket(t) for t in players))
    write_lines(args.out_dir / "draws.txt", (format_ticket(d) for d in draws))

    if not args.no_expected:
        write_expected(args.out_dir, players, draws)

    print(f"Generated {args.players} players and {args.draws} draws in {args.out_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
