#include "Arc.h"
#include "Node.h"

Arc::Arc(Node* const node, const std::string& transportType) : m_node(node), m_arcType(transportType)
{
}

Arc::~Arc()
{
}

const std::string& Arc::ArcType() const
{
	return m_arcType;
}

Node* Arc::ArcNode() const
{
	return m_node;
}

void Arc::SetNode(Node* const node)
{
	this->m_node = node;
}

void Arc::SetArcType(std::string const& arcType)
{
	this->m_arcType = arcType;
}

Arc& Arc::operator=(Arc& otherArc)
{
	std::string const tempStr = otherArc.ArcType();
	otherArc.SetArcType(m_arcType);
	SetArcType(tempStr);
	Node* const tempNodePtr = otherArc.ArcNode();
	otherArc.SetNode(m_node);
	SetNode(tempNodePtr);
	return *this;
}