import pandas as pd

df = pd.read_csv("data 0.01s/JobWaitTime.csv")
print(df['data'].max())
for data in df['data']:
    if data > 9000:
        print(data)