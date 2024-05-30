// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapleAStar.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCoord
{
	GENERATED_BODY()

	FCoord()
	: X(0), Y(0)
	{}

	FCoord(int32 InX, int32 InY)
	: X(InX), Y(InY)
	{}
	
	//
	FORCEINLINE FCoord operator+(const FCoord& Other) const
	{
		return FCoord(X + Other.X, Y + Other.Y);
	}
	
	FORCEINLINE FCoord operator-(const FCoord& Other) const
	{
		return FCoord(X - Other.X, Y - Other.Y);
	}
	
	FORCEINLINE FCoord operator*(const FCoord& Other) const
	{
		return FCoord(X * Other.X, Y * Other.Y);
	}
	
	FORCEINLINE FCoord operator/(const FCoord& Other) const
	{
		return FCoord(X / Other.X, Y / Other.Y);
	}

	//
	FORCEINLINE FCoord operator+=(const FCoord& Other)
	{
		X += Other.X; Y += Other.Y;
		return *this;
	}
	
	FORCEINLINE FCoord operator-=(const FCoord& Other)
	{
		X -= Other.X; Y -= Other.Y;
		return *this;
	}
	
	FORCEINLINE FCoord operator*=(const FCoord& Other)
	{
		X *= Other.X; Y *= Other.Y;
		return *this;
	}
	
	FORCEINLINE FCoord operator/=(const FCoord& Other)
	{
		X /= Other.X; Y /= Other.Y;
		return *this;
	}

	//
	FORCEINLINE bool operator==(const FCoord& Other) const
	{
		return Equals(Other);
	}

	FORCEINLINE bool operator!=(const FCoord& Other) const
	{
		return !Equals(Other);
	}
	
	FORCEINLINE bool Equals(const FCoord& Other) const
	{
		return GetID() == Other.GetID();
	}
	
	FORCEINLINE int64 GetID() const
	{
		return *(int64*)this;
	}

	// x & y.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y = 0;
};

template<>
struct TStructOpsTypeTraits<FCoord> : public TStructOpsTypeTraitsBase2<FCoord>
{
	enum { WithIdenticalViaEquality = true };
};

// 实现 FCoord 类型 hash.
FORCEINLINE uint32 GetTypeHash(const FCoord& Key)
{
	return GetTypeHash(Key.GetID());
}

USTRUCT()
struct FAStarNode
{
	GENERATED_BODY()

	UPROPERTY()
	FCoord Coord = FCoord();
	UPROPERTY()
	FCoord ParentCoord = FCoord();
	UPROPERTY()
	float G = 0.F;
	UPROPERTY()
	float H = 0.F;
	
	FORCEINLINE float GetF() const { return G + H; }

	// todo: 弃用.
	FAStarNode* ParentNode = nullptr;
};

USTRUCT(BlueprintType)
struct FAStarPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCoord> PathCoords;
};

USTRUCT(BlueprintType)
struct FAStarBlockInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfLayers = 0;
};

UCLASS()
class RTS_API AAStarPathGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AAStarPathGenerator();

	UFUNCTION(BlueprintCallable, Category = "A* PathGenerator")
	FAStarPath GeneratePath(FCoord StartCoord, FCoord TargetCoord);

	UFUNCTION(BlueprintCallable, Category = "A* PathGenerator")
	void AddBlock(TArray<FCoord> Coords);
	UFUNCTION(BlueprintCallable, Category = "A* PathGenerator")
	void RemoveBlock(TArray<FCoord> Coords);
	UFUNCTION(BlueprintCallable, Category = "A* PathGenerator")
	void EmptyBlock();
	UFUNCTION(BlueprintPure, Category = "A* PathGenerator")
	bool HasBlock(FCoord Coord) const;
	UFUNCTION(BlueprintPure, Category = "A* PathGenerator")
	TArray<FCoord> GetBlockArray() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<FCoord, FAStarBlockInfo> BlockInfos;
	
	TArray<FAStarNode*> OpenNodes;
	TArray<FAStarNode*> ClosedNodes;

	bool IsUsableCoord(const FCoord& Coord) const;
	bool IsNodeClosed(const FCoord& Coord) const;
	bool IsNodeOpen(const FCoord& Coord, FAStarNode*& OutNode) const;
	float CalculateH(const FCoord& StartCoord, const FCoord& TargetCoord) const;
	TArray<FCoord> GetPathFromNode(FAStarNode* EndNode) const;
};

UCLASS()
class AAStarPathfinding : public AAStarPathGenerator
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class RTS_API UMapleAStar : public UObject
{
	GENERATED_BODY()
};
