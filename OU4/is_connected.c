/*
 * This is a C program which takes a file input as argument,
 * with the file describing a graph of nodes connected by edges.
 * The program reads the graph information from the file, builds the graph,
 * and then asks the user for an origin and destination node.
 * It then checks if there is a path from the origin node to the destination
 * in the graph and prints the result.
 *
 * Authors: Adam Pettersson
 *	    Markus Granath
 *
 *
 * Version information:
 *   2023-06-02: v2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"
#include <ctype.h>
#include "list.h"
#include "queue.h"

#define MAX_INPUT_LENGTH 500
#define MAX_NODE_NAME_LENGTH 40
#define MAX_NODE_CHAR_LENGTH 41

bool validate_node_names(char *src, char *dest);
int get_number_of_edges(FILE *fp);
void add_edges_to_list(FILE *fp, list *l);
struct graph_edges *create_edge(char *src, char *dest);
int info_from_file(int argc, char **argv, list *l);
graph *create_graph(graph *g, list *l);
bool find_path(graph *g, node *src, node *dest);
void check_the_input(char *input, char *origin, char *destination, graph *g);
bool check_for_quit(char *input);
void find_and_show_path(graph *g, char *origin, char *destination);
void freefunc(void *data_to_free);
graph *prepare_graph(int argc, char **argv, list *l);

// adam@adam-VirtualBox:~/edu/doa/OU4$ gcc -I/home/adam/edu/doa/OU4 -o connected is_connected.c graph.c list.c queue.c dlist.c array_1d.c

// Verkar som att input med en nod strular? testa med filen badmap?  
// Vad är det i resultatet som inte stämmer överens med vad som förväntas i 3-directed graph? 

struct graph_edges
{
	char src[MAX_NODE_CHAR_LENGTH];
	char dest[MAX_NODE_CHAR_LENGTH];
};

/**
 * main() = The main function of the program.
 * @argc: Number of command arguments.
 * @argv: Array of command arguments.
 *
 * Returns: 0 if everything works out.
 */

int main(int argc, char **argv)
{
	// Initialize an empty list
	list *l = list_empty(freefunc);

	// Prepare the graph using the file inputs and the list
	graph *g = prepare_graph(argc, argv, l);

	// Initialize input, origin and destination strings
	char input[MAX_INPUT_LENGTH];
	char origin[MAX_NODE_CHAR_LENGTH], destination[MAX_NODE_CHAR_LENGTH];

	// Keep checking for user input until a quit signal is received
	while (1)
	{
		check_the_input(input, origin, destination, g);

		if (check_for_quit(input))

			break;

		find_and_show_path(g, origin, destination);
	}

	// Free up the memory allocated to the graph
	graph_kill(g);

	return 0;
}

/**
 * prepare_graph() - Prepares the graph, reads info from the file and
 * creates the graph based on the infoo.
 *
 * @argc: Number of command line arguments.
 * @argv: Array of command line arguments.
 * @l: Pointer to the list.
 *
 * Returns: Pointer to the created graph.
 */
graph *prepare_graph(int argc, char **argv, list *l)
{
	// Get info from file and get the amount of edges
	int edges_amount = info_from_file(argc, argv, l);

	// Build an empty graph with the amount of edges
	graph *g = graph_empty(edges_amount);

	// Build the graph from the list
	g = create_graph(g, l);

	// Free up the memory
	list_kill(l);

	return g;
}

/**
 * info_from_file() - Get information from a file and populate the list.
 * @argc: Number of command line arguments.
 * @argv: Array of command line arguments.
 * @l: Pointer to the list.
 *
 * Returns: The number of edges found in the file.
 */
