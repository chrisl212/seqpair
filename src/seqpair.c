#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "seqpair.h"

int *_index_table(int *a, int len) {
	int *table = malloc(sizeof(*table) * (len+1));
	for (int i = 0; i < len; i++) {
		table[a[i]] = i;
	}
	return table;
}

void _make_connections(Graph *g, int *seq1, int *seq2) {
	Vertex **v = g->v;
	int len = g->v_len;

	int *table1 = _index_table(seq1, len-1);
	int *table2 = _index_table(seq2, len-1);
	for (int i = 1; i < len; i++) {
		int idx1 = table1[v[i]->label]; 
		int idx2 = table2[v[i]->label];
	
		for (int j = idx2+1, k = 0; j < len - 1; j++, k++) {
			if (table1[seq2[j]] > idx1) {
				int cnt = ++(v[i]->hadj_cnt);
				v[i]->h_adj = realloc(v[i]->h_adj, cnt*sizeof(*(v[i]->h_adj)));
				v[i]->h_adj[cnt-1] = v[seq2[j]];
			}
			if (table1[seq2[j]] < idx1) {
				//above
				int cnt = ++(v[i]->vadj_cnt);
				v[i]->v_adj = realloc(v[i]->v_adj, cnt*sizeof(*(v[i]->v_adj)));
				v[i]->v_adj[cnt-1] = v[seq2[j]];
			}
		}
	}
	free(table1);
	free(table2);
}

Graph *load_from_file(char *fname) {
	FILE *f = fopen(fname, "r");
	if (!f) {
		return NULL;
	}

	int cnt;
	fscanf(f, "%d", &cnt);
	int len = cnt + 1;
	Vertex **v = calloc(1, sizeof(*v) * len);
	v[0] = calloc(1, sizeof(*(v[0])));
	v[0]->dx = v[0]->dy = 0;
	v[0]->hadj_cnt = v[0]->vadj_cnt = cnt;
	v[0]->h_adj = calloc(1, sizeof(*(v[0]->h_adj)) * cnt);
	v[0]->v_adj = calloc(1, sizeof(*(v[0]->v_adj)) * cnt);
	
	for (int i = 1; i < cnt+1; i++) {
		double width, height;
		int label;
		fscanf(f, "%d(%le,%le)", &label, &width, &height);
		v[i] = calloc(1, sizeof(*(v[0])));
		v[i]->width = width;
		v[i]->height = height;
		v[i]->label = label;
		v[i]->dx = v[i]->dy = 0;
		v[0]->h_adj[i-1] = v[i];
		v[0]->v_adj[i-1] = v[i];
	}
	int *seq1 = malloc(sizeof(*seq1) * cnt);
	int *seq2 = malloc(sizeof(*seq2) * cnt);
	for (int i = 0; i < cnt; i++) {
		fscanf(f, "%d", &seq1[i]);
	}
	for (int i = 0; i < cnt; i++) {
		fscanf(f, "%d", &seq2[i]);
	}
	Graph *g = calloc(1, sizeof(*g));
	g->v = v;
	g->v_len = len;
	_make_connections(g, seq1, seq2);
	
	fclose(f);
	free(seq1);
	free(seq2);

	return g;
}

Stack *_init_stack() {
	Stack *s = calloc(1,sizeof(*s));
	return s;
}

void _push(Stack *s, Vertex *v) {
	s->len++;
	s->arr = realloc(s->arr, sizeof(*(s->arr)) * s->len);
	for (int i = s->len - 1; i > 0; i--) {
		s->arr[i] = s->arr[i-1];
	}
	s->arr[0] = v;
}

Vertex *_pop(Stack *s) {
	if (s->len < 1) {
		return NULL;
	}
	Vertex *v = s->arr[0];
	s->len--;
	for (int i = 0; i < s->len; i++) {
		s->arr[i] = s->arr[i+1];
	}
	s->arr = realloc(s->arr, sizeof(*(s->arr)) * s->len);
	return v;
}

int _empty(Stack *s) {
	return (s->len == 0);
}

void _topo_sort(Vertex *v, Stack *s, int h) {
	v->color = BLACK;

	for (int i = 0; i < ((h) ? v->hadj_cnt : v->vadj_cnt); i++) {
		Vertex *u = (h) ? v->h_adj[i] : v->v_adj[i];
		if (u->color == WHITE) {
			_topo_sort(u, s, h);
		}
	}
	_push(s, v);
}

void find_coords(Graph *g) {
	Stack *sh = _init_stack();
	Stack *sv = _init_stack();
	Vertex **v = g->v;
	for (int i = 0; i < g->v_len; i++) {
		if (v[i]->color == WHITE) {
			_topo_sort(v[i], sh, 1);
		}
		if (i != 0) {
			v[i]->dx = v[i]->dy = -INT_MAX;
		}
	}
	for (int i = 0; i < g->v_len; i++) {
		v[i]->color = WHITE;
	}
	for (int i = 0; i < g->v_len; i++) {
		if (v[i]->color == WHITE) {
			_topo_sort(v[i], sv, 0);
		}
	}

	while (!_empty(sh)) {
		Vertex *u = _pop(sh);
		for (int i = 0; i < u->hadj_cnt; i++) {
			Vertex *x = u->h_adj[i];
			if (x->dx <= u->dx + u->width) {
				x->dx = u->dx + u->width;
			}
		}
	}
	while (!_empty(sv)) {
		Vertex *u = _pop(sv);
		for (int i = 0; i < u->vadj_cnt; i++) {
			Vertex *x = u->v_adj[i];
			if (x->dy <= u->dy + u->height) {
				x->dy = u->dy + u->height;
			}
		}
	}
	free(sv);
	free(sh);
}

void save_to_file(Graph *g, char *fname) {
	FILE *f = fopen(fname, "w");
	if (!f) {
		return;
	}

	Vertex **v = g->v;
	int len = g->v_len;
	for (int i = 1; i < len; i++) {
		fprintf(f, "%d(%le,%le)\n", v[i]->label, (double)v[i]->dx, (double)v[i]->dy);
	}
	fclose(f);
}

void free_graph(Graph *g) {
	for (int i = 0; i < g->v_len; i++) {
		free(g->v[i]->v_adj);
		free(g->v[i]->h_adj);
		free(g->v[i]);
	}
	free(g->v);
	free(g);
}
