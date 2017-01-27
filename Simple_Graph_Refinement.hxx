//
//
// C++ Interface for module: Simple_Graph_Refinement
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Simple_Graph_Refinement_Interface
#define Simple_Graph_Refinement_Interface

#include "General.hxx"

//namespace Graph {

const static double default_required_balance = 0.8;
const static int default_max_nongain_moves = 64;

// VFM refinement
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::VFM_refinement
(double required_balance = default_required_balance,
 int max_nongain_moves = default_max_nongain_moves) {
  CPU_Time start_time;

  // explicitly compute all degreees & gains within the separator
  compute_all_degrees();
  compute_all_gains();


  Weight best_separator_size, current_separator_size;
  Partition best_partition = partition;

  best_separator_size = partition.sel_separator_size();

  dgn3(cerr << "separator size initially: " << best_separator_size << endl);
  dgn3(partition.print());
  dgn3(left_gain_info.print());
  dgn3(right_gain_info.print());

  bool done = false;


  dgn3(cout << "-------------------------------------" << endl);

  do {
    //locked_vertices.erase(locked_vertices.begin(), locked_vertices.end());

    current_separator_size = VFM_iteration(required_balance,
					   max_nongain_moves);
    dgn3(cerr << "partition after VFM(1): "; partition.print());
    if (current_separator_size < best_separator_size) {
      best_separator_size = current_separator_size;
      // copy best partition
      best_partition = partition;
    }
    else {
      done = true;
      // revert to best partition
      //if (!best_partition.empty())
      partition = best_partition;
    }

    //kludge: just need to update
    dgn3(cerr << ">> compute all degrees & gains " << endl);
    compute_all_degrees();
    compute_all_gains();
    dgn3(partition.print());
    dgn3(left_gain_info.print());
    dgn3(right_gain_info.print());
  }
  while (!done);


  dgn3(cerr << "partition after VFM refinement: ");
  dgn3(partition.print());

  dgn3(cerr << "VFM_refinement: time elapsed: " << CPU_Time() - start_time << endl);

}


// compute degree of each vertex in the separator
// kludge: we compute for all vertices in the graph for now.
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::compute_all_degrees() {
  Degrees & degrees = partition.degrees;
  hash_set<Vertex_Index>::iterator sep_i = partition.vertex_separator.begin(),
    sep_ie = partition.vertex_separator.end();
  Degrees::iterator degrees_i = degrees.begin(), degrees_ie = degrees.end();
  //for (;sep_i!=sep_ie;sep_i++,degrees_i++) {
  //Vertex_Index u = *sep_i;
  for (Vertex_Index u = 0; u < sel_num_vertices(); u++, degrees_i++) {
    Adj_Vector::iterator adj_u_i = adj_vectors[u].begin(),
      adj_u_ie = adj_vectors[u].end();
    degrees_i->reset_degree();
    for (;adj_u_i!=adj_u_ie;adj_u_i++) {
      Vertex_Index v = *adj_u_i;
      degrees_i->add_degree(partition.sel_vertex_location(v),
			    vertex_weights[v]);
    }
  }

  /*
  cerr << "degrees: ";
  for (degrees_i=degrees.begin(); degrees_i!=degrees_ie; degrees_i++)
    cerr << "(" << degrees_i->left_degree <<"," <<
      degrees_i->right_degree << "):";
  cerr << endl;
  */
}

// compute gains of each vertex in the separator
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::compute_all_gains() {
  left_gain_info.initialize();
  right_gain_info.initialize();
  hash_set<Vertex_Index>::const_iterator
    ns_i = partition.vertex_separator.begin(),
    ns_ie = partition.vertex_separator.end();
  // for all u \in separator
  for (; ns_i!=ns_ie; ns_i++) {
    // compute gain(u,X)
    Vertex_Index v = *ns_i;
    Weight g_v_left = vertex_weights[v] - partition.degrees[v].right_degree;
    left_gain_info.insert(v, g_v_left);
    // compute gain(u,Y)
    Weight g_v_right = vertex_weights[v] - partition.degrees[v].left_degree;
    right_gain_info.insert(v, g_v_right);
  }
}

// compute gains of vertex u
// correct this
template<class Weight, class Alloc>
void Simple_Graph<Weight,Alloc>::compute_gain(Vertex_Index u) {
  Degrees & degrees = partition.degrees;
  Adj_Vector::iterator adj_u_i = adj_vectors[u].begin(),
    adj_u_ie = adj_vectors[u].end();
  degrees[u].reset_degree();
  for (;adj_u_i!=adj_u_ie;adj_u_i++) {
    Vertex_Index v = *adj_u_i;
    degrees[u].add_degree(partition.sel_vertex_location(v),
			   vertex_weights[v]);
  }
  Weight g_u_left = vertex_weights[u] - partition.degrees[u].right_degree;
  left_gain_info.insert(u, g_u_left);
  Weight g_u_right = vertex_weights[u] - partition.degrees[u].left_degree;
  right_gain_info.insert(u, g_u_right);
}

