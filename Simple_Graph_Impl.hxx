//
//
// C++ Interface for module: Simple_Graph_Impl
//
// Description: Common functions go in here
//
//
// Author: exa
//
//

#ifndef Simple_Graph_Impl_Interface
#define Simple_Graph_Impl_Interface

#include "General.hxx"

// perform multilevel bisection
//.uses RM coarsening, trivial bisection and VFM refinement as default
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::bisection
(string & filename,
 int coarsening_threshold = default_coarsening_threshold,
 double required_balance = default_required_balance,
 int max_nongain_moves = default_max_nongain_moves)
{
  CPU_Time start_time;

  srand(time(0));
  list<Simple_Graph*> graph_list = coarsen_graph(coarsening_threshold);
  cout << "number of coarsening levels: " << graph_list.size() << endl;
  ofstream coarsest_graph_file((filename+".coarsest").c_str());
  Simple_Graph& coarsest_graph = *(graph_list.back());
  coarsest_graph_file << coarsest_graph;
  cout << "number of supervertices of coarsest graph: "
       << coarsest_graph.sel_num_vertices() << endl;
  coarsest_graph.trivial_bisection();
  //coarsest_graph.levelized_bisection();
  //coarsest_graph.growing_bisection();
  //cout << "size of coarsest graph: "
  //     << coarsest_graph.partition.sel_total_size() << endl;
  coarsest_graph.VFM_refinement(required_balance, max_nongain_moves);
  uncoarsen_graph(graph_list);
  partition.print_quality();
  cout << "bisection: total time elapsed: " << CPU_Time()-start_time << endl;

  ofstream partition_file((filename+".bisection").c_str());
  partition.print(partition_file);

}

// do multilevel uncoarsening
// precondition L: output of coarsen_graph method of this object
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::uncoarsen_graph
(list<Simple_Graph<Weight,Alloc>*> L)
{
  typedef Simple_Graph<Weight,Alloc> G;
  typedef list<Simple_Graph<Weight,Alloc>*> List;

  CPU_Time start_time;

  List::iterator l_it = L.end(), l_begin = L.begin(), l2_it;
  for (--l_it; l_it!=l_begin; --l_it) {
    G & coarse_graph = **l_it;
    l2_it = l_it; --l2_it;
    G & finer_graph = **l2_it;
    finer_graph.explode(coarse_graph);
    finer_graph.VFM_refinement();
  }

  cout << "uncoarsen_graph: time elapsed: " << CPU_Time()-start_time << endl;
  
}

// project all info from the coarse graph
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::explode
(Simple_Graph<Weight,Alloc>& coarse) {

  //code: this code fragment sucks altogether
  // we need to contrive iterators that work with
  // parallel arrays, this way of doing it is too tedious
  //partition.explode(coarse.partition);
  vector<Vertex_Location>::iterator
    i=coarse.partition.vertex_location.begin(),
    i2=coarse.partition.vertex_location.end();
  for (Vertex_Index s = 0;i!=i2;i++,s++) {
    Vertex_Index u = coarse.map_exp[s];
    Vertex_Index v = match[u];
    assert (v!=-1); // v must not be erased...
    partition.insert(u, vertex_weights[u], *i);
    if (u!=v)
      partition.insert(v, vertex_weights[v], *i);
  }  
}

#endif
