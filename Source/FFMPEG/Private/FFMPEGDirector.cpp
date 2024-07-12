// Copyright by IT2021, Inc. All Rights Reserved.


#include "FFMPEGDirector.h"
#include "FFMPEGSubsystem.h"

bool UFFMPEGDirector::FFMPEGGenerateVideoFromImages(FConversionParameters ConversionParameters, FString& ErrorMessage, TDelegate<void()> const& CPPCallback)
{
	const FString FFMPEGPath = GetFullFFMPEGPath();
	
	FString FFMPEGCommand;
	if (ConversionParameters.FramesPerSecond >= ConversionParameters.FramesPerSecondOfVideo)
	{
		FFMPEGCommand = "-r " + FString::FromInt(ConversionParameters.FramesPerSecond) + " -i \""
			+ FPaths::Combine(ConversionParameters.PathToFolderWithImages,ConversionParameters.DefaultImageName) + "%0"
			+ FString::FromInt(ConversionParameters.LengthOfImageIndex) + "d." + ConversionParameters.ImageFormat
			+ "\" -c:v libx264 -vf \"pad = ceil(iw / 2) * 2:ceil(ih / 2) * 2\" -r " + FString::FromInt(ConversionParameters.FramesPerSecondOfVideo) + " -pix_fmt yuv420p \""
			+ FPaths::Combine(ConversionParameters.PathToCreatedVideo, ConversionParameters.NameToCreatedVideo) + "." + ConversionParameters.VideoFormat;
	}
	else
	{
		FFMPEGCommand = "-framerate " + FString::FromInt(ConversionParameters.FramesPerSecond) + " -i \""
			+ FPaths::Combine(ConversionParameters.PathToFolderWithImages, ConversionParameters.DefaultImageName) + "%0"
			+ FString::FromInt(ConversionParameters.LengthOfImageIndex) + "d." + ConversionParameters.ImageFormat
			+ "\" -c:v libx264 -vf \"pad = ceil(iw / 2) * 2:ceil(ih / 2) * 2, minterpolate = fps = " + FString::FromInt(ConversionParameters.FramesPerSecondOfVideo) +"\"" + " -pix_fmt yuv420p \""
			+ FPaths::Combine(ConversionParameters.PathToCreatedVideo,ConversionParameters.NameToCreatedVideo) + "." + ConversionParameters.VideoFormat;
	}
	
	GenerateVideoFromImagesHandle = FPlatformProcess::CreateProc(*FFMPEGPath, *FFMPEGCommand, true, true, true, nullptr, 0, nullptr, nullptr);
	
	if (!GenerateVideoFromImagesHandle.IsValid())
	{
		ErrorMessage.Append("Unable to create a process!\n");
		CPPCallback.ExecuteIfBound();
		return false;
	}

	OnEndCallback = CPPCallback;
	
	bDeleteImagesAfterFinishingWork = ConversionParameters.bDeleteImagesAfterFinishingWork;
	ImageFolderPath = ConversionParameters.PathToFolderWithImages;
	
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(CheckProcessStatusTimerHandle,this, &UFFMPEGDirector::CheckProcessStatus, FrequencyOfInspections, true);
		return true;
	}

	OnEndCallback.ExecuteIfBound();
	ErrorMessage.Append("World context ptr is not valid! The process has been started, but it is not tracked. It will be automatically deleted after the video creation is completed.\n");
	return false;
}

