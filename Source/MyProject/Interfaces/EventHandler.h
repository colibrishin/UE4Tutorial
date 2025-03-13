// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyProject/Private/Utilities.hpp"
#include "MyProject/Interfaces/EventableContext.h"
#include "UObject/Interface.h"
#include "EventHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEventHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API IEventHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	template <typename T>
	TMap<FText , T>& GetParameterMap() 
	{
		static TMap<FText , T> TTypeMap{};
		return TTypeMap;
	}

	template <typename T>
	bool GetParameter( const FText& Key, T** OutValue )
	{
		check( OutValue != nullptr );
		auto& DesignatedMap = GetParameterMap<T>();
		if ( DesignatedMap.Contains( Key ) )
		{
			*OutValue = &DesignatedMap[ Key ];
			return true;
		}

		*OutValue = nullptr;
		return false;
	}

	template <typename T>
	void AppendParameter( const FText& Key , T&& Value ) 
	{
		auto& DesignatedMap = GetMap<T>();
		if ( !DesignatedMap.Contains( Key ) )
		{
			DesignatedMap.Append( Key , std::move( Value ) );
		}
	}

	template <typename T>
	void SetParameter( const FText& Key , const T& InValue )
	{
		if ( T* Value = nullptr; GetValue( Key , &Value ) ) 
		{
			*Value = InValue;
		}
		else 
		{
			LOG_FUNC( LogTemp, Warning, "Unable to find the parameter ")
		}
	}

	virtual void DoEvent( TScriptInterface<IEventableContext> InContext) = 0;

};
