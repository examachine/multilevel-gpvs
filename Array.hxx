//
//
// C++ Interface for module: Array
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Array_Interface
#define Array_Interface

#include "General.hxx"

//  namespace Graph_Lib {

//  namespace Utility {
  // Array as we know it,
  // homogeneous, static range
  // this one special for graph algorithms
  // range is (1,N)
  template <class Content, class Allocator = Hullavi> class Array {
  public:
    typedef Array<C> Self;

    typedef Array 

    // Index of an array is an integer
    typedef unsigned int Index;

    // Locator classes
    class Locator {
      
    };

    // Traversal classes

    // Capable of traversing fully forward
    class ForwardTraversal  {
    public:
      // Start at first element
      ForwardTraversal(Index p, Index l)
	: position(p), last(l)  {}

      iterate() {
	position++;
	//if position
      }
    private:
      Index position, last;
    };

    typedef ForwardTraversal Traversal;

    Array() {
      rep = new C[ range.length() ];
      rep -= range.start;		// i am a demo coder ...  really :I
    }

    ~Array() {
      delete rep;
    }

    Integer length() { return R::length(); }
    virtual ASequence<C>& copyGASequence();
    virtual void copyDASequence(ASequence<C>&);
    virtual ASequence<C>& subASequenceGASequence(IntegerInterval) ;
    virtual void subASequenceDASequence(IntegerInterval);
    virtual ASequence<C>& reverseGASequence();
    virtual void reverseDASequence();
    virtual Index findASequence(C&);
    virtual ASequence<C>& insertGASequence(C&);
    virtual void insertDASequence(ASequence<C>&);
    virtual void mergeDASequence(ASequence<C>&);

    virtual Self& copyG();
    virtual void copyD(Self&);
    virtual Self& subSequenceG(IntegerInterval);
    virtual void subSequenceD(IntegerInterval);
    virtual Self& reverseG();
    virtual void reverseD();
    virtual Index find(C&);
    virtual Self& insertG(C&);
    virtual void insertD(Self&);
    virtual void mergeD(Self&);

    C& operator [](Index i) {
      if (range.contains(i))
	return rep[i].content;
      throw OutOfRange();		// let's be safe
    }

    Index first() {
      return range.start;
    }

    Index last() {
      return range.end;
    }

    Range range;

  private:
    Node *rep;
  };




//  }
//  }

#endif
