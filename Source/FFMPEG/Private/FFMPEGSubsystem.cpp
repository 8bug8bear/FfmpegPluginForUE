// Copyright by IT2021, Inc. All Rights Reserved.


#include "FFMPEGSubsystem.h"
#include "FFMPEGDirector.h"
#include "Kismet/KismetSystemLibrary.h"

bool UFFMPEGSubsystem::GenerateVideoFromImages(FConversionParameters ConversionParameters, FString& ErrorMessage, FGenerateVideoComplitionCallback CPPCallback)
{
	ErrorMessage.Empty();

	if (!ConversionParameters.DefaultImageName.IsEmpty())
	{
		SplitStringByPoint(ConversionParameters.DefaultImageName, ConversionParameters.DefaultImageName, ConversionParameters.ImageFormat);
	}

	if (ConversionParameters.ImageFormat.IsEmpty())
	{
		ErrorMessage.Append("The format of the converted images is not entered!\n");
		return false;
	}

	if (ConversionParameters.LengthOfImageIndex <= 0)
	{
		ErrorMessage.Append("Incorrect length of image indexes!\n");
		return false;
	}

	if (ConversionParameters.PathToFolderWithImages.IsEmpty())
	{
		ErrorMessage.Append("The folder with the original images is not specified!\n");
		return false;
	}

	if (ConversionParameters.FramesPerSecond <= 0 || ConversionParameters.FramesPerSecond > 250)
	{
		ConversionParameters.FramesPerSecond = 25;
		ErrorMessage.Append("Incorrect number of frames! The number of frames is set by default as 25.\n");
	}

	if (ConversionParameters.PathToCreatedVideo.IsEmpty())
	{
		ConversionParameters.PathToCreatedVideo = UKismetSystemLibrary::GetProjectSavedDirectory();;
	}
	else
	{
		SplitStringByPoint(ConversionParameters.PathToCreatedVideo, ConversionParameters.PathToCreatedVideo, ConversionParameters.VideoFormat);
	}
	
	if (ConversionParameters.NameToCreatedVideo.IsEmpty())
	{
		ConversionParameters.NameToCreatedVideo = GenerateVideoName();
	}
	else
	{
		SplitStringByPoint(ConversionParameters.NameToCreatedVideo, ConversionParameters.NameToCreatedVideo, ConversionParameters.VideoFormat);
	}
	
	if (ConversionParameters.VideoFormat.IsEmpty())
	{
		ConversionParameters.VideoFormat = "mp4";
	}
	else
	{
		ConversionParameters.VideoFormat.RemoveFromStart(".");
	}
	
	if (ConversionParameters.FramesPerSecondOfVideo<=0 || ConversionParameters.FramesPerSecondOfVideo > 250)
	{
		ConversionParameters.FramesPerSecondOfVideo = ConversionParameters.FramesPerSecond;
	}

	bProcessGenerateVideoAtWork = true;
	TDelegate<void()> Callback;
	Callback.BindLambda([this, &CPPCallback, &ConversionParameters]
	{
		bProcessGenerateVideoAtWork = false;
		CPPCallback.ExecuteIfBound(ConversionParameters.PathToCreatedVideo, ConversionParameters.NameToCreatedVideo);
	});

	if (!FFMPEG)
	{
		FFMPEG = NewObject<UFFMPEGDirector>(this);
	}
	
	FFMPEG->FFMPEGGenerateVideoFromImages(ConversionParameters, ErrorMessage, Callback);

	return true;
}

bool UFFMPEGSubsystem::IsProcessGenerateVideoAtWork() const
{
	return bProcessGenerateVideoAtWork;
}

bool UFFMPEGSubsystem::IsRecordingVideoAtWork() const
{
	return bRecordingVideoAtWork;
}

bool UFFMPEGSubsystem::StartRecordingVideo(FRecordingParameters RecordingParameters, FString& ErrorMessage)
{
	ErrorMessage.Empty();
	
	if(RecordingParameters.PathToCreatedVideo.IsEmpty())
	{
		ErrorMessage = "The path to the video is not specified, the video will be saved to the saved folder";
		RecordingParameters.PathToCreatedVideo = UKismetSystemLibrary::GetProjectSavedDirectory();
	}
	else
	{
		SplitStringByPoint(RecordingParameters.PathToCreatedVideo, RecordingParameters.PathToCreatedVideo, RecordingParameters.VideoFormat);
	}

	if(RecordingParameters.NameToCreatedVideo.IsEmpty())
	{
		RecordingParameters.NameToCreatedVideo = GenerateVideoName();
	}
	else
	{
		SplitStringByPoint(RecordingParameters.NameToCreatedVideo, RecordingParameters.NameToCreatedVideo, RecordingParameters.VideoFormat);
	}

	if(RecordingParameters.VideoFormat.IsEmpty())
	{
		RecordingParameters.VideoFormat = "mp4";
	}
	else
	{
		RecordingParameters.VideoFormat.RemoveFromStart(".");
	}
	
	if (RecordingParameters.FramesPerSecond<=0 || RecordingParameters.FramesPerSecond > 250)
	{
		RecordingParameters.FramesPerSecond = 60.f;
	}

	if (!FFMPEG)
	{
		FFMPEG = NewObject<UFFMPEGDirector>(this);
	}
	bRecordingVideoAtWork = true;

	FVector2d ViewportSize;
	if(GetWorld())
	{
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	}
	else
	{
		ViewportSize = FVector2d(1920, 1080);
	}
	
	return FFMPEG->FFMPEGStartRecordingVideo(RecordingParameters, ViewportSize, ErrorMessage);;
}

bool UFFMPEGSubsystem::StopRecordingVideo(FString& ErrorMessage)
{
	bRecordingVideoAtWork = false;
	if (!FFMPEG)
	{
		ErrorMessage = "No FFMPEG object";
		return false;
	}
	
	FFMPEG->FFMPEGStopRecordingVideo(ErrorMessage);
	return true;
}

void UFFMPEGSubsystem::SplitStringByPoint(const FString& StringToSplit, FString& Left, FString& Right)
{
	FString LeftPart;
	FString RightPart;
	if (StringToSplit.Split(TEXT("."), &LeftPart, &RightPart))
	{
		Left = LeftPart;
		if(Right.IsEmpty())
		{
			Right = RightPart;
		}
	}
}

FString UFFMPEGSubsystem::GenerateVideoName()
{
	return "Video" + FDateTime::Now().ToFormattedString(TEXT("%Y%m%d_%H%M%S")); // set current date in video title
}


