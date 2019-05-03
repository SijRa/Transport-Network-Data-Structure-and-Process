#include "Navigation.h"
#include "Node.h"
#include "Arc.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <tuple>

// Converts latitude/longitude into eastings/northings
extern void LLtoUTM(const double Lat, const double Long, double& UTMNorthing, double& UTMEasting);

Navigation::Navigation() : _outFile("Output.txt")
{
}

Navigation::~Navigation()
{
}

void Navigation::GenerateValidTransport(const std::string& transportType)
{
	if (!ValidTransportList.empty())
	{
		ValidTransportList.clear();
	}

	if (transportType == "Rail" || transportType == "Ship")
	{
		ValidTransportList.push_back(transportType);
	}
	else if (transportType == "Bus")
	{
		ValidTransportList.push_back("Rail");
		ValidTransportList.push_back("Ship");
		ValidTransportList.push_back(transportType);
	}
	else if (transportType == "Car")
	{
		ValidTransportList.push_back("Rail");
		ValidTransportList.push_back("Ship");
		ValidTransportList.push_back("Bus");
		ValidTransportList.push_back(transportType);
	}
	else if (transportType == "Bike")
	{
		ValidTransportList.push_back("Rail");
		ValidTransportList.push_back("Ship");
		ValidTransportList.push_back("Bus");
		ValidTransportList.push_back("Car");
		ValidTransportList.push_back(transportType);
	}
	else if (transportType == "Foot")
	{
		ValidTransportList.push_back("Rail");
		ValidTransportList.push_back("Ship");
		ValidTransportList.push_back("Bus");
		ValidTransportList.push_back("Car");
		ValidTransportList.push_back("Bike");
		ValidTransportList.push_back(transportType);
	}
}

bool Navigation::ProcessCommand(const std::string& commandString)
{
	std::istringstream inString(commandString);
	std::string command;
	inString >> command;

	//Set precision to 3 d.p
	_outFile.setf(std::ios::fixed, std::ios::floatfield);
	_outFile.precision(3);

	if (command == "MaxDist")
	{
		//MaxDist
		std::tuple<std::string, std::string, double> MaximumDistance = FindMaximumDistance();
		_outFile << commandString << std::endl;
		_outFile << std::get<0>(MaximumDistance) << "," << std::get<1>(MaximumDistance) << "," << std::get<2>(MaximumDistance) << std::endl;
		_outFile << std::endl;
		return true;
	}
	else if (command == "MaxLink")
	{
		//MaxLink
		std::tuple<std::string, std::string, double> MaximumDistance = FindMaximumDistanceBetweenLinks();
		_outFile << commandString << std::endl;
		_outFile << std::get<0>(MaximumDistance) << "," << std::get<1>(MaximumDistance) << "," << std::get<2>(MaximumDistance) << std::endl;
		_outFile << std::endl;
		return true;
	}
	else if (command == "FindDist")
	{
		std::string idA;
		std::string idB;
		inString >> idA;
		inString >> idB;

		const Node* const A = FindNode(idA);
		const Node* const B = FindNode(idB);
		double const Distance = DistanceBetweenNodes(A, B);
		_outFile << commandString << std::endl;
		_outFile << A->Location() << "," << B->Location() << "," << Distance << std::endl;
		_outFile << std::endl;
		return true;
	}
	else if (command == "FindNeighbour")
	{
		std::string id;
		inString >> id;
		Node const centralNode = *FindNode(id);
		_outFile << commandString << std::endl;
		for (const Arc* const arc : centralNode.Connections())
		{
			_outFile << arc->ArcNode()->ID() << std::endl;
		}
		_outFile << std::endl;
		return true;
	}
	else if (command == "Check")
	{
		//Transport mode
		std::string transportMode;
		inString >> transportMode;
		GenerateValidTransport(transportMode);//Fill valid transports
		//Extract IDs
		std::vector<std::string> idList;
		std::string id;
		while (inString >> id)
		{
			idList.push_back(id);
		}
		_outFile << commandString << std::endl;
		CheckRoute(idList, transportMode);
		_outFile << std::endl;
		return true;
	}
	else if (command == "FindRoute")
	{
		std::string transportMode;
		std::string idA;
		std::string idB;
		inString >> transportMode;
		inString >> idA;
		inString >> idB;

		Node* const A = FindNode(idA);
		Node* const B = FindNode(idB);

		GenerateValidTransport(transportMode);//Valid transports

		_outFile << commandString << std::endl;
		std::vector<Node> route = FindRoute(A, B);

		if (NodeContained(*B, route))
		{
			for (const Node const node : route)
			{
				_outFile << node.ID() << std::endl;
			}
		}
		else
		{
			_outFile << "FAIL" << std::endl;
		}
		_outFile << std::endl;
		return true;
	}
	else if (command == "FindShortestRoute")
	{
		std::string transportMode;
		std::string idA;
		std::string idB;
		inString >> transportMode;
		inString >> idA;
		inString >> idB;

		GenerateValidTransport(transportMode);

		Node* const A = FindNode(idA);
		Node* const B = FindNode(idB);

		_outFile << commandString << std::endl;
		std::vector<Node> route = FindShortestRoute(A, B);

		if (NodeContained(*B, route))
		{
			for (const Node const node : route)
			{
				_outFile << node.ID() << std::endl;
			}
		}
		else
		{
			_outFile << "FAIL" << std::endl;
		}
		_outFile << std::endl;
		return true;
	}

	return false;
}

