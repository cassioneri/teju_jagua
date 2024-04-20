# ---
# jupyter:
#   jupytext:
#     formats: ipynb,py:light
#     text_representation:
#       extension: .py
#       format_name: light
#       format_version: '1.5'
#       jupytext_version: 1.16.1
#   kernelspec:
#     display_name: Python 3 (ipykernel)
#     language: python
#     name: python3
# ---

cpu         = 'amd-ryzen-7-1800X'
os          = 'linux'
compiler    = 'gcc-13.2.1'
population  = 'double_integers'

algos       = ['teju', 'dragonbox', 'ryu']
# Choose from 'elapsed', 'error %', 'instructions', 'branches', 'branch misses', 'total'
metric      = 'elapsed'
stats       = ['mean', 'std', 'min', 'median', 'max']

import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio

results     = pd.read_csv(f'../results/{cpu}_{os}_{compiler}/{population}.csv', sep = ';')
algos_data  = pd.Series([results[results['algorithm'] == algo][['binary', metric]].set_index('binary')
    for algo in algos], index = algos)

# Remove algos we don't have data for
algos = [algo for algo in algos if len(algos_data[algo]) != 0]

metric_data = algos_data['teju'].rename(columns = {metric:'teju'})
for algo in algos:
    if algo == 'teju':
        continue
    metric_data = metric_data.join(algos_data[algo].rename(columns = {metric:algo}))
size = len(metric_data)

for algo in algos:
    if algo == 'teju':
        continue
    
    winnings    = metric_data.apply(lambda t : t['teju'] <  t[algo], axis = 1)
    draws       = metric_data.apply(lambda t : t['teju'] == t[algo], axis = 1)
    losses      = metric_data.apply(lambda t : t['teju'] >  t[algo], axis = 1)
    print(f'Comparison against {algo}\n\
    Winnings = {len(winnings[winnings == True].index) / size:3.1%}\n\
    Draws    = {len(draws   [draws    == True].index) / size:3.1%}\n\
    Losses   = {len(losses  [losses   == True].index) / size:3.1%}\n\
    ')


stats_data  = pd.DataFrame([[getattr(algos_data[algo][metric], stat)() for stat in stats] for algo in algos], index = algos, columns = stats)
baseline    = stats_data['mean']['teju']
stats_data['relative'] = [stats_data['mean'][algo] / baseline for algo in algos]
stats_data.style.format('{:.3e}')

# +
histograms = [go.Histogram(x = algos_data[algo][metric] / 1.0e-9, name = algo, histnorm = 'percent') for algo in algos]
figure     = go.Figure(data = histograms)

figure.update_layout(title = 'Histograms', template = 'plotly_dark', height = 600, yaxis_title = '%', xaxis_title = 'time (ns)')

figure.update_xaxes(autorangeoptions_clipmin = 5, autorangeoptions_clipmax = 20, dtick = 0.5)

figure.show()
# -


