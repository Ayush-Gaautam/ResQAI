def reconstruct_path(parent, destination):
    """
    Reconstruct the shortest path
    from source to destination.
    """

    path = []

    current = destination

    while current is not None:
        path.append(current)
        current = parent[current]

    path.reverse()

    return path