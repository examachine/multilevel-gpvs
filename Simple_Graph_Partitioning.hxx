//
//
// C++ Interface for module: Simple_Graph_Partitioning
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Simple_Graph_Partitioning_Interface
#define Simple_Graph_Partitioning_Interface

#include "General.hxx"

template<class Weight, class Alloc>
//    Simple_Graph<Weight,Alloc>::Node_Location_Vec
void
Simple_Graph<Weight,Alloc>::trivial_bisection() {
  partition.initialize(sel_num_vertices(), sel_num_vertices());
  int v;
  for (v=0; v<sel_num_vertices();v++)
    partition.insert(v, vertex_weights[v], separator);
}

  template<class Weight, class Alloc>
  void
  Simple_Graph<Weight,Alloc>::growing_bisection() {
    int best_size = sel_num_vertices();
    for (Vertex_Index v=0; v<sel_num_vertices();v++) {
      Partition* p = new Partition;
     grow_separator(v,*p);
      if (p->sel_separator_size() < best_size) {
	partition = *p;
	best_size = partition.sel_separator_size();
      }
    }
  }

template<class Weight, class Alloc>
void
Simple_Graph<Weight,Alloc>::grow_separator
(Vertex_Index u,
 Simple_Graph<Weight,Alloc>::Partition &partition) {
  partition.initialize(sel_num_vertices());
  // levelize from diameter
  Level_List *L = levelize(u);
  
  if (L->size()<2) return;
    
  //  find best balance within levels
  Level_List::iterator i = L->begin(), ie = L->end(), vertex_separator_it;
  int l = 0, vertex_separator_index = 0;
  double best_balance = 0;
  Weight left_part = 0, right_part = 0;
  for (;i!=ie;i++)
    right_part += i->sel_weight();
  i = L->begin(), ie = L->end();
  for (;i!=ie;i++,l++) {
    unsigned int separator_size = i->sel_weight();
    right_part -= separator_size;
    cerr << "partition: (" << left_part
	 << "," <<separator_size << "," << right_part << ")" << endl;
    double balance = compute_balance(left_part, right_part);
    //cerr <<"balance: " <<  balance << endl;

    //        cerr << "current balance: " << balance << endl;
    if ( balance > best_balance ) {
      best_balance = balance;
      vertex_separator_it = i;
      vertex_separator_index = l;
    }
    cerr << "Level " << l << " :";
    Level::iterator j = i->begin(), je = i->end();
    for (;j!=je;j++)
      cerr << *j << ":";
    cerr << endl;
    left_part += separator_size;
  }
  cerr << "node separator is level " << vertex_separator_index << endl;
  cerr << "balance: " << best_balance << endl;
  partition.initialize(sel_num_vertices(), vertex_separator_it->size());
  i = L->begin();
  for (;i!=vertex_separator_it;i++) {
    Level::iterator k = i->begin(), ke = i->end();
    for (;k!=ke;k++)
      partition.insert(*k, vertex_weights[*k], left);
  }
  Level::iterator k = i->begin(), ke = i->end();
  for (;k!=ke;k++) {
    partition.insert(*k, vertex_weights[*k], separator);
  }
  for (++i;i!=ie;i++) {
    Level::iterator k = i->begin(), ke = i->end();
    for (;k!=ke;k++)
      partition.insert(*k,vertex_weights[*k], right);
  }

  //     return vertex_separator;
}

  template<class Weight, class Alloc>
    //    Simple_Graph<Weight,Alloc>::Node_Location_Vec
    void
    Simple_Graph<Weight,Alloc>::levelized_bisection() {

    CPU_Time start_time;

    // pseeudo diameter
    pair<Vertex_Index, unsigned int> diameter_info =
      compute_pseudo_diameter();

    // levelize from diameter
    Level_List *L = levelize(diameter_info.first);

    //  find best balance within levels
    Level_List::iterator i = L->begin(), ie = L->end(), vertex_separator_it;
    int l = 0, vertex_separator_index = 0;
    double best_balance = 0;
    Weight left_part = 0, right_part = 0;
    for (;i!=ie;i++)
      right_part += i->sel_weight();
    i = L->begin(), ie = L->end();
    for (;i!=ie;i++,l++) {
      unsigned int separator_size = i->sel_weight();
      right_part -= separator_size;
      cerr << "partition: (" << left_part
	   << "," <<separator_size << "," << right_part << ")" << endl;
      double balance = compute_balance(left_part, right_part);
      //cerr <<"balance: " <<  balance << endl;

//        cerr << "current balance: " << balance << endl;
      if ( balance > best_balance ) {
	best_balance = balance;
	vertex_separator_it = i;
	vertex_separator_index = l;
      }
      cerr << "Level " << l << " :";
      Level::iterator j = i->begin(), je = i->end();
      for (;j!=je;j++)
	cerr << *j << ":";
      cerr << endl;
      left_part += separator_size;
    }
    cerr << "node separator is level " << vertex_separator_index << endl;
    cerr << "balance: " << best_balance << endl;
    partition.initialize(sel_num_vertices(), vertex_separator_it->size());
    i = L->begin();
    for (;i!=vertex_separator_it;i++) {
      Level::iterator k = i->begin(), ke = i->end();
      for (;k!=ke;k++)
	partition.insert(*k, vertex_weights[*k], left);
    }
    Level::iterator k = i->begin(), ke = i->end();
    for (;k!=ke;k++) {
      partition.insert(*k, vertex_weights[*k], separator);
    }
    for (++i;i!=ie;i++) {
      Level::iterator k = i->begin(), ke = i->end();
      for (;k!=ke;k++)
	partition.insert(*k,vertex_weights[*k], right);
    }

    cerr << "bisect: time elapsed: " << CPU_Time()-start_time << endl;

//     return vertex_separator;
  }

  template<class Weight, class Alloc>
    pair<Vertex_Index, unsigned int>
    Simple_Graph<Weight,Alloc>::compute_pseudo_diameter() {
    //return compute_pseudo_diameter(0).second;
    //vector<Vertex_Index> vertex_order(sel_num_vertices());
    //iota(vertex_order.begin(), vertex_order.end(), 0);
    //sort(vertex_order.begin(), vertex_order.end(), Less_Degree(*this));
    Vertex_Index s;// = min_element(vertex_order.begin(), vertex_order.end(),
    //	 Less_Degree(*this));
    vector<Adj_Vector,Alloc>::iterator adj_v = adj_vectors.begin(),
      adj_v_end = adj_vectors.end();
    unsigned int min_degree = -1;
    Vertex_Index v = 0;
    for (s=0;adj_v!=adj_v_end;adj_v++,v++) {
      const Adj_Vector & V = *adj_v;
      unsigned int degree = V.size();
      if (degree < min_degree) {
	min_degree = degree;
	s = v;
      }
    }
    cerr << "minimum degree: " << min_degree << endl;
    cerr << "minimum degree vertex: " << s << endl;
    unsigned int diameter = 0;
    bool done = false;
    while (!done) {
      pair<Vertex_Index, unsigned int> diameter_info =
	compute_pseudo_diameter(s);
      unsigned int new_diameter = diameter_info.second;
      if (new_diameter > diameter) {
	diameter = new_diameter;
	s = diameter_info.first;
      }
      else
	done = true;
    }
    cerr << "pseudo diameter: " << diameter << endl;
    cerr << "circumferent vertex: " << s << endl;
    return pair<Vertex_Index, unsigned int>(s, diameter);
  }

  template<class Weight, class Alloc>
    pair<Vertex_Index, unsigned int>
    Simple_Graph<Weight,Alloc>::compute_pseudo_diameter
    (Vertex_Index s /*,vector<Vertex_Index> vertex_order*/ ) {

    vector<BFS_Data> bfs_data(sel_num_vertices(), BFS_Data(white, -1));
    queue<Vertex_Index> Q;
    //vector<Vertex_Index> iterator i = distance.begin, ie=distance.end();
    //fill(bfs_data.begin(), bfs_data.end(), BFS_Data(white, -1) );
    //bfs_data.resize(sel_num_vertices), BFS_Data(white, -1) );
    bfs_data[s] = BFS_Data(gray,0);
    Q.push(s);
    Vertex_Index furthest_vertex;
    unsigned int max_distance = 0;
    while (Q.size()!=0) {
      Vertex_Index u = Q.front();
      Adj_Vector::const_iterator adj_u = adj_vectors[u].begin(),
	adj_u_end = adj_vectors[u].end();
      for (;adj_u!=adj_u_end;adj_u++) {
	Vertex_Index v = *adj_u;
	if (bfs_data[v].color == white) {
	  bfs_data[v].color = gray;
	  bfs_data[v].distance = max_distance = bfs_data[u].distance + 1;
	  furthest_vertex = v;
	  //cerr << "exploring " << v << endl;
	  Q.push(v);
	}
      }
      Q.pop();
      //cerr << "finished vertex " << u << endl;
      bfs_data[u].color = black;
    }
    return pair<Vertex_Index, unsigned int>(furthest_vertex, max_distance);
  }

  template<class Weight, class Alloc>
    Simple_Graph<Weight,Alloc>::Level_List*
    Simple_Graph<Weight,Alloc>::levelize(Vertex_Index s) {

    Level_List *L = new Level_List;

    vector<BFS_Data> bfs_data(sel_num_vertices(), BFS_Data(white, -1));
    queue<Vertex_Index> Q;
    bfs_data[s] = BFS_Data(gray,0);
    Q.push(s);
    unsigned int distance = -1;
    Level* current_level;
    while (Q.size()!=0) {
      Vertex_Index u = Q.front();
      unsigned int new_distance = bfs_data[u].distance;
      if (new_distance!=distance) {  // new level then
	L->push_back( Level() );
	current_level = &L->back();
	distance = new_distance;
      }
      current_level->add_vertex(u,vertex_weights[u]);
      Adj_Vector::const_iterator adj_u = adj_vectors[u].begin(),
	adj_u_end = adj_vectors[u].end();
      for (;adj_u!=adj_u_end;adj_u++) {
	Vertex_Index v = *adj_u;
	if (bfs_data[v].color == white) {
	  bfs_data[v].color = gray;
	  bfs_data[v].distance = bfs_data[u].distance + 1;
	  Q.push(v);
	}
      }
      Q.pop();
      bfs_data[u].color = black;
    }
    return L;
  }


#endif
