// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGStylesheetCompiler.h"

#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_VariableGet.h"
#include "Blueprint/WidgetTree.h"
#include "Animation/WidgetAnimation.h"
#include "MovieScene.h"

#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "IUMGModule.h"
#include "Editor/WidgetCompilerLog.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "BLAH"

#define CPF_Instanced (CPF_PersistentInstance | CPF_ExportObject | CPF_InstancedReference)

extern COREUOBJECT_API bool GMinimalCompileOnLoad;


FBYGStylesheetCompiler::FBYGStylesheetCompiler()
	: ReRegister(nullptr)
	, CompileCount(0)
{

}

bool FBYGStylesheetCompiler::CanCompile(const UBlueprint* Blueprint)
{
	if ( Blueprint )
	{
		return true;
	}
	return false;
}


void FBYGStylesheetCompiler::Compile(UBlueprint* Blueprint, const FKismetCompilerOptions & CompileOptions, FCompilerResultsLog & Results)
{
	FBYGStylesheetCompilerContext Compiler( Blueprint, Results, CompileOptions );
	Compiler.Compile();
	check( Compiler.NewClass );
}

FBYGStylesheetCompilerContext::FBYGStylesheetCompilerContext(UBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions)
	: Super(SourceSketch, InMessageLog, InCompilerOptions)
{
}

FBYGStylesheetCompilerContext::~FBYGStylesheetCompilerContext()
{
}

template<typename TOBJ>
struct FCullTemplateObjectsHelper
{
	const TArray<TOBJ*>& Templates;

	FCullTemplateObjectsHelper(const TArray<TOBJ*>& InComponentTemplates)
		: Templates(InComponentTemplates)
	{}

	bool operator()(const UObject* const RemovalCandidate) const
	{
		return ( NULL != Templates.FindByKey(RemovalCandidate) );
	}
};


class FBlueprintCompilerLog : public IWidgetCompilerLog
{
public:
	FBlueprintCompilerLog(FCompilerResultsLog& InMessageLog, TSubclassOf<UUserWidget> InClassContext)
		: MessageLog(InMessageLog)
		, ClassContext(InClassContext)
	{
	}

	virtual TSubclassOf<UUserWidget> GetContextClass() const override
	{
		return ClassContext;
	}

	virtual void InternalLogMessage(TSharedRef<FTokenizedMessage>& InMessage) override
	{
		MessageLog.AddTokenizedMessage(InMessage);
	}

private:
	// Compiler message log (errors, warnings, notes)
	FCompilerResultsLog& MessageLog;
	TSubclassOf<UUserWidget> ClassContext;
};

bool FBYGStylesheetCompilerContext::ValidateGeneratedClass(UBlueprintGeneratedClass* Class)
{
	const bool SuperResult = Super::ValidateGeneratedClass(Class);
	const bool Result = false; //UWidgetBlueprint::ValidateGeneratedClass(Class);

	if ( Class->ClassDefaultObject && Class->ClassDefaultObject->GetClass()->IsChildOf( UBYGRichTextStylesheet::StaticClass() ) )
	{
		// Do validation
		UBYGRichTextStylesheet* Obj = Class->GetDefaultObject<UBYGRichTextStylesheet>();
		if ( Obj )
		{
			Obj->Validate( MessageLog );
		}
	}

	return SuperResult && Result;
}

#undef LOCTEXT_NAMESPACE
