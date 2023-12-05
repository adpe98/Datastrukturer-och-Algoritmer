/*
 * This is a C program for creating and managing a graph data structure.
 * The graph is represented using an adjacency list, where each node stores a list of its adjacent nodes.
 * The program uses differet operations, including creating an empty graph, checking if a graph is empty or has any edges,
 * inserting and deleting nodes and edges, finding a node, setting and checking the 'seen' status of a node, and printing the graph.
 * All the nodes in the graph have different names, which is used to identify and compare nodes.
 * The 'seen' status is useful for graph traversal algorithms like breadth-first search.
 *
 * Authors: Adam Pettersson
 *	    Markus Granath
 *
 *
 * Version information:
 *   2023-05-18: v1.0, first public version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "dlist.h"
#include "graph.h"
#include "array_1d.h"

struct graph
{
	array_1d *nodes; // Eftersom att varje nod har en storlek och ett index
	int node_amount;
};

struct node
{
	char *name;
	bool seen;
	dlist *adjacent_nodes; // Använder dlist, eftersom att vi kan komma att vilja ändra antalet
};

/**
 * nodes_are_equal() - Check whether two nodes are equal.
 * @n1: Pointer to node 1.
 * @n2: Pointer to node 2.
 *
 * Returns: true if the nodes are considered equal, otherwise false.
 *
 */
bool nodes_are_equal(const node *n1, const node *n2)
{
	// node names are unique, will therefore compare nodeName of nodes
	return strcmp(n1->name, n2->name) == 0;
}
// =================== GRAPH STRUCTURE INTERFACE ======================

/**
 * graph_empty() - Create an empty graph.
 * @max_nodes: The maximum number of nodes the graph can hold.
 *
 * Returns: A pointer to the new graph.
 */
graph *graph_empty(int max_nodes)
{
	// Alloctes the memory for the graph
	graph *g = malloc(sizeof(*g));
	g->nodes = array_1d_create(0, max_nodes, NULL);
	g->node_amount = 0;

	return g;
}

/**
 * graph_is_empty() - Check if a graph is empty, i.e. has no nodes.
 * @g: Graph to check.
 *
 * Returns: True if graph is empty, otherwise false.
 */
bool graph_is_empty(const graph *g)
{
	return g->nodes == 0;
}

/**
 * graph_has_edges() - Check if a graph has any edges.
 * @g: Graph to check.
 *
 * Returns: True if graph has any edges, otherwise false.
 */
bool graph_has_edges(const graph *g)
{
	// Iterate through all nodes in the graph
	for (int i = 0; i < g->node_amount; i++)
	{
		// Get the current node
		struct node *current_node = array_1d_inspect_value(g->nodes, i);

		// Check if the current node has any adjacent nodes (edges)
		if (!dlist_is_empty(current_node->adjacent_nodes))
		{
			// If any node has an adjacent node, the graph has edges
			return true;
		}
	}

	// If none of the nodes have adjacent nodes, the graph has no edges
	return false;
}

/**
 * graph_insert_node() - Inserts a node with the given name into the graph.
 * @g: Graph to manipulate.
 * @s: Node name.
 *
 * Creates a new node with a copy of the given name and puts it into
 * the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_node(graph *g, const char *s)
{
	// Allocate memory for the new node
	struct node *new_node = malloc(sizeof(struct node));

	// Set the name of the new node
	// Allocate memory for the new string
	new_node->name = malloc(strlen(s) + 1);
	// Check if memory allocation was successful
	if (new_node->name == NULL)
	{
		// Handle memory allocation failure,by returning or freeing already allocated stuff
		free(new_node);
		return NULL;
	}
	// Copy the content of the old string into the new memory location
	strcpy(new_node->name, s);

	// Initialize the adjacent_nodes list for the new node
	new_node->adjacent_nodes = dlist_empty(NULL);

	// Initialize the seen status to false
	new_node->seen = false;

	// Add the new node to the graph's nodes array
	array_1d_set_value(g->nodes, new_node, g->node_amount);

	// Increment the number of nodes in the graph
	g->node_amount++;

	return g;
}

/**
 * graph_find_node() - Find a node stored in the graph.
 * @g: Graph to manipulate.
 * @s: Node identifier, e.g. a char *.
 *
 * Returns: A pointer to the found node, or NULL.
 */
node *graph_find_node(const graph *g, const char *s)
{
	// Go through all nodes in the graph
	for (int i = 0; i < g->node_amount; i++)
	{
		// Get the current node
		node *current_node = array_1d_inspect_value(g->nodes, i);

		// Compare the current nodesname with the given name
		if (strcmp(current_node->name, s) == 0)
		{
			// If the names match return the current node
			return current_node;
		}
	}

	// If no node is found with the given name, return NULL
	return NULL;
}

/**
 * graph_node_is_seen() - Return the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to return seen status for.
 *
 * Returns: The seen status for the node.
 */
bool graph_node_is_seen(const graph *g, const node *n)
{
	return n->seen;
}

/**
 * graph_node_set_seen() - Set the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to set seen status for.
 * @s: Status to set.
 *
 * Returns: The modified graph.
 */
graph *graph_node_set_seen(graph *g, node *n, bool seen)
{
	n->seen = seen;
	return g;
}