void Navigation::BuildNodes()
{
	int counter = 0;
	std::string location;
	std::string id;
	std::string longitude;
	std::string latitude;
	for (const std::string& field : PlaceDataVector)
	{
		if (counter == 0)
		{
			location = field;
		}
		else if (counter == 1)
		{
			id = field;
		}
		else if (counter == 2)
		{
			latitude = field;
		}
		else if (counter == 3)
		{
			longitude = field;
			double UTMNorthing;
			double UTMEasting;
			LLtoUTM(stod(latitude), stod(longitude), UTMNorthing, UTMEasting);
			//Parse and add node
			Node* const currentNode = new Node(location, id, UTMEasting, UTMNorthing);
			NodeList.push_back(currentNode);
			//Reset fields
			location = "";
			id = "";
			longitude = "";
			latitude = "";
			//Reset counter
			counter = -1;
		}
		counter++;
	}
}

void Navigation::BuildLinks()
{
	int counter = 0;
	std::string id;
	std::string destinationId;
	std::string transportType;
	for (const std::string& field : LinkDataVector)
	{
		if (counter == 0)
		{
			id = field;
		}
		else if (counter == 1)
		{
			destinationId = field;
		}
		else if (counter == 2)
		{
			transportType = field;
			Node* currentNode = FindNode(id);
			Node* destinationNode = FindNode(destinationId);
			Arc* const currentArc = new Arc(destinationNode, transportType);
			Arc* const destinationArc = new Arc(currentNode, transportType);
			ArcList.push_back(currentArc);
			ArcList.push_back(destinationArc);
			currentNode->AddLink(currentArc);
			destinationNode->AddLink(destinationArc);
			id = "";
			destinationId = "";
			transportType = "";
			counter = -1;
		}
		counter++;
	}
}

std::tuple<std::string, std::string, double> Navigation::FindMaximumDistance()
{
	double maximumDistance = 0;
	double currentDistance;
	std::string nodeA;
	std::string nodeB;
	for (size_t i = 0; i < NodeList.size() - 1; ++i)
	{
		for (size_t j = 1; j < NodeList.size(); ++j)
		{
			currentDistance = DistanceBetweenNodes(NodeList[i], NodeList[j]);
			if (currentDistance > maximumDistance)
			{
				maximumDistance = currentDistance;
				nodeA = NodeList[i]->Location();
				nodeB = NodeList[j]->Location();
			}
		}
	}
	return make_tuple(nodeA, nodeB, maximumDistance);
}

