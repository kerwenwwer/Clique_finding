
#include <algorithm>
#include <vector>

class Kcore {
 private:
  int k_core;
  int size;
  const bool *const *edge;
  vector<pair<int, int>> degree;
  vector<int> kmax;
  vector<int> num;
  vector<int> adj;

 public:
  Kcore(const bool *const *map, const int sz, int k) : size(sz), k_core(k) {
    edge = map;
    num.resize(size + 1, 0);
  }
  void init_k();
  void _k_core(vector<int> &wa, int &size);
};

void Kcore::init_k() {
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (edge[i][j] && edge[j][i]) {
        num[i]++;
        num[j]++;
      }
    }
  }
  vector<int> temp_num_edges(size + 1);
  temp_num_edges[0] = 0;
  int m = 0;
  for (int i = 0; i < size; ++i) {
    m += num[i];
    temp_num_edges[i + 1] = m;
  }
  adj.resize(m);
  for (int i = 0; i < size + 1; ++i) {
    num[i] = temp_num_edges[i];
  }

  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      if (edge[i][j]) {
        adj[temp_num_edges[i]++] = j;
        adj[temp_num_edges[j]++] = i;
      }
    }
  }

  for (int i = 0; i < size; ++i) {
    sort(adj.begin() + num[i], adj.begin() + num[i + 1],
         [](const int &lhs, const int &rhs) { return lhs > rhs; });
  }
}

void Kcore::_k_core(vector<int> &core, int &ksize) {
  init_k();
  core.resize(size, 0);
  // vector<int> core(size, 0);

  vector<int> vert(size, 0);
  vector<int> pos(size, 0);

  // Maximum degree
  int maxDeg = 0;

  // deg[i] -- contains degree of size i
  for (long i = 0; i < size; i++) {
    core[i] = num[i + 1] - num[i];

    if (core[i] > maxDeg) maxDeg = core[i];
  }

  // Used for bin-sort -- is of size maxDeg + 1
  vector<int> bin(maxDeg + 1);

  for (long i = 0; i < size; i++) bin[core[i]]++;

  unsigned int start = 0;
  for (int d = 0; d < maxDeg + 1; d++) {
    unsigned int num = bin[d];
    bin[d] = start;  // Start index of size in vert with degree d
    start = start + num;
  }

  for (long i = 0; i < size; i++) {
    pos[i] = bin[core[i]];
    vert[pos[i]] = i;
    bin[core[i]]++;
  }

  for (int d = maxDeg; d >= 1; d--) bin[d] = bin[d - 1];
  bin[0] = 0;

  // kcores computation
  for (long i = 0; i < size; i++) {
    // Process the vertices in increasing order of degree
    int v = vert[i];
    for (int j = num[v]; j < num[v + 1]; j++) {
      // TODO;
      int u = adj[j];
      if (core[u] > core[v]) {
        // Swap u with the first size in bin[du]
        unsigned int du = core[u];
        unsigned int pu = pos[u];
        unsigned int pw = bin[du];
        unsigned int w = vert[pw];

        if (u != w) {
          pos[u] = pw;
          vert[pu] = w;
          pos[w] = pu;
          vert[pw] = u;
        }

        bin[du]++;

        core[u]--;
      }
    }
  }

  ofstream kout("k-core.txt");
  printf("K-core:\n");
  for (int i = 0; i < core.size(); i++) {
#ifdef DEBUG
    printf("%d %d\n", i, core[i]);
#endif
    kout << i << " " << core[i] << '\n';
  }

  kout.close();
  // print ans
  vert.clear();
  pos.clear();
}