import networkx as nx
import matplotlib.pyplot as plt

def read_files(graph_file_name):

    full_path_graph = "../Multiway Cut/Instances/"+graph_file_name
    file = open(full_path_graph, 'r').read()
    lines = file.split('\n')

    parts = lines[0].split()
    numof_v = int(parts[0])
    numof_e = int(parts[1])

    edges = []
    for i in range(1, numof_e+1):
        parts = lines[i].split()
        edges.append((int(parts[0]), int(parts[1]), int(parts[2])))

    numof_t = int(lines[numof_e+1])
    terminals = lines[numof_e+2].split()
    terminals = list(map(int, terminals))

    full_path_solution = "../Multiway Cut/Solutions/"+graph_file_name.split(".")[0] +".sol"
    file = open(full_path_solution, 'r').read()
    lines = file.split('\n')

    parts = lines[0].split()
    multiway_cut_cost = int(parts[0])
    numof_cut_edges = int(parts[1])

    cut_edges = []
    for i in range(1, numof_cut_edges+1):
        parts = lines[i].split()
        cut_edges.append((int(parts[0]), int(parts[1])))

    return numof_v, numof_e, edges, terminals, numof_cut_edges, multiway_cut_cost, cut_edges

def create_graph(numof_v, numof_e, edges, terminals, cut_edges):
    graph = nx.Graph()

    graph.add_nodes_from([1, numof_v])
    for (u, v, w) in edges:
        graph.add_edge(u, v, weight = w)

    '''
    for node in terminals:
        graph.nodes[node]['color'] = 'red'

    for (u, v) in cut_edges:
        graph[u][v]['color'] = 'red'
        #try:
        #except TypeError:
        #    graph[v][u]['color'] = 'red'
    '''
    return graph


def main():

    graph_file_name = input()
    numof_v, numof_e, edges, terminals, numof_cut_edges, multiway_cut_cost, cut_edges = read_files(graph_file_name)
    print(cut_edges)
    print(terminals)
    graph = create_graph(numof_v, numof_e, edges, terminals, cut_edges)

    pos = nx.spectral_layout(graph, weight = 'weight')

    node_colors = []
    for i in range(0, numof_v):
        node_colors.append('#426b80')
    for node in terminals:
        node_colors[node] = '#80a167'

    edge_labels = nx.get_edge_attributes(graph,'weight')
    plt.figure(figsize=(12,12))
    nx.draw_networkx(graph, pos, with_labels=True, edge_color = '#253037')
    nx.draw_networkx_nodes(graph, pos, nodelist=terminals, node_color= '#80a167')
    nx.draw_networkx_edges(graph, pos, edgelist=cut_edges, edge_color='#c16124')
    nx.draw_networkx_edge_labels(graph, pos, edge_labels=edge_labels)

    '''
    nx.draw_networkx_nodes(graph,pos,
                       nodelist=terminals,
                       node_color='r',
                       node_size=500)
 node_color = node_colors
    node_labels = nx.get_node_attributes(G,'ID')
    nx.draw_networkx_labels(G, pos, labels = node_labels)
    edge_labels = nx.get_edge_attributes(G,'state')
    nx.draw_networkx_edge_labels(G, pos, labels = edge_labels)
    '''
    img_path = graph_file_name.split(".")[0] + '.png'
    plt.savefig(img_path, dpi=300)
    plt.show()

    graph.toagraph()

if __name__ == "__main__":
    main();
