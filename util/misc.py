"""misc.py

Contains random utilities and nice-to-haves.
"""

import pandas as pd
import json
import numpy as np


def get_csvs(filenames, skip=0):
    """Reads Pandas dataframes from a set of CSV files
    and returns the list of them.

    Has the option to skip the first n rows to ignore
    things such as ARP delays that don't persist in the steady
    state. 
    """
    return [pd.read_csv(filename).iloc[skip:]
            for filename in filenames]


def get_json(filename):
    """Reads a JSON dictionary from a file."""
    with open(filename, 'r') as fp:
        return json.load(fp)


def get_jsons(filenames):
    """Reads JSON dictionaries from a set of JSON files
    and returns the list of them.
    """
    return [get_json(filename)
            for filename in filenames]


def get_metric_csv(dataframes, metric):
    """Maps a list of dataframes to a list of metrics
    based on the provided metric mapping function.
    """
    return np.array([metric(dataframe) 
                     for dataframe in dataframes])


def get_element_json(jsons, key):
    """Maps a list of JSON dictionaries to a list of values
    based on the provided key. Looks up the specified key
    in each JSON to get the value.
    """
    return np.array([json_data[key]
                     for json_data in jsons])