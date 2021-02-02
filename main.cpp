/*main.cpp*/

// Name: Mhd Sami Al Kabbani
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:  
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <cstdlib>
#include <stack>
#include <cstring>
#include <cassert>
#include <utility>
#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"

#include <limits>

using namespace std;
using namespace tinyxml2;

// class prioritize to sort the map
class prioritize{
   public:
      bool operator()(const pair<long long, double>& n1, const pair<long long, double>& n2){
         if(n1.second == n2.second){
            return n1.first > n2.first;
         } 
         else{
            return n1.second > n2.second;
         }
      }
};

// helper function to find an element in the vector
bool isIn(vector<long long> visited, long long element){
   for(auto&E: visited){
      if(E == element){
         return true;
      }
   }
   return false;
}
// Dijkstra algorithem to get the shortest path
vector<long long> Dijkstra(graph<long long, double>& G, long long startV,
						   map<long long, double>& distances,
						   map<long long, long long>& pred){
  vector<long long>  visited;
  const int INF = numeric_limits<int>::max();
   priority_queue <pair<long long,double>, vector<pair<long long,double>>, prioritize> pq;
   // loop through all the vertices and mark all vertices
   for(auto &curV: G.getVertices()){
	  pred [curV] = -1;
      distances[curV] = INF;
      pq.push(pair<long long, double> (curV, INF));
   }
   //initlzing to 0
   distances[startV] = 0;
   pq.push(make_pair(startV, 0));
   
   while(!pq.empty()){
      pair<long long, double> p = pq.top();
      pq.pop();
		
      if(distances[p.first] == INF){
         break;
      }
      else if(isIn(visited, p.first)){
         continue;   
      }
      else{
         visited.push_back(p.first);
      }
      
      set<long long> neighbor = G.neighbors(p.first);
      
      for(auto& E: neighbor){
         double edgeW = 0.0;
         G.getWeight(p.first, E, edgeW);
		 double aledgW = 0.0;
         aledgW = distances[p.first] + edgeW;
         
         if(aledgW < distances[E]){
			pred[E] = p.first;
            distances[E] = aledgW;
            pq.push(pair<long long, double>(E, aledgW));
         }
      }
   }
  return visited;     // shortest path
}// end Dijkstra


