//
//
// C++ Interface for module: Simple Graph of Graph
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Simple_Graph_Interface
#define Simple_Graph_Interface

#include "General.hxx"
#include "Allocators.hxx"
#include "Attributes.hxx"

//  namespace Graph_Lib {

//    using namespace Attributes;

  class Graph_Error : public Exception {
  public:
    Graph_Error(string msg)
      : Exception("Graph Error: " + msg) {}
  };

// Weight must be signed
  template <class Weight, class Allocator = single_client_alloc>
  class Simple_Graph
    //: public Abstract_Vertices
    //, public Abstract_Vertex_Weight_Attribute<Weight>*/
    //, public Adj_Vector_Attribute
  {
  public:

    // common types
    typedef Allocator Alloc;
    typedef vector<Vertex_Index, Allocator> Adj_Vector;
    typedef vector<Weight, Allocator> Weights;
    typedef vector<Adj_Vector,Allocator> Adj_Vectors;
    typedef vector<Vertex_Index, Allocator> Vertex_Indices;

    // constructors
    Simple_Graph()
      : vertex_weights() {}
    Simple_Graph(unsigned int num_of_vertices)
      {
	initialize_vertices(num_of_vertices);
      }

    unsigned int sel_num_vertices() const {
      return adj_vectors.size(); // Simple_Graph class is responsible for
				// for keeping all vectors in sync
    }
    void initialize_vertices(int num_vertices, bool coarse = false) {
      vertex_weights.resize(num_vertices);
      adj_vectors.resize(num_vertices);
      partition.initialize(num_vertices);
      if (coarse)
        map_exp.resize(num_vertices);
    }

    // text file i/o in metis format
    istream& load_metis(istream &input_stream = cin);
    ostream& store_metis(ostream &output_stream = cout) const;

    // top level routines
    void bisection(string & filename,
		   int coarsening_threshold,
		   double required_balance,
		   int max_nongain_moves);

    // simple counter class
    class Counter {
    public:
      Counter(int val): value(val) {}
      int operator()() { return value++; }
      int check() { return value; }
    private:
      int value;
    };

    // compression

    list<Simple_Graph*> coarsen_graph(unsigned int);
    Simple_Graph& coarsening_step();
    void select_vertices();
    Simple_Graph& compress_graph();
    void explode(Simple_Graph&);

    vector<Vertex_Index, Allocator> match;
    vector<Vertex_Index, Allocator> map_imp; // implosion map
    vector<Vertex_Index, Allocator> map_exp; // explosion map
    unsigned int num_imploded_vertices;

    // bisection

    class Level : public list< Vertex_Index > {
    public:
      Level() : weight(0) {}
      void add_vertex( Vertex_Index v, Weight w) {
	push_back(v);
	weight += w;
      }
      const Weight sel_weight() const { return weight;  }
    private:
      Weight weight;
    };
    typedef list< Level > Level_List;

    void trivial_bisection();
    void growing_bisection();
    void levelized_bisection();
    pair<Vertex_Index, unsigned int> compute_pseudo_diameter();
    pair<Vertex_Index, unsigned int> compute_pseudo_diameter(Vertex_Index u);
    Level_List *levelize(Vertex_Index s);

    // refinement algorithms

    // types for handling properties associated
    // with Separator and Gain computation
    typedef list< Vertex_Index > Gain_Bucket;
    enum Vertex_Location { separator, left, right };
    class Vertex_Status {
    public:
      Vertex_Location location;
      Gain_Bucket::iterator gain_iterator;
    };
    struct Degree_Info {
      Degree_Info() : left_degree(0), right_degree(0) {}
      void reset_degree() { left_degree = right_degree = 0; }
      void add_degree(Vertex_Location loc, Weight w) {
	switch(loc) {
	  case separator: break;
	  case left:left_degree += w; break;
	  case right:right_degree += w; break;
	}
      }
//        void add_left_degree(Weight w) { left_degree += w; }
//        void add_right_degree(Weight w) { right_degree += w; }
      Weight left_degree, right_degree;
    };
    typedef vector<Degree_Info, Allocator> Degrees;

    // compute balance
    static double compute_balance(Weight left_part, Weight right_part) {
      double balance;
      if (left_part > right_part)
        swap(left_part,right_part);
      if (right_part!=0)
	balance = double(left_part)/right_part;
      else
	balance = 0;
      return balance;
    }

    // compute metis imbalance
    static double compute_metis_imbalance(Weight left_part,
					  Weight right_part) {
      return double(2*max(left_part,right_part))/(left_part+right_part) - 1;
    }

    class Partition {
    public:

      Partition() : size_left(0), size_right(0), size_separator(0) {}

      void initialize(unsigned int num_vertices, unsigned int size_separator = 0) {
	vertex_location.resize(num_vertices);
	if (size_separator == 0) size_separator = num_vertices/4;
	vertex_separator.resize(size_separator*2);
	degrees.resize(num_vertices);
      }

      bool empty() {
	return vertex_location.empty();
      }

      void insert(Vertex_Index v, Weight w, Vertex_Location loc) {
	//vertex_status[v].location = loc;
	vertex_location[v] = loc;
	switch (loc) {
	  case separator:
	    vertex_separator.insert(v); size_separator += w; break;
	  case left: size_left += w; break;
	  case right: size_right += w; break;
	}
      }
      
      double balance() {
	return Simple_Graph::compute_balance(size_left, size_right);
      }

      double metis_imbalance() {
	return Simple_Graph::compute_metis_imbalance(size_left, size_right);
      }

      Weight sel_separator_size() const {
	return size_separator;
      }

      Weight sel_left_size() const {
	return size_left;
      }

      Weight sel_right_size() const {
	return size_right;
      }

      const Weight sel_total_size() const {
	return size_left + size_separator + size_right;
      }
      
      Vertex_Location sel_vertex_location(Vertex_Index v) {
	return vertex_location[v];
      }

      void move_to_left(Vertex_Index v, Weight w) {
	vertex_location[v] = left;
	vertex_separator.erase(v);
	size_left += w;
	size_separator -= w;
      }
      void move_to_right(Vertex_Index v, Weight w) {
	vertex_location[v] = right;
	vertex_separator.erase(v);
	size_right += w;
	size_separator -= w;
      }
      void move_from_left(Vertex_Index v, Weight w) {
	vertex_location[v] = separator;
	vertex_separator.insert(v);
	size_left -= w;
	size_separator += w;
      }
      void move_from_right(Vertex_Index v, Weight w) {
	vertex_location[v] = separator;
	vertex_separator.insert(v);
	size_right -= w;
	size_separator += w;
      }

      void explode(Partition& coarse_part) {
      }

      ostream& print_quality(ostream& out = cout) {
	out << "Vertex Separator: Size " <<  sel_separator_size()
	    << ", Balance " << balance()
	    << ", Imbalance(metis) " << metis_imbalance() << endl; 
      }

      ostream& print_vertex_separator(ostream& out = cout) {
	out << "separator: " << vertex_separator << endl;
      }

      ostream& print_partition(ostream& out = cout) {
	hash_set<Vertex_Index> left_set, right_set;
	vector<Vertex_Location>::iterator i=vertex_location.begin(),
	i2=vertex_location.end();
	for (int v = 0;i!=i2;i++,v++) {
	  if (*i==left) left_set.insert(v);
	  if (*i==right) right_set.insert(v);
	}
	out <<"partition: (" << left_set << "," << vertex_separator
	    << "," << right_set << ")" << endl;
      }

      ostream& print(ostream& out = cout) {
	print_quality(out);
        out << endl;
	print_partition(out);
      }

      //vector<Vertex_Status> vertex_status;
      vector<Vertex_Location> vertex_location;
      hash_set<Vertex_Index> vertex_separator;
      Degrees degrees;
      Weight size_left, size_right, size_separator;
    };

    Partition partition;

    class Gain_Info {
    public:
      typedef map<Weight, Gain_Bucket> Gain_Bucket_Map;
      struct Vertex_Info {
	Vertex_Info() {}

	Vertex_Info(Gain_Bucket_Map::iterator a, Gain_Bucket::iterator b)
	  : bucket_map_it(a), bucket_it(b){}
	Gain_Bucket_Map::iterator bucket_map_it;
	Gain_Bucket::iterator bucket_it;
      };

      Gain_Info() {
	initialize();
      }

      //code: intialize routine
      void initialize() {
	max_gain_bucket = gain_buckets.end();
	gain_buckets.erase(gain_buckets.begin(), gain_buckets.end());
	vertices.erase(vertices.begin(), vertices.end());
	//max_gain = -(1L<<30);
      }

      // precondition v is not in vertices
      Gain_Bucket::iterator insert(Vertex_Index v, Weight gain) {
	assert(vertices.find(v)==vertices.end());
	bool was_empty = empty();
	Gain_Bucket_Map::iterator bucket_map_it = gain_buckets.find(gain);
	if (bucket_map_it==gain_buckets.end())
	  bucket_map_it = gain_buckets.insert(pair<Weight,Gain_Bucket>
					      (gain, Gain_Bucket())   ).first;
	assert( gain_buckets.find(bucket_map_it->first) != gain_buckets.end());
	Gain_Bucket & bucket = bucket_map_it->second;
	Gain_Bucket::iterator it = bucket.insert(bucket.end(), v);
	vertices[v] = Vertex_Info(bucket_map_it, it);
	if (gain>max_gain_bucket->first || was_empty) {
	  //max_gain = gain;
	  max_gain_bucket = bucket_map_it;
	}
	return it;
      }

      const Weight sel_max_gain() const { return max_gain_bucket->first; }

      bool empty() {
	return gain_buckets.empty();
      }

      // precondition: not empty
      Vertex_Index sel_top_vertex() {
	assert(!empty());
	return max_gain_gucket->second->front();
      }

      // ? precondition: not empty
      // returns -1 if empty
      Vertex_Index extract_top_vertex() {
	assert(!empty());
	assert( max_gain_bucket != gain_buckets.end());
	Gain_Bucket & bucket = max_gain_bucket->second;
	Vertex_Index v = bucket.front();
	bucket.pop_front();
	vertices.erase(v);
	if (bucket.size()==0) {
	  Gain_Bucket_Map::iterator preceding_gain_bucket = max_gain_bucket;
	  preceding_gain_bucket--;
	  gain_buckets.erase(max_gain_bucket);
	  if (preceding_gain_bucket != gain_buckets.end()) {
	    //max_gain = 
	    max_gain_bucket = preceding_gain_bucket;
	  }
	}
	return v;
      }

      // precondition: not empty, v is in vertices
      void remove_vertex(Vertex_Index v) {
	assert(!empty());
	assert( vertices.find(v) != vertices.end() );
	Vertex_Info v_info = vertices[v];
	Gain_Bucket & bucket = v_info.bucket_map_it->second;
	Gain_Bucket_Map::iterator bucket_map_it = v_info.bucket_map_it;
	assert (bucket_map_it->second.size() != 0);
	bucket.erase(v_info.bucket_it);
	vertices.erase(v);
	if (bucket.size()==0) {
	  if (v_info.bucket_map_it == max_gain_bucket) { //is top bucket?
	    Gain_Bucket_Map::iterator preceding_gain_bucket = bucket_map_it;
	    preceding_gain_bucket--;
	    gain_buckets.erase(max_gain_bucket);
	    if (preceding_gain_bucket!=gain_buckets.end()) {
	      //max_gain = new_bucket_map_it->first;
	      max_gain_bucket = preceding_gain_bucket;
	    }
	    //else max_gain = -(1L<<30);
	  }
	  else {
	    gain_buckets.erase(bucket_map_it);
	  }
	}
      }

      void print() {
	if (!empty())
	cerr << "max_gain: " << sel_max_gain() << "="
	     << max_gain_bucket->second << endl;
	cerr << "gain_buckets:" << gain_buckets << endl;
	
      }

      Gain_Bucket_Map gain_buckets;
      Gain_Bucket_Map::iterator max_gain_bucket;
      hash_map<Vertex_Index, Vertex_Info> vertices;
      //vector<Vertex_Status> vertex_info; // indexed by Vertex_Index :)
    };
    // we maintatin two gain info objects for the partitions
    // to which we may move a vertex
    Gain_Info left_gain_info, right_gain_info;
    //hash_set<Vertex_Index> locked_vertices;

    void grow_separator(Vertex_Index u, Partition& partition);
    void uncoarsen_graph(list<Simple_Graph*>);
    void VFM_refinement(double required_balance, int max_non_gain_moves);
    void compute_all_degrees();
    void compute_all_gains();
    void compute_gain(Vertex_Index u);
    Weight VFM_iteration(double required_balance, int mox_non_gain_moves);
    enum VFM_Result { negative, positive, depleted };

//      bool VFM_move_for_balance();
//      bool VFM_move_for_gain();
//      bool VFM_move_to_left(Vertex_Index v);
//      bool VFM_move_to_right(Vertex_Index v);
    VFM_Result VFM_move_for_balance();
    VFM_Result VFM_move_for_gain();
    VFM_Result VFM_move_to_left(Vertex_Index v, Weight gain);
    VFM_Result VFM_move_to_right(Vertex_Index v, Weight gain);
//      Weight VFM_move(Gain_Info & gain_info);

    // implementation
//      private:

    // data structures

    // common data structures
    // Graph representation
    Weights vertex_weights;
    Adj_Vectors adj_vectors;
    
    // friend class Test; //bug: egcs1.1
    friend class Test;

    // algorithms
    struct Less_Degree
      : public binary_function<Vertex_Index, Vertex_Index, bool> {
      Less_Degree(Simple_Graph<Weight,Alloc> g)
	: graph(g) {}
      bool operator()(Vertex_Index u, Vertex_Index v) {
	return graph.adj_vectors[u].size() < graph.adj_vectors[v].size();
      }
      Simple_Graph<Weight,Alloc> graph;
    };

  };

    // types for BFS
  enum BFS_Color { white, gray, black };
  struct BFS_Data {
    BFS_Data() {}
    BFS_Data(BFS_Color c, unsigned int d)
      : color(c), distance(d) {}
    BFS_Color color;
    unsigned int distance;
  };


//  } // namespace end


#endif
