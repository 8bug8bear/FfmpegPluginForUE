// Copyright by IT2021, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FFMPEGSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FFMPEG_API FConversionParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString PathToFolderWithImages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString DefaultImageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	int32 LengthOfImageIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString ImageFormat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString PathToCreatedVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString NameToCreatedVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	FString VideoFormat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	int32 FramesPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	int32 FramesPerSecondOfVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "ConversionParameters")
	bool bDeleteImagesAfterFinishingWork;

	FConversionParameters()
	{
		PathToFolderWithImages = "";
		DefaultImageName = "";
		LengthOfImageIndex = 0;
		ImageFormat = "";
		PathToCreatedVideo = "";
		NameToCreatedVideo = "";
		VideoFormat = "";
		FramesPerSecond = 0;
		FramesPerSecondOfVideo = 0;
		bDeleteImagesAfterFinishingWork = false;
	}
	
	FConversionParameters(FConversionParameters const& Other)
	{
		PathToFolderWithImages = Other.PathToFolderWithImages;
		DefaultImageName = Other.DefaultImageName;
		LengthOfImageIndex = Other.LengthOfImageIndex;
		ImageFormat = Other.ImageFormat;
		PathToCreatedVideo = Other.PathToCreatedVideo;
		NameToCreatedVideo = Other.NameToCreatedVideo;
		VideoFormat = Other.VideoFormat;
		FramesPerSecond = Other.FramesPerSecond;
		FramesPerSecondOfVideo = Other.FramesPerSecondOfVideo;
		bDeleteImagesAfterFinishingWork = Other.bDeleteImagesAfterFinishingWork;
	}

	~FConversionParameters() {};
};

UENUM(BlueprintType, Blueprintable)
enum class EEncodeSettings : uint8
{
	Low,
	Med,
	High,
	Epic
};

USTRUCT(BlueprintType)
struct FFMPEG_API FRecordingParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	FString PathToCreatedVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	FString NameToCreatedVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	FString VideoFormat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	int32 FramesPerSecond;

	// ToDo Add audio recording support
	//UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	//bool bRecordAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "RecordingParameters")
	EEncodeSettings EncodeSettings;

	FRecordingParameters()
	{
		PathToCreatedVideo = "";
		NameToCreatedVideo = "";
		VideoFormat = "mp4";
		FramesPerSecond = 0;
		//bRecordAudio = true;
		EncodeSettings = EEncodeSettings::High;
	}
	
	FRecordingParameters(FRecordingParameters const& Other)
	{
		PathToCreatedVideo = Other.PathToCreatedVideo;
		NameToCreatedVideo = Other.NameToCreatedVideo;
		VideoFormat = Other.VideoFormat;
		FramesPerSecond = Other.FramesPerSecond;
		//bRecordAudio = Other.bRecordAudio;
		EncodeSettings = Other.EncodeSettings;
	}
	
	~FRecordingParameters(){};
	
	int32 GetEncodeSettingsValue() const
	{
		switch (EncodeSettings)
		{
		case EEncodeSettings::Low:
			return 28;
			
		case EEncodeSettings::Med:
			return 23;
			
		case EEncodeSettings::High:
			return 20;
			
		case EEncodeSettings::Epic:
			return 16;
			
		default:
			return 20;
		}
	}
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FGenerateVideoComplitionCallback,const FString&, VideoPath,const FString&, VideoName);

UCLASS()
class FFMPEG_API UFFMPEGSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool GenerateVideoFromImages(FConversionParameters ConversionParameters, FString& ErrorMessage, FGenerateVideoComplitionCallback CPPCallback);
	
	UFUNCTION(BlueprintPure)
	bool IsProcessGenerateVideoAtWork() const;

	UFUNCTION(BlueprintPure)
	bool IsRecordingVideoAtWork() const;

	UFUNCTION(BlueprintCallable)
	bool StartRecordingVideo(FRecordingParameters RecordingParameters, FString& ErrorMessage);

	UFUNCTION(BlueprintCallable)
	bool StopRecordingVideo(FString& ErrorMessage);

private:
	static void SplitStringByPoint(const FString& StringToSplit, FString& Left, FString& Right);

	static FString GenerateVideoName(); 
	
	UPROPERTY()
	class UFFMPEGDirector* FFMPEG;
	
	bool bProcessGenerateVideoAtWork = false;

	bool bRecordingVideoAtWork = false;
	
};
