#pragma once
#include <functional>
#include "Containers/UnrealString.h"

#define DECL_BINDON_NATIVE(Delegate, ...) \
	FDelegateHandle Bind##Delegate(TMulticastDelegate<void(__VA_ARGS__)>::FDelegate& Func) \
	{ \
			return Delegate.Add(Func); \
	}

#define DECL_BINDON_NATIVE_CONST(Delegate, ...) \
	FDelegateHandle Bind##Delegate(const TMulticastDelegate<void(__VA_ARGS__)>::FDelegate& Func) \
	{ \
			return Delegate.Add(Func); \
	}

#define DECL_BINDON_LAMBDA(Delegate, ...) \
	FDelegateHandle Bind##Delegate(const std::function<void(__VA_ARGS__)>& Func) \
	{ \
		return Delegate.AddLambda(Func); \
	}

#define DECL_BINDON_LOCAL(Delegate, ...) \
	template <typename Derived, typename Base, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, Derived>>, typename ULock = std::enable_if_t<std::is_base_of_v<Base, Derived>>> \
	FDelegateHandle Bind##Delegate(Derived* Object, void (Base::*Func)(__VA_ARGS__)) \
	{ \
		return Delegate.AddUObject(Object, Func); \
	}

#define DECL_BINDON_LOCAL_CONST(Delegate, ...) \
	template <typename Derived, typename Base, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, Derived>>, typename ULock = std::enable_if_t<std::is_base_of_v<Base, Derived>>> \
	FDelegateHandle Bind##Delegate(const Derived* Object, void (Base::*Func)(__VA_ARGS__) const) \
	{ \
		return Delegate.AddUObject(Object, Func); \
	}

#define DECL_UNBINDON(Delegate) \
	void Unbind##Delegate(const FDelegateHandle& Handle) \
	{ \
		Delegate.Remove(Handle); \
	}

#define DECL_BINDON(Delegate, ...) \
	DECL_BINDON_LAMBDA(Delegate, __VA_ARGS__) \
	DECL_BINDON_LOCAL(Delegate, __VA_ARGS__) \
	DECL_BINDON_LOCAL_CONST(Delegate, __VA_ARGS__) \
	DECL_BINDON_NATIVE(Delegate, __VA_ARGS__) \
	DECL_BINDON_NATIVE_CONST(Delegate, __VA_ARGS__) \
	DECL_UNBINDON(Delegate) \

#define STRINGIFY(x) #x


template <typename T>
FORCEINLINE T PrintErrorAndReturnDefault(const FString& Message, const UObject* Object)
{
	UE_LOG(LogTemp, Error, TEXT("%s: %s"), *Object->GetName(), *Message);
	return T();
}

#define LOG_FUNC(CategoryName, Verbosity, String) \
	UE_LOG(CategoryName, Verbosity, TEXT("%hs: %s"), __FUNCTION__, TEXT(String))

#define LOG_FUNC_RAW(CategoryName, Verbosity, String) \
	UE_LOG(CategoryName, Verbosity, TEXT("%hs: %s"), __FUNCTION__, String)

#define LOG_FUNC_PRINTF(CategoryName, Verbosity, StringAndFormat, ...) \
	UE_LOG(CategoryName, Verbosity, TEXT("%hs: %s"), __FUNCTION__, *FString::Printf(TEXT(StringAndFormat), __VA_ARGS__))

template <typename T , typename... Args>
FORCEINLINE void ExecuteServer(
	AActor* ActorContext, 
	void (T::*ClientFunction)(Args...), 
	void (T::*ServerFunction)(Args...),
	Args... Arguments
)
{
	
	if (ActorContext->HasAuthority() || IsRunningDedicatedServer() || ActorContext->GetNetMode() == ENetMode::NM_ListenServer)
	{
		(static_cast<T*>(ActorContext)->*ServerFunction)(Arguments...);
	}
	else if (!ActorContext->HasAuthority())
	{
		(static_cast<T*>(ActorContext)->*ClientFunction)(Arguments...);
	}
}

template <typename T , typename... Args>
FORCEINLINE void ExecuteServer(
	const AActor* ActorContext, 
	void (T::*ClientFunction)(Args...) const, 
	void (T::*ServerFunction)(Args...) const,
	Args... Arguments
)
{
	if (ActorContext->HasAuthority() || IsRunningDedicatedServer() || ActorContext->GetNetMode() == ENetMode::NM_ListenServer)
	{
		(static_cast<const T*>(ActorContext)->*ServerFunction)(Arguments...);
	}
	else if (!ActorContext->HasAuthority())
	{
		(static_cast<const T*>(ActorContext)->*ClientFunction)(Arguments...);
	}
}

template <typename T , typename... Args>
FORCEINLINE void ExecuteServer(
	AActor* ActorContext, 
	void (T::*ClientFunction)(Args...), 
	void (T::*ServerFunction)(Args...) const,
	Args... Arguments
)
{
	if (ActorContext->HasAuthority() || IsRunningDedicatedServer() || ActorContext->GetNetMode() == ENetMode::NM_ListenServer)
	{
		(static_cast<const T*>(ActorContext)->*ServerFunction)(Arguments...);
	}
	else if (!ActorContext->HasAuthority())
	{
		(static_cast<T*>(ActorContext)->*ClientFunction)(Arguments...);
	}
}

template <typename T , typename... Args>
FORCEINLINE void ExecuteServer(
	AActor* ActorContext, 
	void (T::*ClientFunction)(Args...) const, 
	void (T::*ServerFunction)(Args...),
	Args... Arguments
)
{
	if (ActorContext->HasAuthority() || IsRunningDedicatedServer() || ActorContext->GetNetMode() == ENetMode::NM_ListenServer)
	{
		(static_cast<T*>(ActorContext)->*ServerFunction)(Arguments...);
	}
	else if (!ActorContext->HasAuthority())
	{
		(static_cast<const T*>(ActorContext)->*ClientFunction)(Arguments...);
	}
}

template<typename T>
inline static FString EnumToString(const T Value)
{
	return UEnum::GetValueAsString(Value);
}

// Creates a new object and copy the properties of every components;
static AA_Collectable* CloneChildActor(AA_Collectable* InObject, const std::function<void(AActor*)>& InDeferredFunction);