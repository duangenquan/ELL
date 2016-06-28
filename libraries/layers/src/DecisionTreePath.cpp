////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DecisionTreePath.cpp (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DecisionTreePath.h"

// utilities
#include "Exception.h" 

// stl
#include <stdexcept>
#include <string>
#include <cassert>

namespace layers
{
    const int DecisionTreePath::_currentVersion;

    DecisionTreePath::DecisionTreePath(std::vector<uint64_t> edgeToInteriorNode, CoordinateList splitRuleCoordinates) :
        _edgeToInteriorNode(std::move(edgeToInteriorNode)), _splitRuleCoordinates(std::move(splitRuleCoordinates))
    {}

    uint64_t DecisionTreePath::GetNegativeOutgoingEdgeIndex(uint64_t interiorNodeIndex) const
    {
        return interiorNodeIndex * 2;
    }

    uint64_t DecisionTreePath::GetPositiveOutgoingEdgeIndex(uint64_t interiorNodeIndex) const
    {
        return interiorNodeIndex * 2 + 1;
    }

    uint64_t DecisionTreePath::GetInteriorNodeIndex(uint64_t incomingEdgeIndex) const
    {
        return _edgeToInteriorNode[incomingEdgeIndex];
    }

    void DecisionTreePath::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        if (NumInteriorNodes() == 0)
        {
            return;
        }

        uint64_t interiorNodeIndex = 0;
        do
        {
            auto splitRuleCoordinate = _splitRuleCoordinates[interiorNodeIndex];
            auto splitRuleValue = inputs[splitRuleCoordinate.GetLayerIndex()][splitRuleCoordinate.GetElementIndex()];
            
            // evaluate split rule
            uint64_t edgeIndex;
            if (splitRuleValue <= 0)
            {
                edgeIndex = GetNegativeOutgoingEdgeIndex(interiorNodeIndex);
            }
            else
            {
                edgeIndex = GetPositiveOutgoingEdgeIndex(interiorNodeIndex);
            }

            // set output
            outputs[edgeIndex] = 1.0;
            
            // move to child node
            interiorNodeIndex = GetInteriorNodeIndex(edgeIndex);
        } 
        while (interiorNodeIndex > 0);
    }

    CoordinateIterator DecisionTreePath::GetInputCoordinateIterator(uint64_t index) const
    {
        return _splitRuleCoordinates.GetIterator(index, 1);
    }

    uint64_t DecisionTreePath::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _splitRuleCoordinates.GetRequiredLayerSize(layerIndex);
    }

    void DecisionTreePath::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        std::string operationName;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("edgeToInteriorNode", _edgeToInteriorNode);
            deserializer.Deserialize("splitRuleCoordinates", _splitRuleCoordinates);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "unsupported version: " + version);
        }
    }

    void DecisionTreePath::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("edgeToInteriorNode", _edgeToInteriorNode);
        serializer.Serialize("splitRuleCoordinates", _splitRuleCoordinates);
    }
}

