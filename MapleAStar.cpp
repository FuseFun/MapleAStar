// Fill out your copyright notice in the Description page of Project Settings.


#include "MapleAStar/MapleAStar.h"

AAStarPathGenerator::AAStarPathGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAStarPathGenerator::BeginPlay()
{
	Super::BeginPlay();
}

FAStarPath AAStarPathGenerator::GeneratePath(FCoord StartCoord, FCoord TargetCoord)
{
	FAStarPath path;
	path.PathCoords.Empty();

	if (!IsUsableCoord(StartCoord) || !IsUsableCoord(TargetCoord))
		return path;

	FAStarNode* startNode = new FAStarNode();
	startNode->Coord = StartCoord;
	startNode->ParentNode = nullptr;
	startNode->G = 0.F;
	startNode->H = CalculateH(StartCoord, TargetCoord);

	OpenNodes.Add(startNode);

	while (OpenNodes.Num() > 0)
	{
		FAStarNode* currentNode = OpenNodes[0];

		// Find the node with the lowest F value
		for (int32 index = 1; index < OpenNodes.Num(); ++index)
		{
			if (OpenNodes[index]->GetF() < currentNode->GetF())
			{
				currentNode = OpenNodes[index];
			}
		}

		OpenNodes.Remove(currentNode);
		ClosedNodes.Add(currentNode);

		if (currentNode->Coord.Equals(TargetCoord))
		{
			// Found the target, construct the path
			path.PathCoords = GetPathFromNode(currentNode);
			break;
		}

		// Add other neighbors if needed
		FCoord surroundCoords[] = {
			FCoord(1, 0), // F
			FCoord(-1, 0), // B
			FCoord(0, 1), // R
			FCoord(0, -1), // L
			
			FCoord(1, 1), // FR
			FCoord(1, -1), // FL
			FCoord(-1, 1), // BR
			FCoord(-1, -1), // BL
		};

		for (const FCoord& surroundCoord : surroundCoords)
		{
			const FCoord newCoord = currentNode->Coord + surroundCoord;

			if (!IsUsableCoord(newCoord) || IsNodeClosed(newCoord))
			{
				continue;
			}

			FAStarNode* NeighborNode = nullptr;

			if (!IsNodeOpen(newCoord, NeighborNode))
			{
				NeighborNode = new FAStarNode();
				NeighborNode->Coord = newCoord;
				NeighborNode->ParentNode = currentNode;
				NeighborNode->G = currentNode->G + 1.F; // Assuming uniform cost
				NeighborNode->H = CalculateH(newCoord, TargetCoord);

				OpenNodes.Add(NeighborNode);
			}
			else
			{
				const float newG = currentNode->G + 1.F; // Assuming uniform cost

				if (newG < NeighborNode->G)
				{
					NeighborNode->ParentNode = currentNode;
					NeighborNode->G = newG;
				}
			}
		}
	}

	// Cleanup
	for (FAStarNode* node : OpenNodes)
	{
		delete node;
	}

	for (FAStarNode* node : ClosedNodes)
	{
		delete node;
	}

	OpenNodes.Empty();
	ClosedNodes.Empty();

	return path;
}

void AAStarPathGenerator::AddBlock(TArray<FCoord> Coords)
{
	for(const FCoord& coord : Coords)
	{
		FAStarBlockInfo& blockInfo = BlockInfos.FindOrAdd(coord);
		++ blockInfo.NumberOfLayers;
	}
}

void AAStarPathGenerator::RemoveBlock(TArray<FCoord> Coords)
{
	TArray<FCoord> removeCoords;
	
	for(const FCoord& coord : Coords)
	{
		if (FAStarBlockInfo* blockInfoPtr = BlockInfos.Find(coord))
		{
			FAStarBlockInfo& blockInfo = *blockInfoPtr;
			
			if (blockInfo.NumberOfLayers > 1)
				-- blockInfo.NumberOfLayers;
			else
				removeCoords.Add(coord);
		}
	}

	for (const FCoord& removeCoord : removeCoords)
	{
		BlockInfos.FindAndRemoveChecked(removeCoord);
	}
}

void AAStarPathGenerator::EmptyBlock()
{
	BlockInfos.Empty();
}

bool AAStarPathGenerator::HasBlock(FCoord Coord) const
{
	if (const FAStarBlockInfo* blockInfoPtr = BlockInfos.Find(Coord))
		return blockInfoPtr->NumberOfLayers > 0;
	return false;
}

TArray<FCoord> AAStarPathGenerator::GetBlockArray() const
{
	TArray<FCoord> returnValue;
	BlockInfos.GetKeys(returnValue);
	return returnValue;
}

bool AAStarPathGenerator::IsUsableCoord(const FCoord& Coord) const
{
	// Implement your own validation logic here
	// For example, check if the location is within the bounds of your game world
	return !HasBlock(Coord);
}

bool AAStarPathGenerator::IsNodeClosed(const FCoord& Coord) const
{
	for (FAStarNode* node : ClosedNodes)
	{
		if (node->Coord.Equals(Coord))
		{
			return true;
		}
	}
	return false;
}

bool AAStarPathGenerator::IsNodeOpen(const FCoord& Coord, FAStarNode*& OutNode) const
{
	for (FAStarNode* Node : OpenNodes)
	{
		if (Node->Coord.Equals(Coord))
		{
			OutNode = Node;
			return true;
		}
	}
	return false;
}

float AAStarPathGenerator::CalculateH(const FCoord& StartCoord, const FCoord& TargetCoord) const
{
	const FVector2D startCoord(StartCoord.X, StartCoord.Y);
	const FVector2D targetCoord(TargetCoord.X, TargetCoord.Y);

	// Dijkstra.
	return 0.F;
	
	// Euclidean Distance.
	// return (targetCoord - startCoord).Size();
	
	// Manhattan Distance.
	// return FMath::Abs(targetCoord.X - startCoord.X) + FMath::Abs(targetCoord.Y - startCoord.Y);
}

TArray<FCoord> AAStarPathGenerator::GetPathFromNode(FAStarNode* EndNode) const
{
	TArray<FCoord> Paths;

	FAStarNode* currentNode = EndNode;

	while (currentNode != nullptr)
	{
		Paths.Add(currentNode->Coord);
		currentNode = currentNode->ParentNode;
	}

	// 反转路径，使其从起点到目标点
	Algo::Reverse(Paths);

	return Paths;
}
