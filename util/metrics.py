"""metrics.py 

Provides utility functions from reading a `stream.csv`
that recorded received packet times / sizes.
"""

import unittest
import pandas as pd
import numpy as np


def compute_latencies(stream_df):
    """
    Computes the latencies of each packet and returns a Pandas series
    of the result.

    """
    return stream_df['recv_time'] - stream_df['send_time']


def compute_mean_latency(stream_df):
    """
    Computes the average latency of a stream.
    """
    return compute_latencies(stream_df).mean()


def compute_mean_throughput(stream_df):
    """
    Computes the average throughput of a stream assuming
    packets are sent at a constant rate from the first 
    packet onwards.

    Does not count the size of the first packet in the calculation since
    that one was sent 'before the timer started', since we're calculating
    elapsed time by (recv_time_of_last_packet - recv_time_of_first_packet)

    Returns the result in bytes per second.
    """
    elapsed_time = max(stream_df['recv_time']) - min(stream_df['recv_time'])
    bytes_sent = sum(stream_df['packet_size'][1:])

    return bytes_sent / elapsed_time


def compute_jitter(stream_df):
    """
    Computes the jitter of packets in a stream, defined
    as (for each pair of consecutive packets) the difference
    in their latency. Returns a Pandas Series whose length
    is one less than the original dataframe.
    """
    latencies = compute_latencies(stream_df).to_numpy()
    first_packet_latencies = latencies[:-1]
    second_packet_latencies = latencies[1:]
    return np.abs((second_packet_latencies - first_packet_latencies))


def compute_mean_jitter(stream_df):
    """
    Computes the mean jitter of the stream.
    """
    return compute_jitter(stream_df).mean()


# === TEST =======================================================================

class TestAnalyzeStreamMethods(unittest.TestCase):

    def test_compute_latencies(self):
        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 12, 24, 36],
            'packet_size': [5, 5, 5, 5],
        })

        expected = pd.Series(data=[1, 2, 4, 6])
        self.assertListEqual(compute_latencies(test_df).to_list(), expected.to_list())

    def test_compute_mean_latency(self):
        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 12, 24, 36],
            'packet_size': [5, 5, 5, 5],
        })

        expected = 3.25
        self.assertAlmostEqual(compute_mean_latency(test_df), expected)

    def test_compute_throughput(self):
        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 12, 24, 36],
            'packet_size': [5, 5, 5, 5],
        })

        expected = 15/35
        self.assertAlmostEqual(compute_mean_throughput(test_df), expected)

        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [5, 15, 25, 35],
            'packet_size': [5, 5, 5, 5],
        })

        expected = 5/10
        self.assertAlmostEqual(compute_mean_throughput(test_df), expected)

    def test_compute_jitter(self):
        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 12, 24, 36],
            'packet_size': [5, 5, 5, 5],
        })

        expected = np.array([1, 2, 2])
        self.assertListEqual(compute_jitter(test_df).tolist(), expected.tolist())

        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 11, 21, 31],
            'packet_size': [5, 5, 5, 5],
        })

        expected = np.array([0, 0, 0])
        self.assertListEqual(compute_jitter(test_df).tolist(), expected.tolist())

    def test_compute_mean_jitter(self):
        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 12, 24, 36],
            'packet_size': [5, 5, 5, 5],
        })

        expected = 5/3
        self.assertAlmostEqual(compute_mean_jitter(test_df), expected)

        test_df = pd.DataFrame(data={
            'send_time': [0, 10, 20, 30],
            'recv_time': [1, 11, 21, 31],
            'packet_size': [5, 5, 5, 5],
        })

        expected = 0
        self.assertAlmostEqual(compute_mean_jitter(test_df), expected)

        
if __name__ == "__main__":
    unittest.main()
