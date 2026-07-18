import pandas as pd


def clean_data(df):
    """
    Remove missing values.
    """
    df = df.dropna()
    return df