from utils.graph_loader import load_graph


def main():

    graph = load_graph()

    if graph is None:
        print("Unable to continue.")
        return

    print("\n==============================")
    print("ResQAI System Ready")
    print("==============================")
    print("Emergency Response Engine Initialized.")


if __name__ == "__main__":
    main()