/*
 * graph_reset_seen() - Reset the seen status on all nodes in the graph.
 * @g: Graph to modify.
 *
 * Returns: The modified graph.
 */
graph *graph_reset_seen(graph *g)
{
	// Get the number of nodes in the graph
	int num_nodes = g->node_amount;

	// Loop over each node in the graph
	for (int node_idx = 0; node_idx < num_nodes; ++node_idx)
	{
		// Check if the node exists in the graph
		if (array_1d_has_value(g->nodes, node_idx))
		{
			// Get the current node
			node *curr_node = array_1d_inspect_value(g->nodes, node_idx);

			// Reset the 'seen' status of the node
			curr_node->seen = false;
		}
	}

	// Return the modified graph
	return g;
}

/**
 * graph_insert_edge() - Insert an edge into the graph.
 * @g: Graph to manipulate.
 * @n1: Source node (pointer) for the edge.
 * @n2: Destination node (pointer) for the edge.
 *
 * NOTE: Undefined unless both nodes are already in the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_edge(graph *g, node *n1, node *n2)
{
	// find the position where the new node should be inserted
	dlist_pos insert_pos = dlist_first(n1->adjacent_nodes);

	// Insert n2 into the adjacent nodes list at the first position
	dlist_insert(n1->adjacent_nodes, n2, insert_pos);

	// Return the graph
	return g;
}

/**
 * graph_delete_node() - Remove a node from the graph.
 * @g: Graph to manipulate.
 * @n: Node to remove from the graph.
 *
 * Returns: The modified graph.
 *
 * NOTE: Undefined if the node is not in the graph.
 */
graph *graph_delete_node(graph *g, node *n);

/**
 * graph_delete_edge() - Remove an edge from the graph.
 * @g: Graph to manipulate.
 * @n1: Source node (pointer) for the edge.
 * @n2: Destination node (pointer) for the edge.
 *
 * Returns: The modified graph.
 *
 * NOTE: Undefined if the edge is not in the graph.
 */
graph *graph_delete_edge(graph *g, node *n1, node *n2);

/**
 * graph_choose_node() - Return an arbitrary node from the graph.
 * @g: Graph to inspect.
 *
 * Returns: A pointer to an arbitrayry node.
 *
 * NOTE: The return value is undefined for an empty graph.
 */
node *graph_choose_node(const graph *g)
{
	// Check if the graph is not empty.
	if (g->node_amount > 0)
	{
		// Loop through the array of nodes in the graph
		for (int i = array_1d_low(g->nodes); i <= array_1d_high(g->nodes); i++)
		{
			// Check if the position in array has a node
			if (array_1d_has_value(g->nodes, i))
			{
				// Return the first node found
				return array_1d_inspect_value(g->nodes, i);
			}
		}
	}

	// If the graph is empty or no nodes are found, return NULL.
	return NULL;
}

/**
 * graph_neighbours() - Return a list of neighbour nodes.
 * @g: Graph to inspect.
 * @n: Node to get neighbours for.
 *
 * Returns: A pointer to a list of nodes. Note: The list must be
 * dlist_kill()-ed after use.
 */
dlist *graph_neighbours(const graph *g, const node *n)
{
	return n->adjacent_nodes;
}

/**
 * graph_kill() - Destroy a given graph.
 * @g: Graph to destroy.
 *
 * Return all dynamic memory used by the graph.
 *
 * Returns: Nothing.
 */
void graph_kill(graph *g)
{
	// Go over the array and destroy all the elements.
	for (int i = array_1d_low(g->nodes); i <= array_1d_high(g->nodes); i++)
	{
		if (array_1d_has_value(g->nodes, i))
		{
			// Inspect the key/value pair.
			node *entry = array_1d_inspect_value(g->nodes, i);

			// Go over the dlist in node and destroy all elements.
			dlist_pos pos = dlist_first(entry->adjacent_nodes);
			while (!dlist_is_end(entry->adjacent_nodes, pos))
			{
				// No need to inspect the value in dlist as it's not used

				pos = dlist_next(entry->adjacent_nodes, pos);
			}

			// Deallocate the neighbours dlist
			dlist_kill(entry->adjacent_nodes);
			// Deallocate the node structure.
			free(entry->name);
			free(entry);
		}
	}
	// Kill the final parts of the array.
	array_1d_kill(g->nodes);
	// same with graph.
	free(g);
}

/**
 * graph_print() - Iterate over the graph elements and print their values.
 * @g: Graph to inspect.
 *
 * Iterates over the graph and prints its contents.
 *
 * Returns: Nothing.
 */
void graph_print(const graph *g)
{
	printf("Graph print start:\n");

	// Loop thorugh the nodes
	for (int i = 0; i < g->node_amount; i++)
	{
		// find the current node
		struct node *current_node = array_1d_inspect_value(g->nodes, i);

		// print node name
		printf("Node: %s\n", current_node->name);

		dlist *adjacent_nodes = current_node->adjacent_nodes;

		dlist_pos pos = dlist_first(adjacent_nodes);

		// loop thorugh adjacent nodes
		while (!dlist_is_end(adjacent_nodes, pos))
		{
			struct node *adjacent_node = dlist_inspect(adjacent_nodes, pos);

			printf("    Adjacent node: %s\n", adjacent_node->name);

			pos = dlist_next(adjacent_nodes, pos);
		}
	}

	printf("Graph print stops.\n");
}