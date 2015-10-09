#pragma once


//////////////////////////////////////////////////////////////////////////

class MeshConverter
{
public:
	struct Settings
	{
		TList<OSPathName>	inputFiles;

		OSPathName	outputDir;

	public:
		template< class S >
		friend S& operator & ( S & serializer, Settings & o )
		{
			return serializer
				& o.inputFiles

				& o.outputDir
				;
		}
	};

public:

	MeshConverter();
	~MeshConverter();

	bool Setup();

	void Convert( const Settings& settings );

	void Close();

private:
	bool Convert( const char* inputFile, const char* outputFile );

private:
	SetupCoreUtil	setupCore;

	irr::IrrlichtDevice *	device;
};

