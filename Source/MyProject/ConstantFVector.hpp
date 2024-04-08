#pragma once

struct FConstantFVectorImpl
{
	float X;
	float Y;
	float Z;

	constexpr FConstantFVectorImpl operator+(const FConstantFVectorImpl& V) const
	{
		return FConstantFVectorImpl{X + V.X, Y + V.Y, Z + V.Z};
	}

	constexpr FConstantFVectorImpl operator*(const float Scale) const
	{
		return FConstantFVectorImpl{X * Scale, Y * Scale, Z * Scale};
	}

};

struct FConstantFVector : public FConstantFVectorImpl
{
public:
	constexpr FConstantFVector() noexcept : FConstantFVectorImpl{0.f, 0.f, 0.f} {}

	constexpr FConstantFVector(const float InX, const float InY, const float InZ) noexcept : FConstantFVectorImpl{InX, InY, InZ} {}

	constexpr FConstantFVector(const FConstantFVector& InVector) noexcept : FConstantFVectorImpl{InVector} {}

	constexpr FConstantFVector(const FConstantFVectorImpl& InVector) noexcept : FConstantFVectorImpl{InVector} {}

	FConstantFVector(const FVector& InVector) noexcept : FConstantFVectorImpl{InVector.X, InVector.Y, InVector.Z} {}

	FVector operator+(const FConstantFVector& V) const
	{
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	constexpr FConstantFVector operator+(const FVector& V) const
	{
		return FConstantFVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	constexpr FConstantFVector operator-(const FVector& V) const
	{
		return FConstantFVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	constexpr FConstantFVector operator*(const float Scale) const
	{
		return FConstantFVector(X * Scale, Y * Scale, Z * Scale);
	}

	constexpr FConstantFVector operator/(const float Scale) const
	{
		return FConstantFVector(X / Scale, Y / Scale, Z / Scale);
	}

	constexpr FConstantFVector operator+(const FConstantFVectorImpl& V) const
	{
		return FConstantFVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	constexpr FConstantFVector operator-(const FConstantFVectorImpl& V) const
	{
		return FConstantFVector(X - V.X, Y - V.Y, Z - V.Z);
	}

public:
	constexpr static FConstantFVectorImpl ZeroVector = {0.f, 0.f, 0.f};
	constexpr static FConstantFVectorImpl OneVector = {1.f, 1.f, 1.f};
	constexpr static FConstantFVectorImpl UpVector = {0.f, 0.f, 1.f};
	constexpr static FConstantFVectorImpl DownVector = {0.f, 0.f, -1.f};
	constexpr static FConstantFVectorImpl ForwardVector = {1.f, 0.f, 0.f};
	constexpr static FConstantFVectorImpl BackwardVector = {-1.f, 0.f, 0.f};
	constexpr static FConstantFVectorImpl RightVector = {0.f, 1.f, 0.f};
	constexpr static FConstantFVectorImpl LeftVector = {0.f, -1.f, 0.f};
};

inline FVector operator+(const FVector& lhs, const FConstantFVector& rhs)
{
	return FVector(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
}
