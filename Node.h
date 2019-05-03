#pragma once

#include <string>
#include <vector>

class Arc;

class Node
{
private:
	const std::string m_location;
	const std::string m_id;
	const double m_latitude;
	const double m_longitude;
	std::vector<Arc*> m_connections;
public:
	Node(const std::string& location, const std::string& id, const double latitude, const double longitude);
	~Node();
	const std::string& Location() const;
	const std::string& ID() const;
	const double Latitude() const;
	const double Longitude() const;
	//std::vector<Arc*> m_connections;
	const std::vector<Arc*>& Connections() const;
	void AddLink(Arc* const arc);
	//Node& operator=(Node* otherNode);
};
