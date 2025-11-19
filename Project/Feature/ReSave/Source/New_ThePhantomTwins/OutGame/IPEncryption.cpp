// Fill out your copyright notice in the Description page of Project Settings.


#include "IPEncryption.h"
#include "SocketSubsystem.h"
#include "Misc/AES.h"
#include "Misc/Base64.h"

FString UIPEncryption::GetLocalIPAddress(bool bHasPort)
{
	bool bCanBindAll;
	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
	return Addr->ToString(bHasPort);
}

FString UIPEncryption::IP2Code(const FString& IP, bool bHasPort)
{
	FString Code;
	FCompactIP EncryptionIP = UIPEncryption::IPEncryption(IP, bHasPort);
	if (bHasPort)
		Code = FString::Printf(TEXT("%016llX"), EncryptionIP.CombinedValue);
	else
		Code = FString::Printf(TEXT("%08X"), EncryptionIP.SplitValue[0]);
	return TransformHexCustom(Code, true);
}

FString UIPEncryption::Code2IP(const FString& Code, bool bHasPort)
{
	FString IP;
	FString Conversion;
	FCompactIP DecryptionIP;

	Conversion = TransformHexCustom(Code, false);
	if (bHasPort)
	{
		DecryptionIP.CombinedValue = FParse::HexNumber64(*Conversion);
		IP = UIPEncryption::IPDecryption(DecryptionIP, bHasPort);
	}
	else
	{
		DecryptionIP.SplitValue[0] = FParse::HexNumber(*Conversion);
		IP = UIPEncryption::IPDecryption(DecryptionIP, bHasPort);
	}
	return IP;
}

FCompactIP UIPEncryption::IPEncryption(const FString& IP, bool bHasPort)
{
	FCompactIP Result;
	Result.CombinedValue = 0;

	FString IPPart, PortPart;

	if (bHasPort)
	{
		if (!IP.Split(TEXT("::"), &IPPart, &PortPart))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid format (missing '::'): %s"), *IP);
			return Result;
		}
	}
	else
	{
		IPPart = IP;
		PortPart = TEXT("0");
	}

	TArray<FString> IPSections;
	IPPart.ParseIntoArray(IPSections, TEXT("."), true);
	if (IPSections.Num() != 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid IP section count: %s"), *IPPart);
		return Result;
	}

	Result.IP.A = static_cast<uint8>(FCString::Atoi(*IPSections[0]));
	Result.IP.B = static_cast<uint8>(FCString::Atoi(*IPSections[1]));
	Result.IP.C = static_cast<uint8>(FCString::Atoi(*IPSections[2]));
	Result.IP.D = static_cast<uint8>(FCString::Atoi(*IPSections[3]));
	Result.IP.Port = static_cast<uint32>(FCString::Atoi(*PortPart));

	return Result;
}

FString UIPEncryption::IPDecryption(const FCompactIP& IP, bool bHasPort)
{
	const FString IPAddress = FString::Printf(TEXT("%u.%u.%u.%u"),
		IP.IP.A, IP.IP.B, IP.IP.C, IP.IP.D);

	return bHasPort
		? FString::Printf(TEXT("%s::%u"), *IPAddress, IP.IP.Port)
		: IPAddress;
}

FString UIPEncryption::TransformHexCustom(const FString& Code, bool Direction)
{
	FString Result;
	Result.Reserve(Code.Len());

	if (Direction)
	{
		// Hex ¡æ Custom (0~9, A~F ¡æ A~P)
		for (TCHAR Char : Code)
		{
			TCHAR UpperChar = FChar::ToUpper(Char);
			int32 Value = -1;

			if (UpperChar >= '0' && UpperChar <= '9')
			{
				Value = UpperChar - '0';
			}
			else if (UpperChar >= 'A' && UpperChar <= 'F')
			{
				Value = 10 + (UpperChar - 'A');
			}

			if (Value >= 0 && Value <= 15)
			{
				Result.AppendChar('A' + Value);
			}
			else
			{
				Result.AppendChar(Char);
			}
		}
	}
	else
	{
		// Custom ¡æ Hex (A~P ¡æ 0~F)
		for (TCHAR Char : Code)
		{
			TCHAR UpperChar = FChar::ToUpper(Char);

			if (UpperChar >= 'A' && UpperChar <= 'P')
			{
				int32 Value = UpperChar - 'A';

				if (Value < 10)
				{
					Result.AppendChar('0' + Value);
				}
				else
				{
					Result.AppendChar('A' + (Value - 10));
				}
			}
			else
			{
				Result.AppendChar(Char);
			}
		}
	}

	return Result;
}
