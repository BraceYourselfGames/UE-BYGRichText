// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "Engine.h"
#include "BYGStylesheetCompiler.h"

class FBYGRichTextEditor : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	class FBYGStylesheetCompiler* GetRegisteredCompiler() { return &StylesheetCompiler; }

protected:
	void OnPostEngineInit();

	FBYGStylesheetCompiler StylesheetCompiler;
};