//
// main
//
int main()
{

  map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
  vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
  vector<BuildingInfo>         Buildings; // info about each building, in no particular order
  XMLDocument                  xmldoc;
  graph<long long, double>     G;
  vector<long long>            paths;
  

  const int INF = numeric_limits<int>::max();
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
  {
    filename = def_filename;
  }

  //
  // Load XML-based map file 
  //
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  
  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == Nodes.size());
  assert(footwayCount == Footways.size());
  assert(buildingCount == Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

	
  for(auto &v: Nodes){                 // looping through the map of Nodes and inserting the nodes 
	  G.addVertex(v.first);            // into the graph
  }
	
  int FootWaySize = Footways.size();                   // size of the vector or Footways
  for(int i=0; i<FootWaySize; i++){
	  int NodesVectorSize = Footways[i].Nodes.size();  // size of the vector inside the struct Footwayinfo
	  for(int j =0; j<NodesVectorSize-1; j++){
		  auto t = Nodes.find(Footways[i].Nodes[j]);   // find the struct for that ID to get the lat and lon
		  auto d = Nodes.find(Footways[i].Nodes[j+1]);		 
		  // calculating the distance using a helper function
		  double distance = distBetween2Points(t->second.Lat, t->second.Lon, d->second.Lat, d->second.Lon);
		  // adding the edges to the graph
		  G.addEdge(Footways[i].Nodes[j], Footways[i].Nodes[j+1], distance);
		  G.addEdge( Footways[i].Nodes[j+1], Footways[i].Nodes[j] ,distance);

	  }
  }

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);
	bool startFlag = false;     //flags to be used in the loops
	bool destFlag  = false;
	string fullName;
	// varablies to store the info of the buildings
	double lat = 0.0;
	double lon = 0.0;
	string fullName1;
	double lat1 = 0.0;
	double lon1 = 0.0;
  while (startBuilding != "#")
  {
	startFlag = false;
	destFlag  = false;
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);
	  

		// looping thrugh the Building vector and searching for the abbreviation
		for(auto &X: Buildings){
			if(startBuilding == X.Abbrev){
				fullName = X.Fullname;
				lat = X.Coords.Lat;
				lon = X.Coords.Lon;
				startFlag = true;             // true if the abbreviation was found
			}
			if(destBuilding == X.Abbrev){
				destFlag = true;
				fullName1 = X.Fullname;
				lat1 = X.Coords.Lat;	
				lon1 = X.Coords.Lon;	
			}
		}	  
	  // looping thrugh the Building vector and searching for the fullname or any part or it
	  // and storing the values in the varablies 
		for(auto &X: Buildings){
			if((X.Fullname.find(destBuilding) != string::npos) && (destFlag == false)){
				destFlag = true;
				fullName1 = X.Fullname;
				lat1 = X.Coords.Lat;	
				lon1 = X.Coords.Lon;	
			}
			if((X.Fullname.find(startBuilding) != string::npos) && (startFlag == false)){
				startFlag = true;
				fullName = X.Fullname;
				lat = X.Coords.Lat;	
				lon = X.Coords.Lon;	
			}				
		}//for
	  // varablies used to store the values from the search and print it out
	  double startMin = INF;
	  double startDest = INF;
	  double minLat = 0.0;
	  double minLon = 0.0;
	  double destLat = 0.0;
	  double destLon = 0.0;
	  long long nodeMin = 0;
	  long long nodeDest = 0;
	  
	  // looping through vector Footways and then through loop Nodes to get every node
	  for(auto &F: Footways){
		  for(auto &N: F.Nodes){
			  auto t = Nodes.find(N);
			  // calculate the distance for every node to get the closest node to the building
			  double distance = distBetween2Points(lat, lon, t->second.Lat, t->second.Lon);
			  if(distance < startMin){
				  startMin = distance;
				  nodeMin = N;
				  minLat = t->second.Lat;
				  minLon = t->second.Lon;
			  }
			  // same opreation for the destnation building
			  distance = distBetween2Points(lat1, lon1, t->second.Lat, t->second.Lon);			  
			  if(distance < startDest){
				  startDest = distance;
				  nodeDest = N;
				  destLat = t->second.Lat;
				  destLon = t->second.Lon;
			  }			  
		  }
	  }	  
	  
	  map<long long, double> distances;          // empty maps to be filled in the algorithem
	  map<long long, long long> pred;
	  Dijkstra( G, nodeMin, distances, pred);
	  // if both inputs are correct, it prints out the values
	  if((startFlag == true) && (destFlag == true)){
		  cout << "Starting point:" << endl
			   << " " << fullName << endl
			   << " (" << lat << ", " << lon << ")" << endl;	
		  cout << "Destination point:" << endl
			   << " " << fullName1 << endl
			   << " (" << lat1 << ", " << lon1 << ")" << endl;
		  cout << endl << "Nearest start node: " << endl
			   << " " << nodeMin << endl << " (" << minLat
			   << ", " << minLon << ")";
		  cout << endl << "Nearest destination node: " << endl
			   << " " << nodeDest << endl << " (" << destLat
			   << ", " << destLon << ")" << endl;
		  cout << endl << "Navigating with Dijkstra..." << endl;
		  
		  if(distances[nodeDest] != INF){
			  // distance to the destination
			  cout << "Distance to dest: " << distances[nodeDest] << " miles" << endl;
			  cout << "Path: ";
			  stack<long long> swapStack;
			  long long predV = nodeDest;
			  while(predV != -1){
				  swapStack.push(predV);
				  predV = pred[predV];
			  }//while
			  // stack used to print the values from the end
			  while(!swapStack.empty()){
				  cout << swapStack.top();
				  swapStack.pop();
				  if(!swapStack.empty()){
					  cout << "->";
				  }
			  }//while
			  cout << endl;
		  }// if
		  else{
			  // if the destination is INF
			  cout << "Sorry, destination unreachable" << endl;
		  }// else
 	  }
	  
	  // if one of the inputs is not corrects it prints a not found 
	  if(startFlag == false){
		  cout << "Start building not found" << endl;
	  }
	  else if(destFlag == false){
		  cout << "Destination building not found" << endl;	
	  }

    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;

  return 0;
}