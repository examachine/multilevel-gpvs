//
//
// C++ Interface for module: Simple_Graph_Coarsening
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Simple_Graph_Coarsening_Interface
#define Simple_Graph_Coarsening_Interface

#include "General.hxx"

//code: namespaces

  const static unsigned int default_coarsening_threshold = 128;

  // multilevel coarsening algorithm
  // coarsens graph down below a required number of vertices
  // G0: graph to coarsen
  // threshold: maximum number of vertices to attain after operation
  template<class Weight, class Alloc>
    list<Simple_Graph<Weight,Alloc>*>
    Simple_Graph<Weight,Alloc>::coarsen_graph
    (unsigned int threshold = default_coarsening_threshold) {

    typedef Simple_Graph<Weight,Alloc> G;

    CPU_Time start_time;

    list<Simple_Graph<Weight,Alloc>*> L;
    G * Gcurr = this;
    L.push_back(Gcurr);
    int num_vertices, prev_num_vertices;
    num_vertices = Gcurr->sel_num_vertices();
    prev_num_vertices = num_vertices<<1;
    bool trapped = false;
    while ( num_vertices > threshold
	    && !trapped ){
      G * Gnew = &Gcurr->coarsening_step();
      L.push_back(Gnew);
      Gcurr = Gnew;
      if (double(num_vertices)/prev_num_vertices > 0.96)
	trapped = true;
      prev_num_vertices = num_vertices;
      num_vertices = Gcurr->sel_num_vertices();
    }

    cout << "coarsen_graph: time elapsed: " << CPU_Time()-start_time << endl;

    return L;
  }

  // a step of the coarsening algorithm
  template<class Weight, class Alloc>
    Simple_Graph<Weight,Alloc>&
    Simple_Graph<Weight,Alloc>::coarsening_step() {

    select_vertices();
    
    return compress_graph();
  }

  template<class Weight, class Alloc>
    void
    Simple_Graph<Weight,Alloc>::select_vertices() {

    CPU_Time start_time;

    CPU_Time start1a;
    // gather workspace
    unsigned int n = sel_num_vertices();
    if (n==0) {
      throw Graph_Error("select_vertices: graph has no vertices");
    }
    match.resize(n);
    map_imp.resize(n,-1);
    //map_exp.resize(n);
    //cout << "job1a: time elapsed: " << CPU_Time() - start1a << endl;

    CPU_Time start1b;
    vector<Vertex_Index> vertex_order(n);
    iota(match.begin(), match.end(), 0);
    iota(vertex_order.begin(), vertex_order.end(), 0);
    random_shuffle(vertex_order.begin(), vertex_order.end());
    Counter vertex_counter(0);
    //cerr << "vertex_order: " << vertex_order << endl;
    bool matched;
    // match vertices iteratively in a random fashion
    //do {
    //matched = false;
    //cout << "job1b: time elapsed: " << CPU_Time() - start1b << endl;


    CPU_Time start2;
    vector<Vertex_Index>::const_iterator i = vertex_order.begin(),
      i_end = vertex_order.end();
//      vector<Vertex_Index,Alloc>::iterator match_start = match.begin(),
//        match_u, match_v;
    for (; i!=i_end; i++) {
      Vertex_Index u = *i;
//        match_u = match_start + u;
//        if (*match_u==u) {
      if (match[u]==u) {	// is u unmatched?
	//cout << "match: " << match << endl;
	//cout << "map_imp: " << map_imp << endl;
	matched = true;
	Adj_Vector adj_u = adj_vectors[u];
	//random_shuffle(adj_u.begin(), adj_u.end());
	//cout << "permuted adj_vec("<< u <<"):" << adj_u << endl;
	bool neighbor_matched = false;
	Adj_Vector::const_iterator j=adj_u.begin(), j_end=adj_u.end();
	while ( (!neighbor_matched) && (j!=j_end)) {
	  Vertex_Index v = *(j++);
	  if (match[v]==v) {
	    match[u] = v;
	    match[v] = u;
	    map_imp[u] = map_imp[v] = vertex_counter();      
      	    neighbor_matched = true;
	  }
//  	  match_v = match_start + v;
//  	  if (*match_v==v) {
//  	    *match_u = v;
//  	    *match_v = u;
//  	    map_imp[u] = map_imp[v] = vertex_counter();
//  	    neighbor_matched = true;
//  	  }
	}
      }
    }
    //} while (!matched); // do
    //cout << "job2: time elapsed: " << CPU_Time() - start2 << endl;
    
    CPU_Time start3;
    // for all u in V(G), if vertex[u]==u then u is unmatched
    vector<Vertex_Index>::iterator k, k_end;
    k = map_imp.begin(), k_end = map_imp.end();
    for (; k!=k_end; k++)
      if (*k==-1)
	*k=vertex_counter();

    num_imploded_vertices = vertex_counter.check(); //note:first_index==0
    //cout << "job3: time elapsed: " << CPU_Time() - start3 << endl;

//      cout << "select_vertices: time elapsed: " << CPU_Time() - start_time
//  	 << endl;

    //cerr << "match: " << match << endl;
    //cerr << "map_imp: " << map_imp << endl;
  }

  template<class Weight, class Alloc>
    Simple_Graph<Weight,Alloc>&
    Simple_Graph<Weight,Alloc>::compress_graph () {
    
    typedef Simple_Graph<Weight,Alloc> Graph;

    // note:local class is not a valid template argument?
    class Mapper {
    public:
      Mapper() {}
      Vertex_Index operator () (Vertex_Index n) {
	return map_imp[n];
      }
    } mapper;

    CPU_Time start_time;

    Graph & Gcomp = *new Graph;;
    vector<Weight> T(sel_num_vertices(), 0); // work area
    queue<Vertex_Index> Q; // queue of vertices, for finding edges

    Gcomp.initialize_vertices(num_imploded_vertices, true);
    vector<Vertex_Index>::iterator i = match.begin(),
      i_end = match.end();
    for (;i!=i_end;i++) {	// for each element in match array
      // kludge: thw algorithm is a bit ad hoc, make it more
      // rigorous by defining the compression scheme in a more
      // definite manner, without resorting to marking with -1 ?

      // kludge: this is a bit counter-intuitive
      // handle it by obtaining u from incrementing
      // and v from *i
      Vertex_Index u = *i;
      if (u == -1) //{
	//cerr << "what the fuck!" << endl;
        continue; // no more processing
	//}
      Vertex_Index v = match[u];
      if (u!=v) match[u] = -1;
      Vertex_Index s = map_imp[u];
      Gcomp.map_exp[s] = v;

      Adj_Vector &Adj_u = adj_vectors[u], &Adj_v = adj_vectors[v];
      //cerr << "Adj_u: " << Adj_u << endl;
      //cerr << "Adj_v: " << Adj_v << endl;
      //note:following line doesn't work
      //transform(Adj_u.begin(), Adj_u.end(), Adj_s.begin(), mapper);
      Adj_Vector::iterator j = Adj_u.begin(), j_end = Adj_u.end();
      for (;j!=j_end;j++) {
	Vertex_Index u_j = map_imp[*j];
	if (u_j!=s) {
	  Weight & edge_weight = T[u_j];
	  if (edge_weight==0)
	    Q.push(u_j);
	  edge_weight += 1; // code: edge weights;
	}
      }
      j = Adj_v.begin(), j_end = Adj_v.end();
      for (;j!=j_end;j++) {
	Vertex_Index v_j = map_imp[*j];
	if (v_j!=s) {
	  Weight & edge_weight = T[v_j];
	  if (edge_weight==0)
	    Q.push(v_j);
	  edge_weight += 1; // code: edge weights;
	}
      }

      // write adjacency vector, clean T
      Gcomp.adj_vectors[s].resize(Q.size());
      Adj_Vector::iterator k = Gcomp.adj_vectors[s].begin(),
	k_end = Gcomp.adj_vectors[s].end();
      for (;k!=k_end;k++) {
	Vertex_Index t_k = Q.front();
	T[t_k] = 0;
	Q.pop();
	*k = t_k;
	//*(k++)=t_k;
      }

      // compute weight of compressed vertex
      if (u==v) {
	Gcomp.vertex_weights[s] = vertex_weights[u];
      }
      else
	Gcomp.vertex_weights[s] = vertex_weights[u] + vertex_weights[v];
    }

//      cerr << "compress_graph: time elapsed: " << CPU_Time()-start_time << endl;

    return Gcomp;
  }

#endif
