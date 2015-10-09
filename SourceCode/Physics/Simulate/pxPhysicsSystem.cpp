/*
=============================================================================
	File:	pxPhysicsSystem.cpp
	Desc:
=============================================================================
*/
#include <Physics_PCH.h>
#pragma hdrstop
#include <Physics.h>

#include <Physics/Collide/Shape/pxShape_Convex.h>
#include <Physics/Collide/Shape/pxShape_HalfSpace.h>
#include <Physics/Collide/Shape/pxShape_Sphere.h>
#include <Physics/Collide/Shape/pxShape_Box.h>
#include <Physics/Collide/Shape/pxShape_StaticBSP.h>

pxShape* pxUtil_CreateShape( const pxcShapeType eShapeType )
{
	switch( eShapeType )
	{
	case PX_SHAPE_HALFSPACE :
		return new pxShape_HalfSpace();

	case PX_SHAPE_SPHERE :
		return new pxShape_Sphere();

	case PX_SHAPE_STATIC_BSP :
		return new pxShape_StaticBSP();

	default:
		Unreachable;
		return nil;
	}
}


namespace Physics
{
	struct SPrivateData
	{
		pxCollisionDispatcher	collisionDispatcher;

		TList< pxShape* >		collisionShapes;

		TList< pxMaterial >		physicsMaterials;

	public:
		SPrivateData()
		{
			physicsMaterials.Reserve(16);

			{
				pxMaterial& defaultMaterial = physicsMaterials.Add();
				defaultMaterial.friction = 0.5f;
				defaultMaterial.restitution = 0.1f;
			}
		}
		~SPrivateData()
		{
			const UINT numShapes = collisionShapes.Num();
			for( UINT iShape = 0; iShape < numShapes; iShape++ )
			{
				collisionShapes[ iShape ]->DestroySelf();
			}
			collisionShapes.Empty();
		}
	};

	static TBlob16<SPrivateData>	gPrivateData;
	#define gData	gPrivateData.Get()

	static NiftyCounter	gPhysSystemInitCounter;


	//-----------------------------------------------------------------------------------------
	pxCollisionDispatcher* GetCollisionDispatcher()
	{
		return &gData.collisionDispatcher;
	}
	//-----------------------------------------------------------------------------------------
	pxShape::Handle AddCollisionShape( pxShape* shape )
	{
		const UINT newIndex = gData.collisionShapes.Num();

		gData.collisionShapes.Add( shape );

		pxShape::Handle	newHandle( shape->GetType(), newIndex );

		return newHandle;
	}
	//-----------------------------------------------------------------------------------------
	pxShape* GetCollisionShape( pxShape::Handle shape )
	{
		return gData.collisionShapes[ shape.index ];
	}
	//-----------------------------------------------------------------------------------------
	void RemoveCollisionShape( pxShape::Handle shape )
	{
		UNDONE;
		//const UINT shapeIndex = shape.index;
		//pxShape* shapePtr = gData.collisionShapes[ shapeIndex ];
		//shapePtr->m_internalIndex = INDEX_NONE;
	}
	//-----------------------------------------------------------------------------------------
	static void InitializeMaterial( const pxMaterialDesc& desc, pxMaterial& rMaterial )
	{
		rMaterial.friction = pxCombineFriction( desc.staticFriction, desc.dynamicFriction );
		rMaterial.restitution = desc.restitution;
	}
	//-----------------------------------------------------------------------------------------
	pxMaterial::Handle NewMaterial( const pxMaterialDesc& desc )
	{
		const UINT iNewMaterialIndex = gData.physicsMaterials.Num();
		pxMaterial& rNewMaterial = gData.physicsMaterials.Add();

		InitializeMaterial( desc, rNewMaterial );

		return iNewMaterialIndex;
	}
	//-----------------------------------------------------------------------------------------
	pxMaterial& GetMaterial( pxMaterial::Handle materialId )
	{
		return gData.physicsMaterials[ materialId ];
	}
	//-----------------------------------------------------------------------------------------
	void RemoveMaterial( pxMaterial::Handle materialId )
	{
		UNDONE;
	}
	//-----------------------------------------------------------------------------------------
	void Initialize()
	{
		if( gPhysSystemInitCounter.IncRef() )
		{
			gPrivateData.Construct();

#if MX_DEVELOPER
			DBGOUT("=== Initializing Physics system ===\n");
			DBGOUT("sizeof pxCollideable = %u\n", (UINT)sizeof pxCollideable);
			DBGOUT("sizeof pxRigidBody = %u\n", (UINT)sizeof pxRigidBody);
			DBGOUT("sizeof pxWorld = %u\n", (UINT)sizeof pxWorld);

			DBGOUT("sizeof pxContactPoint = %u\n", (UINT)sizeof pxContactPoint);
			DBGOUT("sizeof pxContactManifold = %u\n", (UINT)sizeof pxContactManifold);

			DBGOUT("sizeof BspNode = %u\n", (UINT)sizeof BspNode);
			DBGOUT("===================================\n");
#endif // MX_DEVELOPER
		}
	}
	//-----------------------------------------------------------------------------------------
	void Shutdown()
	{
		if( gPhysSystemInitCounter.DecRef() )
		{
			gPrivateData.Destruct();
		}
	}
	//-----------------------------------------------------------------------------------------
	void Serialize( mxArchive& archive )
	{
		// Serialize collision shapes.
		{
			TList< pxShape* > & collisionShapes = gPrivateData.Get().collisionShapes;

			U4 numShapes = collisionShapes.Num();

			archive && numShapes;

			collisionShapes.SetNum( numShapes );

			if( archive.IsWriting() )
			{
				mxStreamWriter& stream = *archive.IsWriter();

				for( UINT iShape = 0; iShape < numShapes; iShape++ )
				{
					pxShape* shapePtr = collisionShapes[ iShape ];

					const U4 shapeType = shapePtr->GetType();
					stream << shapeType;

					shapePtr->Serialize( archive );
				}
			}
			if( archive.IsReading() )
			{
				mxStreamReader& stream = *archive.IsReader();

				for( UINT iShape = 0; iShape < numShapes; iShape++ )
				{
					const pxcShapeType shapeType = (pxcShapeType)ReadUInt32( stream );
					pxShape* shapePtr = pxUtil_CreateShape( shapeType );
					AssertPtr(shapePtr);

					shapePtr->Serialize( archive );
				}
			}
		}//collision shapes

		// Serialize materials.
		{
			UNDONE;
			//SerializeList( archive, gData.physicsMaterials );
		}//materials
	}

}//namespace Physics

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
