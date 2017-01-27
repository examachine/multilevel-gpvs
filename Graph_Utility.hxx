//
//
// C++ Interface for module: Graph_Utility
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Graph_Utility_Interface
#define Graph_Utility_Interface

#include "General.hxx"

//  namespace Graph_Lib {
//  namespace Utility {

  class Exception : public exception {
  public:
    Exception(string _msg) : msg(_msg) {}
    const string& why() { return msg; }
    const char *what() { return msg.c_str(); }
  private:
    string msg;
  };

//    class Time {
//    public:
//      Time() : time_rep(gettimeofday(&time_rep, NULL)) {} // current time
//      //Time(time_t t) : time_rep(t) {}
//      //operator const time_t() { return time_rep; }
//      ostream& print(ostream& out) {
//        out << time_rep.tv_sec << "." << time_rep.tv_usec << " seconds";
//      }
//    private:
//      //time_t time_rep;
//      struct timeval time_rep;
//    };

  class CPU_Time {
  public:
    CPU_Time() : clock_rep( clock() ) {} // current time
    CPU_Time(clock_t c) : clock_rep(c) {}
    //Time(time_t t) : time_rep(t) {}
    //operator const time_t() { return time_rep; }
    CPU_Time operator -(const CPU_Time rhs) {
      return CPU_Time(clock_rep - rhs.clock_rep);
    }
    ostream& print(ostream& out = cout) {
      out << double(clock_rep)/CLOCKS_PER_SEC << " seconds";
      return out;
    }
  private:
    clock_t clock_rep;
  };

  ostream& operator <<(ostream& out, CPU_Time t) { return t.print(out); }

  /*template <class Weight, class Allocator> istream&
    operator>>(istream& is, Simple_Graph<Weight, Allocator>& g) {
  }
  */

  // STL extension: simple I/O facilities

  template <class T, class Alloc> ostream&
    operator<<(ostream& out, const vector<T,Alloc>& vec) {
    out << "<";
    if (vec.size()>0) {
      vector<T,Alloc>::const_iterator i = vec.begin(), i_end = vec.end();
      out << *(i++);
      for (;i!=i_end;i++)
	out << "," << *i;
    }
    out << ">";
  }

  template <class T, class Alloc> ostream&
    operator<<(ostream& out, const list<T,Alloc>& vec) {
    out << "<";
    if (vec.size()>0) {
      list<T,Alloc>::const_iterator i = vec.begin(), i_end = vec.end();
      out << *(i++);
      for (;i!=i_end;i++)
	out << "," << *i;
    }
    out << ">";
  }

  template <class T, class Alloc> ostream&
    operator<<(ostream& out, const set<T,Alloc>& A) {
    out << "{";
    if (A.size()>0) {
      set<T,Alloc>::const_iterator i = A.begin(), i_end = A.end();
      out << *(i++);
      for (;i!=i_end;i++)
	out << "," << *i;
    }
    out << "}";
  }

  template <class T, class Alloc> ostream&
    operator<<(ostream& out, const hash_set<T,Alloc>& A) {
    out << "{";
    if (A.size()>0) {
      hash_set<T,Alloc>::const_iterator i = A.begin(), i_end = A.end();
      out << *(i++);
      for (;i!=i_end;i++)
	out << "," << *i;
    }
    out << "}";
  }

  template <class K, class T, class Alloc> ostream&
    operator<<(ostream& out, const map<K,T,Alloc>& m) {
    out << "<";
    if (m.size()>0) {
      map<K,T,Alloc>::const_iterator i = m.begin(), i_end = m.end();
      out << "[" << i->first << "," << i->second << "]";
      for (++i;i!=i_end;i++)
	out << "[" << i->first << "," << i->second << "]";
    }
    out << ">";
  }

/*
  template <class K, class T, class Alloc> ostream&
    operator<<(ostream& out, const hash_map<K,T,Alloc>& vec) {
    out << "<";
    if (vec.size()>0) {
      map<K,T,Alloc>::const_iterator i = vec.begin(), i_end = vec.end();
      out << "<" << i->first << "," << (i++)->second << ">";
      for (;i!=i_end;i++)
	out << "," << *i;
    }
    out << ">";
  } 
*/
//  }
//  }


#endif