bool UFFMPEGDirector::FFMPEGStartRecordingVideo(FRecordingParameters RecordingParameters, FVector2d ViewportSize, FString& ErrorMessage)
{
	if(bISVideoRecording)
	{
		ErrorMessage.Append("Unable to create a process!/n");
		FFMPEGStopRecordingVideo(ErrorMessage);
	}

	const FString FFMPEGPath = GetFullFFMPEGPath();

	TArray<FString> Params;

	// Set buffer size 
	constexpr int32 BufferSize = 2048;
	Params.Add(FString::Printf(TEXT("-rtbufsize %d"), BufferSize));

	// Set video driver
	Params.Add("-f gdigrab");

	// Set frame rate
	Params.Add(FString::Printf(TEXT("-framerate %d"), RecordingParameters.FramesPerSecond));

	// Set mouse visibility 
	constexpr bool bDrawMouse = false;
	Params.Add(FString::Printf(TEXT("-draw_mouse %d"), bDrawMouse));

	// Set window size
	const FString WindowsSize = FString::Printf(TEXT("-video_size %dx%d"),FMath::FloorToInt32(ViewportSize.X), FMath::FloorToInt32(ViewportSize.Y));
	Params.Add(FString::Printf(TEXT("%s -i desktop"), *WindowsSize));

	// Set video codec
	Params.Add("-c:v libx264");

	// Set codec settings
	const int32 EncodeSettings = RecordingParameters.GetEncodeSettingsValue();
	Params.Add(FString::Printf(TEXT("-vf \"pad = ceil(iw / 2) * 2:ceil(ih / 2) * 2\" -preset veryfast -profile:v high -crf %d -pix_fmt yuv420p"), EncodeSettings));

	// Set output path and name
	Params.Add(FString::Printf(TEXT("\"%s%s.%s\""), *RecordingParameters.PathToCreatedVideo, *RecordingParameters.NameToCreatedVideo, *RecordingParameters.VideoFormat));

	const FString FFMPEGCommand = ParamsToString(Params);

	PipeWriteFfmpeg = nullptr;
	PipeReadFfmpeg = nullptr;
	PipeWritePlugin = nullptr;
	PipeReadPlugin = nullptr;

	FPlatformProcess::CreatePipe(PipeReadPlugin, PipeWriteFfmpeg, false);
	FPlatformProcess::CreatePipe(PipeReadFfmpeg, PipeWritePlugin, true);
	
	RecordingVideoHandle = FPlatformProcess::CreateProc(*FFMPEGPath, *FFMPEGCommand, true, true, true, nullptr, 0, nullptr, PipeWriteFfmpeg, PipeReadFfmpeg);
	if (!RecordingVideoHandle.IsValid())
	{
		ErrorMessage.Append("Unable to create a process!/n");
		return false;
	}
	FPlatformProcess::SetProcPriority(RecordingVideoHandle, 2);
	
	return true;
}

void UFFMPEGDirector::FFMPEGStopRecordingVideo(FString& ErrorMessage)
{
	if(!FPlatformProcess::WritePipe(PipeWritePlugin, TEXT("q\n")))
	{
		ErrorMessage.Append("Unable to write to a process!/n");
	}
	FPlatformProcess::ClosePipe(PipeReadPlugin, PipeWriteFfmpeg);
	FPlatformProcess::ClosePipe(PipeReadFfmpeg, PipeWritePlugin);
}

FString UFFMPEGDirector::GetFullFFMPEGPath()
{
	return FPaths::Combine(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), "Plugins/FFMPEG/Source/ThirdParty/ffmpeg/bin/ffmpeg.exe");
}

FString UFFMPEGDirector::ParamsToString(const TArray<FString>& Params)
{
	FString Result;
	for(const FString& Param : Params)
	{
		Result.Append(Param);
		Result.Append(" ");
	}
	return Result;
}

void UFFMPEGDirector::CheckProcessStatus()
{
	if(!FPlatformProcess::IsProcRunning(GenerateVideoFromImagesHandle))
	{
		OnEndCallback.ExecuteIfBound();

		if(bDeleteImagesAfterFinishingWork)
		{
			IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
			if(FileManager.DirectoryExists(*ImageFolderPath))
			{
				FileManager.DeleteDirectoryRecursively(*ImageFolderPath);
			}
		}

		GetWorld()->GetTimerManager().ClearTimer(CheckProcessStatusTimerHandle);
	}
}


