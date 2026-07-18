import pandas as pd


def load_data(file_path):
    """
    Load CSV dataset.
    """
    try:
        data = pd.read_csv(file_path)
        print("Dataset loaded successfully!")
        return data
    except Exception as e:
        print("Error:", e)
        return None