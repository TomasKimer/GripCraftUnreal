#pragma once

template<typename T>
class TArray3D final
{
public:
	TArray3D(size_t SizeX, size_t SizeY, size_t SizeZ)
	{
		this->SizeX = SizeX;
		this->SizeY = SizeY;
		this->SizeZ = SizeZ;

		Array = new T[Count()];
	}

	~TArray3D()
	{
		delete[] Array;
	}

	FORCEINLINE T& Get(int X, int Y, int Z)
	{
		return Array[FlattenIndex(X, Y, Z)];
	}

	void ClearData()
	{
		memset(Array, 0, Count() * sizeof(T));
	}

private:
	T* Array;

	size_t SizeX;
	size_t SizeY;
	size_t SizeZ;

	FORCEINLINE int FlattenIndex(int X, int Y, int Z) const
	{
		check(X >= 0 && X < SizeX);
		check(Y >= 0 && Y < SizeY);
		check(Z >= 0 && Z < SizeZ);

		return X + SizeX * (Y + SizeY * Z);
	}

	FORCEINLINE int Count() const
	{
		return SizeX * SizeY * SizeZ;
	}
};
