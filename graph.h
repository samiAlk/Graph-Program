/*graph.h*/

//
// << Mhd Sami Al Kabbani >>
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <utility>
#include <map> 

using namespace std;


template<typename VertexT, typename WeightT>
class graph
{
private:

  map <VertexT, int> indexMap;           // map of vertices and its indexes

  vector<map<int, WeightT>> adjList;     // vector of the index of the vertices and the weight

  vector<VertexT>  Vertices;             // vector of vertices
  
  int edgeCounter;                       // counter for the edges

  //
  // _LookupVertex
  //
  // Finds the vertex in the Vertices vector and returns it's 
  // index position if found, otherwise returns -1.
  //
  int _LookupVertex(VertexT v) const
  {
	auto t = indexMap.find(v);         // look up the vertex
	if(t == indexMap.end()){
		return -1;            // not found
	}
	else{
		return t->second;     // return its index if found
	}
  }

public:
  //
  // constructor:
  //
  // Constructs an empty graph where n is the max # of vertices 
  // you expect the graph to contain.
  //
  // NOTE: the graph is implemented using an adjacency matrix.
  // If n exceeds the dimensions of this matrix, an exception 
  // will be thrown to let you know that this implementation
  // will not suffice.
  //

  graph()
  {
	edgeCounter = 0;             //inilizing the counter to zero
  }  

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  
  int NumVertices() const
  {

	return adjList.size();         // getting the size of the map
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  
  int NumEdges() const
  {
    //
    // loop through the adjacency matrix and count how many 
    // edges currently exist:
    //

    return edgeCounter;            // returning the counter
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.

  bool addVertex(VertexT v)
  {
    if (_LookupVertex(v) != -1)     // if found
      return false;

    map<int, WeightT> M;
   
   Vertices.push_back(v);        // add the vertex to the vector
   int size = NumVertices();
   indexMap[v] = size;
   adjList.push_back(M);         // add an empty map
	
    return true;
  }
  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.

  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    int row = _LookupVertex(from);

    if (row == -1)  // not found:
      return false;

    int col = _LookupVertex(to);

    if (col == -1)  // not found:
      return false;
	  
	  
	if(adjList[row].find(col) == adjList[row].end()){       // c++ return .end when it is not found
		edgeCounter++;    // adds one if the edge is new
	}
	  
	adjList[row][col] = weight;        // adds the weight to the map
	   
    return true;
  }
  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
    //
    // we need to search the Vertices and find the position 
    // of each vertex; this will denote the row and col to
    // access in the adjacency matrix:
    //
    int row = _LookupVertex(from);

    if (row == -1)                    // not found:
      return false;

    int col = _LookupVertex(to);

    if (col == -1)                    // not found:
      return false;

    //
    // the vertices exist, but does the edge exist?
    //

	
	auto t = adjList[row].find(col);        // search for the weight
	if(t == adjList[row].end()){
		return false;                       // not found
	}
	else{
		weight = t->second;                  // found
		return true;
	}
	

    return false;
  }
  ///////////////////////////////////////////////////////////
  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.

  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;

    //
    // we need to search the Vertices and find the position 
    // of v, that will be the row we need in the adjacency 
    // matrix:
    //
    int row = _LookupVertex(v);

    if (row == -1)  // not found:
      return S;

    //
    // we found the row, so loop along the row and for every
    // edge that exists, add the column vertex to V:
    //
    // NOTE: how many columns are there?  The # of vertices.
    //
//     int index = indexMap(v);
	for(auto X: adjList[row]){            // looping through weight of the map
		S.insert(Vertices[X.first]);      // adding any vertex that has a an edge to it
		
	}
    return S;
  }
  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
    return Vertices;  // returns a copy:
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
	int counter = 0;
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << NumVertices() << endl;
    output << "**Num edges: " << NumEdges() << endl;
	
    output << endl;
    output << "**Vertices:" << endl;
	for(auto V: Vertices)
    {
      output << " " << counter << ". " << V << endl;
	  counter++;
    }
    output << endl;
    output << "**Edges:" << endl;
	
	
    for (int row = 0; row < NumVertices(); ++row){
        output << " row " << row << ": ";
        for (int col = 0; col < NumVertices(); ++col){
			auto t = adjList[row].find(col);
			if(t == adjList[row].end()){
				output << "F ";
			}
			else{
			  output << "(T,"
				<< t->second
				<< ") ";
			}
		}
		output << endl;
	}

    output << "**************************************************" << endl;
  }

};