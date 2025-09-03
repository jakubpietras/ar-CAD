#include "arpch.h"
#include "HoleDetector.h"
#include "core/Scene/Components.h"
// #define DEBUG

namespace ar
{

	std::vector<ar::Hole> HoleDetector::Detect(std::vector<Entity> surfaces)
	{
		m_Adjacency.clear();
		m_EdgeTable.clear();

		BuildGraph(surfaces);
#ifdef DEBUG
		PrintAdjacency();
		PrintEdgeTable();
#endif
		// triples of points that (in order) form a triangular hole
		return ConvertTriplesToHoles(FindTriples());
	}

	std::unordered_map<uint32_t, std::unordered_set<uint32_t>> HoleDetector::m_Adjacency;
	std::unordered_map<EdgeKey, EdgeInfo, EdgeKeyHash> HoleDetector::m_EdgeTable;

	void HoleDetector::BuildGraph(std::vector<Entity> surfaces)
	{
		for (auto& surface : surfaces)
			ExtractEdges(surface);
	}

	std::vector<std::array<uint32_t, 3>> HoleDetector::FindTriples()
	{
		std::vector<std::array<uint32_t, 3>> triples;
		std::set<std::set<uint32_t>> seenTriangles;

		// starting from every corner point (endpoint)
		for (auto& [point, neighbors] : m_Adjacency)
		{
			// iterate over its neighbors
			for (auto& neighbor : neighbors)
			{
				for (auto& secondNeighbor : m_Adjacency[neighbor])
				{
					// if neighbor's neighbor connects back to starting point
					if (m_Adjacency[secondNeighbor].contains(point))
					{
						std::set<uint32_t> triangleSet = { point, neighbor, secondNeighbor };
						// .second == true if triangleSet is unique in seenTriangles (can be added)
						if (seenTriangles.insert(triangleSet).second)	
						{
							triples.push_back({ point, neighbor, secondNeighbor });
						}
					}
				}
			}
		}

		return triples;
	}

