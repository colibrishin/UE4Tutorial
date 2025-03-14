// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProject/Frameworks/Subsystems/SS_EventGameInstance.h"
#include "SS_EventGameInstance.h"

TScriptInterface<IEventHandler> USS_EventGameInstance::AddEvent( const TSubclassOf<UObject>& Handler )
{
	if ( Handler ) 
	{
		if ( Handlers.Contains( Handler ) )
		{
			return Handlers[ Handler ];
		}
		else 
		{
			UObject* Processor = NewObject<UObject>( GetTransientPackage() , Handler, NAME_None , RF_Transient );
			Handlers.Add( Handler , Processor);

			return Processor;
		}
	}

	return nullptr;
}
