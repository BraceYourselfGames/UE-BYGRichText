// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BYGStylesheetCompiler.h"
#include "KismetCompiler.h"
#include "KismetCompilerModule.h"
#include "ComponentReregisterContext.h"
#include "Components/WidgetComponent.h"


//////////////////////////////////////////////////////////////////////////
// FBYGStylesheetCompiler 

class FBYGStylesheetCompiler : public IBlueprintCompiler
{

public:
	FBYGStylesheetCompiler();

	virtual bool CanCompile(const UBlueprint* Blueprint) override;
	//virtual void PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	virtual void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results) override;
	//virtual void PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	//virtual bool GetBlueprintTypesForClass(UClass* ParentClass, UClass*& OutBlueprintClass, UClass*& OutBlueprintGeneratedClass) const override;

private:
	/** The temporary variable that captures and reinstances components after compiling finishes. */
	TComponentReregisterContext<UWidgetComponent>* ReRegister;

	/**
	* The current count on the number of compiles that have occurred.  We don't want to re-register components until all
	* compiling has stopped.
	*/
	int32 CompileCount;

};

//////////////////////////////////////////////////////////////////////////
// FBYGStylesheetCompilerContext


class FBYGStylesheetCompilerContext : public FKismetCompilerContext
{
protected:
	typedef FKismetCompilerContext Super;

public:
	FBYGStylesheetCompilerContext( UBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions );
	virtual ~FBYGStylesheetCompilerContext();

protected:
	//void ValidateWidgetNames();

	// FKismetCompilerContext
	//virtual UEdGraphSchema_K2* CreateSchema() override;
	//virtual void CreateFunctionList() override;
	//virtual void SpawnNewClass(const FString& NewClassName) override;
	//virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	//virtual void PrecompileFunction(FKismetFunctionContext& Context, EInternalCompilerFlags InternalFlags) override;
	//virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO) override;
	//virtual void SaveSubObjectsFromCleanAndSanitizeClass(FSubobjectCollection& SubObjectsToSave, UBlueprintGeneratedClass* ClassToClean) override;
	//virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	//virtual void CreateClassVariablesFromBlueprint() override;
	//virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject);
	//virtual void FinishCompilingClass(UClass* Class) override;
	virtual bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;
	//virtual void OnPostCDOCompiled() override;
	// End FKismetCompilerContext

protected:
};

