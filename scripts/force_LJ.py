import argparse
import numpy as np
import matplotlib.pyplot as plt

def f(E0, d, dmoy):
    return abs(24 * E0 * (2 * (d / dmoy) ** 13 - (d / dmoy) ** 7) / d)

def main():
    parser = argparse.ArgumentParser(description="Tracer une fonction en échelle logarithmique.")
    parser.add_argument("--E0", type=float, required=True, help="Valeur du paramètre E0")
    parser.add_argument("--d", type=float, required=True, help="Valeur du paramètre d")
    parser.add_argument("--dmoy", type=float, required=True, help="Valeur du paramètre dmoy")
    parser.add_argument("--param1", type=str, choices=["E0", "d", "dmoy"], required=True, help="Premier paramètre à faire varier")
    parser.add_argument("--param2", type=str, choices=["E0", "d", "dmoy"], required=True, help="Deuxième paramètre à faire varier")

    args = parser.parse_args()

    param1_value = getattr(args, args.param1)
    param2_value = getattr(args, args.param2)

    x_values = np.logspace(np.log10(param1_value) - 5, np.log10(param1_value) + 5, 500)
    y_values = np.logspace(np.log10(param2_value) - 5, np.log10(param2_value) + 5, 10)  # 10 valeurs espacées log

    plt.figure(figsize=(8, 6))

    for val in y_values:
        if args.param1 == "E0":
            if args.param2 == "d":
                y_data = f(x_values, val, args.dmoy)
            elif args.param2 == "dmoy":
                y_data = f(x_values, args.d, val)
        elif args.param1 == "d":
            if args.param2 == "E0":
                y_data = f(val, x_values, args.dmoy)
            elif args.param2 == "dmoy":
                y_data = f(args.E0, x_values, val)
        elif args.param1 == "dmoy":
            if args.param2 == "E0":
                y_data = f(val, args.d, x_values)
            elif args.param2 == "d":
                y_data = f(args.E0, val, x_values)

        plt.loglog(x_values, y_data, label=f"{args.param2}={val:.2e}")

    plt.xlabel(args.param1)
    plt.ylabel("f(x)")
    plt.title(f"Influence de {args.param1} et {args.param2} sur f")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.show()

if __name__ == "__main__":
    main()