	void HoleDetector::ExtractEdges(Entity surface)
	{
		auto& s = surface.GetComponent<ar::SurfaceComponent>();
		auto& points = surface.GetComponent<ar::ControlPointsComponent>().Points;
		uint32_t firstPointID = 0, secondPointID = 0;

		for (size_t sv = 0; sv < s.Description.Segments.v; sv++)
			for (size_t su = 0; su < s.Description.Segments.u; su++)
			{
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::BOTTOM, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::RIGHT, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::TOP, su, sv), surface);
				ProcessEdge(ExtractEdge(surface, EdgeInfo::Placement::LEFT, su, sv), surface);
			}
	}

	EdgeInfo HoleDetector::ExtractEdge(Entity surface, EdgeInfo::Placement placement, uint32_t segmentU, uint32_t segmentV)
	{
		auto& size = surface.GetComponent<ar::SurfaceComponent>().Description.Size;
		auto getIndex = [&size](int u, int v) -> uint32_t {
			return v * size.u + u;
		};
		auto& patchPoints = surface.GetComponent<ar::ControlPointsComponent>().Points;
		std::vector<Entity> points, neighbors;

		size_t		baseV = 3 * segmentV,
					baseU = 3 * segmentU;
		uint32_t	firstIndex = 0,
					secondIndex = 0;
		uint32_t	firstPointID = 0,
					secondPointID = 0;
	
		switch (placement)
		{
			case EdgeInfo::Placement::BOTTOM:
			{
				firstIndex = getIndex(baseU + 0, baseV + 0);
				secondIndex = getIndex(baseU + 3, baseV + 0);
				firstPointID = patchPoints[firstIndex].GetID();
				secondPointID = patchPoints[secondIndex].GetID();

				for (int offsetU = 0; offsetU < 4; offsetU++)
				{
					points.push_back(patchPoints[getIndex(baseU + offsetU, baseV)]);
					neighbors.push_back(patchPoints[getIndex(baseU + offsetU, baseV + 1)]);
				}

				break;
			}
			case EdgeInfo::Placement::RIGHT:
			{
				firstIndex = getIndex(baseU + 3, baseV + 0);
				secondIndex = getIndex(baseU + 3, baseV + 3);
				firstPointID = patchPoints[firstIndex].GetID();
				secondPointID = patchPoints[secondIndex].GetID();

				for (int offsetV = 0; offsetV < 4; offsetV++)
				{
					points.push_back(patchPoints[getIndex(baseU + 3, baseV + offsetV)]);
					neighbors.push_back(patchPoints[getIndex(baseU + 2, baseV + offsetV)]);
				}

				break;
			}
			case EdgeInfo::Placement::TOP:
			{
				firstIndex = getIndex(baseU + 3, baseV + 3);
				secondIndex = getIndex(baseU + 0, baseV + 3);
				firstPointID = patchPoints[firstIndex].GetID();
				secondPointID = patchPoints[secondIndex].GetID();

				for (int offsetU = 3; offsetU >= 0; offsetU--)
				{
					points.push_back(patchPoints[getIndex(baseU + offsetU, baseV + 3)]);
					neighbors.push_back(patchPoints[getIndex(baseU + offsetU, baseV + 2)]);
				}

				break;
			}
		
			case EdgeInfo::Placement::LEFT:
			{
				firstIndex = getIndex(baseU + 0, baseV + 3);
				secondIndex = getIndex(baseU + 0, baseV + 0);
				firstPointID = patchPoints[firstIndex].GetID();
				secondPointID = patchPoints[secondIndex].GetID();

				for (int offsetV = 3; offsetV >= 0; offsetV--)
				{
					points.push_back(patchPoints[getIndex(baseU + 0, baseV + offsetV)]);
					neighbors.push_back(patchPoints[getIndex(baseU + 1, baseV + offsetV)]);
				}

				break;
			}
		}
		return { placement, {firstPointID, secondPointID}, points, neighbors, surface };
	}

	void HoleDetector::ProcessEdge(EdgeInfo edge, Entity surface)
	{
		auto& points = surface.GetComponent<ar::ControlPointsComponent>().Points;
		uint32_t firstEndpointID = edge.EndpointIDs.first;
		uint32_t secondEndpointID = edge.EndpointIDs.second;
		
		// Both endpoints the same - edge collapsed to a single point
		if (firstEndpointID == secondEndpointID)
			return;

		// Adjacency info (only point connectivity)
		m_Adjacency[firstEndpointID].insert(secondEndpointID);
		m_Adjacency[secondEndpointID].insert(firstEndpointID);
		
		// Edge info (full info for reproduction)
		m_EdgeTable[EdgeKey(firstEndpointID, secondEndpointID)] = edge;	

		EdgeInfo edgeReverse(edge);
		std::swap(edgeReverse.EndpointIDs.first, edgeReverse.EndpointIDs.second);
		std::reverse(edgeReverse.Points.begin(), edgeReverse.Points.end());
		std::reverse(edgeReverse.Neighbors.begin(), edgeReverse.Neighbors.end());
		m_EdgeTable[EdgeKey(secondEndpointID, firstEndpointID)] = edgeReverse;
	}

	std::vector<ar::Hole> HoleDetector::ConvertTriplesToHoles(std::vector<std::array<uint32_t, 3>> triples)
	{
		std::vector<ar::Hole> holes;
		for (const auto& triple : triples)
		{
			Hole h({
				m_EdgeTable[EdgeKey(triple[0], triple[1])],
				m_EdgeTable[EdgeKey(triple[1], triple[2])],
				m_EdgeTable[EdgeKey(triple[2], triple[0])]
				}, triple);

			holes.push_back(std::move(h));
		}
		return holes;
	}

	void HoleDetector::PrintAdjacency()
	{
		std::string logMessage;
		for (auto& entry : m_Adjacency)
		{
			logMessage += std::to_string(entry.first) + ": {";
			for (auto& ref : entry.second)
				logMessage += std::to_string(ref) + " ";
			logMessage += "}\n";
		}
		AR_TRACE(logMessage);
	}

	void HoleDetector::PrintEdgeTable()
	{
		std::string logMessage;
		for (auto& entry : m_EdgeTable)
		{
			logMessage += "[(" + std::to_string(entry.first.first) + ", " + std::to_string(entry.first.second) + ")]: "
				+ "patch " + std::to_string(entry.second.Patch.GetID()) + ", grid index (a="
				+ std::to_string(entry.second.EndpointIDs.first) + ", b=" + std::to_string(entry.second.EndpointIDs.second) + ")\n";
		}
		AR_TRACE(logMessage);
	}

}