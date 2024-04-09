#pragma once
#include <functional>

#define DECL_BINDON_LAMBDA(Delegate, ...) \
	void Bind##Delegate(const std::function<void(__VA_ARGS__)>& Func) \
	{ \
		Delegate.AddLambda(Func); \
	}

#define DECL_BINDON_LOCAL(Delegate, ...) \
	template <typename T, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, T>>> \
	void Bind##Delegate(T* Object, void (T::*Func)(__VA_ARGS__)) \
	{ \
		Delegate.AddUObject(Object, Func); \
	}

#define DECL_BINDON_LOCAL_CONST(Delegate, ...) \
	template <typename T, typename ObjectLock = std::enable_if_t<std::is_base_of_v<UObject, T>>> \
	void Bind##Delegate(const T* Object, void (T::*Func)(__VA_ARGS__) const) \
	{ \
		Delegate.AddUObject(Object, Func); \
	}

#define DECL_BINDON(Delegate, ...) \
	DECL_BINDON_LAMBDA(Delegate, __VA_ARGS__) \
	DECL_BINDON_LOCAL(Delegate, __VA_ARGS__) \
	DECL_BINDON_LOCAL_CONST(Delegate, __VA_ARGS__)


template <typename T>
FORCEINLINE T PrintErrorAndReturnDefault(const FString& Message, const UObject* Object)
{
	UE_LOG(LogTemp, Error, TEXT("%s: %s"), *Object->GetName(), *Message);
	return T();
}