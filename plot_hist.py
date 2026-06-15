#!/usr/bin/env python3

import math
import numpy as np
import matplotlib.pyplot as plt


def parse_file(filename: str):
    with open(filename, "r") as f:
        lines = [line.strip() for line in f if line.strip()]

    global_title = lines[0]
    datasets: list[tuple[int, dict[str, list[float]]]] = []

    i = 1
    while i < len(lines):
        if lines[i].startswith("n_elements:"):
            n_elements = int(lines[i].split(":")[1])

            columns = lines[i + 1].split()

            data: dict[str, list[float]] = {col: [] for col in columns}

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
    plot("find__uint32_t.dat")
    plot("add__uint32_t.dat")
    plot("find__string_view.dat")
    plot("add__string_view.dat")


def plot(filename: str):
    global_title, datasets = parse_file(filename)

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

            values = np.array(data[name])

            ax.bar(x + offset, values, width=width, label=name)

            cumulative = np.cumsum(values)

            ax.plot(x + offset, cumulative, label=name + "_cumulative")

        ax.set_xticks(x)
        ax.set_xticklabels(x_labels)

        ax.set_title(f"n_elements = {n_elements}")
        ax.set_xlabel("n")
        ax.set_ylabel("cycles")

        ax.grid(axis="y", linestyle="--", alpha=0.5)
        ax.legend()

    # Hide unused subplots
    for ax in axes.flat[len(datasets) :]:
        ax.set_visible(False)

    fig.suptitle(global_title, fontsize=16)
    fig.tight_layout()

    plt.savefig(global_title)


if __name__ == "__main__":
    main()
