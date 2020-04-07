#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <string>

#include "clique.hpp"
#include "kcore.hpp"

#define MAXE 1000000000000

extern char *optarg;
extern int optind, opterr, optopt;

using namespace std;

void read_the_file(char *filename, bool **&com, int &size) {
  // ifstream input(filename);
  freopen(filename, "r", stdin);
  // error detect
  // assert(input.is_open());

  set<int> v;
  multimap<int, int> edge;
  int v1, v2;
  while (scanf("%d%d", &v1, &v2) != EOF) {
    v.insert(v1);
    v.insert(v2);
    edge.insert(make_pair(v1, v2));
  }

  size = *v.rbegin() + 1;
  com = new bool *[size];

  for (int i = 0; i < size; i++) {
    com[i] = new bool[size];
    memset(com[i], 0, size * sizeof(bool));
  }
  for (auto itr = edge.begin(); itr != edge.end(); itr++) {
    com[itr->first][itr->second] = com[itr->second][itr->first] = true;
  }
  cout << "|E| = " << edge.size() << "  |V| = " << v.size()
       << " p = " << (double)edge.size() / (v.size() * (v.size() - 1) / 2)
       << '\n';
  // input.close();
}

int main(int argc, char **argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);

  bool **com;
  int size, k;
  int *qmax, qsize;
  vector<int> kmax;
  int ksize;
  char *inputfile_name;
  int opt;
  while ((opt = getopt(argc, argv, "f:k:")) != -1) {
    switch (opt) {
      case 'f':
        inputfile_name = optarg;
        break;
      case 'k':
        if (isdigit(*optarg)) {
          k = atoi(optarg);
        } else {
          cout << "Please use digit number." << endl;
          exit(1);
        }
        break;
      default:
        fprintf(stderr, "Usage: %s [-f] input file name [-k] k number.\n",
                argv[0]);
        exit(1);
    }
  }

  printf("Start read file....\n");

  read_the_file(inputfile_name, com, size);

  printf("Start clique finding...\n");

  MaxClique md(com, size, 0.025);
  Kcore kcore(com, size, k);

  thread t1(&MaxClique::mcqdyn, &md, ref(qmax), ref(qsize));
  thread t2(&Kcore::_k_core, &kcore, ref(kmax), ref(ksize));
  // md.mcqdyn(qmax, qsize);u
  t1.join();
  t2.join();
  sort(qmax, qmax + qsize);

  ofstream clique("clique.txt");

  if (!clique) {
    printf("I/O Format error.\n");
    return 1;
  }
  printf("qsize is: %d\n", qsize);
  printf("Max Clique is: \n");
  for (int i = 0; i < qsize; i++) {
    cout << qmax[i] << " ";
    clique << qmax[i] << '\n';
  }
  putchar('\n');

  clique.close();

  // kmax.~vector();
  delete[] qmax;
  for (int i = 0; i < size; i++) delete[] com[i];
  delete[] com;
  return 0;
}
