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

	friend FArchive& operator<<(FArchive& Ar, TArray3D*& Obj)
	{
		if (Ar.IsLoading() == true)
		{
			size_t InSizeX, InSizeY, InSizeZ;

			Ar << InSizeX;
			Ar << InSizeY;
			Ar << InSizeZ;

			if (Obj != nullptr)
				delete Obj;

			Obj = new TArray3D<T>(InSizeX, InSizeY, InSizeZ);

			for (size_t i = 0; i < Obj->Num(); ++i)
			{
				Ar << Obj->Array[i];
			}
		}
		else
		{
			Ar << Obj->SizeX;
			Ar << Obj->SizeY;
			Ar << Obj->SizeZ;

			for (size_t i = 0; i < Obj->Num(); ++i)
			{
				Ar << Obj->Array[i];
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
