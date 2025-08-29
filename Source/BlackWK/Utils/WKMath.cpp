// Fill out your copyright notice in the Description page of Project Settings.


#include "WKMath.h"

float FWKMath::CalculateDirection(const FVector& InVector, const FVector& InForward, const FVector& InRight)
{
	if (InVector.IsNearlyZero() || InForward.IsNearlyZero())
	{
		return 0.f;
	}
	
	// cross product result vector is determined by right-hand rule.

	const FVector PlaneNormal = FVector::CrossProduct(InRight, InForward); // up vector
	const FVector NormalizedVec = FVector::VectorPlaneProject(InVector, PlaneNormal).GetSafeNormal();
	const FVector Forward = InForward.GetSafeNormal();
	const FVector Right = FVector::CrossProduct(Forward, PlaneNormal).GetSafeNormal();

	//check((InForward | InRight) == 0);

	// get a cos(alpha) of forward vector vs vec
	const float ForwardCosAngle = FVector::DotProduct(Forward, NormalizedVec);
	// now get the alpha and convert to degree
	float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

	// depending on where right vector is, flip it (means InVector is on the left side of InForward)
	const float RightCosAngle = FVector::DotProduct(Right, NormalizedVec);
	if (RightCosAngle < 0)
	{
		ForwardDeltaDegree *= -1;
	}

	return ForwardDeltaDegree;
}

float FWKMath::CalculateDirection(const FVector& InVector, const FVector& InForward)
{
	if (InVector.IsNearlyZero() || InForward.IsNearlyZero())
	{
		return 0.f;
	}
	
	const FVector NormalizedVec = FVector::VectorPlaneProject(InVector, FVector::UpVector).GetSafeNormal();
	const FVector Forward = InForward.GetSafeNormal2D();
	const FVector Right = FVector::CrossProduct(FVector::UpVector, InForward);

	//check((InForward | InRight) == 0);

	// get a cos(alpha) of forward vector vs vec
	const float ForwardCosAngle = FVector::DotProduct(Forward, NormalizedVec);
	// now get the alpha and convert to degree
	float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

	// depending on where right vector is, flip it (means InVector is on the left side of InForward)
	const float RightCosAngle = FVector::DotProduct(Right, NormalizedVec);
	if (RightCosAngle < 0)
	{
		ForwardDeltaDegree *= -1;
	}

	return ForwardDeltaDegree;
}

float FWKMath::CalculateDirection(const FVector& InVector, const FRotator& BaseRotation)
{
	if (InVector.IsNearlyZero())
	{
		return 0.f;
	}
	
	const FMatrix RotMatrix = FRotationMatrix(BaseRotation);
	const FVector Forward = RotMatrix.GetScaledAxis(EAxis::X);
	const FVector Right = RotMatrix.GetScaledAxis(EAxis::Y);
	const FVector NormalizedVec = FVector::VectorPlaneProject(InVector, RotMatrix.GetScaledAxis(EAxis::Z)).GetSafeNormal();

	//check((InForward | InRight) == 0);

	// get a cos(alpha) of forward vector vs vec
	const float ForwardCosAngle = FVector::DotProduct(Forward, NormalizedVec);
	// now get the alpha and convert to degree
	float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

	// depending on where right vector is, flip it (means InVector is on the left side of InForward)
	const float RightCosAngle = FVector::DotProduct(Right, NormalizedVec);
	if (RightCosAngle < 0)
	{
		ForwardDeltaDegree *= -1;
	}

	return ForwardDeltaDegree;
}
