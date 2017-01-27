//
//
// C++ Interface for module: Graph_Test
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Graph_Test_Interface
#define Graph_Test_Interface

#include "General.hxx"

#include "Simple_Graph.hxx"

//  namespace Graph_Lib {

//    using namespace Allocators;

  class Test {
  public:
    typedef Simple_Graph<int> Graph;

    // test custom allocators
    void allocator() {
//       vector<int, Hullavi> v;
//       v.push_back(1);
//       v.push_back(2);
//       v.push_back(3);
//       cout << "size of v is " << v.size() << endl;
//       cout << "v: < ";
//       vector<int, Hullavi>::iterator a = v.begin(), b = v.end();
//       for (;a!=b; ++a) {
// 	cout << *a << ' ';
//       }
//       cout << '>' << endl;
//       v.erase(v.begin(), v.end());
//       cout << "after erasing, size of v is " << v.size() << endl;
    };

    void bisection(Graph & g, string & filename) {
      //g.store_metis(cout);
      //cout << g;
      //L("Select Vertices:\n");
      //g.select_vertices();
      //L("Compress Graph:\n");
      list<Graph*> graph_list = g.coarsen_graph();
      cout << "number of coarsening levels: " << graph_list.size() << endl;
      ofstream coarsest_graph_file((filename+".coarsest").c_str());
      Graph& coarsest_graph = *(graph_list.back());
      coarsest_graph_file << coarsest_graph;
      cout << "number of supervertices of coarsest graph: "
	   << coarsest_graph.sel_num_vertices() << endl;
      cout << coarsest_graph;
      coarsest_graph.trivial_bisection();
      cout << "size of coarsest graph: "
	   << coarsest_graph.partition.sel_total_size() << endl;
      coarsest_graph.VFM_refinement(); 
      coarsest_graph.partition.print();
      list<Graph*>::iterator i = graph_list.end();
      i--; i--;
      Graph & finer_graph = **i;
      cout << "finer graph:" << endl;
      cout << "------------" << endl;
      cout << finer_graph;
      finer_graph.explode(coarsest_graph);
      finer_graph.partition.print();
      finer_graph.VFM_refinement();
      finer_graph.partition.print();

    }

  };
//  }

#endif