// single VFM iteration
// the VFM(1) algorithm
template<class Weight, class Alloc>
Weight Simple_Graph<Weight,Alloc>::VFM_iteration(double required_balance,
						 int max_non_gain_moves) {
  int non_gain_moves = 0;
  double balance = partition.balance();
  bool moves_depleted = false;

  // first attain balance
  while (partition.balance() < required_balance && !moves_depleted) {
    VFM_Result r = VFM_move_for_balance();
    if (r==negative)
      ++non_gain_moves;
    moves_depleted = r==depleted;
    dgn3(partition.print());
    dgn3(left_gain_info.print());
    dgn3(right_gain_info.print());
  }

  // then do iteration
  while (partition.balance() >= required_balance
	 && non_gain_moves <= max_non_gain_moves
	 && !moves_depleted) {
    VFM_Result r = VFM_move_for_gain();
    if (r==negative)
      ++non_gain_moves;
    moves_depleted = r==depleted;
    dgn3(partition.print());
    dgn3(left_gain_info.print());
    dgn3(right_gain_info.print());
  }

  return partition.sel_separator_size();
}

template<class Weight, class Alloc>
//bool
Simple_Graph<Weight,Alloc>::VFM_Result
Simple_Graph<Weight,Alloc>::VFM_move_for_balance() {
  if (partition.sel_left_size() < partition.sel_right_size()) {
    if (left_gain_info.empty())
      return depleted;
    Weight gain = left_gain_info.sel_max_gain();
    Vertex_Index v = left_gain_info.extract_top_vertex();
    return VFM_move_to_left(v, gain);
  }
  else {
    if (right_gain_info.empty())
      return depleted;
    Weight gain = right_gain_info.sel_max_gain();
    Vertex_Index v = right_gain_info.extract_top_vertex();
    return VFM_move_to_right(v, gain);
  }
}

template<class Weight, class Alloc>
//bool
Simple_Graph<Weight,Alloc>::VFM_Result
Simple_Graph<Weight,Alloc>::VFM_move_for_gain() {
  bool left_empty = left_gain_info.empty();
  bool right_empty = right_gain_info.empty();
  if (left_empty && right_empty)
    return depleted;
  else {
    Weight left_gain = left_gain_info.sel_max_gain(),
      right_gain = right_gain_info.sel_max_gain();
    if (left_gain > right_gain && !left_empty) {
      Vertex_Index v = left_gain_info.extract_top_vertex();
      return VFM_move_to_left(v, left_gain);
    }
    else if (right_empty)
	return depleted;
    else {
      Vertex_Index v = right_gain_info.extract_top_vertex();
      return VFM_move_to_right(v, right_gain);
    }
  }
}

template<class Weight, class Alloc>
//bool
Simple_Graph<Weight,Alloc>::VFM_Result
Simple_Graph<Weight,Alloc>::VFM_move_to_left(Vertex_Index v,
					     Weight gain) {
  // move to left partition
  assert(partition.sel_vertex_location(v) == separator);
  dgn3( cerr << "** move vertex " << v << " to left " << endl);
  /*if (locked_vertices.find(v)!=locked_vertices.end()) {
    if (left_gain_info.empty())
      return depleted;
    else {
      Weight gain = left_gain_info.sel_max_gain();
      Vertex_Index v = left_gain_info.extract_top_vertex();
      return VFM_move_to_left(v,gain);
    }
    }*/
  right_gain_info.remove_vertex(v); // lock vertex v
  partition.move_to_left(v, vertex_weights[v]);
  //locked_vertices.insert(v);
  Adj_Vector::iterator i = adj_vectors[v].begin(),
    ie = adj_vectors[v].end();
  for (;i!=ie;i++) {
    Vertex_Index y = *i;
    if (partition.sel_vertex_location(y)==right)
      partition.move_from_right(y, vertex_weights[y]);
    //locked_vertices.insert(y);
    //compute_gain(y);
  }
  return (gain>=0 ? positive : negative) ;
}

template<class Weight, class Alloc>
//bool
Simple_Graph<Weight,Alloc>::VFM_Result
Simple_Graph<Weight,Alloc>::VFM_move_to_right(Vertex_Index v,
					      Weight gain) {
  // move to right partition
  assert(partition.sel_vertex_location(v) == separator);
  dgn3( cerr << "** move vertex " << v << " to right" << endl);
  /*if (locked_vertices.find(v)!=locked_vertices.end()) {
    if (right_gain_info.empty())
      return depleted;
    else {
      Weight gain = right_gain_info.sel_max_gain();
      Vertex_Index v = right_gain_info.extract_top_vertex();
      return VFM_move_to_right(v,gain);
    }
    }*/
  left_gain_info.remove_vertex(v); // lock vertex v
  partition.move_to_right(v, vertex_weights[v]);
  //locked_vertices.insert(v);
  Adj_Vector::iterator i = adj_vectors[v].begin(),
    ie = adj_vectors[v].end();
  for (;i!=ie;i++) {
    Vertex_Index x = *i;
    if (partition.sel_vertex_location(x)==left)
      partition.move_from_left(x, vertex_weights[x]);
    //locked_vertices.insert(x);
    //    compute_gain(x);
  }
  return (gain>=0 ? positive : negative);
}

// generic move algorithm
// code: do it with the template specialization ;)
//  template<class Weight, class Alloc>
//  Weight Simple_Graph<Weight,Alloc>::VFM_move(Gain_Info & gain_info) {
//    //move to partition
//    Vertex_Index v = gain_info.extract_top_vertex();
//    return 
//  }

//} // namespace
#endif
