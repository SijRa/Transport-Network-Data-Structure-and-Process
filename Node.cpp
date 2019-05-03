#include "Node.h"
#include "Arc.h"
#include "iostream"

Node::Node(const std::string& location, const std::string& id, const double latitude, const double longitude) : m_location(location), m_id(id), m_latitude(latitude), m_longitude(longitude), m_connections()
{
}

Node::~Node()
{
}

const std::string& Node::Location() const
{
	return m_location;
}

const std::string& Node::ID() const
{
	return m_id;
}

const double Node::Latitude() const
{
	return m_latitude;
}

const double Node::Longitude() const
{
	return m_longitude;
}

const std::vector<Arc*>& Node::Connections() const
{
	return m_connections;
}

void Node::AddLink(Arc* const arc)
{
	m_connections.push_back(arc);
}