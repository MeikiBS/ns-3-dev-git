import networkx as nx
import matplotlib.pyplot as plt
nodes = []
edges = []
nodes.append(('FT1', 'FT', 'NOT_ASSOCIATED', 0, 0))
nodes.append(('PT1', 'PT', 'NOT_ASSOCIATED', 137.493, 0))
nodes.append(('PT2', 'PT', 'ASSOCIATED', 92.1414, 19.5853))
edges.append(('FT1', 'PT2'))
nodes.append(('PT3', 'PT', 'NOT_ASSOCIATED', 121.263, 53.99))
nodes.append(('PT4', 'PT', 'NOT_ASSOCIATED', 108.435, 78.7827))
nodes.append(('PT5', 'PT', 'NOT_ASSOCIATED', 95.8331, 106.433))
nodes.append(('PT6', 'PT', 'ASSOCIATED', 33.3351, 57.738))
edges.append(('FT1', 'PT6'))
nodes.append(('PT7', 'PT', 'ASSOCIATED', 26.8374, 82.597))
edges.append(('FT1', 'PT7'))
nodes.append(('PT8', 'PT', 'NOT_ASSOCIATED', 13.7427, 130.753))
nodes.append(('PT9', 'PT', 'ASSOCIATED', -8.26365, 78.6234))
edges.append(('FT1', 'PT9'))
nodes.append(('PT10', 'PT', 'ASSOCIATED', -34.4998, 106.18))
edges.append(('FT1', 'PT10'))
nodes.append(('PT11', 'PT', 'ASSOCIATED', -51.8204, 89.7556))
edges.append(('FT1', 'PT11'))
nodes.append(('PT12', 'PT', 'ASSOCIATED', -79.5945, 88.3986))
edges.append(('FT1', 'PT12'))
nodes.append(('PT13', 'PT', 'ASSOCIATED', -73.2938, 53.251))
edges.append(('FT1', 'PT13'))
nodes.append(('PT14', 'PT', 'ASSOCIATED', -98.186, 43.7152))
edges.append(('FT1', 'PT14'))
nodes.append(('PT15', 'PT', 'NOT_ASSOCIATED', -143.175, 30.4327))
nodes.append(('PT16', 'PT', 'NOT_ASSOCIATED', -143.577, 8.13442e-14))
nodes.append(('PT17', 'PT', 'ASSOCIATED', -116.984, -24.8657))
edges.append(('FT1', 'PT17'))
nodes.append(('PT18', 'PT', 'NOT_ASSOCIATED', -116.057, -51.6719))
nodes.append(('PT19', 'PT', 'ASSOCIATED', -45.6651, -33.1776))
edges.append(('FT1', 'PT19'))
nodes.append(('PT20', 'PT', 'ASSOCIATED', -78.1961, -86.8456))
edges.append(('FT1', 'PT20'))
nodes.append(('PT21', 'PT', 'ASSOCIATED', -9.57553, -16.5853))
edges.append(('FT1', 'PT21'))
nodes.append(('PT22', 'PT', 'ASSOCIATED', -22.8442, -70.3072))
edges.append(('FT1', 'PT22'))
nodes.append(('PT23', 'PT', 'ASSOCIATED', -5.80835, -55.2628))
edges.append(('FT1', 'PT23'))
nodes.append(('PT24', 'PT', 'NOT_ASSOCIATED', 14.0605, -133.777))
nodes.append(('PT25', 'PT', 'ASSOCIATED', 18.3476, -56.4681))
edges.append(('FT1', 'PT25'))
nodes.append(('PT26', 'PT', 'ASSOCIATED', 47.4901, -82.2553))
edges.append(('FT1', 'PT26'))
nodes.append(('PT27', 'PT', 'ASSOCIATED', 36.1596, -40.1593))
edges.append(('FT1', 'PT27'))
nodes.append(('PT28', 'PT', 'ASSOCIATED', 40.0296, -29.0832))
edges.append(('FT1', 'PT28'))
nodes.append(('PT29', 'PT', 'NOT_ASSOCIATED', 136.958, -60.9777))
nodes.append(('PT30', 'PT', 'ASSOCIATED', 68.5456, -14.5698))
edges.append(('FT1', 'PT30'))

G = nx.Graph()
for name, typ, status, x, y in nodes:
    G.add_node(name, type=typ, status=status, pos=(x, y))
G.add_edges_from(edges)
pos = {name: (x, y) for name, typ, status, x, y in nodes}

status_colors = {
    'ASSOCIATED': 'dodgerblue',
    'ASSOCIATION_PENDING': 'orange',
    'WAITING_FOR_SELECTED_FT': 'yellow',
    'ASSOCIATION_PREPARING': 'pink',
    'NOT_ASSOCIATED': 'gray',
    'UNKNOWN': 'black'
}

colors = [
    'red' if G.nodes[n]['type'] == 'FT'
    else status_colors.get(G.nodes[n].get('status', 'UNKNOWN'), 'black')
    for n in G.nodes
]

plt.figure(figsize=(10, 6))
nx.draw(G, pos, with_labels=True, node_color=colors, node_size=600, font_size=8, edge_color='gray')
plt.title('DECT-2020 NR Topologie mit FT-PT Verbindungen')
plt.axis('equal')
plt.grid(True)
plt.show()