std::tuple<std::string, std::string, double> Navigation::FindMaximumDistanceBetweenLinks()
{
	double maximumDistance = 0;
	double currentDistance;
	std::string nodeA;
	std::string nodeB;

	for (size_t i = 0; i < NodeList.size(); ++i)
	{
		for (const Arc* const link : NodeList[i]->Connections())
		{
			currentDistance = DistanceBetweenNodes(NodeList[i], link->ArcNode());
			if (currentDistance > maximumDistance)
			{
				maximumDistance = currentDistance;
				nodeA = NodeList[i]->ID();
				nodeB = link->ArcNode()->ID();
			}
		}
	}
	return make_tuple(nodeA, nodeB, maximumDistance);
}

bool Navigation::CheckRoute(const std::vector<std::string>& idList, const std::string& transportType)
{
	auto itr = idList.begin();
	while (itr != idList.end() - 1)
	{
		Node* const A = FindNode(*itr);
		Node* const B = FindNode(*(itr + 1));
		std::string const result = MoveToVia(A, B, transportType);
		if (result.empty())//No path found
		{
			_outFile << A->ID() << "," << B->ID() << "," << "FAIL" << std::endl;
			return false;
		}
		else
		{
			_outFile << A->ID() << "," << B->ID() << "," << "PASS" << std::endl;
			itr++;
		}
	}
	return true;
}

const std::string Navigation::MoveToVia(const Node* const A, const Node* const B, const std::string& const transportType) const
{
	for (const Arc* const link : A->Connections())
	{
		if (B->ID() == link->ArcNode()->ID() && transportType == link->ArcType())
		{
			return link->ArcType();
		}
	}
	return {};//Empty string
}

std::vector<Node> Navigation::FindRoute(const Node* const A, const Node* const B)
{
	std::vector<Node> visted;
	visted.push_back(*A);
	std::vector<Node> Route = SearchNode(A, visted, B);
	return Route;
}

std::vector<Node> Navigation::FindShortestRoute(const Node* const A, const Node* const B)
{
	std::vector<Node> visted;
	visted.push_back(*A);
	std::vector<Node> Route = SearchNodeShortest(A, visted, B);
	return Route;
}

bool Navigation::NodeContained(const Node& node, const std::vector<Node> const& list) const
{
	if (!list.empty())
	{
		for (const Node const currentNode : list)
		{
			if (currentNode.ID() == node.ID())
			{
				return true;
			}
		}
	}
	return false;
}

bool Navigation::TransportValid(const std::string& transportMode, const std::vector<std::string> const& list) const
{
	for (const std::string const transport : list)
	{
		if (transportMode == transport)
		{
			return true;//Valid travel
		}
	}
	return false;
}

std::vector<Node> Navigation::SearchNode(const Node* const currentNode, std::vector<Node>& searchList, const Node* const targetNode)
{
	Node* const childNode = NextChild(currentNode, searchList, *targetNode);
	if (childNode == nullptr)
	{
		if (searchList.size() > 1)
		{
			Node* const parent = FindParent(*currentNode, searchList);
			return SearchNode(parent, searchList, targetNode);
		}
		return searchList;//No results
	}

	searchList.push_back(*childNode); //insert(*child);

	if (childNode->ID() == targetNode->ID())
	{
		return searchList;
	}
	return SearchNode(childNode, searchList, targetNode);
}

std::vector<Node> Navigation::SearchNodeShortest(const Node* const currentNode, std::vector<Node>& searchList, const Node* const targetNode)
{
	Node* const childNode = ClosestChild(currentNode, searchList, targetNode);
	if (childNode == nullptr)
	{
		if (searchList.size() > 1)
		{
			Node* const parent = FindParent(*currentNode, searchList);
			return SearchNode(parent, searchList, targetNode);
		}
		return searchList;//No results
	}
	searchList.push_back(*childNode);

	if (childNode->ID() == targetNode->ID())
	{
		return searchList;
	}

	return SearchNode(childNode, searchList, targetNode);
}

Node* Navigation::FindParent(const Node const& childNode, const std::vector<Node> const& searchList)
{
	if (searchList.size() > 1)
	{
		for (size_t i = 0; i < searchList.size(); ++i)
		{
			if (childNode.ID() == searchList[i].ID())
			{
				return FindNode(searchList[i + 1].ID());
			}
		}
	}
	return nullptr;
}

