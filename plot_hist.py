#!/usr/bin/env python3

import argparse
import math
import numpy as np
import matplotlib.pyplot as plt


def parse_file(filename):
    with open(filename, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    global_title = lines[0]
    datasets = []

    i = 1
    while i < len(lines):
        if lines[i].startswith("n_elements:"):
            n_elements = int(lines[i].split(":")[1])

            columns = lines[i + 1].split()

            data = {col: [] for col in columns}

            i += 2

            while i < len(lines) and not lines[i].startswith("n_elements:"):
                values = list(map(int, lines[i].split()))

                for col, value in zip(columns, values):
                    data[col].append(value)

                i += 1

            datasets.append((n_elements, data))

        else:
            i += 1

    return global_title, datasets


def main():
    parser = argparse.ArgumentParser(
        description="Plot benchmark results from a data file."
    )

    parser.add_argument("filename", help="Input benchmark data file")

    parser.add_argument(
        "--logy", action="store_true", help="Use logarithmic scale on Y axis"
    )

    args = parser.parse_args()

    global_title, datasets = parse_file(args.filename)

    nplots = len(datasets)

    ncols = 3
    nrows = math.ceil(nplots / ncols)

    fig, axes = plt.subplots(
        nrows, ncols, figsize=(6 * ncols, 4 * nrows), squeeze=False
    )

    for ax, (n_elements, data) in zip(axes.flat, datasets):
        x_labels = data["n"]
        x = np.arange(len(x_labels))

        series_names = [c for c in data.keys() if c != "n"]

        width = 0.8 / len(series_names)

        for idx, name in enumerate(series_names):
            offset = (idx - (len(series_names) - 1) / 2) * width

            ax.bar(x + offset, data[name], width=width, label=name)

        ax.set_xticks(x)
        ax.set_xticklabels(x_labels)

        ax.set_title(f"n_elements = {n_elements}")
        ax.set_xlabel("n")
        ax.set_ylabel("cycles")

        if args.logy:
            ax.set_yscale("log")

        ax.grid(axis="y", linestyle="--", alpha=0.5)
        ax.legend()

    # Hide unused subplots
    for ax in axes.flat[len(datasets) :]:
        ax.set_visible(False)

    fig.suptitle(global_title, fontsize=16)
    fig.tight_layout()

    plt.show()


if __name__ == "__main__":
    main()
