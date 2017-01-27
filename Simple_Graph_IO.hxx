//
//
// C++ Interface for module: Simple_Graph_IO
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Simple_Graph_IO_Interface
#define Simple_Graph_IO_Interface

#include "General.hxx"

  // allows at most 65536-1 chars in a line
  template<class Weight, class Alloc>
    istream& Simple_Graph<Weight,Alloc>::load_metis(istream& input) {
    int num_vertices, num_edges;
    bool has_vertex_weight, has_edge_weight;
    char buffer[65536];
    int line_num = 0;
    int edges_read;
    int num_zero_vertices; // number of zero degree vertices

    CPU_Time start_time;

    while (!input.eof()) {
      input.getline(buffer, 65536);
      
      if ((!input) & (!input.eof())) // bug: iostream bug?
	throw Graph_Error(" load_metis: cannot read graph from stream");
      if (buffer[0] != '%' /*&& buffer[0]!=0*/) {	      // skip comment 
	//cout << buffer << endl;
	istrstream line(buffer, 65536);// bug:this sucks
	
	if (line_num==0) {
	  int mode;
	  line >> num_vertices >> num_edges >> mode; // strstream sux
	  //if (sscanf(buffer, "%d%d%d", &num_vertices, &num_edges, &mode)<2)
	  //  throw Graph_Error("File format faulty");
	  has_vertex_weight = (mode/10==1);
	  has_edge_weight = (mode%10==1);
	  initialize_vertices(num_vertices);
	  cout << "Loading Metis graph with "
	       << num_vertices << " vertices and "
	       << num_edges << " undirected edges ("
	       << num_edges*2 << " directed edges)" << endl;
	}
	else {
	  if ( (line_num>num_vertices) || (false) )
	    break;
	  //char* cursor;
	  int v = line_num - 1; //note:first_index==0
	  Weight vertex_weight;
	  unsigned int adjacent_vertex;
	  Weight edge_weight;
	  if (has_vertex_weight) {
	    line >> vertex_weight;
	    //if (sscanf(buffer, "%d", &vertex_weight)!=1)
	    //  throw Graph_Error("File format error");
	    vertex_weights[v] = vertex_weight;
	  }
	  else
	    vertex_weights[v] = 1;
	  list<Vertex_Index> tmp_adj;
	  list<Weight> tmp_edge_weight;
	  bool okay=true;
	  /*while (okay) {
	    if (sscanf(buffer, "%d", &adjacent_vertex)==1) {
	      tmp_adj.push_back(adjacent_vertex-1);//note:first_index==0
	      if (has_edge_weight) {
		if (sscanf(buffer, "%d", &edge_weight)==1)
		  tmp_edge_weight.push_back(edge_weight);
		else
		  throw Graph_Error("File format error");
	      }
	    }
	    else okay = false;
	    }*/
	  while ((line >> adjacent_vertex).good()) {
	    tmp_adj.push_back(adjacent_vertex-1);//note:first_index==0
	    if (has_edge_weight) {
	      line >> edge_weight;
	      tmp_edge_weight.push_back(edge_weight);
	    }
	  }
	  Adj_Vector & adj_v = adj_vectors[v];
	  int num_edges = tmp_adj.size();
	  adj_v.resize(num_edges);
	  edges_read += num_edges;
	  if (num_edges==0) {
	    //throw Graph_Error("File : Zero Degree vertex");
	    //cerr << "vertex " << v << " has zero degree " << endl;
	  }
	  copy(tmp_adj.begin(), tmp_adj.end(), adj_v.begin());
	  if (has_edge_weight) {
	    // code: edge weights
	  }
	  //L("vertex %d: %d edges\n", v, tmp_adj.size());
	}
	++line_num;
      }
      else {
	input.ignore(1<<20, '\n');
      }
    }
    int vertices_read = line_num - 1;
    L("%d vertices read\n", vertices_read);
    L("%d directed edges read\n", edges_read);
    if (edges_read!=num_edges*2)
      throw Graph_Error("File : Edge number mismatch");
    if (vertices_read!=num_vertices)
      throw Graph_Error("File : Vertex number mismatch");
    CPU_Time end_time;
    cerr << "load_graph: time elapsed: " << end_time-start_time << endl;
    return input;
  }

  template<class Weight, class Alloc>
    ostream& Simple_Graph<Weight,Alloc>::store_metis (ostream& output)
    const {
    int num_vertices, num_edges;
    bool has_vertex_weight, has_edge_weight;

    // first line
    num_vertices = sel_num_vertices();
    num_edges = 0;
    vector<Adj_Vector, Alloc>::const_iterator i = adj_vectors.begin(),
      i_end = adj_vectors.end();
    for (;i!=i_end;i++) num_edges += i->size();
    int mode = 10;
    //has_vertex_weight = has_edge_weight = true;
    // defaults
    has_vertex_weight = true;
    has_edge_weight = false;
    assert(num_edges % 2 == 0);
    output << num_vertices << ' ' << num_edges/2 << ' ' << mode << endl;

    // data
    i = adj_vectors.begin();
    vector<Weight,Alloc>::const_iterator j = vertex_weights.begin();
    for (; i!=i_end; i++,j++) {
      Weight edge_weight;
      if (has_vertex_weight)
	output << *j << ' ';
      vector<Vertex_Index, Alloc>::const_iterator k = i->begin(),
	k_end = i->end();
      for (;k!=k_end;k++) {
	output << (*k)+1 << " "; //note:first_index==0
	if (has_edge_weight) {
	  // code: edge weights
	}
      }
      output << endl;
    }
    return output;
  }
  
  template <class Weight, class Allocator> istream&
    operator>>(istream& is, Simple_Graph<Weight, Allocator>& g) {
    return g.load_metis(is);
  }

  template <class Weight, class Allocator> ostream&
    operator<<(ostream& os, const Simple_Graph<Weight, Allocator>& g) {
    return g.store_metis(os);
  }


#endif
