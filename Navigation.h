#pragma once

#include <string>
#include <vector>
#include <fstream>

class Node;
class Arc;

class Navigation
{
	std::ofstream _outFile;
	std::vector<std::string> PlaceDataVector;
	std::vector<std::string> LinkDataVector;
	std::vector<Node*> NodeList;
	std::vector<Arc*> ArcList;
	std::vector<std::string> ValidTransportList;
public:
	Navigation();
	~Navigation();
	void GenerateValidTransport(const std::string& transportType);
	void BuildNodes();
	void BuildLinks();
	bool BuildNetwork(const std::string& fileNamePlaces, const std::string& fileNameLinks);
	bool ProcessCommand(const std::string& commandString);
	bool ReadData(const std::string& fileNamePlaces, const std::string& fileNameLinks);
	bool CheckRoute(const std::vector<std::string>& idList, const std::string& transportMode);
	bool NodeContained(const Node& node, const std::vector<Node>& list) const;
	bool TransportValid(const std::string& transportMode, const std::vector<std::string>& transports) const;
	std::tuple <std::string, std::string, double> FindMaximumDistance();
	std::tuple <std::string, std::string, double> FindMaximumDistanceBetweenLinks();
	const std::string MoveToVia(const Node* const A, const Node* const B, const std::string& const transportMode) const;
	std::vector<Node> FindRoute(const Node* A, const Node* B);
	std::vector<Node> FindShortestRoute(const Node* A, const Node* B);
	std::vector<Node> SearchNode(const Node* currentNode, std::vector<Node>& searchList, const Node* targetNode);
	std::vector<Node> SearchNodeShortest(const Node* currentNode, std::vector<Node>& searchList, const Node* targetNode);
	Node* FindParent(const Node const& node, const std::vector<Node> const& searchList);
	Node* FindNode(const std::string& id);
	Node* NextChild(const Node* const node, const std::vector<Node>& searchList, const Node& targetNode) const;
	Node* ClosestChild(const Node* node, const std::vector<Node>& searchList, const Node* targetNode) const;
	double DistanceBetweenNodes(const Node* nodeA, const Node* nodeB) const;
};