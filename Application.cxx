//
//
// C++ Implementation file for application
//
// Description: includes the main(...) function
//
// exa
//
//

#include "General.hxx"

#include "Graph.hxx"

#include "Graph_Test.hxx"

//using Graph_Lib::Simple_Graph;
//using Graph_Lib::Hullavi;
//using Graph_Lib::Test;

//  using namespace Graph_Lib;

typedef Simple_Graph<int> Graph;

void main(int argc, char *argv[])
{
  try {

    // Allocate work space
    Hullavi::init_class();
    
    if (argc==1)
      throw Exception("No arguments given");
    
    Graph g;
    string filename(argv[1]);
    ifstream file(filename.c_str());
    if (!file.good())
      throw Exception("Cannot open file");
    //g.load_metis(file);
    file >> g;
    //L("Graph loaded:\n");

    // test
    //Test test;
    //test.bisection(g, filename);

    if (argc==2)
      g.bisection(filename);
    else {
      if (argc!=5)
	throw Exception("Wrong number of arguments");
      int coarsening_threshold;
      double required_balance;
      int max_nongain_moves;
      sscanf(argv[2], "%d", &coarsening_threshold);
      sscanf(argv[3], "%lf", &required_balance);
      sscanf(argv[4], "%d", &max_nongain_moves);
      g.bisection(filename, coarsening_threshold, required_balance,
		  max_nongain_moves);
    }
    return 1;
  }
  catch (Exception e) {
    cout << "Unhandled exception" << endl << e.why() << endl;
  }
  catch (exception e) {
    cout << "Unhandled C++ exception" << endl << e.what() << endl;
  }

  return 0;
}