Node* Navigation::FindNode(const std::string & id)
{
	for (Node* const node : NodeList)
	{
		if (id == node->ID())
		{
			return node;
		}
	}

	return nullptr;
}

Node* Navigation::NextChild(const Node * const mainNode, std::vector<Node> const& searchList, Node const& targetNode) const
{
	Node* resultNode = nullptr;
	for (Arc* const arc : mainNode->Connections())
	{
		if (arc->ArcNode()->ID() == targetNode.ID())
		{
			return arc->ArcNode();
		}
		else if (!NodeContained(*arc->ArcNode(), searchList) && TransportValid(arc->ArcType(), ValidTransportList))
		{
			resultNode = arc->ArcNode();
		}
	}
	return resultNode;
}

Node* Navigation::ClosestChild(const Node * const node, const std::vector<Node> const& searchList, const Node * const targetNode) const
{
	double shortestDistance = std::numeric_limits<double>::max();
	double currentDistance;
	Node* closestNode = nullptr;
	for (const Arc* const arc : node->Connections())
	{
		if (arc->ArcNode()->ID() == targetNode->ID())
		{
			return arc->ArcNode();
		}
		if (!NodeContained(*arc->ArcNode(), searchList) && TransportValid(arc->ArcType(), ValidTransportList))
		{
			currentDistance = DistanceBetweenNodes(arc->ArcNode(), targetNode);
			if (currentDistance < shortestDistance)
			{
				shortestDistance = currentDistance;
				closestNode = arc->ArcNode();
			}
		}
	}
	return closestNode;
}

/* Pythagoras Theorem */
/* Returns distance between two cooridnates */
double Navigation::DistanceBetweenNodes(const Node * const nodeA, const Node * const nodeB) const
{
	double const xChange = nodeB->Longitude() - nodeA->Longitude();
	double const yChange = nodeB->Latitude() - nodeA->Latitude();
	double const aSqrd = xChange * xChange;
	double const bSqrd = yChange * yChange;
	double const c = sqrt(aSqrd + bSqrd);
	return (c / 1000);
}

bool Navigation::BuildNetwork(const std::string & fileNamePlaces, const std::string & fileNameLinks)
{
	const std::fstream finPlaces(fileNamePlaces);
	const std::fstream finLinks(fileNameLinks);
	if (finPlaces.fail() || finLinks.fail()) return false;

	if (ReadData(fileNamePlaces, fileNameLinks))// if true, continue
	{
		BuildNodes();
		BuildLinks();
	}

	return true;
}

bool Navigation::ReadData(const std::string & fileNamePlaces, const std::string & fileNameLinks)
{
	/* Places.csv */
	std::fstream finPlaces(fileNamePlaces);
	while (!finPlaces.eof())
	{
		std::string row;
		std::getline(finPlaces, row);
		std::stringstream stringStream(row);
		std::string itemWord;
		//Check by character
		char ch;
		while (stringStream >> ch)
		{
			itemWord.push_back(ch);//Append to string itemWord
			if (stringStream.peek() == ',')//Current stream input
			{
				stringStream.ignore();//Skip character
				PlaceDataVector.push_back(itemWord);//Append to itemVector
				itemWord = "";
			}
		}
		PlaceDataVector.push_back(itemWord);
		itemWord = "";
	}

	/* Links.csv */
	std::fstream finLinks(fileNameLinks);
	while (!finLinks.eof())
	{
		std::string row;
		std::getline(finLinks, row);
		std::stringstream stringStream(row);
		std::string itemWord;
		//Check by character
		char ch;
		while (stringStream >> ch)
		{
			itemWord.push_back(ch);//Append to string itemWord
			if (stringStream.peek() == ',')//Current stream input
			{
				stringStream.ignore();//Skip character
				LinkDataVector.push_back(itemWord);//Append to itemVector
				itemWord = "";
			}
		}
		LinkDataVector.push_back(itemWord);
		itemWord = "";
	}

	return true;
}