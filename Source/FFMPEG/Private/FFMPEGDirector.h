// Copyright by IT2021, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FFMPEGDirector.generated.h"

struct FConversionParameters;
struct FRecordingParameters;

UCLASS()
class FFMPEG_API UFFMPEGDirector : public UObject
{
	GENERATED_BODY()
	
public:
	bool FFMPEGGenerateVideoFromImages(FConversionParameters ConversionParameters, FString& ErrorMessage, TDelegate<void()> const& CPPCallback);

	bool FFMPEGStartRecordingVideo(FRecordingParameters RecordingParameters, FVector2d ViewportSize, FString& ErrorMessage);

	void FFMPEGStopRecordingVideo(FString& ErrorMessage);

private:
	static FString GetFullFFMPEGPath();

	static FString ParamsToString(const TArray<FString>& Params);
	
#pragma region GenerateVideoFromImages
	UFUNCTION()
	void CheckProcessStatus();

	FTimerHandle CheckProcessStatusTimerHandle;

	const float FrequencyOfInspections = 0.25f;
	
	bool bDeleteImagesAfterFinishingWork;
	
	FString ImageFolderPath;

	FProcHandle GenerateVideoFromImagesHandle;

	TDelegate<void()> OnEndCallback;
	
#pragma endregion

#pragma region VideoRecording
	bool bISVideoRecording = false;

	FProcHandle RecordingVideoHandle;

	// he end of the pipe for this plugin
	void* PipeWriteFfmpeg;
	void* PipeReadFfmpeg;
	// The end of the pipe for ffmpeg
	void* PipeWritePlugin;
	void* PipeReadPlugin;

#pragma endregion
	
};
