//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EcsEventElement.generated.h"

class AEcsContext;

UINTERFACE(MinimalAPI, Blueprintable)
class UEcsEventElement : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for elements in the ECS event chain (Systems or Nested Contexts).
 */
class UECS_API IEcsEventElement
{
	GENERATED_BODY()

public:
	/**
	 * Initialise the element with the given context and registry.
	 * Can be implemented in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "ECS|Event Element")
	void Initialize(AEcsContext* InContext);

	/**
	 * Update the element. Called every frame if part of an update loop.
	 * Can be implemented in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "ECS|Event Element")
	void Update(float DeltaTime);

	/**
	 * Deinitialise the element.
	 * Can be implemented in Blueprint.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "ECS|Event Element")
	void Deinitialize();
};

/**
 * Utility to wrap a UObject into a TScriptInterface<IEcsEventElement>.
 * Returns an empty interface if the object is null or does not implement IEcsEventElement.
 */
inline TScriptInterface<IEcsEventElement> MakeEcsEventElement(UObject* Obj)
{
	TScriptInterface<IEcsEventElement> Interface;
	if (Obj && Obj->Implements<UEcsEventElement>())
	{
		Interface.SetObject(Obj);
		Interface.SetInterface(Cast<IEcsEventElement>(Obj));
	}
	return Interface;
}
