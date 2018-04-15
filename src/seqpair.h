#ifndef SEQPAIR_H
#define SEQPAIR_H

#define WHITE 0
#define GRAY 1
#define BLACK 2

typedef struct _Vertex {
	int label;
	int color;
	double width, height;
	double x,y;
	int dx;
	int dy;
	struct _Vertex **h_adj;
	struct _Vertex **v_adj;
	int hadj_cnt, vadj_cnt;
} Vertex;

typedef struct _Graph {
	Vertex **v;
	int v_len;
} Graph;


typedef struct _Q {
	Vertex **arr;
	int len;
} Stack;

Graph *load_from_file(char *fname);
void find_coords(Graph *g);
void save_to_file(Graph *g, char *fname);
void free_graph(Graph *g);

#endif
