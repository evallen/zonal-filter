# Doing linear regression to create model of performance
# for MAC functions.
#
# The paper by Buhhler et al. that benchmarks the MAC functions
# provides graphs of their latency vs. payload length in bytes,
# but does not provide tables of the data. We need a model
# of the computation time for each of the fastest hash functions,
# so we take a picture of the graph and measure the points on
# each line to form a linear regression model.

import numpy as np
from sklearn.linear_model import LinearRegression

# List of tuples of measurement values for each hash function
# on the graph. The first element of each tuple is
# the x value (how many bytes were in the payload that was
# MAC'd), and the second element is the pixel distance
# from the x-axis to the line. 
siphash_pixels = [
    (200, 19),
    (400, 34),
    (600, 48),
    (800, 63),
    (1000, 78),
    (1200, 93),
    (1400, 108),
]

chachapoly_pixels = [
    (200, 32),
    (400, 40),
    (600, 49),
    (800, 57),
    (1000, 66),
    (1200, 74),
    (1400, 83),
]

# The graph shows a gridline of 500 microseconds at
# 100 pixels away from the x-axis.
microseconds_per_pixel = 500 / 100


def create_linear_regression(tuples):
    payload_sizes, pixel_values = zip(*tuples)
    payload_sizes = np.array(payload_sizes).reshape(-1, 1)
    pixel_values = np.array(pixel_values)
    microseconds = pixel_values * microseconds_per_pixel

    reg = LinearRegression().fit(payload_sizes, microseconds)
    return reg.coef_, reg.intercept_, reg.score(payload_sizes, microseconds)


if __name__ == "__main__":
    siphash_coefs, siphash_intercept, siphash_score = create_linear_regression(siphash_pixels)
    print(f"SIPHASH")
    print(f"\tcoefs={siphash_coefs},\tintercept={siphash_intercept},\tscore={siphash_score}")
    print(f"\tequivalent bps={1/(siphash_coefs[0]*1e-6)}")

    chachapoly_coefs, chachapoly_intercept, chachapoly_score = create_linear_regression(chachapoly_pixels)
    print(f"CHACHAPOLY")
    print(f"\tcoefs={chachapoly_coefs},\tintercept={chachapoly_intercept},\tscore={chachapoly_score}")
    print(f"\tequivalent bps={1/(chachapoly_coefs[0]*1e-6)}")

