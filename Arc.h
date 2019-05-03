#pragma once

#include <string>

class Node;

class Arc
{
private:
	Node* m_node;
	std::string m_arcType;
public:
	Arc(Node* const node, const std::string& transportType);
	~Arc();
	const std::string& ArcType() const;
	Node* ArcNode() const;
	void SetNode(Node* const node);
	void SetArcType(std::string const& arcType);
	Arc& operator=(Arc& otherArc);
};