int info_from_file(int argc, char **argv, list *l)
{

	// Check if we have a file as input
	if (argc < 2)
	{
		printf("No file as input\n");
		exit(EXIT_FAILURE);
	}

	// Open file
	FILE *fp = fopen(argv[1], "r");
	if (!fp)
	{
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	// Extract the amount of edges from the file
	int edges_amount = get_number_of_edges(fp);

	if (edges_amount == 0)
	{
		printf("ERROR: Number of edges was not added \n");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	// Add the edges to the list
	add_edges_to_list(fp, l);

	// Close the file
	fclose(fp);

	return edges_amount * 2;
}

/**
 * get_number_of_edges() - Get the number of edges from a file.
 * @fp: Pointer to the file.
 *
 * Returns: The number of edges found in the file.
 */
int get_number_of_edges(FILE *fp)
{
	char info_from_file[MAX_INPUT_LENGTH];

	// Start the edge count
	int edgeCount = 0;
	bool foundEdges = false;

	// Read info from the file via the loop
	while (fgets(info_from_file, MAX_INPUT_LENGTH, fp) != NULL)
	{
		if (info_from_file[0] >= '0' && info_from_file[0] <= '9')
		{
			size_t file_size = strlen(info_from_file);

			if (info_from_file[file_size - 1] == '\n')
			{
				info_from_file[file_size - 1] = '\0';

				edgeCount = strtol(info_from_file, NULL, 10);

				foundEdges = true;
				break;
			}
		}
	}

	if (foundEdges && edgeCount > 0)
	{
		return edgeCount * 2;
	}
	else
	{
		return 0;
	}
}

/**
 * add_edges_to_list() - Add edges from a file to a list.
 * @fp: Pointer to the file.
 * @l: Pointer to the list.
 */
void add_edges_to_list(FILE *fp, list *l)
{
	char info_from_file[MAX_INPUT_LENGTH];

	list_pos current_position = list_first(l);

	while (fgets(info_from_file, MAX_INPUT_LENGTH, fp) != NULL)
	{
		if (info_from_file[0] != '#' && info_from_file[0] != '\n') // Check if the line does not start with '#' or '\n'
		{
			char src[MAX_NODE_CHAR_LENGTH], dest[MAX_NODE_CHAR_LENGTH];

			sscanf(info_from_file, "%s%s", src, dest);

			if (validate_node_names(src, dest))
			{
				current_position = list_insert(l, create_edge(src, dest), current_position);
			}
			else
			{
				fclose(fp);
				exit(EXIT_FAILURE);
			}
		}
	}
}

/**
 * create_edge() - Create an edge from source to destination.
 * @src: Source node name.
 * @dest: Destination node name.
 *
 * Returns: Pointer to the created edge.
 */
struct graph_edges *create_edge(char *src, char *dest)
{
	struct graph_edges *edge = malloc(sizeof(struct graph_edges));

	memcpy(edge->src, src, MAX_NODE_CHAR_LENGTH);
	edge->src[MAX_NODE_CHAR_LENGTH - 1] = '\0';

	memcpy(edge->dest, dest, MAX_NODE_CHAR_LENGTH);
	edge->dest[MAX_NODE_CHAR_LENGTH - 1] = '\0';

	return edge;
}

/**
 * validate_node_names() - Validate if source and destination node names are alphanumeric
 * @src: Source node name.
 * @dest: Destination node name.
 *
 * Returns: true if both source and destination node names are alphanumeric, false otherwise.
 */
bool validate_node_names(char *src, char *dest)
{
	// Iterate over characters, checking if they're alphanumeric and counting them
	int scource_name = 0;
	int destination_name = 0;

	while (src[scource_name] != '\0')
	{
		if (!isalnum(src[scource_name]))
		{
			printf("ERROR: Node name for source %s ", src);
			printf("is not alphanumeric. \n");
			return false;
		}
		scource_name++;
	}

	while (dest[destination_name] != '\0')
	{
		if (!isalnum(dest[destination_name]))
		{
			printf("ERROR: Node name for destination %s ", dest);
			printf("is not alphanumeric. \n");
			return false;
		}
		destination_name++;
	}

	// Check if length of node names exceed maximum allowed length
	if (scource_name > MAX_NODE_NAME_LENGTH)
	{
		printf("ERROR: Source node name too long. \n");
		printf("Max length is 40 characters per node name. \n");
		printf("Current length of source name: %d characters\n", scource_name);
		return false;
	}

	if (destination_name > MAX_NODE_NAME_LENGTH)
	{
		printf("ERROR: Destination node name too long. \n");
		printf("Max length is 40 characters per node name. \n");
		printf("Current length of destination name: %d characters\n", destination_name);
		return false;
	}

	// If all checks passed, return true.
	return true;
}

/**
 * create_graph() - Create a graph from the list.
 * @g: Pointer to the graph.
 * @l: Pointer to the list.
 *
 * Returns: Pointer to the created graph.
 */
graph *create_graph(graph *g, list *l)
{
	// Loopa igenom alla positioner i listan, från första till sista
	for (list_pos pos = list_first(l); pos != list_end(l); pos = list_next(l, pos))
	{
		// Inspektera nuvarande position i listan och lagra kanten i 'edge'
		struct graph_edges *edge = list_inspect(l, pos);

		// Försök hitta noden 'src' i grafen 'g'
		node *src = graph_find_node(g, edge->src);

		// Om noden 'src' inte finns i grafen, lägg till den
		if (src == NULL)
		{
			g = graph_insert_node(g, edge->src);
			src = graph_find_node(g, edge->src);
		}

		// Försök hitta noden 'dest' i grafen 'g'
		node *dest = graph_find_node(g, edge->dest);

		// Om noden 'dest' inte finns i grafen, lägg till den
		if (dest == NULL)
		{
			g = graph_insert_node(g, edge->dest);
			dest = graph_find_node(g, edge->dest);
		}

		// Lägg till en kant mellan noderna 'src' och 'dest' i grafen
		g = graph_insert_edge(g, src, dest);
	}

	// Returnera den uppdaterade grafen
	return g;
}

/**
 * visit_node() - Marks a node as visited and enqueues it in the queue.
 * @g: The graph containing the node.
 * @n: The node to visit.
 * @q: The queue used in the BFS traversal.
 *
 * This function is used in the breadth-first search (BFS) traversal of the graph.
 * It checks if a node has been visited (seen), if not, it marks it as visited
 * and enqueues it in the queue for further processing.
 */
void visit_node(graph *g, node *n, queue *q)
{
	if (!graph_node_is_seen(g, n))
	{
		graph_node_set_seen(g, n, 1);
		q = queue_enqueue(q, n);
	}
}

/**
 * destination_found() - Handles the scenario where the destination node is found.
 * @g: The graph containing the nodes.
 * @q: The queue used in the BFS traversal.
 *
 * This function is called when the destination node has been found in the BFS traversal.
 * It resets all nodes in the graph to "not seen" and frees the memory used by the queue.
 * Returns: true to indicate that the destination has been found.
 */
bool destination_found(graph *g, queue *q)
{
	graph_reset_seen(g);
	queue_kill(q);
	return 1;
}

/**
 * find_path() - Finds if there is a path between the source and destination nodes using BFS.
 * @g: The graph containing the nodes.
 * @src: The source node.
 * @dest: The destination node.
 *
 * This function uses breadth-first search (BFS) to find if there is a path
 * between the source and destination nodes. It marks the source node as visited,
 * then visits all its neighboring nodes by adding them to a queue.
 * If the destination node is found during this process, it returns true.
 * If the queue is emptied and the destination node has not been found, it returns false.
 * Returns: true if a path exists, false otherwise.
 */
bool find_path(graph *g, node *src, node *dest)
{
	// Mark the source node as seen
	g = graph_node_set_seen(g, src, 1);

	// Create an empty queue and put the source node into it
	queue *q = queue_empty(NULL);
	q = queue_enqueue(q, src);

	// Loop while the queue is not empty
	while (!queue_is_empty(q))
	{
		// Take a node away from the queue and get its neighbors
		node *newNode = queue_front(q);
		q = queue_dequeue(q);
		dlist *neighbourSet = graph_neighbours(g, newNode);
		dlist *neighbourSetCopy = dlist_empty(NULL);
		dlist_pos pos = dlist_first(neighbourSet);

		// Create a copy of the neighbourSet
		while (!dlist_is_end(neighbourSet, pos))
		{
			void *element = dlist_inspect(neighbourSet, pos);
			dlist_insert(neighbourSetCopy, element, dlist_first(neighbourSetCopy));
			pos = dlist_next(neighbourSet, pos);
		}

		pos = dlist_first(neighbourSetCopy);

		// Go through each neighbor
		while (!dlist_is_end(neighbourSetCopy, pos))
		{
			node *neighbourNode = dlist_inspect(neighbourSetCopy, pos);

			// If a neighbour is the destination we found the path
			if (nodes_are_equal(neighbourNode, dest))
			{
				// dlist_kill(neighbourSet); // Do not deallocate neighbourSet here
				dlist_kill(neighbourSetCopy);
				return destination_found(g, q);
			}

			// Visit the neighbour, set as seen and remove
			visit_node(g, neighbourNode, q);

			// Go to the next neighbour
			pos = dlist_next(neighbourSetCopy, pos);
		}

		// kill the neighbours list after using it
		//  dlist_kill(neighbourSet); // Do not deallocate neighbourSet here
		dlist_kill(neighbourSetCopy);
	}

	// No path found
	queue_kill(q);
	graph_reset_seen(g);
	return false;
}

/**
 * check_the_input() - Process user input for origin and destination nodes.
 * @input: Array to store user input.
 * @origin: Array to store origin node.
 * @destination: Array to store destination node.
 */
void check_the_input(char *input, char *origin, char *destination, graph *g)
{
	node *src = NULL;
	node *dest = NULL;

	while (src == NULL || dest == NULL)
	{
		printf("Enter origin and destination (quit to exit): ");
		fgets(input, MAX_INPUT_LENGTH, stdin);

		// Remove trailing newline character
		if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
			input[strlen(input) - 1] = '\0';

		if (check_for_quit(input)) 
			exit(0);

		int numInputs = sscanf(input, "%s%s", origin, destination);

		if (numInputs < 2)
		{
			printf("You need to input both origin and destination nodes. Try again.\n\n");
			continue;
		}

		src = graph_find_node(g, origin);
		dest = graph_find_node(g, destination);

		if (src == NULL || dest == NULL)
		{
			printf("One or both nodes do not exist. Try again.\n\n");
			src = NULL;
			dest = NULL;
		}
	}
}

/**
 * check_for_quit() - Check if the user wants to quit.
 * @input: User input.
 *
 * Returns: true if the user wants to quit, false otherwise.
 */
bool check_for_quit(char *input)
{
	// Check if user wants to quit
	if (strncmp(input, "quit", 4) == 0)
	{
		printf("Normal exit.\n");
		return true;
	}

	return false;
}

/**
 * find_and_show_path() - Finds if there is a path between the input nodes and displays the result.
 * @g: The graph where the search will be performed.
 * @origin: The name of the source node.
 * @destination: The name of the destination node.
 *
 * This function first checks if the nodes exist in the graph. If they do,
 * it uses the find_path function to check if there's a path between them.
 * The result is then printed to the user.
 */
void find_and_show_path(graph *g, char *origin, char *destination)
{
	// Find nodes corresponding to origin and destination in the graph
	node *scource_node = graph_find_node(g, origin);
	node *destination_node = graph_find_node(g, destination);

	// If one or both nodes don't exist, print an error message
	if (scource_node == NULL || destination_node == NULL)
	{
		printf("One or both nodes do not exist. Try again. \n");
	}

	else
	{
		// If nodes exist, find if a path exists between them
		if (find_path(g, scource_node, destination_node))
		{
			printf("There is a path from %s to %s.\n\n", origin, destination);
		}
		else
		{
			printf("There is no path from %s to %s.\n\n", origin, destination);
		}
	}
}

/**
 * freefunc() - A helper function for freeing memory used by list elements.
 * @data_to_free: The data to be freed.
 *
 * This function is used as the free function when the list is created.
 * It gets called every time an element is removed from the list.
 */
void freefunc(void *data_to_free)
{
	free(data_to_free);
}