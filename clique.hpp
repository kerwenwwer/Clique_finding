
#ifndef MCQD
#define MCQD

#include <assert.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono;

class Vertex {
  int i, d;

 public:
  void set_i(const int ii) { i = ii; }
  int get_i() const { return i; }
  void set_degree(int dd) { d = dd; }
  int get_degree() const { return d; }
};

class ColorClass {
  int *i;
  int size;

 public:
#ifdef DBG
  void dbg_i(const string msg = "") const {
    std::cout << msg << " Class: [";
    for (int ii = 0; ii < size; ii++) std::cout << i[ii] << " ";
    std::cout << "]" << std::endl;
  }
#endif
  ColorClass() : size(0), i(0) {}
  ColorClass(const int size) : size(size), i(0) { init(size); }
  ~ColorClass() {
    if (i) delete[] i;
  }
  void init(const int size) {
    i = new int[size];
    rewind();
  }
  void push(const int ii) { i[size++] = ii; };
  void pop() { size--; };
  void rewind() { size = 0; }
  int get_size() const { return size; }
  int &at(const int ii) const { return i[ii]; }
  ColorClass &operator=(const ColorClass &dh) {
    for (int j = 0; j < dh.size; j++) i[j] = dh.i[j];
    size = dh.size;
    return *this;
  }
};

class StepCount {
  int i1, i2;

 public:
  StepCount() : i1(0), i2(0) {}

  // Contral unit
  void set_i1(const int ii) { i1 = ii; }
  void set_i2(const int ii) { i2 = ii; }

  int get_i1() const { return i1; }
  int get_i2() const { return i2; }

  // use to inc i1's value
  void inc_i1() { i1++; }
};

class MaxClique {
  const bool *const *e;
  int pk, level;
  const float Tlimit;
  class Vertices {
    Vertex *v;
    int size_ver;
    static bool desc_degree(const Vertex vi, const Vertex vj) {
      return (vi.get_degree() > vj.get_degree());
    }

   public:
    Vertices(int size) : size_ver(0) { v = new Vertex[size]; }
    ~Vertices() {}
    void dispose() {
      if (v) delete[] v;
    }
    void sort() { std::sort(v, v + size_ver, desc_degree); }
    void init_colors();
    void set_degrees(MaxClique &);
    int get_size() const { return size_ver; }
    void push(const int ii) { v[size_ver++].set_i(ii); };
    void pop() { size_ver--; };
    Vertex &at(const int ii) const { return v[ii]; };
    Vertex &end() const { return v[size_ver - 1]; };
  };

  Vertices V;
  ColorClass *C, QMAX, Q;
  StepCount *S;

  bool mapection(const int i, const int j) const { return e[i][j]; }
  bool cut1(const int, const ColorClass &);
  void cut2(const Vertices &, Vertices &);
  void color_sort(Vertices &);
  void expand_dyn(Vertices);
  void _find_max_clique(int *&, int &);
  void degree_sort(Vertices &R) {
    R.set_degrees(*this);
    R.sort();
  }

 public:
  MaxClique(const bool *const *map, const int size, const float t_l)
      : pk(0), level(1), Tlimit(t_l), V(size), Q(size), QMAX(size) {
    assert(map != 0 && size > 0);
    for (int i = 0; i < size; i++) V.push(i);
    e = map;
    C = new ColorClass[size + 1];
    for (int i = 0; i < size + 1; i++) C[i].init(size + 1);
    S = new StepCount[size + 1];
  }
  int steps() const { return pk; }
  void mcqdyn(int *&maxclique, int &size) { _find_max_clique(maxclique, size); }
  ~MaxClique() {
    if (C) delete[] C;
    if (S) delete[] S;
    V.dispose();
  };
};

void MaxClique::_find_max_clique(int *&maxclique, int &size) {
  V.set_degrees(*this);
  V.sort();
  V.init_colors();

  for (int i = 0; i < V.get_size() + 1; i++) {
    S[i].set_i1(0);
    S[i].set_i2(0);
  }

  expand_dyn(V);

  maxclique = new int[QMAX.get_size()];
  for (int i = 0; i < QMAX.get_size(); i++) {
    maxclique[i] = QMAX.at(i);
  }
  size = QMAX.get_size();
}

void MaxClique::Vertices::init_colors() {
  const int max_degree = v[0].get_degree();
  for (int i = 0; i < max_degree; i++) v[i].set_degree(i + 1);
  for (int i = max_degree; i < size_ver; i++) v[i].set_degree(max_degree + 1);
}

void MaxClique::Vertices::set_degrees(MaxClique &m) {
  for (int i = 0; i < size_ver; i++) {
    int d = 0;
    for (int j = 0; j < size_ver; j++)
      if (m.mapection(v[i].get_i(), v[j].get_i())) d++;
    v[i].set_degree(d);
  }
}

bool MaxClique::cut1(const int pi, const ColorClass &A) {
  for (int i = 0; i < A.get_size(); i++)
    if (mapection(pi, A.at(i))) return true;
  return false;
}

void MaxClique::cut2(const Vertices &A, Vertices &B) {
  for (int i = 0; i < A.get_size() - 1; i++) {
    if (mapection(A.end().get_i(), A.at(i).get_i())) B.push(A.at(i).get_i());
  }
}

void MaxClique::color_sort(Vertices &R) {
  int j = 0;
  int maxno = 1;
  int min_k = QMAX.get_size() - Q.get_size() + 1;
  C[1].rewind();
  C[2].rewind();
  int k = 1;
  for (int i = 0; i < R.get_size(); i++) {
    int pi = R.at(i).get_i();
    k = 1;
    while (cut1(pi, C[k])) k++;
    if (k > maxno) {
      maxno = k;
      C[maxno + 1].rewind();
    }
    C[k].push(pi);
    if (k < min_k) {
      R.at(j++).set_i(pi);
    }
  }
  if (j > 0) R.at(j - 1).set_degree(0);
  if (min_k <= 0) min_k = 1;
  for (k = min_k; k <= maxno; k++)
    for (int i = 0; i < C[k].get_size(); i++) {
      R.at(j).set_i(C[k].at(i));
      R.at(j++).set_degree(k);
    }
}

void MaxClique::expand_dyn(Vertices R) {
  S[level].set_i1(S[level].get_i1() + S[level - 1].get_i1() -
                  S[level].get_i2());
  S[level].set_i2(S[level - 1].get_i1());

  while (R.get_size()) {
    if (Q.get_size() + R.end().get_degree() > QMAX.get_size()) {
      Q.push(R.end().get_i());
      Vertices Rp(R.get_size());
      cut2(R, Rp);

      if (Rp.get_size()) {
        if ((float)S[level].get_i1() / ++pk < Tlimit) {
          degree_sort(Rp);
        }
        color_sort(Rp);
        S[level].inc_i1();
        level++;
        expand_dyn(Rp);
        level--;
      } else if (Q.get_size() > QMAX.get_size()) {
        std::cout << "step = " << pk
                  << " current max. clique size = " << Q.get_size()
                  << std::endl;
        QMAX = Q;
      }
      Rp.dispose();
      Q.pop();
    } else {
      return;
    }
    R.pop();
  }
}

#endif
