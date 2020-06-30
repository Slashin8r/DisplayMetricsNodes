// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericPlatform/GenericApplication.h"
#include "DisplayMetricsBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

UCLASS()
class UDisplayMetricsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "DisplayMetrics")
	static TArray<FString> GetMonitorIDs()
	{
		FDisplayMetrics Display;
		FDisplayMetrics::RebuildDisplayMetrics(Display);
		auto MonitorInfo = Display.MonitorInfo;
		TArray<FString> MonitorIDs;

		for (int i = 0; i < MonitorInfo.Num(); i++)
		{
			MonitorIDs.Add(MonitorInfo[i].ID);
		}

		return MonitorIDs;
	}

	UFUNCTION(BlueprintPure, Category = "DisplayMetrics")
	static TArray<FString> GetMonitorNames()
	{
		FDisplayMetrics Display;
		FDisplayMetrics::RebuildDisplayMetrics(Display);
		auto MonitorInfo = Display.MonitorInfo;
		TArray<FString> MonitorNames;

		for (int i = 0; i < MonitorInfo.Num(); i++)
		{
			MonitorNames.Add(MonitorInfo[i].Name);
		}

		return MonitorNames;
	}

	UFUNCTION(BlueprintCallable, Category = "DisplayMetrics")
	static void SetMonitor(int32 MonitorIndex)
	{
		FDisplayMetrics DisplayMetrics;
		FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
		/*
		// Start on the main display by default
		int32 MonitorNumber = 0;
		FParse::Value(FCommandLine::Get(), TEXT("monitor="), MonitorNumber);

		// Get the correct monitor index
		int32 MonitorIndex = INDEX_NONE;
		if (MonitorNumber == 0)
		{
			// Start monitor on the main screen
			for (int32 Index = 0; Index < DisplayMetrics.MonitorInfo.Num(); Index++)
			{
				if (DisplayMetrics.MonitorInfo[Index].bIsPrimary)
				{
					MonitorIndex = Index;
					break;
				}
			}
		}
		else
		{
			// Normalize
			MonitorIndex = MonitorNumber - 1;
		}
		*/
		if (DisplayMetrics.MonitorInfo.IsValidIndex(MonitorIndex))
		{
			const FMonitorInfo Monitor = DisplayMetrics.MonitorInfo[MonitorIndex];
			// check if UserSettings.ResolutionSizeX > Monitor.NativeWidth || UserSettings.ResolutionSizeY > Monitor.NativeHeight
			// if true then change your game resolution to Monitor.NativeWidth, Monitor.NativeHeight

			const int32 WindowPosX = Monitor.WorkArea.Left;
			const int32 WindowPosY = Monitor.WorkArea.Top;
			const FVector2D Position(static_cast<float>(WindowPosX), static_cast<float>(WindowPosY));

			if (GEngine && GEngine->GameViewport)
			{
				TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow();

				// Hack for linux
#if PLATFORM_LINUX
				const EWindowMode::Type CurrentWindowMode = Window->GetWindowMode();
				Window->SetWindowMode(EWindowMode::Windowed);
#endif

				Window->MoveWindowTo(Position);

#if PLATFORM_LINUX
				// Set back to original
				Window->SetWindowMode(CurrentWindowMode);
#endif
			}
		}
	}
};
