#pragma once
#include <functional>

#define DECL_BINDON_LAMBDA(Delegate, ...) \
	FDelegateHandle Bind##Delegate(const std::function<void(__VA_ARGS__)>& Func) \
	{ \
		return Delegate.AddLambda(Func); \
	}

#define DECL_BINDON_LOCAL(Delegate, ...) \
	template <typename T, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, T>>> \
	FDelegateHandle Bind##Delegate(T* Object, void (T::*Func)(__VA_ARGS__)) \
	{ \
		return Delegate.AddUObject(Object, Func); \
	}

#define DECL_BINDON_LOCAL_CONST(Delegate, ...) \
	template <typename T, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, T>>> \
	FDelegateHandle Bind##Delegate(const T* Object, void (T::*Func)(__VA_ARGS__) const) \
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
	DECL_UNBINDON(Delegate) \


template <typename T>
FORCEINLINE T PrintErrorAndReturnDefault(const FString& Message, const UObject* Object)
{
	UE_LOG(LogTemp, Error, TEXT("%s: %s"), *Object->GetName(), *Message);
	return T();
}

#define LOG_FUNC(CategoryName, Verbosity, String) \
	UE_LOG(CategoryName, Verbosity, TEXT("%hs: %s"), __FUNCTION__, TEXT(String))