import osmnx as ox


def load_graph(filepath="data/raw/chandigarh.graphml"):
    """
    Load the road network graph from a GraphML file.
    """

    print("=" * 50)
    print("Loading Chandigarh Road Network...")
    print("=" * 50)

    try:
        # Load graph
        G = ox.load_graphml(filepath)

        print("Graph loaded successfully!")
        print(f"Total Nodes : {len(G.nodes)}")
        print(f"Total Edges : {len(G.edges)}")

        return G

    except FileNotFoundError:
        print("Graph file not found.")
        return None

    except Exception as e:
        print("Error:", e)
        return None