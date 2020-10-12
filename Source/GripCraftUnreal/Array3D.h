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

		Array = new T[Num()];
	}

	~TArray3D()
	{
		delete[] Array;
	}

	FORCEINLINE T& Get(int X, int Y, int Z) const
	{
		return Array[FlattenIndex(X, Y, Z)];
	}

	FORCEINLINE size_t Num() const
	{
		return SizeX * SizeY * SizeZ;
	}

	friend FArchive& operator<<(FArchive& Ar, TArray3D*& Array)
	{
		if (Ar.IsLoading() == true)
		{
			size_t SizeX, SizeY, SizeZ;

			Ar << SizeX;
			Ar << SizeY;
			Ar << SizeZ;

			if (Array != nullptr)
				delete Array;

			Array = new TArray3D<T>(SizeX, SizeY, SizeZ);

			for (size_t i = 0; i < Array->Num(); ++i)
			{
				Ar << Array->Array[i];
			}
		}
		else
		{
			Ar << Array->SizeX;
			Ar << Array->SizeY;
			Ar << Array->SizeZ;

			for (size_t i = 0; i < Array->Num(); ++i)
			{
				Ar << Array->Array[i];
			}
		}

		return Ar;
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
};
