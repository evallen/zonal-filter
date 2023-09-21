"""pena_latench_regression.py

This file does a quick linear regression on the latency data 
from the paper by Pena et al. measuring the latency of encryption
and decryption (combined) on the MARVELL 88E1680M board.

The resulting model should have a very high R-squared value, indicating
a strong linear correlation between packet size and MACsec latency.
"""

import pandas as pd
from sklearn.linear_model import LinearRegression

df = pd.DataFrame(data={
    'bytes': [64, 128, 256, 512, 1024, 1280, 1518],
    'latency': [0.008e6, 0.011e6, 0.015e6, 0.023e6, 0.039e6, 0.047e6, 0.055e6]
})
print(df)
print(df['bytes'])
print(df['latency'])

model=LinearRegression()
bytes_data = df['bytes'].to_numpy().reshape(-1, 1)
latency_data = df['latency'].to_numpy().reshape(-1, 1)
model.fit(bytes_data, latency_data)

r2_score = model.score(bytes_data, latency_data)
print(model)

print(f"R-squared value: {r2_score}")
print(f"Intercept: {model.intercept_}; Coeffs: {model.coef_}")
