#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "seqpair.h"

int main(int argc, char **argv) {
	if (argc != 3) {
		return EXIT_FAILURE;
	}

	char *input_f = argv[1];
	char *output_f = argv[2];

	Graph *g = load_from_file(input_f);
	if (!g) {
		return EXIT_FAILURE;
	}
	find_coords(g);
	save_to_file(g, output_f);
	free_graph(g);

	return EXIT_SUCCESS;
}
